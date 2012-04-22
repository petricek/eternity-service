#include "./H/majordomo.h"

#define DEFAULT_DEPTH 0 
#define DEFAULT_WIDTH 0 

/**
 * Serves CMD_REQ_FOR_HEADERS message.
 *
 * <pre>processes the incoming request for headers
 *  - continues the search in the search tree
 *  - looks for the file on this server
 *  - sends the headers found on this server 
 *    back to the client
 * </pre>
 *
 * @param   dataField encrypted request comming with message
 * @param   acid ID of this Eso access certificate which was used to address thid message
 * @return  error code <pre>
 *  - KO in the case of bad structure of dataField
 *       or if fatal error comes (ex. NULL ptr to Finder)
 *  - OK otherwise     
 *       (that means even in case of mistake in any step
 *        of the algorithm [ex. cannot send the message
 *        back to client] - these situations are only
 *        written to the logFile)</pre>
 * @author  Marek
 * @see     TCBWrapper, Finder, ACManager
 */
Err Majordomo::DRCRequestForHeaders(const MsgField *dataField, const ACID acid)
{
    //incoming GMessage and its fields
  GMessage *inMsg;
  MsgField *aFAM;
  Int depth, width;
    //pointer to Finder and TCBWrapper
  Finder *fi;
  TCBWrapper *tcb;
    //headers of found files
  Queue *headers;
  GMessage *aHeader;
    //Eso-Service outMsg
  GMessage *outMsg;
    //reply to info (about client)
  TID replyToTID;   
  GMessage *replyToAC;
    //result of operations
  Err result;
    //message for Mix
  GMessage *mixMsg;
  GMessage *transportKey;
    
DBG;
    //initializing tests    
  if ((fi = (((Eso *)six)->finder)) == NULL)
    return KO;
  if ((tcb = (((Eso *)six)->tcbWrapper)) == NULL)
    return KO;

DBG;
    //parse dataField into GMessage
  if (dataField == NULL)
    return KO; 
  if ((inMsg = ((MsgField*)dataField)->GetAsGMessage()) == NULL) 
    return KO;

DBG;
    //get file attribute mask, width and depth of search
  result = OK;
  if (inMsg->GetAsInt(GM_SEARCH_DEPTH, &depth) != OK)
    depth = DEFAULT_DEPTH;
  if (inMsg->GetAsInt(GM_SEARCH_WIDTH, &width) != OK)
    width = DEFAULT_WIDTH;
  if ((aFAM = inMsg->GetAsMsgField(GM_FAM)) == NULL)
    result = KO;
  if ((transportKey = inMsg->GetAsGMessage(GM_TRANSPORT_KEY)) == NULL)
    result = KO;
  if (result != OK){
    delete inMsg;
    if (aFAM)
      delete aFAM;
    if (transportKey)
      DELETE(transportKey);
   WriteString(llWarning, "Majordomo::DRCRequestForHeaders():" 
                        "Bad structure of incoming Message");
   return KO;
  }

DBG;
    //process the search tree
  if (depth > 0){
       //ask other Esos
       //:save info about where to send the reply
DBG;
    if(fi->SaveForwardInfo(tID, inMsg, depth) != OK)
      WriteString(llError, "Majordomo::DRCRequestForHeaders():"
                           "cannot save fwd info into finder");

       //:create a new outMsg and fill it as RequestForHeaders
    if((outMsg = new GMessage()) != NULL){
      if(FillRequestForHeaders(outMsg, aFAM, transportKey, depth, width) == OK){        
DBG;
          //:if it's OK, send it to width Esos
        SendToEsos(CMD_REQ_FOR_HEADERS, outMsg, width);  
      }
      delete outMsg;
    }
  }

DBG;
    //search for headers on this Eso [!!!]
  headers = tcb->GetFileHeadersByFAM(acid, aFAM, transportKey);
  if (headers == NULL) {
    delete aFAM;
    DELETE(transportKey);
    delete inMsg;
    return KO;
  }

DBG;
    //process all headers
  aHeader = (GMessage*) headers->First();
  while (aHeader != NULL){
     aHeader->DebugHeaders(logFile, "n-th. header got from this Eso");
      //if the header wasn't already sent
    if(!(fi->AlreadySent(tID, aHeader))){
DBG;
 
      if(fi->SaveReplyInfo(tID, aHeader, depth) != OK)
        WriteString(llError, "Majordomo::DRCRequestForHeaders():"
                           "cannot save reply info into finder");

      result = OK;
        //get ReplyToTID from inMsg (doesn't have to be anywhere else)
      if(inMsg->GetAsID(GM_REPLY_TO_TID, &replyToTID))
        result = KO; 
         
        //get ReplyToAC from inMsg
      if((replyToAC = inMsg->GetAsGMessage(GM_REPLY_TO_AC)) == NULL)
        result = KO;

      if((outMsg = new GMessage()) == NULL)
        result = KO;

      if(result == OK){
        
          //fill outMsg as ReplyHeader
        if(FillReplyHeader(outMsg, replyToTID, aHeader) == OK){
         
            //create "send data" msg for Mix 
          mixMsg = CreateSendDataMsg(CMD_ANSWER_HEADER, outMsg, replyToAC); 

            //put it into outMsgQueue
          if(mixMsg == NULL){
           WriteString(llError, "Majordomo::DRCRequestForHeaders():"
                                "Cannot create mixMsg");
          }
          else
            outMsgQueue->Insert(mixMsg);
        }
      }
      if (replyToAC)
        delete replyToAC;
      if (outMsg)
        delete outMsg;
    }
    delete aHeader;          
    aHeader = (GMessage*) headers->Next();
  }
  delete headers;

  delete aFAM;
  DELETE(transportKey);
  delete inMsg;

  return OK;
}

