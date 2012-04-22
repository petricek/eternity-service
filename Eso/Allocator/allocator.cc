#include "./H/allocator.h"
#include "./../../Common/GMessage/H/msgField.h"
#include "./../../Common/RandomGenerator/H/randomGenerator.h"

/**
 * Constructor.
 *
 * Creates allocation table and links to its parent Eso
 *
 *
 * @param   anEso pointer to Eso owning this class
 * @param   allocLogFile pointer to log file where Allocator should log its work
 * @author  Pavel
 * @see     Eso, Majordomo, SixOffspring, Debugable
 */
Allocator::Allocator(Eso *anEso, LogFile *allocLogFile)
: Debugable(allocLogFile),
  SixOffspring(anEso)
{
  Char *tmp;
  TableFieldQueue *definition;

    // initialize allocTable 
  definition = CreateDefinition();
  if(definition == NULL)
    WriteString(llError, "Allocator::Allocator(): cannot create table definition");
  allocTable = new Table(tmp = ((Eso *) six)->GetPath(ALLOC_PATH""ALLOC_TABLE_NAME), definition);
  DELETE(tmp);
  if(allocTable == NULL)
    WriteString(llError, "Allocator::Allocator(): cannot create table");

  if(definition != NULL)
    delete definition;
}


/**
 * Destructor.
 *
 * Destroyes allocation table
 *
 *
 * @author  Pavel
 * @see     Table
 */
Allocator::~Allocator()
{
  if(allocTable != NULL)
    delete allocTable;
}


/**
 * Creates definition of allocTable.
 *
 * Creates the definition of allocation table by inserting
 * all needed fields in it.
 *
 *
 * @return  definition of allocation table
 * @author  Pavel
 * @see     Table, TableField
 */
TableFieldQueue *Allocator::CreateDefinition()
{
  TableFieldQueue *definition;
  TableField *field;

  definition = new TableFieldQueue;
  if(definition == NULL)
    return NULL;

  field = new IDField(ALLOC_TBLFLD_KEY_ID);
  if(field == NULL)
  {
    WriteString(llError, "Allocator::CreateDefinition(): cannot create TableField");
    delete definition;
    return NULL;
  }
  definition->Insert(field);

  field = new SizeField(ALLOC_TBLFLD_FILE_SIZE);
  if(field == NULL)
  {
    WriteString(llError, "Allocator::CreateDefinition(): cannot create TableField");
    delete definition;
    return NULL;
  }
  definition->Insert(field);

  field = new StringField(ALLOC_TBLFLD_FILE_NAME);
  if(field == NULL)
  {
    WriteString(llError, "Allocator::CreateDefinition(): cannot create TableField");
    delete definition;
    return NULL;
  }
  definition->Insert(field);

  return definition;
}


/**
 * Creates allocation file.
 *
 * Creates a dummy file in ALLOC_FILES_PATH directory of given size
 *
 *
 * @param   fileSize size of file to allocate on disk
 * @param   anAllocFileName out parameter that tells the name of newly created allocation file
 * @return  error code
 * @author  Pavel
 * @see     RandomGenerator, CreateTempFileName()
 */
Err Allocator::CreateAllocFile(const Size fileSize, Char **anAllocFileName)
{
  Char *allocFileName;
  RandomGenerator *randomGen;
  MsgField *randomBytesFld;
  Err   result;
  Char *tmp;

    // create temp name
  allocFileName = CreateTempFileName(tmp = ((Eso *) six)->GetPath(ALLOC_FILES_PATH"%"), NULL, logFile);
  DELETE(tmp);
  if(allocFileName == NULL)
  {
    WriteString(llError, "Allocator::CreateAllocFile(): cannot create allocation file name");
    return KO;
  }
  WriteString(llDebug, "  allocation file name: %s", allocFileName);

    // acces random generator 
  randomGen = new RandomGenerator();
  if(randomGen == NULL)
  {
    WriteString(llError, "Allocator::CreateAllocFile(): cannot initiate random generator");
    return KO;
  }
    // generate required bytes
  randomBytesFld = randomGen->RandomBytes(fileSize);
  if(randomBytesFld == NULL)
  {
    WriteString(llError, "Allocator::CreateAllocFile(): cannot generate %u random bytes", fileSize);
    delete randomGen;
    return KO;
  }

    // store the generated info to a allocation file
  randomBytesFld->SetLogFile(logFile);
  result = randomBytesFld->GetAsFile(allocFileName);
  if(result == KO)
    WriteString(llError, "Allocator::CreateAllocFile(): cannot create allocation file %s",
    allocFileName);

    // close ups
  (*anAllocFileName) = allocFileName;
  delete randomGen;
  delete randomBytesFld;

  WriteString(llDebug, "Allocator::CreateAllocFile(): returning %d, %s", result, *anAllocFileName);
  return result;
}


