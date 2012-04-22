#include "./H/majordomo.h"


/**
 * Switches data_received command.
 *
 * Overrides and calls SixMajordomo::SwitchDataReceivedCommand and chooses a method 
 * to handle the command from the incomming message.
 *
 *
 * @param   command one of data_received commands, comming from the world
 * @param   dataFld data field comming along with command
 * @param   acid ID of this Eso access certificate which was used as delivery address
 * @return  error code
 * @author  Pavel
 * @see     SixMajordomo
 */
Err 
Majordomo::SwitchDataReceivedCommand(const Char     *command, 
                                         const MsgField *dataFld,
					 const ACID      acid)
{
  Err result = KO;
  
  if(strcmp(command, CE_REQ_FOR_STOR) == 0)
    result = DRCRequestForStorage(dataFld);
  else if(strcmp(command, CE_DATA_TO_STOR) == 0)
    result = DRCDataToStore(dataFld, acid);
  else if(strcmp(command, CMD_REQ_FOR_HEADERS) == 0)
    result = DRCRequestForHeaders(dataFld, acid);
  else if(strcmp(command, CMD_REQ_FOR_FILE) == 0)
    result = DRCRequestForFile(dataFld, acid);
  else if(strcmp(command, CMD_ANSWER_HEADER) == 0)
    result = DRCAnswerHeader(dataFld);
  else if(strcmp(command, CMD_ANSWER_FILE) == 0)
    result = DRCAnswerFile(dataFld);
  else if(strcmp(command, CMD_SYNCHRONIZER_REQUEST) == 0)
    result = ((Eso *) six)->timeSynchronizer->TS_Request((MsgField*)dataFld);
  else if(strcmp(command, CMD_SYNCHRONIZER_RESPONSE) == 0)
    result = ((Eso *) six)->timeSynchronizer->TS_Response((MsgField*)dataFld);
  else if((result = SixMajordomo::SwitchDataReceivedCommand(command, dataFld, acid)) == OK)
    result = OK;
  else
  {
    WriteString(llWarning, __FILE__":%d:Unknown data_received command(%s).", __LINE__, command);
    WriteString(llWarning, __FILE__":%d:Recognizing only: %s, %s, %s, %s, %s, %s.", __LINE__,
                   CE_REQ_FOR_STOR, CE_DATA_TO_STOR, CMD_REQ_FOR_HEADERS, 
		   CMD_REQ_FOR_FILE, CMD_ANSWER_HEADER, CMD_ANSWER_FILE);
    result = KO;
  }

  return result;
}

