#ifndef LABELS_H
#define LABELS_H

  /**@name Values for config file options */
  /*@{*/
  /// So that we can reconstruct some situations when debugging
#define CFG_DEBUG_RANDOM_LONG_SEED "debug_random_long_seed"
  /// For debugging purposes
#define CFG_DEBUG_ADD_HOPS_TO_SEND_DATA "debug_add_hops_to_send_data"
  /// For debugging purposes
#define CFG_DEBUG_ONION_LENGTH "debug_onion_length"
  /// For debugging purposes
#define CFG_DEBUG_LOOPS "debug_loops"
  /// Where are the certificates stored
#define CFG_CERTIFICATES_DIRECTORY "certificates_directory"
  /// For padding sending - is padding algorithm dependet
#define CFG_START_INTERVAL_LENGTH "start_interval_length"
  /// The start rate at which messages are sent
#define CFG_START_LOAD "start_load"
  /// Filename in which to store the current service certificate
#define CFG_CERTIFICATE "certificate"
  /// Both asymetric keys - public and private
#define CFG_ASYMETRIC_KEY_PAIR "asymetric_key_pair"
  /// Number of Mix certificates to keep
#define CFG_MIX_CERTIFICATES_POOL_SIZE "mix-certificates_pool_size"
  /// Certificate of Mix
#define CFG_MIX_CERTIFICATE "mix_certificate"
  /// With how many hops to protect itself by default
#define CFG_ADD_HOPS "default_Add-Hops"
  /// How long onion to generate by default
#define CFG_DEFAULT_ONION_LENGTH "default_onion_length"
  /// In what file do we write debug messages, warnings and errors?
#define CFG_LOGFILE "logfile"
  /// Private key of Mix
#define CFG_MIX_PRIVATE_KEY "mix_private_key"
  /// Name of this service - used to identify Six
#define CFG_NAME "name"
  /// Port where we listen to Mix
#define CFG_FROM_MIX_PORT "from_mix_port"
  /// Port where we listen to Six
#define CFG_FROM_SIX_PORT "from_six_port"
  /// Where to listen for incoming messages
#define CFG_INCOMING_PORT "incoming_port"
  /// Where to send outgoing messages
#define CFG_OUTGOING_PORT "outgoing_port"
  /// Chunk has always fixed size as it goes through the Net 
#define CFG_FIXED_CHUNK_SIZE "fixed_chunk_size"
  /// On how many Eso's to store the file by default
#define CFG_DEFAULT_REDUNDANCY "default_redundancy"
  /// How many Eso's to ask if they have the file
#define CFG_NUMBER_OF_ESOS_TO_QUERY "number_of_esos_to_query"
  /// Address suffix
#define CFG__ADDRESS "_address"
  /// Address of mix (IP or hostname)
#define CFG_MIX_ADDRESS "mix_address"
  /// Six prefix for lookups in configuration file
#define CFG_SIX_ "six_"
  /// From prefix for lookups in configuration file
#define CFG_FROM_ "from_"
  /// Port suffix for lookups in config.
#define CFG__PORT "_port"
  /// Port of Mix
#define CFG_MIX_PORT "mix_port"
  /// Protocol version used to communicate between Mix and Six
#define CFG_MIX_SIX_PROTOCOL_VERSION "mix_six_protocol_version"
  /// Maximum count of access certificates that lead to this Eso, Eso  should hold
#define CFG_THIS_ESO_AC_COUNT "this_eso_AC_count"
  /*@}*/

  /**@name AC types */
  /*@{*/
  /// Certificate type Mix
#define ACTYPE_MIX      "mix"     
  /// Certificate type Bank
#define ACTYPE_BANK     "bank"     
  /// Certificate type Acs
#define ACTYPE_ACS      "acs"     
  /// Certificate type Eso
#define ACTYPE_ESO      "eso"     
  /// General certificate type 
#define ACTYPE_OTHER    "other"     
  /// Special certificate type of this Six
#define ACTYPE_THIS_SIX "this_Six"
  /*@}*/

  /**@name GMessage labels */
  /*@{*/
#define GM_MIX_CERTIFICATE_TUPLE "Mix-Certificate-Tuple"
  /// 1st recipient that should get this message
#define GM_RECIPIENT_1 "Recipient-1"
  /// 2nd recipient that should get this message
#define GM_RECIPIENT_2 "Recipient-2"
  /// Where it came from?
#define GM_ORIGIN "Origin"
  /// Access certificate ID
#define GM_ACID "ACID"
  /// Address - IP or symbolic or special name
#define GM_ADDRESS "Address"
  /// Number of hops to add on onion
#define GM_ADD_HOPS "Add-Hops"
  /// Which asymetric cipher is used - default is RSA 
#define GM_ASYMETRIC_CIPHER_TYPE "Asymetric-Cipher-Type"
  /// Which asymetric cipher is used 
#define GM_ASYMETRIC_CIPHER_VERSION "Asymetric-Cipher-Version"
  /// Body of message