/**
 * Insert into allocTable.
 *
 * Inserts a record to table of allocations
 *
 *
 * @param   tID transaction ID
 * @param   fileSize size of allocated file
 * @param   fileName name of allocated file
 * @return  error code
 * @author  Pavel
 * @see     Table, Record
 */
Err Allocator::AllocTableInsert(const TID tID, const Size fileSize, const Char *fileName)
{
  Record * allocRecord;
  Err result;

  if(allocTable == NULL)
    return KO;

    // create new record
  allocRecord = allocTable->NewRecord();
  if(allocRecord == NULL)
  {
    WriteString(llError, "Allocator::AllocTableInsert(): cannot create new allocation record");
    return KO;
  }

    // fill new record
  result = allocRecord->SetField(ALLOC_TBLFLD_KEY_ID, (void*)&tID);
  if(allocRecord->SetField(ALLOC_TBLFLD_FILE_SIZE, (void*)&fileSize) == KO)
    result = KO;
  if(allocRecord->SetField(ALLOC_TBLFLD_FILE_NAME, (void*)fileName) == KO)
    result = KO;
  if(result == KO)
  {
    WriteString(llError, "Allocator::AllocTableInsert(): cannot fill new allocation record");
    delete allocRecord;
    return KO;
  }

    // insert new record into allocTable
  if(allocTable->Open() == KO)
  {
    WriteString(llError, "Allocator::AllocTableInsert(): cannot open allocation table");
    delete allocRecord;
    return KO;
  }
  result = allocTable->InsertRecord(allocRecord);
  WriteString(llDebug, "Allocator::AllocTableInsert(): InsertRecord = %d",
  result);
  if(result == KO)
    WriteString(llError, "Allocator::AllocTableInsert(): cannot insert to allocation table");
  delete allocRecord;
  allocTable->Close();

  return result;
}


/**
 * Searches allocTable.
 *
 * Finds first record with tID and returns fileSize and fileName, if
 * desired. "shouldClose" says wether to close allocTable again.
 *
 *
 * @param   tID transaction ID
 * @param   shouldClose tells if the table should be closed after a successfull search
 * @param   fileSize size of found file
 * @param   fileName name of found file
 * @return  error code
 * @author  Pavel
 * @see     Table
 */
Err Allocator::AllocTableFind(const TID tID, const Int shouldClose, Size *fileSize, Char **fileName)
{
  Int          getFileName = 0;
  SizeField   *fileSizeFld;
  StringField *fileNameFld;
  Record      *allocRecord;

  if(allocTable == NULL)
    return KO;

  if(fileName != NULL)
  {
    getFileName = 1;
    if((*fileName) != NULL)
      delete (*fileName);
  }

    // open allocation table
  if(allocTable->Open() == KO)
  {
    WriteString(llError, "Allocator::AllocTableFind(): cannot open allocation table");
    return KO;
  }

    // find record with tID
  allocRecord = allocTable->FindFirst(ALLOC_TBLFLD_KEY_ID, (void*)&tID);
  if(allocRecord == NULL)
    return KO;

    // close allocation table, if desired
  if(shouldClose == 1)
    allocTable->Close();

    // fill fileSize and fileName
  if(fileSize != NULL)
  {
      // !! this doesn't create new instance -> don't delete
    fileSizeFld = (SizeField*)allocRecord->GetField(ALLOC_TBLFLD_FILE_SIZE); 
    if(fileSizeFld == NULL)
      return KO;
    (*fileSize) = fileSizeFld->GetIDValue();
  }
  if(getFileName == 1)
  {
    fileNameFld = (StringField*)allocRecord->GetField(ALLOC_TBLFLD_FILE_NAME);
    if(fileNameFld == NULL)
      return KO;
    (*fileName) = fileNameFld->AsString();
  }

  return OK;
}


/**
 * Allocates space.
 *
 * Tries to allocate space for a specified time
 *
 *
 * @param   tID transaction ID
 * @param   fileSize size of allocated file
 * @return  error code
 * @author  Pavel
 * @see     CreateAllocFile(), AllocTableInsert()
 */