/**
 * Serves CMD_REQ_FOR_FILE message.
 *
 * <pre>processes the incoming request for file
 *  - searches for file on this server
 *  - if not found, continues the search 
 *    in the search tree
 *  - if found, sends the file back to the client</pre>
 *
 *
 * @return  error code <pre>
 *  - KO in the case of bad structure of dataField
 *       or if fatal error comes (ex. NULL ptr to Finder)
 *  - OK otherwise     
 *       (that means even in case of mistake in any step
 *        of the algorithm [ex. cannot send the message
 *        back to client] - these situations are only
 *        written to the logFile)</pre>
 * @author  Marek
 * @see     TCBWrapper, Finder, ACManager
 */
Err 
Majordomo::DRCRequestForFile(const MsgField *dataField, const ACID acid)
{
    //incoming message and its fields
  GMessage *inMsg;
  MsgField *aFFID;
  Int depth, width;
  GMessage *transportKey;
    //finder and TCBWrapper
  Finder *fi;
  TCBWrapper *tcb;
    //reply to info
  TID replyToTID;
  GMessage *replyToAC;
    //message for MIX
  GMessage *mixMsg;
    //found file
  MsgField *file;
    //result of operations 
  Err result;
    //Eso-service outMessage 
  GMessage *outMsg;
DBG;
    //initializing tests    
  if ((fi = (((Eso *)six)->finder)) == NULL)
    return KO;
  if ((tcb = (((Eso *)six)->tcbWrapper)) == NULL)
    return KO;
DBG;
    //parse dataField into GMessage
  if (dataField == NULL)
    return KO;
  if ((inMsg = ((MsgField*)dataField)->GetAsGMessage()) == NULL) 
    return KO;
DBG;
    //get file identificator, transport key,
    //depth and width of search
  result = OK;
  if ((aFFID = inMsg->GetAsMsgField(GM_FFID)) == NULL)
    result = KO;
  if((transportKey = inMsg->GetAsGMessage(GM_TRANSPORT_KEY)) == NULL)
    result = KO;
  if((inMsg->GetAsInt(GM_SEARCH_DEPTH, &depth)) != OK)
    result = KO;
  if((inMsg->GetAsInt(GM_SEARCH_WIDTH, &width)) != OK)
    result = KO;
DBG;
  if (result != OK){
    delete inMsg;
    if (transportKey)
      delete transportKey;
  }
DBG;
    //search for file on this Eso [!!!]
  if ((file = tcb->GetFileByFinderFID(acid, aFFID, transportKey)) != NULL){
     
     //get ReplyTo info from inMsg
    result = OK;
    if((inMsg->GetAsID(GM_REPLY_TO_TID, &replyToTID)) != OK)
      result = KO; 
    if((replyToAC = inMsg->GetAsGMessage(GM_REPLY_TO_AC)) == NULL)
      result = KO;
DBG;
    if (result == OK){
DBG;
        //fill outMsg as ReplyFile
      if((outMsg = new GMessage()) != NULL) {
	if(FillReplyFile(outMsg, replyToTID, file) == OK){

  DBG;
	    //create "send data" msg for Mix 
	  mixMsg = CreateSendDataMsg(CMD_ANSWER_FILE, outMsg, replyToAC); 
	  DELETE(outMsg);

  DBG;
	    //put it into outMsgQueue
	  if(mixMsg == NULL){
	    WriteString(llError, "Majordomo::DRCRequestForHeaders():"
				  "Cannot create mixMsg");
	  }
	  else {
  DBG;
	    outMsgQueue->Insert(mixMsg);
	  }
	}	  
      }
      else
        WriteString(llError, __FILE__":%d:Cannot create outMsg.", __LINE__);
    }            
    delete file;
  }
  else{
      //search for file in the Service
    if (depth > 0){
  DBG; 
        //save info about where to send the reply
      if(fi->SaveForwardInfo(tID, inMsg, depth) != OK)
        WriteString(llError,"Majordomo::DRCRequestForFile():"
                            "cannot save fwd info");

     DBG;   
        //create a new outMsg
      if((outMsg = new GMessage()) != NULL){
        if(FillRequestForFile(outMsg, aFFID, transportKey,  depth, width)){
            //:if it's OK, send it to width Esos
          SendToEsos(CMD_REQ_FOR_FILE, outMsg, width);             ;        
        }
        delete outMsg;
      }
    }
  }
  delete transportKey;
  delete inMsg;
DBG; 
  return OK;
}

