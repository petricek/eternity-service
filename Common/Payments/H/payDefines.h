#ifndef PAY_DEFINES_H
#define PAY_DEFINES_H

  /**@name GMessage fields' names*/
  /*@{*/
#define GM_PAYMENT_PLAN       "Payment-Plan"
#define GM_PAYMENT_ID         "Payment-ID"
#define GM_SUCCESS            "Success"
#define GM_O_AUTHENTIFICATION "O-Authentification"
#define GM_S_AUTHENTIFICATION "S-Authentification"
#define GM_AMOUNT             "Amount"
#define GM_MAC                "MAC"
#define GM_TIME               "Time"
#define GM_REASON_OF_FAIL     "Reason-Of-Fail"
#define GM_BANK_ACID          "Bank-ACID"
#define GM_ACCOUNT            "Account"
  /*@}*/

  /**@name Table fields' names*/
  /*@{*/
#define TF_TIME               GM_TIME
#define TF_TID                "TID"
#define TF_PAY_ID             "Pay-ID"
#define TF_BANK_ACID          GM_BANK_ACID
#define TF_BANK_AC            "Bank-AC"
#define TF_AMOUNT             GM_AMOUNT
#define TF_O_AUTH             "O-Auth"
#define TF_S_AUTH             "S-Auth"
#define TF_SEAL_S_AUTH        "Seal-S-Auth"
#define TF_MAC                GM_MAC
  /*@}*/

  /**@name Values of 'success' field*/
  /*@{*/
#define OK_MORE_PAYS          3
#define UNKNOWN_PAY_ID        4
#define BAD_TIME              5
#define BAD_MAC               6
#define BAD_S_AUTH            7
#define OTHER                 20
  /*@}*/

  /**@name Value of "bad" or missing ID*/
  /*@{*/
#define BAD_ID                0
  /*@}*/

#endif
