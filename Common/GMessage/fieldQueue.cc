#include "./H/fieldQueue.h"

/**
 * FieldQueue onstructor.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
FieldQueue::FieldQueue(LogFile *aLogFile)
: Queue(), Debugable(aLogFile)
{}


/**
 * Copy constructor.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
FieldQueue::FieldQueue(FieldQueue &aQueue, LogFile *aLogFile)
: Queue(), Debugable(aLogFile)
{
  Field *aField;

  aField = aQueue.First();
  while(aField != NULL) {
    Insert(aField->CreateCopy());
    aField = aQueue.Next();
  }
  
  if(aLogFile == NULL)
    SetLogFile(aQueue.logFile);
}


/**
 * Destructor - deletes all Fields stored in the queue.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
FieldQueue::~FieldQueue()
{
  DiscardAll();
}


/**
 * Returns index of a field in this queue starting from 1..
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Int FieldQueue::GetIndexOf(Char *aFieldName)
{
  Field *aField;
  Int index;

  index = 1;
  aField = First();
  while(aField != NULL)
  {
    if(aField->NameEqualTo(aFieldName)){
      break;
    }
    index += 1;
    aField = Next();
  }

  return (aField != NULL) ? index : 0;
}


/**
 * Returns a field of name "aFieldName".
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Field *FieldQueue::GetField(Char *aFieldName)
{
  Field* aField;

  aField = First();
  while(aField != NULL) {
    if(aField->NameEqualTo(aFieldName)){
      break;
    }
    aField = Next();
  }

  return aField;
}


/**
 * Returns a field of index "anIndex".
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Field *FieldQueue::GetFieldByIndex(Int anIndex)
{
  Field *aField;
  Int index;

  index = 1;
  aField = First();
  while(aField != NULL)
  {
    if(index == anIndex)
      break;
    index++;
    aField = Next();
  }

  return aField;
}


/**
 * Counts the SUM of Fields sizes.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Size FieldQueue::SizeOf()
{
  Size len;
  Field* aField;


  len = 0;
  aField = First();
  while(aField != NULL) {
    len += aField->SizeOf();
    aField = Next();
  }

  return len;
}


/**
 * Inserts a field into the queue.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err FieldQueue::Insert(Field *aField)
{
  if(aField != NULL)
    aField->SetLogFile(logFile);

  return Queue::Insert(aField);
}


/**
 * Deletes all Fields stored in the queue.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Void FieldQueue::DiscardAll()
{
  Field *aField;

    // removes objects pointed to by items of a queue
  aField = First();
  while(aField != NULL) {
    DELETE(aField);
    aField = Next();
  }
    // removes the real items of a queue
  DeleteAll();
}


/**
 * Returns pointer to the first field.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Field *FieldQueue::First()
{
  return (Field*)(Queue::First());
}


/**
 * Returns pointer to a field which is next to the current one.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Field *FieldQueue::Next()
{
  return (Field*)(Queue::Next());
}


/**
 * Copy constructor.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
TableFieldQueue::TableFieldQueue(LogFile *aLogFile)
:FieldQueue(aLogFile)
{
}


/**
 * Returns 1 if all TableFields are fixed, 0 otherwise.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Int TableFieldQueue::FieldsFixed()
{
  TableField *aField;

  aField = First();
  while(aField != NULL) {
    if(aField->IsFixed() == 0)
      return 0;
    aField = Next();
  }

  return 1;
}


/**
 * Returns 1 if the value of the field at index "anIndex" equals.
 * to "aFieldValue". 0 otherwise..
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Int TableFieldQueue::FieldByIndexEqualTo(Int anIndex, Void *aFieldValue)
{
  TableField *aField;

  aField = (TableField*)GetFieldByIndex(anIndex);
  if(aField != NULL)
    return aField->EqualTo(aFieldValue);
  else
    return 0;
}


/**
 * Finds a field of name "aFieldName" (if such is not found returns.
 * KO) and sets its value to "aFieldValue".
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
Err TableFieldQueue::SetField(Char *aFieldName, Void *aFieldValue)
{
  TableField *aField;

  aField = (TableField*)GetField(aFieldName);
  if(aField == NULL)
    return KO;
  else {
    aField->SetValue(aFieldValue);
    return OK;
  }
}

/**
 * Returns pointer to the first field.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
TableField *TableFieldQueue::First()
{
  return (TableField*)(FieldQueue::First());
}


/**
 * Returns pointer to the next field.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Pavel
 * @see     co_se_toho_tyka
 */
TableField *TableFieldQueue::Next()
{
  return (TableField*)(FieldQueue::Next());
}


