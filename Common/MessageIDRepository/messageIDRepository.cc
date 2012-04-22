#include "./H/messageIDRepository.h"

/**
 * Message ID repository.
 *
 * Initializes its parent classes.
 *
 * @param   messageIDRepositoryLogFile Log file to write to.
 * @return  
 * @author  Venca
 * @see     Registerable
 */
MessageIDRepository::MessageIDRepository(LogFile * messageIDRepositoryLogFile)
  :Debugable(messageIDRepositoryLogFile)
{
}

/**
 * Reveal which object is waiting for this message.
 *
 * Returns pointer to object that is waiting for this reply.
 * Deletes this object from its table.
 *
 *
 * @param   messageID   ID of message we are inspecting.
 * @return  Pointer to registerable object that registered for
 * this message. NULL if noone found.
 * @author  Venca
 * @see     Registerable
 */
Registerable * MessageIDRepository::GetRegistrator(ID messageID)
{
  Registerable * registrator = NULL;
  RegistratorRecord * registratorRecord = NULL;
  DBG;

  registratorRecord = registrators[messageID];
  DBG;
  registrators.erase(registrators.find(messageID));
  DBG;
  registrator = registratorRecord->registrator;
  DBG;

  return registrator;
}

/**
 * Reveal who is waiting for this message ID.
 *
 * Returns pointer to structure containing object that is waiting for
 * this reply or the name of Six willing to receive the message.
 * <p>
 * Deletes this structure from its table
 *
 * @param   messageID  ID of message we are deciding where to send
 * @return  pointer to structure that contains pointer to Registerable
 *          object or name of Six.
 * @author  Venca
 * @see     Registerable, Majordomo
 */
RegistratorRecord * MessageIDRepository::GetRegistratorRecord(ID messageID)
{
  DBG;
  RegistratorRecord * registratorRecord = NULL;

  registratorRecord = registrators[messageID];
  DBG;
  registrators.erase(registrators.find(messageID));
  DBG;

  return registratorRecord;
}

/**
 * Register object registrator for reply to messageID.
 *
 * Remembers pointer to requesting object together with ID  
 * so that is able to call them when reply arrives.
 *
 * @param   messageID     ID of message registrator is waiting for reply to.
 * @param   registrator   Object that wants to be notified when reply
 *                        arrives.
 * @param   destination   Six that wants to receive the reply.
 * @return  KO if someone is already registered for this ID.
 * @author  Venca
 * @see     Registerable
 */
Err MessageIDRepository::Register(ID messageID, Registerable * registrator, string destination)
{
  DBG;
  if(registrators.find(messageID) == registrators.end())
  {
    DBG;
    RegistratorRecord * regRec = new RegistratorRecord();
    DBG;
    regRec->registrator = registrator;
    DBG;
    regRec->destination = destination;
    DBG;
    registrators[messageID] = regRec;
    DBG;
    return OK;
  }
  else
  {
    DBG;
    return KO;
  }
}
