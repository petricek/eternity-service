#ifndef ESO_LABELS_H
#define ESO_LABELS_H

/**Eso timeout commands*/
/*@{*/
  /// Allocation timed out.
#define TO_ALLOC "to_allocation"
  /// Finder's forwarding tables obsolete.
#define TO_DEL_FINDER_TBL "to_del_finder_tbl"
  /// Time to check count of ACs of certian type.
#define TO_CHECK_AC_COUNT "to_check_AC_count"
//#define TO_GENERATE_AC "to_generate_AC".
  /// Time to ask a bank for money.
#define TO_PAYMENT "to_payment"
  /// ???.
#define TO_CURR_PAY "to_curr_pay"
  /// It is time to start synchronization.
#define TO_TIME_TO_SYNCHRONIZE "to_time_to_synchronize"
  /// Response to synchronization messages timed out.
#define TO_RESPONSE_TO_SYNCHRONIZE "to_response_to_synchronize"
/*@}*/

/**Eso synchronization commands*/
/*@{*/
  /// Request for synchronization.
#define CMD_SYNCHRONIZER_REQUEST "cmd_syncronizer_request"
  /// Response to request for synchronization.
#define CMD_SYNCHRONIZER_RESPONSE "cmd_syncronizer_response"
/*@}*/

/**Commands from and to Service*/
/*@{*/
  // Mix-Eso test.
//#define CE_TEST "Test"
  /// Client's request for storage of a file.
#define CE_REQ_FOR_STOR "ce_request_for_storage"
  /// Eso's reply to CE_REQ_FOR_STOR.
#define EC_REPLY_ALLOC  "ec_reply_allocation"
  /// Client sending data to store.
#define CE_DATA_TO_STOR "ce_data_to_store"
  /// Eso's reply to CE_DATA_TO_STOR .
#define EC_REPLY_STOR   "ec_reply_storage"
  /// Client's search by keywords.
#define CE_REQ_FOR_FIND "CE_RequestForFind"
  /// Eso's reply to CE_REQ_FOR_FIND.
#define EC_REPLY_HEADER "EC_ReplyHeader"
  /// Client's request to download a file.
#define CE_REQ_FOR_FILE "CE_RequestForFile"
  /// Eso sending a file.
#define EC_REPLY_FILE "EC_ReplyFile"
/*@}*/
 
#endif
