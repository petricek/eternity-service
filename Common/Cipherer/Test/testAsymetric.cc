//*************************************************************************
// 
// File: testAsymetric.cc
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

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <string>

#include "./../../LogFile/H/logFile.h"
#include "./../H/cipherer.h"

#define LONG_TEST_DATA "Kdyz se snasi noc, na strechy Peterbuggu, pada na me zal. Zatoulany pes, nevzal si ani kurku chleba kterou jsem mu dal. Lasku moji, knize Igor si bere, nad sklenkou vodky, hraju si s revolverem, havran useda, na strechy Peterburgu, pada na me zal."

#define SHORT_TEST_DATA "Come on baby, light my fire !"
#define DATA LONG_TEST_DATA

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
  LogFile *logFile = new LogFile("log.log");
  Cipherer *cipherer = new Cipherer(logFile);
  cipherer->WriteString(llDebug, __FILE__ ":%d:Asymetric test started.", 
			__LINE__);

    // choose some data for encryption
  char *plainData = (char *) DATA;
  int plainDataLen = strlen(plainData);
  //int plainDataLen = 0;
  cipherer->WriteString(llDebug, __FILE__ ":%d:Cista data jako string "
                        "[%d]-> '%s'", __LINE__, plainDataLen, plainData);

  display = displayStream(plainData, plainDataLen);
  cipherer->WriteString(llDebug, __FILE__ ":%d:Cista data jako display "
                        "[%ld] -> '%s'", __LINE__, plainDataLen, 
			display.c_str());

  GMessage *pSymKey = NULL;
  pSymKey = cipherer->GenerateSymetricKey();
  pSymKey->DebugHeaders(logFile, "Vygenerovany symetricke INFO.");
  tmpChar = (char *) pSymKey->GetAsBytes(GM_SYMETRIC_KEY);
  display = displayStream(tmpChar, 8);
  cipherer->WriteString(llDebug, __FILE__ ":%d:Vygenerovany symetricky "
                        "KLIC pro kontrolu [%ld] -> '%s'.", __LINE__, 8,
			display.c_str());
  DELETE(tmpChar);

  GMessage *asymKey = NULL;
  asymKey = cipherer->GenerateAsymetricKey();
  asymKey->DebugHeaders(logFile, "What is in just generated asymetric "
                        "key info?");

  GMessage *pubKey = NULL;
  pubKey = asymKey->GetAsGMessage(GM_PUBLIC_KEY_INFO);
  pubKey->DebugHeaders(logFile, "What is in public key info?");

  GMessage *privKey = NULL;
  privKey = asymKey->GetAsGMessage(GM_PRIVATE_KEY_INFO);
  privKey->DebugHeaders(logFile, "What is in private key info?");

  tmpChar = (char *) pSymKey->StoreToBytes(&tmpSize);
  display = displayStream(tmpChar, tmpSize);
  cipherer->WriteString(llDebug, __FILE__ ":%d:Symetricke INFO, hozene do "
                        "streamu, jako display [%ld] -> '%s'.", __LINE__, 
			tmpSize, display.c_str());
  DELETE(tmpChar);

  cipherer->WriteString(llDebug, __FILE__ ":%d:Ted zasifruju symetricke INFO "
                        "public klicem.", __LINE__);
  
  MsgField *eSymKey = NULL;
  cipherer->EncryptKeyWithPublicKey(pubKey, pSymKey, &eSymKey);
  DELETE(pSymKey);

  tmpChar = (char *) eSymKey->GetAsBytes(&tmpSize);
  display = displayStream(tmpChar, tmpSize);
  cipherer->WriteString(llDebug, __FILE__ ":%d:Zasifrovane symetricke INFO "
                        "pro kontrolu [%ld] -> '%s'.", __LINE__, tmpSize, 
			display.c_str());
  DELETE(tmpChar);

  cipherer->WriteString(llDebug, __FILE__ ":%d:Ted odsifruju symetricke INFO "
                        "privatem klicem.", __LINE__);

  GMessage *dSymKey = NULL;
  cipherer->DecryptKeyWithPrivateKey(privKey, eSymKey, &dSymKey);
  DELETE(eSymKey);

  dSymKey->DebugHeaders(logFile, "Odsifrovane symetricke INFO jako GMessage.");

  tmpChar = (char *) dSymKey->StoreToBytes(&tmpSize);
  DELETE(dSymKey); 
  display = displayStream(tmpChar, tmpSize);
  cipherer->WriteString(llDebug, __FILE__ ":%d:Odsifrovane symetricke INFO pro "
                        "kontrolu [%ld] -> '%s'.", __LINE__, tmpSize, 
			display.c_str());
  DELETE(tmpChar);

