#include "./../H/finder.h"


void PrintResult(Finder *fi, char *str, Err result)
{
  if(fi == NULL)
    return;

  fi->WriteString(llDebug, "%s: %s", str, result == KO ? "KO" : "OK");
};



GMessage *CreateAC(ID acid, Char *text)
{
  GMessage *ac;
  ESTime *t;

  ac = new GMessage();
  ac->SetAsID("ACFLD_ACID", acid);
 
  t = new ESTime();
  ac->SetAsBasicTime("ACFLD_EXPIRATION", *t); 
  DELETE(t);

  ac->SetAsString("ACFLD_INFO", text);
  
  return ac;
}

GMessage *CreateInMsgForSaveFwdInfo(ID id, ID acID, Char *text)
{
  GMessage *inMsg, *ac;

  inMsg = new GMessage();
  
  ac = CreateAC(acID, text);
  inMsg->SetAsGMessage(DATAFLD_REPLY_TO_AC, ac);
  DELETE(ac);

  inMsg->SetAsID(DATAFLD_REPLY_TO_TID, id);

  return inMsg;

}

GMessage *CreateHeader(ID ffID)
{
  GMessage *inMsg;

  inMsg = new GMessage();
  
  inMsg->SetAsID(F_HD_FLD_FFID, ffID);

  return inMsg;

}
#define TID1 123L
#define TID2 124L

void main()
{
  Finder *fi;
  GMessage *gm;
  TID id;

  fi = new Finder();

  gm = CreateInMsgForSaveFwdInfo(TID1, 123L, "123L");
  PrintResult(fi, "TestFinder: SaveForwardInfo:", fi->SaveForwardInfo(TID1, gm, 0));  
  DELETE(gm); 
  
  gm = CreateInMsgForSaveFwdInfo(TID2, 123L, "123L");
  PrintResult(fi, "TestFinder: SaveForwardInfo:", fi->SaveForwardInfo(TID2, gm, 0));  
  DELETE(gm);

  gm = new GMessage();
  PrintResult(fi, "TestFinder: GetForwardInfo:", fi->GetForwardInfo(TID1, gm, &id));
  fi->WriteString(llDebug, "TestFinder: got ID %ld", id);  
  DELETE(gm);

  PrintResult(fi, "TestFinder: DeleteForwardInfo:", fi->DeleteForwardInfo(TID1));

  gm = new GMessage();
  PrintResult(fi, "TestFinder: GetForwardInfo:", fi->GetForwardInfo(TID1, gm, &id));
  fi->WriteString(llDebug, "TestFinder: got ID %ld", id);  
  DELETE(gm);
  

  gm = CreateHeader(TID1);
  PrintResult(fi, "TestFinder: SaveReplyInfo:", fi->SaveReplyInfo(TID1, gm, 0));  
  DELETE(gm);

  gm = CreateHeader(TID2);
  PrintResult(fi, "TestFinder: SaveReplyInfo:", fi->SaveReplyInfo(TID2, gm, 0));  

  fi->WriteString(llDebug, "TestFinder: Already Sent: %d", fi->AlreadySent(TID2, gm) );  
  DELETE(gm);

 DELETE(fi);

}