#define GM_BODY "Body"
  /// Certificate of a Six or Mix
#define GM_CERTIFICATE "Certificate"
  /// Certificates of a Sixes or Mixes
#define GM_CERTIFICATES "Certificates"
  /// 1st certificate of a Six or Mix
#define GM_CERTIFICATE_1 "Certificate-1"
  /// Type of certificate - mix, acs, eso ...
#define GM_CERTIFICATE_TYPE "Certificate-Type"
  /// A piece of data
#define GM_CHUNK "Chunk"
  /// Unique ID for message chunk that identifies its position in message
#define GM_CHUNK_ID "Chunk-ID"
  /// Specifies the action that ougth to be taken
#define GM_COMMAND "Command"
  /// Number of items
#define GM_COUNT "Count"
  /// Message data
#define GM_DATA "Data"
  /// Size of message data
#define GM_DATA_SIZE "Data-Size"
  /// For debugging purposes - should be 0 in real world
#define GM_DEBUG "Debug"
  /// Where to send this message?
#define GM_DESTINATION "Destination"
  /// Date after which the GMessage is not vlid anymore
#define GM_EXPIRATION "Expiration"
  /// If we see this in a message then we can be sure that it is the last mix and we should process it
#define GM_FINAL_STOP "Final-Stop"
  /// Number of hops an onion should include
#define GM_HOP_COUNT "Hop-Count"
  /// ID of a message this is a reply to
#define GM_IN_REPLY_TO_TID "In-Reply-To-TID"
  // the string has to be short because of limited chunk len we can encode in RSA functions
  /// Initialization vector used in CBC mode of DES
#define GM_INITIALIZATION_VECTOR "I-V"

  ///@name Labels used when specifying file characteristics
  /*@{*/
    /// Finder file identificator - used as result of file searches and as a download identificator
#define GM_FFID "FFID"
    /// File identificator - used to manage file in Eso
#define GM_FID "FID"
    /// Size of file in bytes
#define GM_FILE_SIZE "File-Size"
    /// Relative time for which the file should be stored
#define GM_STORAGE_TIME "Storage-Time"
  /*@}*/
  
  ///@name Labels used when searching for file
  /*@{*/
  /// A word describing the file
#define GM_KEYWORD "Keyword"
  /// GMessage of keywords
#define GM_KEYWORDS "Keywords"
  /// Maximum accepted size
#define GM_MAX_SIZE "Max-Size"
  /// Minimum accepted size
#define GM_MIN_SIZE "Min-Size"
  /// Depth of the search to perform
#define GM_SEARCH_DEPTH "Depth"
  /// Width of the search to perform
#define GM_SEARCH_WIDTH "Width"
  /// Header info
#define GM_HEADER "Header"
  /*@}*/

  /// Unique identification of message.
#define GM_MESSAGE_ID "Message-ID"
  /// Total length of message (used by Linker)
#define GM_MESSAGE_LENGTH "Message-Length"
  /// Mix.
#define GM_MIX "Mix"
  /// Mix certificate.
#define GM_MIX_CERTIFICATE "Mix-Certificate"
  /// A GMessage of Mix certificates.
#define GM_MIX_CERTIFICATES "Mix-Certificates"
  /// First Mix certificate.
#define GM_MIX_CERTIFICATE_1 "Mix-Certificate-1"
  /// Second Mix certificate.
#define GM_MIX_CERTIFICATE_2 "Mix-Certificate-2"
  /// Number of Mix certificates in GMessage.
#define GM_MIX_CERTIFICATE_COUNT "Mix-Certificate-Count"
  /// First pair of Mix certificates.
#define GM_MIX_CERTIFICATE_TUPLE_1 "Mix-Certificate-Tuple-1"
  /// Name.
#define GM_NAME "Name"

  ///@name When is the certificate valid?
  /*@{*/
  /// Last date of validity.
#define GM_NOT_VALID_AFTER "Not-Valid-After"
  /// First day of validity.
#define GM_NOT_VALID_BEFORE "Not-Valid-Before"
  /*@}*/

  /// Lable for random string used to proove wheter a money requesting Eso is really storing a file
#define GM_OAUTH "O-Auth"
  /// A structure that represents encrypted routing information that is used in onion routing
#define GM_ONION "Onion"
  /// GMessage containing additional parameters.
#define GM_PARAMETERS "Parameters"
  /// schedule that says when and how much should be paid
#define GM_PAY_PLAN "Payment-Plan"
  /// u_short specifying the number of port to which to connect
#define GM_PORT "Port"
  /// Private part of asymetric key
#define GM_PRIVATE_KEY "Private-Key"
  /// includes private key and its version
#define GM_PRIVATE_KEY_INFO "Private-Key-Info"
  /// Version of protocol used
#define GM_PROTOCOL_VERSION "Protocol-Version"
  /// Public part of asymetric key
#define GM_PUBLIC_KEY "Public-Key"
  /// Includes public key and its version