//----------------
  {
    cipherer->WriteString(llDebug, __FILE__ ":%d:Ted zasifruju cista data "
			  "public klicem.", __LINE__);

      // insert data into right structures for encryption 
    BytesMsgField *pDataField = new BytesMsgField();
    pDataField->SetAsString(plainData);
    MsgField *eDataField = NULL;
    cipherer->EncryptWithPublicKey(pubKey, pDataField, &eDataField);
    //DELETE(pubKey); 
    tmpChar = (char *) eDataField->GetAsBytes(&tmpSize);
    display = displayStream(tmpChar, tmpSize);
    cipherer->WriteString(llDebug, __FILE__ ":%d:Zasifrovana data pro "
			  "kontrolu [%ld] -> '%s'", __LINE__, tmpSize, 
			  display.c_str());
    DELETE(tmpChar); DELETE(pDataField);

    cipherer->WriteString(llDebug, __FILE__ ":%d:Ted odsifruju prave "
			  "zasifrovana data private klicem.", __LINE__);

    MsgField *pDataMsgField = NULL;
    cipherer->DecryptWithPrivateKey(privKey, eDataField, &pDataMsgField);
    // DELETE(privKey);
    DELETE(eDataField);

    tmpChar = (char *) pDataMsgField->GetAsBytes(&tmpSize);
    DELETE(pDataMsgField);
    display = displayStream(tmpChar, tmpSize);
    cipherer->WriteString(llDebug, __FILE__ ":%d:Odsifrovana data pro "
			  "kontrolu [%ld] -> '%s'.", __LINE__, tmpSize,
			  display.c_str());

    cipherer->WriteString(llDebug, __FILE__ ":%d:Odsifrovana data jako string "
			  "[%ld]-> '%s'", __LINE__, tmpSize, tmpChar);
    DELETE(tmpChar); 
  }

//----------------
  {
    cipherer->WriteString(llDebug, __FILE__ ":%d:Ted zasifruju cista data "
			  "private klicem.", __LINE__);

      // insert data into right structures for encryption 
    BytesMsgField *pDataField = new BytesMsgField();
    pDataField->SetAsString(plainData);
    MsgField *eDataField = NULL;
    cipherer->EncryptWithPrivateKey(privKey, pDataField, &eDataField);
    DELETE(privKey); 
    tmpChar = (char *) eDataField->GetAsBytes(&tmpSize);
    display = displayStream(tmpChar, tmpSize);
    cipherer->WriteString(llDebug, __FILE__ ":%d:Zasifrovana data pro "
			  "kontrolu [%ld] -> '%s'", __LINE__, tmpSize, 
			  display.c_str());
    DELETE(tmpChar); DELETE(pDataField);

    cipherer->WriteString(llDebug, __FILE__ ":%d:Ted odsifruju (prave "
			  "zasifrovana data) public klicem.", __LINE__);

    MsgField *pDataMsgField = NULL;
    cipherer->DecryptWithPublicKey(pubKey, eDataField, &pDataMsgField);
    DELETE(pubKey); DELETE(eDataField);

    tmpChar = (char *) pDataMsgField->GetAsBytes(&tmpSize);
    DELETE(pDataMsgField);
    display = displayStream(tmpChar, tmpSize);
    cipherer->WriteString(llDebug, __FILE__ ":%d:Odsifrovana data pro "
			  "kontrolu [%ld] -> '%s'.", __LINE__, tmpSize,
			  display.c_str());

    cipherer->WriteString(llDebug, __FILE__ ":%d:Odsifrovana data jako string "
			  "[%ld]-> '%s'", __LINE__, tmpSize, tmpChar);
    DELETE(tmpChar); 
  }
//---------------

  DELETE(cipherer); DELETE(logFile);

  return 0;
}
