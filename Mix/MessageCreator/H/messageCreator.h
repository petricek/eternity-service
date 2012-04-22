#ifndef MESSAGE_CREATOR_H
#define MESSAGE_CREATOR_H

class MessageCreator;

#include "./../../../Common/Debugable/H/debugable.h"
#include "./../../../Common/CertificateRepository/H/certificateRepository.h"
#include "./../../../Common/Cipherer/H/cipherer.h"
#include "./../../../Common/KeyManager/H/keyManager.h"

  // How many recipients are in one onion peel.
#define RECIPIENTS_IN_ONE_PEEL 2 
#define NO_PARAM -1

  // this can be *safely* changed
#define MAX_HOPS_TO_ASK_FOR 5

  /**
   * Rough estimation about the maximum length of onion bottom and one
   * onion peel. Estimation is oversized a bit.
   */
  /*@{*/
#define BOTTOM_ONION_MAX_LENGTH_ESTIMATION 2000
#define ONE_ONION_PEEL_MAX_LENGTH_ESTIMATION 700
  /*@}*/

  /**
   * Because of fixed size of the ,,rest of path length'', we have to limit
   * the onion to (aproximately) half of FULL_REST_OF_PATH_LEN.
   *
   * @see FULL_REST_OF_PATH_LEN
   */
#define HALF_REST_OF_PATH_LEN ( ( (BOTTOM_ONION_MAX_LENGTH_ESTIMATION \
                                   + (ONE_ONION_PEEL_MAX_LENGTH_ESTIMATION \
		                      * MAX_HOPS_TO_ASK_FOR) \
                                  ) / 8 \
		                ) * 8 \
		              )

  /**
   * Due to security reason, onion (,,the rest of path'') should be always
   * the same length. The enemy cat't estimate how many hops is chunk still
   * to go. So the macro is the fixed onion length.
   *
   * @see HALF_REST_OF_PATH_LEN
   */
#define FULL_REST_OF_PATH_LEN ( ( (HALF_REST_OF_PATH_LEN \
				   + (ONE_ONION_PEEL_MAX_LENGTH_ESTIMATION \
				      * MAX_HOPS_TO_ASK_FOR) \
				  ) / 8 \
			        ) * 8 \
			      )

/**
 * Message Creator.
 *
 * Specializes in creation of messages, chunks onions and similar things.
 * Understands structure of onions and messages.
 *
 * @author  Pechy
 * @see     PaddingGenerator, Mix
 */
class MessageCreator : public Debugable
{
  protected:

  /**@name attributes*/
  /*@{*/
      /// We ask him for Mix's asymetric keys.
    KeyManager * keyManager;
      /// Needed when creating chunks and onions.
    Cipherer * cipherer;
      /// Repository containing certificates.
    CertificateRepository * certificateRepository;
      /// Configuration file.
    ConfigFile * configFile;
  /*@}*/

  public:

  /**@name methods */
  /*@{*/
      /// Constructor.
    MessageCreator(ConfigFile * mcConfigFile, 
                   LogFile * messageCreatorLogFile, 
                   CertificateRepository * certificateRepositorya,
                   Cipherer * cipherer,
                   KeyManager * keyManager);
      /// Creates a chunk in a format that is suitable for Sender.
    GMessage *CreateChunk(GMessage *message);
      /// Creates an onion. The recipient is in the bottom.
    GMessage *CreateOnion(GMessage *request);
      /// For the need of Mix only. When asking for certificates for Acs.
    GMessage *CreateOnionFromMixCertificate(GMessage *request);
      /// Desctructor.
    ~MessageCreator();
  /*@}*/

  private:

  /**@name methods2 */
  /*@{*/
      /// Returns Mix's certificates that we use for onion or chunk creation.
    Err ReturnGMsgFullOfMixCertifs(     int tupleWidth, 
                                        Size hopCount,
                                        GMessage **cerfifsGMsg); 

      /**
       * Creates the onion bottom with symetric keys and recipient 
       * identification.
       */
    GMessage *CreateOnionBottom(        char  *xOrigin, GMessage *symKeys,
                                        GMessage **pubKeyInfo); 

    void PushMyCertifOnToCertifsGMessage(       int tupleWidth, 
                                                GMessage *certifsGMsg);

      /// Creates recipient's GMessage that is in each onion peel.
    GMessage *MakeRecipientGMessage(    GMessage *mixCertif, 
                                        GMessage *SymKey);

       /// Add Size type integer before the data (it's denotes the data length).
    void AnnotateWith4ByteLength(       GMessage *onion,
                                        char **onionBottomStream,
                                        Size *onionBottomStreamLen);

       /// Add additional peels on the top of an onion.
    Err MakeMoreOnionLayers(           GMessage *properties,
                                        GMessage *callBackOnionInfo,
                                        GMessage *onion);

       /// Returns certificates and symetric keys for onion and chunk creation.
    Err ReturnCallBackOnionInfo(        GMessage *onionProperties,
                                        GMessage **callBackOnionInfo);

    Err AddLocalCertifs(               GMessage *callBackOnionInfo);

      /// Padd ,,rest of path'' field to the original length.
    char *PaddOnionRestOfPath(char *restOfPath, Size restOfPathLen, 
                              Size requestedLen);
  /*@}*/
};
#endif
