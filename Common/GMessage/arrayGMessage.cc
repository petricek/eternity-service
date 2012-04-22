#include "./H/arrayGMessage.h"

/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
ArrayGMessage::ArrayGMessage(LogFile *aLogFile)
: GMessage(aLogFile)
{
}

      
/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
Char *
ArrayGMessage::FieldName(Char *prefix, Int i)
{
  Char *name = new Char[strlen(prefix) + 6];
  sprintf(name, "%s-%d", prefix, i);
  return name;
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
Char *
ArrayGMessage::CountName(Char *prefix)
{
  Char *name = new Char[strlen(prefix) + 8];
  sprintf(name, "%s-Count", prefix);
  return name;
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
Err 
ArrayGMessage::SetField(Char *prefix, Int i, MsgField *contents)
{
  Char *tmp;
  Err result = SetAsMsgField(tmp = FieldName(prefix, i), contents);
  DELETE(tmp);

  return result;
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
MsgField *
ArrayGMessage::GetField(Char *prefix, Int i)
{
  Char *tmp = FieldName(prefix, i);
  MsgField *aMsgField = GetAsMsgField(tmp);
  DELETE(tmp);

  return aMsgField;
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
Err 
ArrayGMessage::AddField(Char *prefix, MsgField *contents)
{
  Int count;

  if(contents == NULL)
    return KO;

  Char *countName = CountName(prefix);

    // get current keyword count
  if(GetAsInt(countName, &count) != OK)
    count = 0;

    // increase keyword count
  count++;

    // create new field name and add new keyword
  
  if(SetField(prefix, count, contents) != OK)
    return KO;

    // write new keyword count
  if(SetAsInt(countName, count) != OK)
    return KO;

  DELETE(countName);

  return OK;
}



/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
Err 
ArrayGMessage::LoadFromLines(Char *prefix, Char *str)
{
  MsgField *msgFld;


  Int strLen = strlen(str);
  Int i = 0;
  Int beginning;
  
  while(i < strLen){

     beginning = i;
     while((str[i] != ' ') && (i < strLen))
       i++;

     msgFld = new BytesMsgField();
     msgFld->SetAsBytes(str + beginning, i - beginning);

     AddField(prefix, msgFld);   
     DELETE(msgFld);
    
     while((str[i] == ' ') && (i <strLen))
       i++;

  }
  return OK;
}


/**
 * Kratky_komentar_s_teckou_na_konci.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  jmeno
 * @see     co_se_toho_tyka
 */
GMessage *
ArrayGMessage::GetAsGMessage(Char *prefix, Int i)
{
  GMessage *result;  
  Char *tmp;
  result = GMessage::GetAsGMessage(tmp = FieldName(prefix,i));
  DELETE(tmp);

  return result;
}

