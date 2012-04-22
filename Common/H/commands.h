#ifndef COMMANDS_H
#define COMMANDS_H

  /**@name commands */
  /*@{*/

  /**Mix commands */
  /*@{*/
  /// Test command
#define CMD_TEST "test"
  /// Command for mix to send data
#define CMD_SEND_DATA "send_data"
  /// Data arrived
#define CMD_DATA_RECEIVED "data_received"
  /// Request for certificates
#define CMD_GET_CERTIFICATES "get_certificates"
  /// Certificates arrived
#define CMD_CERTIFICATES_RECEIVED "certificates_received"
  /// Request for onion
#define CMD_GENERATE_ONION "generate_onion"
  /// Onion arrived
#define CMD_ONION_GENERATED "onion_generated"
  /// Request for publishing certificates to Acs
#define CMD_PUBLISH_CERTIFICATES "publish_certificates"
  /*@}*/


// test msg:
#define CE_TEST          "ce_test"

  /// file storage:
  /*@{*/
  /// Request if it is possible to store a file
#define CE_REQ_FOR_STOR  "ce_request_for_storage"
  /// If allocation was successful
#define EC_REPLY_ALLOC   "ec_reply_allocation"
  /// When sending the whole data for storage
#define CE_DATA_TO_STOR  "ce_data_to_store"
  /// If storage was successful
#define EC_REPLY_STOR    "ec_reply_storage"
  /// Client asks Bank to transfer money to Eso's account
#define CB_PAY           "cb_pay"
  /// 
#define BC_ANSW_PAY     "bc_reply_pay"
  /// 
#define CE_PAY_DONE      "ce_pay_done"
  /*@}*/

  /// finding file:
  /*@{*/
  /// Request for information about file
#define CMD_REQ_FOR_HEADERS  "req_for_headers"
  /// Headers arrived
#define CMD_ANSWER_HEADER    "answer_header"
  /// Give me that file
#define CMD_REQ_FOR_FILE     "req_for_file"
  /// Answer to request for file
#define CMD_ANSWER_FILE      "answer_file"
  /*@}*/

  /// payment to ESO:
  /*@{*/
#define EB_REQ_FOR_PAY   "eb_req_for_pay"
  /// Challlenge from Bank to Eso
#define BE_CHALL         "be_chall"
  /// Eso responds to Bank's challenge
#define EB_RESP_TO_CHALL "eb_resp_to_chall"
  /// Bank tells Eso if he guessed it right
#define BE_REPLY_TO_RESP "be_reply_to_resp"
  /*@}*/
  /*@}*/

#endif
