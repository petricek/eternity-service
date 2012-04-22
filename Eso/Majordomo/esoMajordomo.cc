//*************************************************************************
// 
// File: majordomo.cc
// Name: Majordomo
// Author: Venca
// Last Modified: 2.1.1999 V
// 
// Description:
// Processes requests comming from network for certificates
//
//*************************************************************************

#include "H/majordomo.h"
#include "../../Common/H/labels.h"
#include "../../Common/H/commands.h"

/**
 * Eso Majordomo constructor.
 *
 * Calls SixMajordomo constructor.
 *
 *
 * @param   parent Eso this Majordomo belongs to
 * @param   majordomoLogFile log file for debugging messages
 * @author  Pavel
 * @see     Six, SixMajordomo
 */
Majordomo::Majordomo(Eso * parent, LogFile * majordomoLogFile)
: SixMajordomo(parent, majordomoLogFile)
{
}

/**
 * Asks for Eso certificates.
 *
 * Generates message for missing ACs and sends it to Mix.
 *
 *
 * @return  error code
 * @author  Marek
 * @see     Six, SixMajordomo
 */
Err 
Majordomo::AskForEsoCertificates()
{
 DBG;

  outMsgQueue = new Queue();
  GenerateRequestForMissingACs(ACTYPE_ESO);
  FeedSenderWithOutMsgQueue();
  return OK;
}

/**
 * Inserts message to outMsgQueue.
 *
 * Puts the message to queue that is periodically feeded to majordomoToSender
 * MessageQueue, which is then emptied to Mix.
 *
 * @param   msg message to be sent
 * @return  error code
 * @author  Marek
 * @see     SixMajordomo
 */
Err 
Majordomo::InsertToOutMsgQueue(GMessage *msg)
{
  if(outMsgQueue == NULL)
    return KO;
  return outMsgQueue->Insert(msg);
}

