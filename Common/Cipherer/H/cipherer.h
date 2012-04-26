#ifndef CIPHERER_H
#define CIPHERER_H

class Cipherer;

  // has to be the FIRST include !
#include "./../../H/common.h"
#include "./../../Debugable/H/debugable.h"

#include "./../../GMessage/H/gMessage.h"

  /// We assume DES symetric encryption only for now, for simplicity.
#define SYM_KEY_LEN 8
  /// Default modulus len for whole Eternity Service for now.
#define RSA_DEFAULT_MODULUS_BIT_LEN 512 
  /// Count RSA block length from RSA_DEFAULT_MODULUS_BIT_LEN
#define RSA_BLOCK_LEN (((RSA_DEFAULT_MODULUS_BIT_LEN - 1) \
                        / 8 ) + 1)
  /**
   * According to PKCS1 standard, RSA block needs these bytes to be 
   * unused by user.
   */
#define RSAREF_RESERVED_BYTES 11 

  /** 
   * Encryption flags for asymetric coding for using only one method
   * for both dectryption and encryption.
   */
  /*@{*/
#define ENCRYPT_FLAG 1 
#define DECRYPT_FLAG 0 
  /*@}*/

  /// do not use magic numbers, please
#define BITS_IN_BYTE 8 

/**
 * Class that handles encryption algorithms.
 *
 * It implements various algorithms like DES, RSA, MD5 ...
 * All the data passed to it should be in a message field (MsgField).
 * The data returned is also a message field. (data + length)
 *
 * @author  Pechy
 * @see     Translator Client Bank MessageCreator Eso 
 */
class Cipherer:public Debugable
{
  public:

  /**@name methods */
  /*@{*/
    Cipherer(LogFile * ciphererLogFile);
    ~Cipherer();

      /**
       * General methods for asymetric encryption and decription,
       * keys have type of the key information inside. For now, only
       * RSA algorithm is implemented.
       */
      /*@{*/
    Err DecryptWithPrivateKey(GMessage *privateKey, MsgField * eData,
                                                    MsgField ** dData);
    Err EncryptWithPublicKey(GMessage *publicKey,   MsgField * pData,
                                                    MsgField ** eData);
    Err DecryptWithPublicKey(GMessage *publicKey,   MsgField * eData,
                                                    MsgField ** dData);
    Err EncryptWithPrivateKey(GMessage *privateKey, MsgField * pData,
                                                    MsgField ** eData);
      /*@}*/

      /**
       * Hi-level methods for asymetric encryption of symetric keys. 
       * Symetric key can of any type, but symKey->SizeOf() can't be 
       * bigger than (RSA_BLOCK_LEN - RSAREF_RESERVED_BYTES).
       *
       * @see cipherer.h
       */
      /*@{*/
    Err EncryptKeyWithPublicKey(       GMessage *publicKey, 
                                       GMessage *pSymKey, 
                                       MsgField **eSymKey);

    Err DecryptKeyWithPrivateKey(	GMessage *privKey,
                                        MsgField *eSymKey, 
                                        GMessage **dSymKey);
      /*@}*/

      /**
       * General methods for symetric encryption and decryption,
       * keys have type of the key information inside. For now, only
       * DES algorithm is implemented (because of RSAEuro toolkit).
       */
      /*@{*/
    Err DecryptWithSymetricKey(GMessage *symKey,    MsgField * eData,
                                                    MsgField ** dData);
    Err EncryptWithSymetricKey(GMessage *symKey,    MsgField * pData,
                                                    MsgField ** eData);
    Err DecryptWithSymetricKeys(GMessage *symKeys,  MsgField * eData,
                                                    MsgField ** dData);
    Err EncryptWithSymetricKeys(GMessage *symKeys,  MsgField * pData,
                                                    MsgField ** eData,
						    int useAlsoKeyOne = 1);
      /*@}*/

      /**
       * General methods for symetric sealing and opening digital
       * envelopes. For now, only RSA, DES and MD5 is implemented.
       */
      /*@{*/
    Err Seal(GMessage *privateKeyInfo, GMessage *publicKeyInfo,
             MsgField *dataIn, MsgField **dataOut,
             GMessage *parameters = NULL);
    Err OpenSealed(GMessage *privateKeyInfo, GMessage *publicKeyInfo,
                   MsgField *dataIn, MsgField **dataOut);
      /*@}*/

      /**
       * General methods for signing, verification and digesting.
       * Only RSA and MD5 is implemented for now.
       */
      /*@{*/
    Err MessageDigest(MsgField *dataToDigest, MsgField **digest, 
                      GMessage *parameters = NULL);
    Err Sign(GMessage *privateKey, MsgField *dataToSign, MsgField **signature, 
             GMessage *paraneters = NULL);
    Err VerifySignature(GMessage *publicKey, MsgField *dataToVerify,
                        MsgField *sig, GMessage *parameters = NULL);

      /*@}*/

      /**
       * General methods for key generation of symetric and asymetric
       * keys. For now, only RSA and DES keys are implemented.
       */
      /*@{*/
    GMessage *GenerateSymetricKey(GMessage *parameters = NULL);
    GMessage *GenerateAsymetricKey(GMessage *parameters = NULL);
      /*@}*/

      /// Auxiliary public methods, not too efficient, but fully functional.
    unsigned char GenerateRandomByte();

      /// Auxiliary method for MessageCreator's methods.
    GMessage *ReturnGMsgFullOfSymKeys(Size hopCount, GMessage *params = NULL);

  /*@}*/

  private:

  /**@name methods */
  /*@{*/
      /// Used by methods for symetric enctryption.
    Err ProcessWithSymetricKey(GMessage * symKey,   MsgField * inData, 
                                                    MsgField ** outData,
                                                    int actionFlag);

      /**
       * Used by ProcessWithSymetricKey(). Inside, RSAEuro's DES
       * implementation is used.
       */
    Err ProcessWithSymetricKey_RawVersion(GMessage *symKey, char *dataIn,
                                          Size dataInLen, char **dataOut,
					  Size *dataOutLen, int flag);

      /// Process one PKCS1 block. Methods use RSAEuro's RSA implementation.
      /*@{*/
    Err EncryptBlockWithPublicKey(	GMessage *pubKey, char *dataIn, 
                                  	Size dataInLen, char **dataOut,
				  	Size *dataOutLen);
    Err EncryptBlockWithPrivateKey(	GMessage *privKey, char *dataIn, 
                                  	Size dataInLen, char **dataOut,
				  	Size *dataOutLen);
    Err DecryptBlockWithPublicKey(	GMessage *pubKey, char *dataIn, 
                                  	Size dataInLen, char **dataOut,
				  	Size *dataOutLen);
    Err DecryptBlockWithPrivateKey(	GMessage *privKey, char *dataIn, 
                                  	Size dataInLen, char **dataOut,
				  	Size *dataOutLen);
      /*@}*/
  /*@}*/
};
#endif
