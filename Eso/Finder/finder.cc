#include "./H/finder.h"
#include "./../../Common/H/labels.h"

/**
 * Finder constructor.
 *
 * Creates fwd and reply Tables
 *
 *
 * @param   aLogFile Log file.
 * @param   eso Pointer to Eso object.
 * @author  Marek
 * @see     Table, CreateFwdTableDefinition(), CreateReplyTableDefinition(), Six
 */
Finder::Finder(LogFile *aLogFile, Eso *eso)
:Debugable(aLogFile), SixOffspring(eso)
{
  TableFieldQueue *definition;
DBG;
  WriteString(llDebug, "Finder::Constructor");
    // initialize forwardTable 
  definition = CreateFwdTableDefinition();
  if(definition == NULL)
    WriteString(llError, "Finder::Finder(): cannot create fwd table definition");
  fwdTable = new Table(six->GetPath(FINDER_FWD_TABLE_NAME), definition);
  if(fwdTable == NULL)
    WriteString(llError, "Finder::Finder(): cannot create fwd table");

  if(definition != NULL)
    DELETE(definition);
DBG;
  // initialize replyTable 
  definition = CreateReplyTableDefinition();
  if(definition == NULL)
    WriteString(llError, "Finder::Finder(): cannot create reply table definition");
  replyTable = new Table(six->GetPath(FINDER_REPLY_TABLE_NAME), definition);
  if(replyTable == NULL)
    WriteString(llError, "Finder::Finder(): cannot create reply table");

  if(definition != NULL)
    DELETE(definition);
DBG;
}

/**
 * Finder destructor.
 *
 * Destroyes tables
 *
 * @author  Marek
 * @see
 */
Finder::~Finder()
{
  if(fwdTable)
    DELETE(fwdTable);
  if(replyTable)
    DELETE(replyTable);
  if(logFile)
    DELETE(logFile);
}

/**
 * Creates definition of Finder fwd table.
 *
 * Creates definition for fwd Table by putting all needed TableFields in it.
 * Fwd table includes reply-to info [Reply-To-AC and Reply-To-TID] of requestor, TID
 * of this Eso and time, when this information expires.
 *
 * @param   void 
 * @return  TableFieldQueue *
 * @author  Marek
 * @see     TableFieldQueue, TableField
 */
TableFieldQueue *Finder::CreateFwdTableDefinition()
{
  TableFieldQueue *definition;
  TableField *field;

  definition = new TableFieldQueue;
  if(definition == NULL)
    return NULL;
    
    //TID 
  field = new IDField(FI_TID);
  if(field == NULL){
    DELETE(definition);
    return NULL;
  }
  definition->Insert(field);


   //ReplyToAC
 field = new BytesField(FI_RAC);
  if(field == NULL){
    DELETE(definition);
    return NULL;
  }
  definition->Insert(field);


   //ReplyToTID
 field = new IDField(FI_RID);
  if(field == NULL){
    DELETE(definition);
    return NULL;
  }
  definition->Insert(field);


    //Expiration
  field = new TimeField(FI_EXP);
  if(field == NULL)
  {
    DELETE(definition);
    return NULL;
  }
  definition->Insert(field);

  return definition;
}

/**
 * Creates definition of Finder reply table.
 *
 * Creates definition for replyTable by putting all needed TableFields in it.
 * Reply table includes the FFID together with ID of transaction,
 * in which it was sent to requestor and time, in which this info can
 * be deleted.
 *
 * @param   void
 * @return  TableFieldQueue *
 * @author  Marek
 * @see     TableFieldQueue, TableField
 */
TableFieldQueue *Finder::CreateReplyTableDefinition()
{
  TableFieldQueue *definition;
  TableField *field;

  definition = new TableFieldQueue;
  if(definition == NULL)
    return NULL;
    
    //TID 
  field = new IDField(FI_TID);
  if(field == NULL){
    DELETE(definition);
    return NULL;
  }
  definition->Insert(field);

   //FFID
 field = new BytesField(FI_FFID);
  if(field == NULL){
    DELETE(definition);
    return NULL;
  }
  definition->Insert(field);


    //Expiration
  field = new TimeField(FI_EXP);
  if(field == NULL)
  {
    DELETE(definition);
    return NULL;
  }
  definition->Insert(field);

  return definition;
}
 
