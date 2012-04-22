#include "./../H/tcbTable.h"
#include "./../../../Common/GMessage/H/table.h"

int main()
{
  LogFile *log = new LogFile("testTCBTable.log");
  
  TableFieldQueue *def = new TableFieldQueue(log);
  def->Insert( new IDField("ID"));
  def->Insert( new StringField("Text"));

  Cipherer cipherer(log);
  GMessage *ringKey = cipherer.GenerateSymetricKey();
  ringKey->DebugHeaders(log, "ringKey:");

  TCBTable *tcbTab = new TCBTable("testTCBTable.tab", def, ringKey, log);
  DELETE(def);
  DELETE(ringKey);

  log->WriteString(llDebug, "Opening tcbTab...");
  log->WriteString(llDebug, "Opening tcbTab: %s", tcbTab->Open() == OK ? "OK": "KO");

  log->WriteString(llDebug, "Inserting to tcbTab...");
  Record *r = tcbTab->NewRecord();
  ID i = 1;
  r->SetField("ID", &i);
  r->SetField("Text", "hromada kecu");
  log->WriteString(llDebug, "Opening tcbTab: %s", tcbTab->InsertRecord(r) == OK ? "OK": "KO");
  DELETE(r);

  log->WriteString(llDebug, "Retrieving records:");
  IDField *idf;
  StringField *stf;
  Char *tmp;
  r = tcbTab->First();
  while(r != NULL)
  {
    idf = (IDField *)r->GetField("ID");
    stf = (StringField *)r->GetField("Text");
    log->WriteString(llDebug, "  %ld, %s", idf->GetIDValue(), tmp = stf->AsString());
    DELETE(tmp);
    DELETE(r);

    r = tcbTab->Next();
  }

  log->WriteString(llDebug, "Closing tcbTab...");
  log->WriteString(llDebug, "Closing tcbTab: %s", tcbTab->Close() == OK ? "OK": "KO");

  DELETE(tcbTab);
  DELETE(log);

  return 0;
}
