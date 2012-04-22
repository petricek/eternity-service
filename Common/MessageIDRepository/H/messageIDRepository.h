#ifndef MESSAGEIDREPOSITORY_H
#define MESSAGEIDREPOSITORY_H

class MessageIDRepository;

#include <string>
#include <map>

#include "./../../Debugable/H/debugable.h"
#include "./../../Registerable/H/registerable.h"

/**
 * Registrator record.
 *
 * Contains information about the entity that sent a request with the Request ID
 * object or six name -> RequestID
 *
 * @author  Venca
 * @see     Majordomo, MessageIDRepository
 */
class RegistratorRecord
{
  public:
  /**@name attributes */
  /*@{*/
    /// Pointer to object
    Registerable * registrator;
    /// Six name
    string destination;
  /*@}*/
};


/**
 * Message Id repository.
 *
 * Remembers the pairs (object or Six) and request ID so that reply can send reply to  
 * the correct requestor.
 *
 * @author  Venca
 * @see     Majordomo, RegistratorRecord
 */
class MessageIDRepository : public Debugable
{
  protected:

  /**@name attributes */
  /*@{*/
    map<ID, RegistratorRecord *, less<ID> > registrators;
  /*@}*/

  public:
  /**@name methods */
  /*@{*/
    /// Constructor
    MessageIDRepository(LogFile * messageIDRepositoryLogFile);
    /// Get who is registered
    RegistratorRecord * GetRegistratorRecord(ID messageID);
    /// Get who is registered
    Registerable * GetRegistrator(ID messageID);
    /// Register for Imessage ID
    Err Register(ID messageID, Registerable * registrator, string destination = "");
  /*@}*/

};
#endif