/**
 * Saves info about where to forward the reply.
 *
 * Gets Reply-to-AC and Reply-to-TID fields from inMsg
 * and saves them together with aTID and default search Time 
 * multiplied by depth of search.. 
 *
 * @param   aTID transaction ID
 * @param   inMsg message storing the information
 * @param   depth depth of search
 * @return  error code
 * @author  Marek
 * @see
 */
Err Finder::SaveForwardInfo(TID aTID, GMessage *inMsg, 
                            const Int depth)
{
  Record *r;
  MsgField *msgFld;
  TID tid;
  RelTime *relTime;
  ESTime *time;
  Err result;

  if (fwdTable == NULL)
    return KO;

    //create new record of fwdTable
  r = fwdTable->NewRecord();
  if (r == NULL) return KO;

  r->SetField(FI_TID, &aTID);  

  if (inMsg == NULL){
     DELETE(r);
     return KO;
  }

  msgFld = inMsg->GetAsMsgField(GM_REPLY_TO_AC);
  if(msgFld == NULL) {
    DELETE(r);
    return KO;
  }
  r->SetField(FI_RAC, msgFld);
  DELETE(msgFld);

  if (inMsg->GetAsID(GM_REPLY_TO_TID, &tid) != OK){
    DELETE(r);
    return KO;
  }
  r->SetField(FI_RID, &tid);


  relTime = new RelTime(CFG_FINDER_SEARCH_TIME_FOR_1_HOP* depth);
  time = new ESTime(*relTime);
  r->SetField(FI_EXP, time);
  DELETE(time);
  DELETE(relTime);

    // insert new record into fwdTable
  if(fwdTable->Open() == KO)
  {
    WriteString(llError, "Finder::SaveForwardInfo(): cannot open table");
    DELETE(r);
    return KO;
  }
  result = fwdTable->InsertRecord(r);
  DELETE(r);
  fwdTable->Close();

  return result;
}

/**
 * Saves info about what was forwarded.
 *
 * Gets the FFID from header and saves it together with
 * ID of transaction and default expiration time multiplied by depth of search.
 *
 * @param   aTID transaction ID
 * @param   aHeader a GMessage storing the header
 * @param   depth of search
 * @return  error code
 * @author  Marek
 * @see     
 */
Err Finder::SaveReplyInfo(TID aTID, GMessage *aHeader, 
                          const Int depth)
{
  Record *r;
  MsgField * ffid;  
  RelTime *relTime;
  ESTime *time;
  Err result;

  if (replyTable == NULL)
    return KO;


    //create new record of replyTable
  r = replyTable->NewRecord();
  if (r == NULL) return KO;

  r->SetField(FI_TID, &aTID);  

  if ((ffid  = aHeader->GetAsMsgField(GM_FFID)) == NULL) {
   DELETE(r);
   return KO;  
  }
  r->SetField(FI_FFID, ffid);

  relTime = new RelTime(CFG_FINDER_SEARCH_TIME_FOR_1_HOP * depth);
  time = new ESTime(*relTime);
  r->SetField(FI_EXP, time);
  DELETE(time);
  DELETE(relTime);
 
   // insert new record into replyTable
  if(replyTable->Open() == KO)
  {
    WriteString(llError, "Finder::SaveReplyInfo(): cannot open table");
    DELETE(r);
    return KO;
  }
  result = replyTable->InsertRecord(r);
  DELETE(r);
  replyTable->Close();

  return result;
}
   
/**
 * Gets info about where to forward.
 *
 * According to TID gets the Reply-To-AC and Reply-To-TID.
 *
 * @param   aTID transaction ID
 * @param   anAC AC got from fwd table
 * @param   replyToTID Reply-To-TID got from fwd Table 
 * @return  error code
 * @author  Marek
 * @see     
 */
