//**********************
//
// File: testMajordomo.cc
//
// Author: Marek
//
// Contents: tests the SwitchMixCommand procedure of Majordomo
//
//**********************

#include "./../H/majordomo.h"
#include "./../../ACManager/H/esoAC.h"




void PrintString(Majordomo *maj, Char *c, Err result)
{
 if (maj != NULL)
   maj->WriteString(llDebug, "%s: %s", c, ((result == OK) ? OK : KO)); 
};






class TestMajordomo: public Majordomo
{
  public:
   TestMajordomo(Eso *parent, LogFile *logFile);

   void TestACManagerMethods();
}; 


//-----------------------
// TestMajordomo::TestMajordomo
// ~~~~~~~~~~~~~~~~~~~
//-----------------------
TestMajordomo::TestMajordomo(Eso *parent, LogFile *logFile):Majordomo(parent, logFile)
{
}


//-----------------------
// TestMajordomo::TestACManagerMethods
// ~~~~~~~~~~~~~~~~~~~
//-----------------------
void TestMajordomo::TestACManagerMethods()
{
  EsoAC *ac;
  ACID acid;
  GMessage *acPubKey;
  MsgField *acPubKeyFld;
  GMessage *tcbPubKey;
  MsgField *tcbPubKeyFld;
  BytesMsgField *onion;
  Err result;

  WriteString(llDebug, "TestMajordomo::TestACManagerMethods()");

  ac = new EsoAC();
  if (ac == NULL){
    WriteString(llDebug, "--> Cannot create esoAC");
    return;
  }

  PrintString(this, "--> TCB::GenerateACKeys()", 
	 (result = eso->tcbWrapper->GenerateACKeys(&acid, &acPubKey, &tcbPubKey)));

  if (result != OK)
    return;


  acPubKeyFld = acPubKey->StoreToMsgField();
  tcbPubKeyFld = tcbPubKey->StoreToMsgField();

  onion = new BytesMsgField("Prdel");
  onion->SetAsString("Hovno");

  PrintString(this, "--> EsoAC::SetFields()", 
	      (result = ac->SetFields(ACTYPE_ESO, acid, onion, acPubKeyFld,
				      tcbPubKeyFld)));
  
  delete acPubKey;
  delete acPubKeyFld;
  delete tcbPubKey;
  delete tcbPubKeyFld;

  ac->DebugHeaders(logFile, "--> Debugging created AC");


  if (result != OK)
    return; 

  PrintString(this, "--> Inserting AC", InsertAC(ACTYPE_ESO, ac));
  


}









//***********************************
void main()
{
  TestMajordomo *t;
  Eso *eso;
  LogFile *l;

  l = new LogFile("testMaj.log");
  eso = new Eso();

  t= new TestMajordomo(eso, l);

  t->TestACManagerMethods();
}
