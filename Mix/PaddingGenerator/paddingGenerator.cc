  // has to be the *FIRST* include !
#include "./../../Common/H/common.h"

#include "./H/paddingGenerator.h"

/**
 * PaddingGenerator constructor.
 *
 * Saves parameters that gets.
 *
 * @param   parent Pointer to Mix main object.
 * @param   logFile Log file.
 * @author  Venca
 * @see     Padder, traffic analysis discussion
 */
PaddingGenerator::PaddingGenerator(Mix * parent, LogFile * logFile)
  :Runable(logFile), MixOffspring(parent)
{

}

/**
 * The main loop for Padder thread.
 *
 * In a loop, cycles and sends padding messages.
 *
 * @param   void pointer
 * @return  void pointer
 * @author  Venca
 * @see     Padder, traffic analysis discussion
 */
void *
PaddingGenerator::Run(void *)
{
  pthread_sigmask(SIG_BLOCK, NULL, NULL);

    // Generates padding andpushes it to padder
    // It sleeps whenever the queue to padder is full
  for(;;)
  {
    SendPaddingMessage(); 
  }
}

/**
 * Send a padding message.
 *
 * Following a simple algorithm, creates and sends a padding message.
 * The algoritm, though simple, is very powerful and hard to break by
 * the enemy.
 *
 * @param   void 
 * @return  void
 * @author  Venca
 * @see     Padder, traffic analysis discussion
 */
void
PaddingGenerator::SendPaddingMessage()
{
  GMessage * message = new GMessage();
  message->SetAsString(GM_DEBUG, "Padding");
  GMessage * mixCert = mix->certificateRepository->GetCertificate(ACTYPE_MIX);
  message->SetAsGMessage(GM_MIX, mixCert);
  DELETE(mixCert);
  Size randData = random();
  message->SetAsBytes(GM_DATA, (void *)randData, sizeof(Size));
  Size hops = DEFAULT_PADDING_HOPS;
  message->SetAsBytes(GM_ADD_HOPS, (void *)hops, sizeof(Size));
  DELETE(message);
  list< GMessage * > chunks = mix->chopper->CreateChunks(message);
 
  while( ! chunks.empty() ) 
  {
    (chunks.front())->DebugHeaders(logFile, "What is in padding chunk?");
    message = mix->msgCreator->CreateChunk(chunks.front());
    mix->paddingGeneratorToPadder->Append(message);
    chunks.pop_front();
  }
}
