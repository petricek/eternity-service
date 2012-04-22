#include "H/sixMajordomo.h"
#include "../../Common/H/labels.h"
#include "../../Common/H/commands.h"

/**
 * Constructor of SixMajordomo.
 *
 * Creates transaction manager and calls its ancestors' constructors.
 *
 *
 * @param   parent pointer to Six 
 * @param   majordomoLogFile logFile to write to 
 * @author  Pavel 
 * @see     TransactionManager, Runable, SixOffspring
 */
SixMajordomo::SixMajordomo(Six * parent, LogFile * majordomoLogFile)
  : Runable(majordomoLogFile), SixOffspring(parent)
{
    // create transactionManager
  Char *tmpPath;
  transactionManager = new TransactionManager(tmpPath = six->GetPath("transaction.log"), logFile);
  DELETE(tmpPath);
}


/**
 * Destructor of SixMajordomo.
 *
 * Deletes transaction manager.
 *
 *
 * @author  Pavel 
 * @see     TransactionManager
 */
SixMajordomo::~SixMajordomo()
{
  DELETE(transactionManager);
}


/**
 * Main running loop of SixMajordomo.
 *
 * In a loop gets a message from receiverToMajordomo queue,
 * creates outMsgQueue, processes the message and
 * puts all the messages (if there are any) from outMsgQueue
 * into the majordomoToSender queue.
 *
 *
 * @return  void
 * @author  Pavel
 * @see     MessageQueue
 */
void *SixMajordomo::Run(void *)
{
  ID currentTID;
  GMessage *inMsg;

  for ( ; ; )
  {
      // Get a message
    if(six->receiverToMajordomo->GetMsg(&inMsg) != OK)
      continue;
  
      // Start transaction
    BeginTransaction(&currentTID);
     
      // Handle message
    HandleMixMessage(inMsg);

      // End transaction
    EndTransaction(currentTID);

    if(inMsg)
      DELETE(inMsg);
  }
}


/**
 * Starts the transaction.
 *
 * Calls the transactionManager method and thus begins new transaction.
 *
 *
 * @param   parametr
 * @return  newTID returned by the method 
 * @author  error code 
 * @see     TransactionManager
 */
Err SixMajordomo::BeginTransaction(ID *newTID)
{
  Err result = transactionManager->BeginTransaction(newTID);
  if(result != OK)
    return KO;

  tID = *newTID;
  return OK;
}


/**
 * Finishes transaction.
 *
 * Calls the transaction manager to finish the transaction.
 *
 *
 * @param   tid tid of transaction to finish
 * @return  error code 
 * @author  Pavel
 * @see     TransactionManager
 */
Err SixMajordomo::EndTransaction(ID tid)
{
  return transactionManager->EndTransaction(tid);
}


/**
 * Returns current TID.
 *
 * Returns the value of the attribute tID.
 *
 *
 * @return  ID currentTID. 
 * @author  Pavel
 */
ID SixMajordomo::GetTID()
{
  return tID;
}


/**
 * Puts the messages from outMsgQueue to the majordomoToSender queue.
 *
 * If outMsgQueue is not null and not empty, takes all messages from it
 * and puts into majordomoToSender queue.
 *
 *
 * @return  error code
 * @author  Marek
 * @see     Queue
 */
Err SixMajordomo::FeedSenderWithOutMsgQueue()
{
  Err result;
  GMessage *mixMsg;  

DBG;
  result = OK;
  mixMsg = (GMessage*) outMsgQueue->First();

  while(mixMsg != NULL){
DBG;
    result = FeedSenderWithMsgForMix(mixMsg);

    DELETE(mixMsg);
    mixMsg = (GMessage *)outMsgQueue->Next();
  }

DBG;
  return result;
}

/**
 * Puts one message to the majordomoToSender queue.
 *
 * Appends parameter into the majordomoToSenderQueue.
 *
 *
 * @param   mixMsg message to append
 * @return  error code
 * @author  Marek
 * @see     GMessage, Sender, MessageQueue
 */
Err SixMajordomo::FeedSenderWithMsgForMix(GMessage *mixMsg)
{
  Err result = OK;
DBG;
  if(mixMsg == NULL)
    return KO;
  mixMsg->DebugHeaders(logFile, "Message for MakeMessageForSendingToMix()");
  
DBG;  
  GMessage *messageForSender = six->sender->MakeMessageForSendingToMix(mixMsg);
  messageForSender->DebugHeaders(logFile, "How does a msg for Sender look like ?");
DBG;    
  if(six->majordomoToSender->Append(messageForSender) != OK) {
     DBG;
      result = KO;
  }
DBG;
  return result;
}







