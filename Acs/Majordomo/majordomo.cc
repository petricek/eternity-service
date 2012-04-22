#include "H/majordomo.h"
#include "../../Common/H/labels.h"
#include "../../Common/H/commands.h"

/**
 * Acs Majordomo constructor.
 *
 * Just initializes the parent classes as it has nothing else to do.
 *
 * @author  Venca
 * @param   parent  Pointer to the parent Acs.
 * @param   logFile Log file where to write messages
 * @see     Acs
 */
Majordomo::Majordomo(Acs * parent, LogFile * majordomoLogFile)
  : Runable(majordomoLogFile), AcsOffspring(parent)
{

}

/**
 * Acs majordomo Run() method.
 *
 * Gets message with the following structure:
 * <pre>
 * Command: data_received
 * Parameters:
 *   Data:
 *     Command: get_certificates
 *     Request-ID:                        ... Mix's request ID
 *     Parameters:
 *       Onion:
 *       Certificate-Type:
 *       Count:
 *       Request-ID:
 * </pre>
 *
 * or
 *
 * <pre>
 * Command: data_received
 * Parameters:
 *   Data:
 *     Command: publish_certificates
 *     Parameters:
 *       Certificate-1:
 *       Certificate-2:
 *       ... 
 *       Certificate-n:
 * </pre>
 *
 * And sends back a reply:
 * <pre>
 * Command: send_data
 * Parameters:
 *   Add-Hops:
 *   Onion:
 *   Data:
 *     Command: certificates_received
 *     Request-ID:                        ... Mix's request ID
 *     Parameters:
 *       Request-ID:
 *       Certificates:
 *         Certificae-1:
 *         Certificae-2:
 *            ...
 *         Certificae-n:
 * </pre>
 *
 * @return  (void *)NULL - no result is returned
 * @author  Venca
 * @see     Majordomo, GetCertificates(), PublishCertificates()
 */
void * Majordomo::Run(void *)
{

  GMessage *message = NULL ;
  GMessage *data = NULL, *parameters = NULL; 
  for ( ; ; )
  {
    DBG;

      // Get a message
    acs->receiverToMajordomo->GetMsg(&message);
    message->DebugHeaders(logFile, "message from mix");
    parameters = message->GetAsGMessage(GM_PARAMETERS);
    parameters->DebugHeaders(logFile, "parameters from mix");
    char * command = message->GetAsString(GM_COMMAND);
    if(!strcmp(command,CMD_DATA_RECEIVED))
    {
      data = parameters->GetAsGMessage(GM_DATA);
      data->DebugHeaders(logFile, "six-six parameters");
      DELETE(command);
    DBG;
      command = data->GetAsString(GM_COMMAND);
    DBG;
      GMessage * params = data->GetAsGMessage(GM_PARAMETERS);
      MsgField * mixRequestID = data->GetAsMsgField(GM_REQUEST_ID);
    DBG;
      params->DebugHeaders(logFile, "six-six parameters");
      DBG;
      DELETE(message);
      DBG;
	// If it is a request for certificates
      if(!strncmp(command, CMD_GET_CERTIFICATES, strlen(CMD_GET_CERTIFICATES)))
      {
	MsgField * onion = params->GetAsMsgField(GM_ONION);
	DBG;
	    // Get from parameters field what certificates to send
	//parameters = params->GetAsGMessage(GM_PARAMETERS);
	//DELETE(data);
      DBG;
	char * type = params->GetAsString(GM_CERTIFICATE_TYPE);
      DBG;
	Size *count = (Size *) params->GetAsBytes(GM_COUNT);
      DBG;
	MsgField * sixRequestID = params->GetAsMsgField(GM_REQUEST_ID);
      DBG;

	GMessage * certs = NULL;
	  // Get the certificates
	certs = acs->certificateRepository->GetCertificates(*count, string(type));
	DELETE(count);
      DBG;

	  // Construct a reply
	params->Clear();
        DBG;
	params->SetAsMsgField(GM_REQUEST_ID, sixRequestID);
        DBG;
	params->SetAsGMessage(GM_CERTIFICATES, certs);
        DBG;

	data = new GMessage();
        DBG;
	data->Clear();
        DBG;
	data->SetAsString(GM_COMMAND, CMD_CERTIFICATES_RECEIVED);
        DBG;
	data->SetAsMsgField(GM_REQUEST_ID, mixRequestID);
        DBG;
	data->SetAsGMessage(GM_PARAMETERS, params);
        DBG;
	DELETE(params);
        DBG;

	parameters->Clear();
        DBG;
	Size hops = 0;
	parameters->SetAsBytes(GM_ADD_HOPS, &hops, sizeof(Size));
        DBG;
	parameters->SetAsMsgField(GM_ONION, onion);
        DBG;
	DELETE(onion);
        DBG;
	parameters->SetAsGMessage(GM_DATA,data);
        DBG;
	DELETE(data);
        DBG;


        GMessage *newMessage = new GMessage();
        DBG;
	newMessage->SetAsString(GM_COMMAND, CMD_SEND_DATA);
        DBG;
	newMessage->SetAsGMessage(GM_PARAMETERS, parameters);
        DBG;
	DELETE(parameters);

	GMessage * messageForSender = new GMessage();
        DBG;

	messageForSender->SetAsGMessage(GM_CHUNK, newMessage);
	DELETE(newMessage);
        DBG;
	GMessage *recipient = new GMessage();
	  // got port number where receiver will listen to the incoming messages
	string tmp;
        DBG;
	tmp = acs->configFile->GetValue(CFG_MIX_ADDRESS);
	recipient->SetAsString(GM_ADDRESS, (char *) tmp.c_str());
	WriteString(llDebug, __FILE__ ":%d:Got mix address %s.", __LINE__,
		     tmp.c_str());
	u_short port = atoi(acs->configFile->GetValue(CFG_MIX_PORT).c_str());
	recipient->SetAsBytes(GM_PORT, &port, sizeof(u_short));
	WriteString(llDebug, __FILE__ ":%d:What is in recipient?", __LINE__);
	recipient->DebugHeaders(logFile);
	WriteString(llDebug, __FILE__ ":%d:Got mix port number %s.", __LINE__,
		    tmp.c_str());

	messageForSender->SetAsGMessage("Recipient-1", recipient);
	DELETE(recipient);

	WriteString(llDebug, __FILE__ ":%d:What is in messageForSender?", __LINE__);
	messageForSender->DebugHeaders(logFile);
	acs->majordomoToSender->Append(messageForSender);
	DBG;
	continue;

      }
	// If it is a request for certificates
      else if(!strncmp(command, CMD_PUBLISH_CERTIFICATES, strlen(CMD_PUBLISH_CERTIFICATES)))
      {
        DBG;
	acs->certificateRepository->PutCertificates(params);
        DBG;
        DELETE(params);
	DBG;
      }
	// Other commands are ignored - just a warning
      else
      {
	WriteString(llWarning, "Unknown command:%s",command);
      DBG;
      }
    }
  }
}
