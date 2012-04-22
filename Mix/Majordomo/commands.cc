  // has to be the *FIRST* include !
#include "./../../Common/H/common.h"

#include <stdlib.h>

#include "./../../Common/H/commands.h"
#include "./../../Common/H/labels.h"

#include "./H/majordomo.h"

#define DEFAULT_ONION_LEN 5
#define DEFAULT_REQUEST_ID 12321
#define CERTAINITY_COEFICIENT 2

/**
 * Publishes certificates to Acs servers.
 *
 * Every server using Mixes probably wants to advertise itself, so 
 * this method publishes certificates it gets.
 *
 * @param   request
 * <pre>
 * GM_PARAMETERS
 *   GM_CERTIFICATE_1
 *   GM_CERTIFICATE_2
 *   ...
 *   GM_CERTIFICATE_"n"
 * </pre>
 * @return  void
 * @author  Venca
 * @see     Acs, CertificateRepository
 */
void Majordomo::PublishCertificates(GMessage * request)
{
  WriteString(llDebug, __FILE__ ":%d:Entering PublishCertificate in Majordomo.",
              __LINE__);

    // STORE CERTS TO repository
  GMessage * certificates = request->GetAsGMessage(GM_PARAMETERS);
  certificates->DebugHeaders(logFile,"Certificates");
  mix->certificateRepository->PutCertificates(certificates);
  
    // PUBLISH TO ACS
    // Make onion to Acs
    // Get ACS certificate
  GMessage * acscert = mix->certificateRepository->GetCertificate(ACTYPE_ACS);
  acscert->DebugHeaders(logFile,"acscert");
    // extract mix certificate
  GMessage * mixcert = acscert->GetAsGMessage(GM_MIX_CERTIFICATE);
  mixcert->DebugHeaders(logFile,"mixcert");
  string name = acscert->GetAsString(GM_NAME);
  DELETE(acscert);
  GMessage * properties = new GMessage();
  properties->SetAsGMessage(GM_MIX_CERTIFICATE, mixcert);
  properties->SetAsString(GM_DESTINATION, (char *)name.c_str());
    // Make onion from mix certificate
  GMessage * onion = mix->msgCreator->CreateOnionFromMixCertificate(properties);

    // Set all fields in message for chopper
  Size hops = atoi(mix->configFile->
                     GetValue(CFG_ADD_HOPS).c_str());
  GMessage * parameters = new GMessage();
  parameters->SetAsBytes(GM_ADD_HOPS, (void *) &hops, sizeof (Size));
  parameters->SetAsGMessage(GM_DATA, request);
  parameters->SetAsGMessage(GM_ONION, onion);
  parameters->DebugHeaders(logFile, "parameters");
  DELETE(onion);
  GMessage * message = new GMessage();
  message->SetAsString(GM_COMMAND, CMD_SEND_DATA);
  message->SetAsGMessage(GM_PARAMETERS, parameters);
  message->DebugHeaders(logFile, "message");

  mix->receiverToChopper->Append(message);
}

/**
 * Returns certificates from Mix's repository (Certificate Repository).
 *
 * Mix returns all certificates from its repository. When there is not
 * enough certificates of requested type, Mix asks for them Acs. When
 * certificates come to Mix from Acs, they are returned to the Six
 * that wanted them.
 *
 * @param   request Request's GMessage:
 * <pre>
 *   GM_PARAMETERS
 *     CERTIFICATE_TYPE
 *     GM_COUNT
 * </pre>
 * @return  void
 * @author  Venca
 * @see     CertificateRepository, Acs
 */
void Majordomo::GetCertificates(GMessage * request)
{
  WriteString(llDebug, __FILE__ ":%d:Entering GetCertificate in Majordomo.",
              __LINE__);

  GMessage * message = new GMessage();

    // Construct return onion
  GMessage * returnOnion = NULL;
  GMessage * properties = new GMessage();
  GMessage * mixcert = NULL;
  mix->addrMgr->ReturnLocalMixCertificate(&mixcert);
  properties->SetAsGMessage(GM_MIX_CERTIFICATE, mixcert);
  properties->SetAsString(GM_DESTINATION, LBL_MIX_LOCAL);
  returnOnion = mix->msgCreator->CreateOnionFromMixCertificate(properties);
  GMessage * params = request->GetAsGMessage(GM_PARAMETERS);
  params->SetAsGMessage(GM_ONION, returnOnion);
  string origin = request->GetAsString(GM_ORIGIN);
  ID requestID = random();
  mix->messageIDRepository->Register(requestID, NULL, origin);
  request->SetAsBytes(GM_REQUEST_ID, &requestID, sizeof(ID));
  request->SetAsGMessage(GM_PARAMETERS, params);
  DELETE(params); DELETE(returnOnion);

    // Make onion to Acs
    // Get ACS certificate
  GMessage * acscert = mix->certificateRepository->GetCertificate(LBL_ACS);
  acscert->DebugHeaders(logFile,"acscert");
    // extract mix certificate
  mixcert = acscert->GetAsGMessage(GM_MIX_CERTIFICATE);
  mixcert->DebugHeaders(logFile,"mixcert");
  string name = acscert->GetAsString(GM_NAME);
  DELETE(acscert);
 
  properties->Clear();
  properties->SetAsGMessage(GM_MIX_CERTIFICATE, mixcert);
  properties->SetAsString(GM_DESTINATION, (char *)name.c_str());
    // Make onion from mix certificate
  GMessage * onion = mix->msgCreator->CreateOnionFromMixCertificate(properties);

  Size hops = atoi(mix->configFile->
                     GetValue(CFG_ADD_HOPS).c_str());
  GMessage * parameters = new GMessage();
  parameters->SetAsBytes(GM_ADD_HOPS, (void *) &hops, sizeof (Size));
  parameters->SetAsGMessage(GM_DATA, request);
  parameters->SetAsGMessage(GM_ONION, onion);
  parameters->DebugHeaders(logFile, "parameters");
  DELETE(onion);
  message->SetAsString(GM_COMMAND, CMD_SEND_DATA);
  message->SetAsGMessage(GM_PARAMETERS, parameters);
  message->DebugHeaders(logFile, "message");

  mix->receiverToChopper->Append(message);
}
