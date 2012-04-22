#include "../GMessage/H/fields.h"
#include "../GMessage/H/msgField.h"

// -----------------------------------------------------------
// IntField
// -----------------------------------------------------------
void TestInt(File *aFile)
{
  Int i;
  Size bytesToEORec;
  TableField *tbFld;

  i = 3;
  tbFld = new IntField("", i);

  tbFld->WriteToFile(aFile);
  aFile->SeekStart();

  i = 5;
  bytesToEORec = 20;
  tbFld->SetValue(&i);
  tbFld->ReadFromFile(aFile, &bytesToEORec);

  printf("This is what I read: %d\nBytes to EORec: %ld\n", ((IntField *)tbFld)->GetIntValue(), bytesToEORec);

  DELETE(tbFld);
}


// -----------------------------------------------------------
// IDField
// -----------------------------------------------------------
void TestID(File *aFile)
{
  Size i;
  Size bytesToEORec;
  TableField *tbFld;

  i = 30L;
  tbFld = new IDField("", i);

  tbFld->WriteToFile(aFile);
  aFile->SeekStart();

  i = 50L;
  bytesToEORec = 20;
  tbFld->SetValue(&i);
  tbFld->ReadFromFile(aFile, &bytesToEORec);

  printf("This is what I read: %ld\nBytes to EORec: %ld\n", ((IDField *)tbFld)->GetIDValue(), bytesToEORec);

  DELETE(tbFld);
}


// -----------------------------------------------------------
// StringField
// -----------------------------------------------------------
void TestString(File *aFile)
{
  Char *tmp;
  Size bytesToEORec;
  TableField *tbFld;

  tbFld = new StringField("", "ahoj - cau");

  tbFld->WriteToFile(aFile);
  aFile->SeekStart();

  bytesToEORec = 20;
  tbFld->SetValue("kabel");
  tbFld->ReadFromFile(aFile, &bytesToEORec);

  tmp = ((StringField *)tbFld)->AsString();
  printf("This is what I read: %s\nBytes to EORec: %ld\n", tmp, bytesToEORec);

  DELETE(tmp);
  DELETE(tbFld);
}


// -----------------------------------------------------------
// TimeField
// -----------------------------------------------------------
void TestTime(File *aFile)
{
  ESTime time;
  Size bytesToEORec;
  TableField *tbFld;
  Char tmp[100];

  time.SetTime(99,1,15);
  tbFld = new TimeField("", &time);

  tbFld->WriteToFile(aFile);
  aFile->SeekStart();

  time.SetTime(99,12,17);
  bytesToEORec = 20;
  tbFld->SetValue(&time);
  tbFld->ReadFromFile(aFile, &bytesToEORec);

  ((TimeField*)tbFld)->GetESTimeValue().Write(tmp, 100);
  printf("This is what I read: %s\nBytes to EORec: %ld\n", tmp, bytesToEORec);

  DELETE(tbFld);
}


// -----------------------------------------------------------
// BytesField
// staci vykomentovat File nebo Bytes pro jednotlive varianty.
// POZOR: FileMsgField potrebuje soubor "testField.file"
// -----------------------------------------------------------
void TestBytes(File *aFile)
{
  Char tmpStr[100], *tmp;
  MsgField *field;
  Size bytesToEORec, len;
  TableField *tbFld;

  strcpy(tmpStr, "To ladeni me fakt uz ale sere.");
//  field = new BytesMsgField("", tmpStr, strlen(tmpStr)+1);
  field = new FileMsgField("", "testField.file");
  tbFld = new BytesField("", field);

  tbFld->WriteToFile(aFile);
  aFile->SeekStart();

  bytesToEORec = 50;
  strcpy(tmpStr, "Ladeni je jiskrou meho zivota.");
  field->SetAsBytes(tmpStr, strlen(tmpStr)+1);
  tbFld->SetValue(field);
  tbFld->ReadFromFile(aFile, &bytesToEORec);

  DELETE(field);
  field = ((BytesField *)tbFld)->GetAsMsgField();
  tmp = (Char *)field->GetAsBytes(&len);
  printf("This is what I read: %s\nIts length: %ld\nBytes to EORec: %ld\n", tmp, len, bytesToEORec);

  DELETE(tmp);
  DELETE(field);
  DELETE(tbFld);
}


void main()
{
  File *aFile;

  aFile = new File("testField.tab", "w+b");

  aFile->Open();

  TestTime(aFile);

  aFile->Close();

  DELETE(aFile);
}