Err Allocator::AllocSpace(TID tID, Size fileSize)
{
  Err result, resultRemove;
  Char *allocFileName;

#define RESULT (result == KO ? "KO" : "OK")

  WriteString(llDebug, "AllocSpace - start");

    // try to alloc
  if((result = CreateAllocFile(fileSize, &allocFileName)) == KO)
  {
    if(allocFileName != NULL)
      delete allocFileName;
    WriteString(llError, "Allocator::AllocSpace(): cannot create allocation file");
    return KO;
  }

  WriteString(llDebug, "  after creation of alloc file (%s, %s)", RESULT,
  allocFileName);

    // if succesful, insert allocation record into the allocTable
  result = AllocTableInsert(tID, fileSize, allocFileName);

  WriteString(llDebug, "  after table insert (%s, %d)", RESULT, result);

    // if insert unsuccessful, remove allocation file
  if(result == KO)
  {
    WriteString(llError, "Allocator::AllocSpace(): cannot insert a record to "ALLOC_TABLE_NAME);
    File *file = new File(allocFileName);
    if(file != NULL)
      resultRemove = file->Remove();
    if((file == NULL) || (resultRemove == KO))
      WriteString(llWarning, "Allocator::AllocSpace(): cannot remove file %s", allocFileName);
    if(file != NULL)
      delete file;
  }

  WriteString(llDebug, "AllocSpace - end (%s, %d)", RESULT, result);

  delete allocFileName;
  return result;

#undef RESULT
}


/**
 * Is allocation valid?.
 *
 * Tests if there is an allocation (identified by tID)
 * with greater or equal size to fileSize, which was not
 * yet timed out.
 *
 *
 * @param   tID transaction ID
 * @param   fileSize checked size of file
 * @return  error code
 * @author  Pavel
 * @see     AllocTableFind
 */
Err Allocator::AllocValid(TID tID, Size fileSize)
{
  Size allocSize;

    // find allocation record with ID tID
  if(AllocTableFind(tID, YES, &allocSize) == KO)
    return KO;

    // if found, compare the size of allocation with fileSize
  return (allocSize >= fileSize) ? OK : KO;
}


/**
 * Frees space.
 *
 * Frees specified allocation space
 *
 *
 * @param   tID transaction ID
 * @return  error code
 * @author  Pavel
 * @see     AllocTableFind(), Table
 */
Err Allocator::FreeSpace(TID tID)
{
  Size allocSize;
  Char *allocFileName;

  if(allocTable == NULL)
    return KO;

    // find allocation record with ID tID
  allocFileName = NULL;
  if((AllocTableFind(tID, NO, &allocSize, &allocFileName) == KO) ||
     (allocFileName == NULL))
  {
    WriteString(llDebug, "Allocator::FreeSpace(%u): record not found");
    allocTable->Close();
    return KO;
  }

  // delete record from allocTable
  if(allocTable->DeleteRecord() == KO)
    WriteString(llWarning, "Allocator::FreeSpace(): cannot remove allocation record for %s", allocFileName);
  allocTable->Close();

  // delete file
  File *file = new File(allocFileName);
  if(file != NULL)
  {
    if(file->Remove() == KO)
      WriteString(llError, "Allocator::FreeSpace(): cannot remove file %s", allocFileName);
    delete file;
    return OK;
  }
  else
  {
    WriteString(llError, "Allocator::FreeSpace(): cannot create File object to remove allocation file"); 
  }

  return OK;
}


/**
 * Rolls back a transaction.
 *
 * Rollbacks allocation created in tID transaction
 *
 *
 * @param   tID transaction ID
 * @return  error code
 * @author  Pavel
 * @see     Rollbackable, FreeSpace()
 */
Err Allocator::Rollback(TID tID)
{
  return FreeSpace(tID);
}


/**
 * Allocator test.
 *
 * Test method used only for debugging
 *
 *
 * @return  void
 * @author  Pavel
 */
void Allocator::Test()
{
  WriteString(llDebug, "Allocator::Test(): AllocTableInsert(1,  512, \"prvni file\")");
  AllocTableInsert(1L,  512L, "prvni file");
  WriteString(llDebug, "Allocator::Test(): AllocTableInsert(2, 1024, \"druhy file\")");
  AllocTableInsert(2L, 1024L, "druhy file");
  WriteString(llDebug, "Allocator::Test(): AllocValid(1, 512) == %d", 
                                           AllocValid(1L, 512L));
}
