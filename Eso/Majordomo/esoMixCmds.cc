#include "./H/majordomo.h"
#include "./../../Common/GMessage/H/esoAC.h"

/**
 * Chooses serving method for Mix command.
 *
 * According to field Command (in message poped from outMsgQueue) choose handling
 * method.
 *
 *
 * @param   commandFld command from Mix
 * @param   dataFld data comming along with command
 * @return  error code
 * @author  Marek
 * @see     SixMajordomo::SwitchMixCommand()
 */
Err
Majordomo::SwitchMixCommand(MsgField *commandFld, MsgField *dataFld)
{
  Char *command;
  Err result;

    // call inherited SwitchMixCommand
  if((result = SixMajordomo::SwitchMixCommand(commandFld, dataFld)) == OK)
    return OK;
    
    // field command cannot be empty
  if(commandFld == NULL)
    return KO;

    // get command string
  command = commandFld->GetAsString();
  if(command == NULL)
    return KO; 
  DBG;
  WriteString(llDebug, "Majordomo::SwitchMixCommand(): command = %s", command);

    // call appropriate command handling method
    // commands from MIX
  if(strcmp(command, CMD_CERTIFICATES_RECEIVED) == 0)
    result = CommandCertificatesReceived(dataFld);
  else if(strcmp(command, TO_TIME_TO_SYNCHRONIZE) == 0)
    result = ((Eso *) six)->timeSynchronizer->TO_TimeToSynchronize();
  else if(strcmp(command,TO_RESPONSE_TO_SYNCHRONIZE) == 0)
    result = ((Eso *) six)->timeSynchronizer->TO_Response(dataFld);
  else if(strcmp(command, TO_ALLOC) == 0) 
    result = CommandTimeoutAllocation(dataFld);
  else if(strcmp(command, TO_DEL_FINDER_TBL) == 0)
    result = CommandDelFinderTbl(dataFld);
  else if(strcmp(command, TO_CHECK_AC_COUNT) == 0)
    result = CommandControllACCount(dataFld);
  else
  {
    WriteString(llWarning, __FILE__":%d: unknown Mix command", __LINE__);
    result = KO;
  }
  
  delete command;
  
  return result;
}



/**
 * Generates access certificate for this Eso.
 *
 * Overrides method of SixMajordomo to make it possible
 * for this Eso to generate its own access certificates. To generate
 * keys uses TCBWrapper.
 *
 *
 * @param   onion
 * @param   generatedAC out parameter returning
 * @return  error code
 * @author  Marek
 * @see     TCBWrapper::GenerateACKeys(), EsoAC
 */
Err
Majordomo::GenerateThisSixAC(MsgField *onion, GMessage **generatedAC)
{
  EsoAC *ac;
  TCBWrapper *tcb;
  GMessage *acPubKey;
  GMessage *tcbPubKey;
  ACID acid;
  Err result;
 
  DBG;
  tcb = ((Eso *) six)->tcbWrapper;
  
  if(tcb->GenerateACKeys(&acid, &acPubKey, &tcbPubKey) != OK){
    DBG;
    return KO;
  }
  DBG;
  tcbPubKey->DebugHeaders(logFile, "Generated TCB PubKey");

  if((ac = new EsoAC()) == NULL){
    delete acPubKey;
    delete tcbPubKey;
    return KO;
  }

  DBG;
  WriteString(llDebug, "Mjd:CommandOnionGenerated():ac generated");

  result = OK;
  if (ac->SetFields(ACTYPE_ESO, acid, onion, acPubKey, tcbPubKey) != OK)
    result = KO;

  delete acPubKey;
  delete tcbPubKey;

  *generatedAC = ac;

  return result;
}
 
/**
 * Handles generated access certificate.
 *
 * Saves given access certificates and publishes it
 * (through Mix) to Acs.
 *
 *
 * @param   generatedAC given access certificate
 * @return  error code
 * @author  Marek
 * @see     InsertAC(), CreatePublishCertificateMsg()
 */
Err
Majordomo::HandleGeneratedAC(GMessage *generatedAC)
{
    GMessage *mixMsg;

    DBG;
    if(InsertAC(ACTYPE_THIS_SIX, generatedAC) != OK)    
      WriteString(llWarning, "Mjd::Cannot save generated AC");
    DBG;
    string directory = ((Eso *) six)->configFile->GetValue(CFG_CERTIFICATES_DIRECTORY);
    DBG;
    string cert = ((Eso *) six)->configFile->GetValue(CFG_CERTIFICATE);
    DBG;
    
    WriteString(llDebug, "%s", (char *)(directory+"/"+cert).c_str());
    DBG;
   
       //for debugging only !!!!!!!!!!!
       generatedAC->StoreToFile((char *)(directory+"/"+cert).c_str());

       
    mixMsg = CreatePublishCertificateMsg(generatedAC);

      //put it into outMsgQueue
    if(mixMsg == NULL){
      WriteString(llError, "Majordomo::CommandControllACCount():"
                           "Cannot create mixMsg");
    }
    else
      outMsgQueue->Insert(mixMsg);
   DBG;
   return OK;
  }




/**
 * Processes CMD_CERTIFICATES_RECEIVED command.
 * 
 * Processes CMD_CERTIFICATES_RECEIVED command by putting received
 * certificates to appropriate ACManager.
 *
 *
 * @param   dataFld data comming along with command
 * @return  error code
 * @author  Marek
 * @see     InsertAC()
 */
Err
Majordomo::CommandCertificatesReceived(MsgField *dataFld)
{
  GMessage *inMsg;
  ArrayGMessage *allACs;
  MsgField *acMsgFld;
  EsoAC *ac;
  Int i;
  Char *acType;
 
DBG;
  if(dataFld == NULL)
    return KO;

DBG;
  if ((inMsg = dataFld->GetAsGMessage()) == NULL)
    return KO;

DBG;
  if((allACs = (ArrayGMessage *) inMsg->GetAsGMessage(GM_CERTIFICATES)) == NULL){
DBG;
    delete inMsg;
    return KO;
  }
 
DBG;
allACs->DebugHeaders(logFile, "Prisly certifikaty!!!!");
  delete inMsg;
  i = 1;
DBG;
  acMsgFld = allACs->GetField(GM_CERTIFICATE, i);
  while(acMsgFld != NULL){
    
DBG;
    if((ac = (EsoAC*) acMsgFld->GetAsGMessage()) != NULL){
      acType = ac->GetType();
DBG;
      InsertAC(acType, ac);

      delete ac;
      DELETE(acType);
    }

DBG;
    delete acMsgFld;
    i++;
DBG;
    acMsgFld = allACs->GetField(GM_CERTIFICATE, i);
  }

DBG;
  delete allACs;
  return OK;
}


