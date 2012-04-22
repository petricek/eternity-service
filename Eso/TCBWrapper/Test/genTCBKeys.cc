#include "./../H/const.h"
#include "./../../Common/H/constants.h"
#include "./../../../Common/Cipherer/H/cipherer.h"
#include "./../../../Common/ConfigFile/H/configFile.h"
#include "./../../../Common/H/labels.h"

int main()
{
  GMessage *masterKey2, 
	   *storageRingKey2, 
	   *acRingKey2;
  MsgField *eRingKeys2,
           *dRingKeys2; 
  GMessage *masterKey, 
           *dRingKeys, 
	   *storageRingKey, 
	   *acRingKey;
  MsgField *eRingKeys;
  LogFile  *log = new LogFile("genTCBKeys.log");
  Cipherer cipherer(log);
  ConfigFile configFile("/home/eternity/conf/eso.conf");

    // get system path
  string path = configFile.GetValue(CFG_ESO_PATH);
  if(path.empty())
    log->WriteString(llFatalError, "No eso path specified. "
                          "Eso will not function properly");
  Char *masterKeyFileName = new Char[strlen(path.c_str()) + 30];
  strcpy(masterKeyFileName, path.c_str());
  strcat(masterKeyFileName, TCBW_MASTER_KEY_FILE);
  Char *ringKeysFileName = new Char[strlen(path.c_str()) + 30];
  strcpy(ringKeysFileName , path.c_str());
  strcat(ringKeysFileName , TCBW_RING_KEYS_FILE);
  log->WriteString(llDebug, "MasterKeyFileName: %s", masterKeyFileName);
  log->WriteString(llDebug, "ringKeysFileName: %s", ringKeysFileName);

    // generate all needed keys
  masterKey = cipherer.GenerateSymetricKey();
  masterKey->DebugHeaders(log, "Master Key 1");
  storageRingKey = cipherer.GenerateSymetricKey(); 
  acRingKey = cipherer.GenerateSymetricKey();

    // put all ring keys into a single GMessage
  dRingKeys = new GMessage();
  dRingKeys->SetAsGMessage(TCBW_RING_KEY_STORAGE, storageRingKey);
  dRingKeys->SetAsGMessage(TCBW_RING_KEY_CERTIFICATES, acRingKey);

    // encrypt ring keys with master key
  cipherer.EncryptWithSymetricKey(  masterKey, 
                                    dRingKeys->StoreToMsgField(),
				    &eRingKeys);
				    
    // save master key and encrypted ring keys
  File *file = new File(ringKeysFileName, "wb");
  file->Open();
  masterKey->StoreToFile(masterKeyFileName);
  eRingKeys->WriteToFile(file);
  file->Close();

    //************** REVERSE TESTING ***************

    //load master and encrypted ring keys
  masterKey2 = new GMessage();
  masterKey2->DebugHeaders(log, "Master Key 2");
  masterKey2->LoadFromFile(masterKeyFileName);
  eRingKeys2 = new FileMsgField("", ringKeysFileName);

    // decrypt ring keys with master key
  cipherer.DecryptWithSymetricKey(  masterKey2,
                                    eRingKeys2,
                                    &dRingKeys2);

    // convert to GMessage
  GMessage *tmp = dRingKeys2->GetAsGMessage();

    // debug
  masterKey->DebugHeaders(log, "Decrypted Master Key 1");
  masterKey2->DebugHeaders(log, "Decrypted Master Key 2");
  dRingKeys->DebugHeaders(log, "Decrypted Ring Keys 1");
  tmp->DebugHeaders(log, "Decrypted Ring Keys 2");
  
    // dispose off
  DELETE(masterKeyFileName);
  DELETE(ringKeysFileName);
  DELETE(masterKey);
  DELETE(dRingKeys);
  DELETE(storageRingKey);
  DELETE(acRingKey);
  DELETE(eRingKeys);
  DELETE(file);
  DELETE(log);
 
  return 0;
}
