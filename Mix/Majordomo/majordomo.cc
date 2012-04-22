  // has to be the FIRST include !
#include "./../../Common/H/common.h"

#include "./../../Common/H/labels.h"
#include "./../../Common/H/commands.h"
#include "./../../Common/Registerable/H/registerable.h"

#include "./H/majordomo.h"


/**
 * Majordomo constructor.
 *
 * Saves pointers to object passed as parameters.
 *
 * @param   parent Pointer to the main object Mix.
 * @param   majordomoLogFile Log file.
 * @author  Venca
 * @see     ~Majordomo()
 */
Majordomo::Majordomo(Mix * parent, LogFile * majordomoLogFile)
  : Runable(majordomoLogFile), MixOffspring(parent)
{

}

/**
 * Decides what to do with messages for which Mix is a final point.
 *
 * When a message ends in Mix, it can have different recipients - Six
 * or Mix. We use GM_X_DESTINATION and GM_ORIGIN to decide which type
 * of the message it is. Message can go from Mix to Mix, from Six to
 * Mix and from Mix to Six. According to this, Majordomo passes chunks
 * along the way or gives them to the object that is the real
 * recipient in Mix.
 *
 * @author  Venca, Pechy
 * @see     GM_X_DESTINATION, GM_ORIGIN
 */
