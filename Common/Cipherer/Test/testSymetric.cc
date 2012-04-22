//*************************************************************************
// 
// File: testSymetric.cc
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

#define HOP_COUNT 10 
#define LOG_FILE_NAME "log.log"

#define LONG_TEST_DATA "Kdyz se snasi noc, na strechy Peterburgu, pada na \
                        me zal. Zatoulany pes, nevzal si ani kurku chleba \
			kterou jsem mu dal. Lasku moji, knize Igor si bere, \
			nad sklenkou vodky, hraju si s revolverem, havran \
			useda, na strechy Peterburgu, pada na me zal."

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
  LogFile *logFile = new LogFile(LOG_FILE_NAME);
  Cipherer *cipherer = new Cipherer(logFile);
  cipherer->WriteString(llDebug, __FILE__ ":%d:Symetric test started.", 
  			__LINE__);

    // choose some data for encryption
  char *plainData = (char *) SHORT_TEST_DATA;
  // comment out if you want to test empty fields
  //int plainDataLen = 0;
  int plainDataLen = strlen(plainData) + 1;
  cipherer->WriteString(llDebug, __FILE__ ":%d:Cista data jako string "
                        "[%d]-> '%s'", __LINE__, plainDataLen, plainData);

  display = displayStream(plainData, plainDataLen);
  cipherer->WriteString(llDebug, __FILE__ ":%d:Cista data jako display "
                        "[%ld] -> '%s'", __LINE__, plainDataLen, 
			display.c_str());

    // generate symetric key
  cipherer->WriteString(llDebug, __FILE__ ":%d:Generuji symetricke INFO.", 
  			__LINE__);
  GMessage *pSymKey = cipherer->GenerateSymetricKey(); 
  tmpChar = (char *) pSymKey->GetAsBytes(GM_SYMETRIC_KEY);
  display = displayStream(tmpChar, 8);
  cipherer->WriteString(llDebug, __FILE__ ":%d:Vygenerovany symetricky KLIC "
			"pro kontrolu [%ld] -> '%s'.", __LINE__, 8, 
			display.c_str());
  DELETE(tmpChar);

    // insert data into right structures for encryption 
  BytesMsgField *pDataField = new BytesMsgField("", (void *) plainData, 
                                  plainDataLen, logFile);

    // check if we insert data into BytesMsgField correctly
  char *tmpData = (char *) pDataField->GetAsBytes(&tmpSize);
  display = displayStream(tmpData, tmpSize);
  cipherer->WriteString(llDebug, __FILE__ ":%d:Kontrola vlozenych cistych "
                        "dat do MsgField [%ld] -> '%s'.", __LINE__, tmpSize,
			display.c_str());
  DELETE(tmpData);

    // zasifruj
  MsgField *eDataField;
  cipherer->EncryptWithSymetricKey(pSymKey, pDataField, &eDataField);
  DELETE(pDataField);

    // zkontroluj, zda data jsou zasifrovana
  tmpChar = (char *) eDataField->GetAsBytes( &tmpSize);
  display = displayStream(tmpChar, tmpSize);
  cipherer->WriteString(llDebug, __FILE__ ":%d:Kontrola zasifrovanych "
                        "dat [%ld] -> '%s'.", __LINE__, tmpSize, 
			display.c_str());
  DELETE(tmpChar);

  cipherer->WriteString(llDebug, __FILE__ ":%d:Ted budu symetricky "
                        "odsifrovavat prave zasifrovana data.", __LINE__);

  MsgField *pDataMsgField = NULL;
  cipherer->DecryptWithSymetricKey(pSymKey, eDataField, &pDataMsgField);
  DELETE(eDataField); DELETE(pSymKey);
  tmpChar = (char *) pDataMsgField->GetAsBytes( &tmpSize);
  DELETE(pDataMsgField);

  /* comment out if you want to test empty fields
  if (! tmpSize) {
    DELETE(tmpChar);
    tmpChar = new char[1];
    tmpChar[0] = '\0';
  }
  */
  display = displayStream(tmpChar, tmpSize);
    
  cipherer->WriteString(llDebug, __FILE__ ":%d:Snad zcela cista data "
                        "ve stringu-> '%s'", __LINE__, tmpChar);
  DELETE(tmpChar);

  cipherer->WriteString(llDebug, __FILE__ ":%d:Snad zcela cista data "
                        "[%ld] -> '%s'", __LINE__, tmpSize, display.c_str());

  cipherer->WriteString(llDebug, __FILE__ ":%d:Ted budu %d-nasobne "
                        "sifrovat opet data '%s'.", __LINE__, HOP_COUNT, 
			plainData);
			
  pDataField = new BytesMsgField("", (void *) plainData, plainDataLen);
  GMessage *symKeys = cipherer->ReturnGMsgFullOfSymKeys( HOP_COUNT);
  symKeys->DebugHeaders(logFile, "What is in returned symKeys?");
  cipherer->EncryptWithSymetricKeys(symKeys, pDataField, &eDataField);
  DELETE(pDataField);

    // zkontroluj, zda data jsou zasifrovana
  tmpChar = (char *) eDataField->GetAsBytes( &tmpSize);
  display = displayStream(tmpChar, tmpSize);
  cipherer->WriteString(llDebug, __FILE__ ":%d:Puvodne cista data "
                        "nekolikrat zasifrovana [%ld] -> '%s'", __LINE__, 
			tmpSize, display.c_str());
  DELETE(tmpChar);

  cipherer->WriteString(llDebug, __FILE__ ":%d:Ted budu odsifrovavat ta "
                        "%d-nasobne nabalena data.", __LINE__, HOP_COUNT);

  cipherer->DecryptWithSymetricKeys(symKeys, eDataField, &pDataMsgField);
  DELETE(eDataField); DELETE(symKeys); 
  tmpChar = (char *) pDataMsgField->GetAsBytes( &tmpSize);
  DELETE(pDataMsgField);
    
  /* comment out if you want to test empty fields
  if (! tmpSize) {
    DELETE(tmpChar);
    tmpChar = new char[1];
    tmpChar[0] = '\0';
  }
  */
  display = displayStream(tmpChar, tmpSize);
  cipherer->WriteString(llDebug, __FILE__ ":%d:Snad zcela cista data jako "
                        "string-> '%s'.", __LINE__, tmpChar);
  DELETE(tmpChar); 
  cipherer->WriteString(llDebug, __FILE__ ":%d:Snad opet cista data [%ld] "
                        "-> '%s'.", __LINE__, tmpSize, display.c_str());

  DELETE(cipherer); DELETE(logFile);

  return 0;
}