/**
 * Serves CMD_ANSWER_HEADER message.
 *
 * processes the incoming found header
 *  - forwards found header to client
 *
 *
 * @param   dataField encrypted request comming with message
 * @param   acid ID of this Eso access certificate which was used to address thid message
 * @return  error code
 * @author  Marek
 * @see     TCBWrapper, Finder
 */
Err 
Majordomo::DRCAnswerHeader(const MsgField *dataField)
{
    //pointer to Finder
  Finder *fi;
    //inMsg and its fields
  GMessage *inMsg;
  TID inReplyToTID;
  GMessage *fileHeader;
    //info about the client that asked for header
  GMessage *anAC;
  TID replyToTID;
    //message for Mix
  GMessage *mixMsg;
    //rtesult of the operations
  Err result;
  GMessage *outMsg;

   //initializing tests    
  if ((fi = (((Eso *)six)->finder)) == NULL)
    return KO;
  
    //parse dataField into GMessage
  if (dataField == NULL)
    return KO;
  if ((inMsg = ((MsgField*)dataField)->GetAsGMessage()) == NULL) 
    return KO;

    //get InReplyToTID from inMsg
  result = OK;
  if ((inMsg->GetAsID(GM_IN_REPLY_TO_TID, &inReplyToTID)) == KO)
    result = KO;
  if((fileHeader = inMsg->GetAsGMessage(GM_HEADER)) == NULL)
    result = KO;
  if (result != OK){
    delete inMsg;
    if (fileHeader)
      delete fileHeader;
    return KO;
  }

    //get client info from forwardTable according to InReplyToTID
  if((anAC = new GMessage()) == NULL){
    delete inMsg;
    delete fileHeader;
    return KO;
  }
  if ((fi->GetForwardInfo(inReplyToTID, &anAC, &replyToTID)) == OK){

      //if the header wasn't sent
    if(fi->AlreadySent(inReplyToTID, fileHeader)){
     

        //fill outMsg as ReplyHeader
      if((outMsg = new GMessage()) != NULL){
        if(FillReplyHeader(outMsg, replyToTID,fileHeader) == OK){
  
            //create "send data" msg for Mix 
          mixMsg = CreateSendDataMsg(CMD_ANSWER_HEADER, outMsg, anAC); 

            //put it into outMsgQueue
          if(mixMsg == NULL){
            WriteString(llError, "Majordomo::DRCAnswerHeader():"
                                "Cannot create mixMsg");
          }
          else
            outMsgQueue->Insert(mixMsg);
	}    
      }        
    }
  }         
  delete anAC;
  delete fileHeader;
  delete inMsg;
  return OK;
}  