void * Majordomo::Run(void *)
{

  MaskAllSignals();

  char /* *debug ,*/ *destination, *origin;
  GMessage *message;

  WriteString(llDebug, __FILE__ ":%d:Entering Majordomo::Run()", __LINE__);

  for ( ; ; ) {

    mix->linkerChopperToMajordomo->GetMsg(&message);

    WriteString(llDebug, __FILE__ ":%d:Majordomo got a message.", __LINE__);

      // also Debug and Protocol-Version should be here
    destination = message->GetAsString(GM_X_DESTINATION);
    origin = message->GetAsString(GM_ORIGIN);
    //debug = string(message->GetAsString(GM_DEBUG));

    WriteString(llDebug, __FILE__ ":%d:GM_X_DESTINATION=%s, GM_ORIGIN=%s.", 
                __LINE__, destination, origin);

      // X-Origin is "mix", X-Destination is LBL_MIX_LOCAL
      // ... from network to mix - must be a reply to something
      // so we pass it the object that expects this reply
    if (!strcmp(destination, LBL_MIX_LOCAL) && !strcmp(origin, "mix")) { 

      DELETE(destination); DELETE(origin);

      WriteString(llDebug, __FILE__ ":%d:Msg from mix to mix_local.", 
                  __LINE__);

      message->DebugHeaders(logFile,"message from linker");
      GMessage *messageData = message->GetAsGMessage(GM_DATA);
      messageData->DebugHeaders(logFile, "messageData");

      ID * requestID = (ID *)messageData->GetAsBytes(GM_REQUEST_ID,NULL);

      RegistratorRecord *registratorRec = mix->messageIDRepository->GetRegistratorRecord(*requestID);

      if(registratorRec->registrator != NULL)
      {
        // Passes data as a GMessage to object that awaits it 
	registratorRec->registrator->HereYouAre(messageData);
	continue;
      }

      if(!registratorRec->destination.empty())
      {
        // Send it to apropriate SIX

	 // we send DATA, PORT, IP to sender 
	string destination = registratorRec->destination;
	string sixPort = string("");
	sixPort = mix->configFile->GetValue(sixPort + CFG_SIX_
					    + destination + CFG__PORT);
	string sixAddress = string("");
	sixAddress = mix->configFile->GetValue(sixAddress + CFG_SIX_
					       + destination + CFG__ADDRESS);
//	DELETE(origin);

	GMessage *chunk = new GMessage();

	GMessage *rcpt = new GMessage();
	rcpt->SetAsString(GM_ADDRESS, (char *) sixAddress.c_str());
	u_short port = atoi(sixPort.c_str());
	rcpt->SetAsBytes(GM_PORT, (char *) (&port), sizeof(u_short));

	  // create a gMessage for upper Sender
	GMessage *newMessage = new GMessage();
	newMessage->SetAsGMessage(GM_CHUNK, messageData);
	messageData->DebugHeaders(logFile,"messageData");
	DELETE(chunk);
	newMessage->SetAsGMessage(GM_RECIPIENT_1, rcpt);
	rcpt->DebugHeaders(logFile,"rcpt");
	DELETE(rcpt);
	//newMessage->SetAsString(GM_DEBUG, (char *) debug.c_str());

	newMessage->DebugHeaders(logFile,"newMessage");
	mix->majordomoToSender->Append(newMessage);
	continue;
      }
      else
      {
        //registrator and even destination are empty - that should not happen
      }
    }

      // X-Origin is "mix", X-Destination is ,,some Six'' 
      // ... it is meant for any Six (ie. client) and it came from 
      // a remote mix
    if (!strcmp(origin, "mix")) {

      WriteString(llDebug, __FILE__ ":%d:Msg from Mix to Six.", __LINE__);

       // Then we send DATA, PORT, IP to sender 
       // command data_received is added to data 
      string sixPort = string("");
      sixPort = mix->configFile->GetValue(sixPort + CFG_SIX_
                                          + destination + CFG__PORT);
      string sixAddress = string("");
      sixAddress = mix->configFile->GetValue(sixAddress + CFG_SIX_
                                             + destination + CFG__ADDRESS);
      DELETE(destination); DELETE(origin);

      GMessage *chunk = new GMessage();

      GMessage *rcpt = new GMessage();
      rcpt->SetAsString(GM_ADDRESS, (char *) sixAddress.c_str());
      u_short port = atoi(sixPort.c_str());
      rcpt->SetAsBytes(GM_PORT, (char *) (&port), sizeof(u_short));

      GMessage *parameters = new GMessage();
      MsgField *tmpData = message->GetAsMsgField(GM_DATA);
      DELETE(message);
      parameters->SetAsMsgField(GM_DATA, tmpData);
      DELETE(tmpData);

      chunk->SetAsString(GM_COMMAND, CMD_DATA_RECEIVED);
      chunk->SetAsGMessage(GM_PARAMETERS, parameters);
      DELETE(parameters);

        // create a gMessage for upper Sender
      GMessage *newMessage = new GMessage();
      newMessage->SetAsGMessage(GM_CHUNK, chunk);
      DELETE(chunk);
      newMessage->SetAsGMessage(GM_RECIPIENT_1, rcpt);
      DELETE(rcpt);
      //newMessage->SetAsString(GM_DEBUG, (char *) debug.c_str());

      mix->majordomoToSender->Append(newMessage);

      continue;
    }

      // X-Origin is ,,any Six'', X-Destination is LBL_MIX_LOCAL
      // ... a request from Six (ie. a ,,command'')
    if (!strcmp(destination, LBL_MIX_LOCAL)) {

      DELETE(destination);

      WriteString(llDebug, __FILE__ ":%d:Message from Six to Mix.", 
                  __LINE__);

      char *command = message->GetAsString(GM_COMMAND);
      GMessage *parameters = message->GetAsGMessage(GM_PARAMETERS);
      
        // Send-Data: - is done by chopper

        // Generate-Onion:
      if (!strcmp(command, CMD_GENERATE_ONION)) {

        WriteString(llDebug, __FILE__ ":%d:Generate_onion request.", 
                  __LINE__);

	message->Clear();
	
          // create a reply message for Six
        parameters->SetAsString(GM_ORIGIN, (char *) origin);
        GMessage * onion = mix->msgCreator->CreateOnion(parameters);
	parameters->SetAsGMessage(GM_ONION, onion);
	DELETE(onion);
        WriteString(llDebug, __FILE__ ":%d:After GenerateOnion() in "
	            "Majordomo.", __LINE__);

        GMessage *chunk = new GMessage();
	chunk->SetAsString(GM_COMMAND, CMD_ONION_GENERATED);
	chunk->SetAsGMessage(GM_PARAMETERS, parameters);
	DELETE(parameters);

        message->SetAsGMessage(GM_CHUNK, chunk);
	DELETE(chunk);
        //message->SetAsString(GM_DEBUG, (char *) debug.c_str());
   
        GMessage *rcpt = new GMessage();

	string sixPort = string("");
	sixPort = mix->configFile->GetValue(sixPort + CFG_SIX_ + origin
	                                    + CFG__PORT);
	string sixAddress = string("");
	sixAddress = mix->configFile->GetValue(sixAddress + CFG_SIX_
	                                       + origin + CFG__ADDRESS);
        DELETE(origin);
        rcpt->SetAsString(GM_ADDRESS, (char *) sixAddress.c_str());
	u_short port = atoi(sixPort.c_str());
	rcpt->SetAsBytes(GM_PORT, (char *) (&port), sizeof(u_short));

        message->SetAsGMessage(GM_RECIPIENT_1, rcpt);
	DELETE(rcpt);
          // feed the Sender !!
	mix->majordomoToSender->Append(message);
      }

        // Get-Certificates:
      if(!strcmp(command, CMD_GET_CERTIFICATES)) {
        GetCertificates(message);
      }

        // Publish-Certificate:
      if(!strcmp(command, CMD_PUBLISH_CERTIFICATES)) {
        PublishCertificates(message);
      }

      continue;
    }

    WriteString(llError, __FILE__ ":%d:There's something rotten here...",
                __LINE__);
  }
}
