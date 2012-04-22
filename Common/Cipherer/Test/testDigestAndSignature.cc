//*************************************************************************
// 
// File: testDigestAndSignature.cc
// Name:
// Author: Pechy
// Last Modified: 30.1.1999
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

#include "./../../RSAEuro/H/rsaref.h"

#define LOG_FILE "log.log"

#define LONG_TEST_DATA "Kdyz se snasi noc, na strechy Peterburgu, pada na me zal. Zatoulany pes, nevzal si ani kurku chleba kterou jsem mu dal. Lasku moji, knize Igor si bere, nad sklenkou vodky, hraju si s revolverem, havran useda, na strechy Peterburgu, pada na me zal."

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
  int result;

    // create cipherer object
  LogFile *logFile = new LogFile(LOG_FILE);
  Cipherer *cipherer = new Cipherer(logFile);
  cipherer->WriteString(llDebug, __FILE__ ":%d:Digest and signature test "
                        "started.", __LINE__);

    // choose some data for encryption
  char *plainData = (char *) SHORT_TEST_DATA;
  int plainDataLen = strlen(plainData) + 1;
  cipherer->WriteString(llDebug, __FILE__ ":%d:Cista data jako string "
                        "[%d]-> '%s'", __LINE__, plainDataLen, plainData);

  display = displayStream(plainData, plainDataLen);
  cipherer->WriteString(llDebug, __FILE__ ":%d:Cista data jako display "
                        "[%ld] -> '%s'", __LINE__, plainDataLen, 
			display.c_str());

    // insert data into right structures for encryption 
  BytesMsgField *pDataField = new BytesMsgField("", (void *) plainData,
                                  plainDataLen, logFile);

  GMessage *asymKey = NULL;
  asymKey = cipherer->GenerateAsymetricKey();
  asymKey->DebugHeaders(logFile, "What is in just generated asymetric "
                        "key info ?");

  GMessage *pubKey = NULL;
  pubKey = asymKey->GetAsGMessage(GM_PUBLIC_KEY_INFO);
  pubKey->DebugHeaders(logFile, "What is in public key info?");

  GMessage *privKey = NULL;
  privKey = asymKey->GetAsGMessage(GM_PRIVATE_KEY_INFO);
  privKey->DebugHeaders(logFile, "What is in private key info?");

  cipherer->WriteString(llDebug, __FILE__ ":%d:Ted udelam digest.", __LINE__);

  MsgField *digest;
  if ((result = cipherer->MessageDigest(pDataField, &digest)) != IDOK) {
    cipherer->WriteString(llDebug, __FILE__ ":%d:Digest failed.", __LINE__);
  }

  tmpChar = (char *) digest->GetAsBytes(&tmpSize);
  DELETE(digest);
  display = displayStream(tmpChar, tmpSize);
  cipherer->WriteString(llDebug, __FILE__ ":%d:Digest data display [%ld] "
     			"-> '%s'", __LINE__, tmpSize, display.c_str());
  DELETE(tmpChar); 

  cipherer->WriteString(llDebug, __FILE__ ":%d:Ted udelam signature.", 
                        __LINE__);

  MsgField *signature;
  if ((result = cipherer->Sign(privKey, pDataField, &signature)) != IDOK) {
    cipherer->WriteString(llDebug, __FILE__ ":%d:Signature failed.", __LINE__);
    exit(1);
  }

  tmpChar = (char *) signature->GetAsBytes(&tmpSize);
  display = displayStream(tmpChar, tmpSize);
  cipherer->WriteString(llDebug, __FILE__ ":%d:Signature display [%ld] "
     			"-> '%s'", __LINE__, tmpSize, display.c_str());
  DELETE(tmpChar); 

  cipherer->WriteString(llDebug, __FILE__ ":%d:Ted signature overim.", 
                        __LINE__);

  if ((result = cipherer->VerifySignature(pubKey, pDataField, signature)) 
                                          != IDOK) {
    cipherer->WriteString(llDebug, __FILE__ ":%d:Verification failed.", 
                          __LINE__);
    exit(1);
  }
  else {
    cipherer->WriteString(llDebug, __FILE__ ":%d:Verification succeded.", 
                          __LINE__);
  }

  DELETE(pubKey); DELETE(privKey); DELETE(signature); DELETE(pDataField);

  DELETE(cipherer); DELETE(logFile);

  return 0;
}