Err Finder::GetForwardInfo(TID aTID, GMessage **anAC, TID *replyToTID)
{
  Record *r;

  if (fwdTable == NULL)
    return KO;

   // try to find the record in the table
  if(fwdTable->Open() == KO){
    WriteString(llError, "Finder::GetForwardInfo(): cannot open table");
    return KO;
  }
  r = fwdTable->FindFirst(FI_TID, &aTID);
  fwdTable->Close();

  if (r == NULL) return KO;
  
  *anAC = ((BytesField*)r->GetField(FI_RAC))->GetAsGMessage();

  *replyToTID = ((IDField*)r->GetField(FI_RID))->GetIDValue();

  return OK;  
}
    
/**
 * Returns 0/1 if the record wasn't/was already sent.
 *
 * Looks in the fwd table for the tuple [aTID, aHeader]. 
 * Returns 1/0 if found/found not.
 *
 * @param   aTID transaction ID
 * @param   aHeader
 * @return  boolean value
 * @author  Marek
 * @see     
 */
Int Finder::AlreadySent(TID aTID, GMessage *aHeader)
{
  Record *r;
  MsgField * ffid;  
  Int b;

  b = 0;  
  
  if (replyTable == NULL)
    return 0;

  if ((ffid = aHeader->GetAsMsgField(GM_FFID)) == NULL) 
    return 0;  
  

   // try to find the record in the table
  if(replyTable->Open() == KO){
    WriteString(llError, "Finder::GetReplyInfo(): cannot open table");
    return 0;
  }
  r = replyTable->FindFirst(FI_TID, &aTID);
  replyTable->Close();

  
  while (r != NULL) {
    
    if (r->GetField(FI_FFID)->EqualTo((void *)ffid)){
      b = 1;
      break;
    }
    DELETE(r);
    r = replyTable->FindNext();
  }
    
  return b;
}

/**
 * Deletes forwarding information according to TID.
 *
 * Finds the record in the fwd table and deletes it.
 *
 *
 * @param   aTID transaction ID
 * @return  error code
 * @author  Marek
 * @see     
 */
Err Finder::DeleteForwardInfo(TID aTID)
{
  Record *r;
 
  if (fwdTable == NULL)
    return KO;

     // try to find the record in the table
  if(fwdTable->Open() == KO){
    WriteString(llError, "Finder::GetReplyInfo(): cannot open table");
    return KO;
  }
  
  r = fwdTable->FindFirst(FI_TID, &aTID);
  while (r != NULL) {
   fwdTable->DeleteRecord();
   DELETE(r);
   r = fwdTable->FindNext();

  }
 
  fwdTable->Close();

  return OK;
}

/**
 * Deletes records in tables according to expiration.
 *
 * <pre>
 * Looks in both tables for records with expiration < currentTime
 * and deletes them.
 * </pre>
 *
 * @param   currentTime
 * @return  error code  
 * @author  Marek
 * @see     
 */
Err Finder::DeleteRecordsByExpiration(ESTime currentTime)
{
  Record *r;
 
  if (fwdTable == NULL)
    return KO;

     // try to find the record in the table
  if(fwdTable->Open() == KO){
    WriteString(llError, "Finder::GetReplyInfo(): cannot open table");
    return KO;
  }
  r = fwdTable->First();
  while (r != NULL) {
   if (((TimeField*) r->GetField(FI_EXP))->GetESTimeValue() < currentTime)
   {
     fwdTable->DeleteRecord();
   }
   DELETE(r);
   r = fwdTable->Next();
  }
  fwdTable->Close();


 if (replyTable == NULL)
    return KO;

     // try to find the record in the table
  if(replyTable->Open() == KO){
    WriteString(llError, "Finder::GetReplyInfo(): cannot open table");
    return KO;
  }
  r = replyTable->First();
  while (r != NULL) {
   if (((TimeField*) r->GetField(FI_EXP))->GetESTimeValue() < currentTime)
   {
     replyTable->DeleteRecord();
   }
   DELETE(r);
   r = replyTable->Next();
  }
  replyTable->Close();

  return OK;
}