#define GM_PUBLIC_KEY_INFO "Public-Key-Info"
  /// Random data
#define GM_RANDOM "Random-String"
  /// Address and port of recipient machine
#define GM_RECIPIENT "Recipient"
  /// Access certificate which to reply to 
#define GM_REPLY_TO_AC "Reply-To-AC"
  /// Message ID that should be included in reply as GM_IN_REPLY_TO_TID
#define GM_REPLY_TO_TID "Reply-To-TID"
  /// ID of the request that should be returned so that we know what question is the answer for
#define GM_REQUEST_ID "Request-ID"
  /// Encrypted part of onion that hides all the remaining hops
#define GM_REST_OF_PATH "Rest-Of-Path"
  /// What happened?
#define GM_RESULT "Result"
  /// GMessage containing symetric data key and its version
#define GM_SYMETRIC_DATA_KEY_INFO "Symetric-Data-Key-Info"
  /// GMessage containing symetric path key and its version
#define GM_SYMETRIC_PATH_KEY_INFO "Symetric-Path-Key-Info"
  /// GMessage containing symetric data keys and their versions
#define GM_SYMETRIC_DATA_KEY_INFOS "Symetric-Data-Key-Infos"
  /// Key for a symetric cipher (DES)
#define GM_SYMETRIC_KEY "Symetric-Key"
  /// Number of symetric keys
#define GM_SYMETRIC_KEY_COUNT "Symetric-Key-Count"
  // the string has to be short because of limited chunk len we can encode in RSA functions
  /// Type of symetric cipher - usually DES
#define GM_SYMETRIC_CIPHER_TYPE "S-C-T"
  /// GMessage containing symetric key and its version
#define GM_SYMETRIC_KEY_INFO "Symetric-Key-Info"
  /// Public key of trusted 'computing base'
#define GM_TCB_PUBLIC_KEY "TCB-Public-Key"
  /// Transaction ID.
#define GM_TID "TID"
  /// Access certificate of an Acs to which to send data.
#define GM_TO_AC "To-AC"
  /// Linker and Chopper info about number of chunks in message
#define GM_TOTAL_CHUNKS "Total-Chunks"
  /// Version
#define GM_VERSION "Version"
  /// Helper header for directing message inside Mix.
#define GM_X_DESTINATION "X-Destination"

  ///@name GMessage fields for sealing and opening
  /*@{*/
  /// Encrypted data
#define GM_ENCRYPTED_CONTENT "Encrypted-Content"
  /// Encrypted key
#define GM_ENCRYPTED_KEY "Encrypted-Key"
  /// Encrypted result of a hash function over data.
#define GM_ENCRYPTED_SIGNATURE "Encrypted-Signature"
  /*@}*/

  /// Initialization vector for CBC mode
#define GM_IV "IV"
#define GM_FAM "FAM"
  /// Key used to encrypt for transport security.
#define GM_TRANSPORT_KEY "Transport-Key"
  /// File find ID.
#define GM_FFID "FFID"
  /// Data of file to store.
#define GM_FILE "File"
  /*@}*/

  // AC Manager fields
  /**@name Certificate fields */
  /*@{*/
  /// Certificate type
#define ACFLD_TYPE             GM_CERTIFICATE_TYPE
  /// Certificate unique ID
#define ACFLD_ACID             GM_ACID
  /// Certificate Public key
#define ACFLD_PUB_KEY          GM_PUBLIC_KEY
  /// Expiration date of certificate
#define ACFLD_EXPIRATION       GM_EXPIRATION
  /// Mix certificate included in access certificate
#define ACFLD_MIX_CERTIFICATE  GM_ONION
  /// Public key of trusted computing base
#define ACFLD_TCB_PUB_KEY      GM_TCB_PUBLIC_KEY
  /// Valid from
#define ACFLD_NOT_VALID_BEFORE GM_NOT_VALID_BEFORE
  /// Valid till
#define ACFLD_NOT_VALID_AFTER  GM_NOT_VALID_AFTER
  /*@}*/

  /**@name GMessage fields' values */
  /*@{*/
  /// This Mix
#define LBL_MIX_LOCAL		"mix_local"
  /// Remote Mix
#define LBL_MIX_REMOTE		"mix_remote"
  /// Mix
#define LBL_MIX			"mix"
  /// Recipient is Six named acs
#define LBL_ACS 		"acs"
  /// Recipient is Six named eso
#define LBL_ESO 		"eso"
  /// Recipient is Six named client
#define LBL_CLIENT 		"client"
  /// Positive reply - yes
#define LBL_YES			"yes"
  /// Helper label
#define LBL_NO_DEBUG_NOTE	"No debug for now!"
  /// Protocol version used to communicate between Mix and Six
#define LBL_MIXSIX_PROTOCOL_VERSION_BETA "mix_six_pv_beta"
  /// Esos' protocol
#define LBL_ESO_OTHERS_PROTOCOL_VERSION_BETA "eso_others_pv_beta"
  /*@}*/

#endif
