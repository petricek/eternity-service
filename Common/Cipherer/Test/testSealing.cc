//*************************************************************************
// 
// File: testSealing.cc
// Name:
// Author: Pechy
// Last Modified: 28.1.1999
// 
// Description:
//
//*************************************************************************

  // has to be the *FIRST* include !
#include "./../../../Common/H/common.h"

#include "./../../../Common/H/labels.h"

#include <string.h>
#include <stdio.h>
#include <string>

#include "./../../LogFile/H/logFile.h"
#include "./../H/cipherer.h"

#define LOG_FILE "log.log"

#define LONG_TEST_DATA "Kdyz se snasi noc, na strechy Peterbuggu, pada na me zal. Zatoulany pes, nevzal si ani kurku chleba kterou jsem mu dal. Lasku moji, knize Igor si bere, nad sklenkou vodky, hraju si s revolverem, havran useda, na strechy Peterburgu, pada na me zal."

#define SHORT_TEST_DATA "Come on baby, light my fire !"

string displayStream(char *octetStream, int charsToPrint);

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
int main()
{
  Size tmpSize;
  char *tmpChar;
  string display;

    // create cipherer object
  LogFile *logFile = new LogFile(LOG_FILE);
  Cipherer *cipherer = new Cipherer(logFile);
  cipherer->WriteString(llDebug, __FILE__ ":%d:Sealing test started.", 
  			__LINE__);

    // choose some data for encryption
  char *plainData = (char *) SHORT_TEST_DATA;
  int plainDataLen = strlen(plainData) + 1;
  cipherer->WriteString(llDebug, __FILE__ ":%d:Cista data jako string "
                        "[%d]-> '%s'", __LINE__, plainDataLen, plainData);

  display = displayStream(plainData, plainDataLen);
  cipherer->WriteString(llDebug, __FILE__ ":%d:Cista data jako display "
                        "[%ld] -> '%s'", __LINE__, plainDataLen, 
			display.c_str());

  GMessage *asymKey1 = NULL;
  asymKey1 = cipherer->GenerateAsymetricKey();
  asymKey1->DebugHeaders(logFile, "What is in just generated asymetric "
                        "key info 1?");

  GMessage *asymKey2 = NULL;
  asymKey2 = cipherer->GenerateAsymetricKey();
  asymKey2->DebugHeaders(logFile, "What is in just generated asymetric "
                        "key info 2?");

  GMessage *pubKey1 = NULL;
  pubKey1 = asymKey1->GetAsGMessage(GM_PUBLIC_KEY_INFO);
  pubKey1->DebugHeaders(logFile, "What is in public key info 1 ?");

  GMessage *pubKey2 = NULL;
  pubKey2 = asymKey2->GetAsGMessage(GM_PUBLIC_KEY_INFO);
  pubKey2->DebugHeaders(logFile, "What is in public key info 2 ?");

  GMessage *privKey1 = NULL;
  privKey1 = asymKey1->GetAsGMessage(GM_PRIVATE_KEY_INFO);
  privKey1->DebugHeaders(logFile, "What is in private key info 1 ?");

  GMessage *privKey2 = NULL;
  privKey2 = asymKey2->GetAsGMessage(GM_PRIVATE_KEY_INFO);
  privKey2->DebugHeaders(logFile, "What is in private key info 2 ?");

    // insert data into right structures for encryption 
  BytesMsgField *pDataField = new BytesMsgField("", (void *) plainData,
                                  plainDataLen, logFile);

  cipherer->WriteString(llDebug, __FILE__ ":%d:Ted zacnu sealovat.", __LINE__);

  MsgField *eDataField;
  int result;
  if ( (result = cipherer->Seal(privKey1, pubKey2, pDataField, 
                                &eDataField)) != OK) {
    cipherer->WriteString(llDebug, __FILE__ ":%d:Seal failed.", __LINE__);
    exit(1);
  }

  tmpChar = (char *) eDataField->GetAsBytes(&tmpSize);
  display = displayStream(tmpChar, tmpSize);
  cipherer->WriteString(llDebug, __FILE__ ":%d:Zasealovana data [%ld] "
     			"-> '%s'", __LINE__, tmpSize, display.c_str());
  DELETE(tmpChar); 

  cipherer->WriteString(llDebug, __FILE__ ":%d:Ted zacnu otevirat.", __LINE__);
  MsgField *pDataMsgField = NULL; 
  if ( (result = cipherer->OpenSealed(privKey2, pubKey1, eDataField, 
                                      &pDataMsgField)) 
       != OK) {
    cipherer->WriteString(llDebug, __FILE__ ":%d:OpenSealed failed.", __LINE__);
    exit(1);
  }

  DELETE(pubKey1); DELETE(pubKey2); DELETE(privKey1); DELETE(privKey2);

  tmpChar = (char *) pDataMsgField->GetAsBytes(&tmpSize);
  DELETE(pDataMsgField); DELETE(eDataField);
  display = displayStream(tmpChar, tmpSize);
  cipherer->WriteString(llDebug, __FILE__ ":%d:Otevrena data [%ld] "
     			"-> '%s'", __LINE__, tmpSize, display.c_str());
  DELETE(tmpChar); 

  DELETE(cipherer); DELETE(logFile);

  return 0;
}
