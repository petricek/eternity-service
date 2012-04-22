#include "./../GMessage/H/record.h"
#include "./../GMessage/H/msgField.h"

void main()
{
  File *aFile;
  Record *r;
  TableFieldQueue *def;
  Size aSize;
  Char *tmp;
  MsgField *aMsgField;

  def = new TableFieldQueue();
  def->Insert( new IntField("int", 3));
  def->Insert( new IDField("id", 3333333));
  def->Insert( new StringField("str", NULL /*"cau vole"*/));
  def->Insert( new BytesField("bytes", aMsgField = new BytesMsgField("", "samy bajty", 11)));
  DELETE(aMsgField);

  r = new Record(def);
  DELETE(def);

  aFile = new File("testRecord.tab", "w+b");
  aFile->Open();

  r->WriteToFile(aFile);

  aSize = 100;
  r->SetField("id", &aSize);

  aFile->SeekStart();

  r->ReadFromFile(aFile, &aSize);
  printf("I read %ld bytes.\n", aSize);
  printf("IntField: %d\n", ((IntField*)r->GetField("int"))->GetIntValue());
  printf("IDField: %ld\n", ((IDField*)r->GetField("id"))->GetIDValue());
  printf("StringField: %s\n", tmp = ((StringField*)r->GetField("str"))->AsString());
  DELETE(tmp);
  aMsgField = ((BytesField*)r->GetField("bytes"))->GetAsMsgField();
  if(aMsgField != NULL)
  {
    tmp = aMsgField->GetAsBytes(&aSize);
    printf("BytesField: %s\n", tmp == NULL ? "bytes = NULL" : tmp);
    DELETE(tmp);
    DELETE(aMsgField);
  }
  else
    printf("BytesField: MsgField = NULL\n");

  DELETE(aFile);
  DELETE(r);
}
