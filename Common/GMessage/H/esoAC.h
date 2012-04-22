#ifndef ESOAC_H
#define ESOAC_H

#include "./../../GMessage/H/gMessage.h"
#include "./../../H/labels.h"

  ///default AC validation is 14 days
#define CFG_AC_VALIDATION "00:00:14:00:00:00"

/**
 * Eso access certificate.
 *
 * GMessage encapsulating Eso access certificate.
 *
 * @author  Pavel
 * @see     Eso, GMessage
 */
class EsoAC: public GMessage
{
public:
  /**@name methods */
  /*@{*/
 /// constructor
 Err SetFields(Char *certType, 
               ID certID, 
               MsgField *mixCert,
               GMessage *pubKey,
               GMessage *tcbPubKey,
               ESTime *notValidBefore = NULL,
               ESTime *notValidAfter = NULL);
 /// Returns type of certificate ( eso )
 Char* GetType();
  /*@}*/
};

#endif
