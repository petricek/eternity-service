  // has to be the FIRST include !
#include "./../../Common/H/common.h"

#include <stdio.h>
#include <unistd.h>

#include "./H/padder.h"

#define START_REST_TIME 100
#define DEFAULT_INTERVAL_LENGTH 1000000

/**
 * Padder constructor.
 *
 * Initializes himself with some information taken from the config file.
 *
 * @param   parent Mix object.
 * @param   padderLogFile Log file.
 * @author  Venca
 * @see     padding discussion, traffic analysis discussion, ~Padder()
 */
Padder::Padder(Mix * parent, LogFile *padderLogFile)
  :Runable(padderLogFile), MixOffspring(parent), Schedulable(), 
   Shutdownable(parent->killer)
{
  intervalLength = 
    atoi(mix->configFile->GetValue(CFG_START_INTERVAL_LENGTH).c_str());

  if(intervalLength == 0)
    intervalLength = DEFAULT_INTERVAL_LENGTH;

  load = atoi(mix->configFile->GetValue(CFG_START_LOAD).c_str());
}

/**
 * Padder destructor.
 *
 * Does nothing for now, since Padder is stateless.
 *
 * @author  Venca
 */
Padder::~Padder()
{

}

/**
 * Padds the stream of chunks that goes through the Mix.
 *
 * Simple strategy (the simpler, the best -> see unix...) for now.
 *
 * @return
 * @author  Venca
 * @see     discussion on padding generation, traffic analysis discussion
 */
void *Padder::Run(void *)
{
  MaskAllSignals();

  GMessage *msg;

  WriteString(llDebug, __FILE__ ":%d:Entering Padder::Run()", __LINE__);

  /*
  for ( ; ; ) {
    // DBG;
    Size count = mix->chopperTranslatorToPadder->MsgCount();
    if(count >= load)
    {
      // DBG;
      // Send load messages
      for(Size i = load;i>0;i--)
      {
        mix->chopperTranslatorToPadder->GetRandomMsg(&msg);
        // DBG;
        mix->padderToSender->Append(msg);
      }
      // Speed up
      if(count > load )
      {
        load++;
      }
      // DBG;
    }
    else
    {
      for(Size i = count;i>0;i--)
      {
	mix->chopperTranslatorToPadder->GetRandomMsg(&msg);
	// DBG;
	mix->padderToSender->Append(msg);
	// DBG;
      }
      load--;
    }
    pthread_yield();
    usleep(intervalLength);
    sleep(2);
    pthread_yield();
  }*/
  for ( ; ; ) {
    mix->chopperTranslatorToPadder->GetMsg(&msg);
    mix->padderToSender->Append(msg);
  }
}

/**
 * Prepare for clean shutdown.
 *
 * This method should implement all the tasks necessary for
 * preparation for a clean shutdown like saving inconsistent data to
 * disk etc. Now it is empty as Padder is completely stateless as the
 * loss of some partially received messages is not fatal to the
 * system.
 *
 * @return  void
 * @author  Venca
 * @see     Shutdownable, Killer
 */
void Padder::Shutdown()
{

}


//-------------------------------------------------------------------------
// HandleEvent
// ~~~~~~~~~~~
//-------------------------------------------------------------------------
void Padder::HandleEvent(int /*eventID*/)
{

}
