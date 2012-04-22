#ifndef TRANSLATOR_H
#define TRANSLATOR_H

  // has to be the FIRST include !
#include "./../../../Common/H/common.h"

#include "./../../../Common/Runable/H/runable.h"
#include "./../../../Common/Cipherer/H/cipherer.h"
#include "./../../../Common/MessageQueue/H/messageQueue.h"
#include "./../../../Common/KeyManager/H/keyManager.h"

#include "./../../MixOffspring/H/mixOffspring.h"

/**
 * Message translator.
 *
 * Decrypts one peel from message that is sent using onion routing.
 * In the decrypted peel it finds the information necessary to decide
 * whether the message is at its destination or whether it should be
 * send to further to another Mix.
 *
 * @author  Venca, Pechy
 * @see     Cipherer, MessageCreator
 */
class Translator: public Runable, public MixOffspring
{

  /**@name methods */
  /*@{*/
  public:
    Translator(Mix * parent, LogFile * translatorLogFile);
    char *PaddRestOfPath(char *path, Size pathLen, Size peelLen);
    virtual ~Translator();

    virtual void * Run(void *);
  /*@}*/
};
#endif