/**
 * Serves CMD_ANSWER_FILE command.
 *
 * processes the incoming found file
 *  - forwards found file to client
 *
 *
 * @param   dataField encrypted request comming with message
 * @param   acid ID of this Eso access certificate which was used to address thid message
 * @return  error code
 * @author  Marek
 * @see     TCBWrapper, Finder
 */
Err 
Majordomo::DRCAnswerFile(const MsgField *dataField)
{
 //pointer to Finder
  Finder *fi;
    //inMsg and its fields
  GMessage *inMsg;
  TID inReplyToTID;
  MsgField *mfFile;
    //info about the client that asked for header
  GMessage *anAC;
  TID replyToTID;
    //message for Mix
  GMessage *mixMsg;
    //rtesult of the operations
  Err result;
  GMessage *outMsg;

   //initializing tests    
  if ((fi = (((Eso *)six)->finder)) == NULL)
    return KO;
  
    //parse dataField into GMessage
  if (dataField == NULL)
    return KO;
  if ((inMsg = ((MsgField*)dataField)->GetAsGMessage()) == NULL) 
    return KO;

    //get InReplyToTID from inMsg
  result = OK;
  if ((inMsg->GetAsID(GM_IN_REPLY_TO_TID, &inReplyToTID)) == KO)
    result = KO;
  if((mfFile = inMsg->GetAsMsgField(GM_FILE)) == NULL)
    result = KO;
  if (result != OK){
    delete inMsg;
    if (mfFile)
      delete mfFile;
    return KO;
  }

    //get client info from forwardTable according to InReplyToTID
  if((anAC = new GMessage()) == NULL){
    delete inMsg;
    delete mfFile;
    return KO;
  }
  if ((fi->GetForwardInfo(inReplyToTID, &anAC, &replyToTID)) == OK){

      if(fi->DeleteForwardInfo(inReplyToTID) != OK)
        WriteString(llError, "Majordomo::DRCAnswerFile():"
                             "cannot delete fwd info from finder");
      
        //fill outMsg as ReplyHeader
      if((outMsg = new GMessage()) != NULL){
        if(FillReplyFile(outMsg, replyToTID, mfFile) != OK){

            //create "send data" msg for Mix 
          mixMsg = CreateSendDataMsg(CMD_ANSWER_FILE, outMsg, anAC); 

            //put it into outMsgQueue
          if(mixMsg == NULL){
            WriteString(llError, "Majordomo::DRCAnswerFile():"
                                "Cannot create mixMsg");
          }
          else
            outMsgQueue->Insert(mixMsg);
	}    
      }       
  }
  delete anAC;
  delete mfFile;          
  delete inMsg;    
  return OK;
}

