#ifndef ESO_BANK_DEFINES_H
#define ESO_BANK_DEFINES_H

#include "./../../../Common/Payments/H/payDefines.h"

  /**@name BankerDefines */
  /*@{*/

  /// banker's files dir
  /*@{*/
#define BANKER_FILES_DIR      "Banker/"
  /*@}*/

  /**@name bankers' files */
  /*@{*/
#define P_PLANS_TAB           BANKER_FILES_DIR"pPlans.tab"
#define CLIENT_P_PLAN_TAB     BANKER_FILES_DIR"clientPPlan.tab"
#define PAYMENTS_TAB          BANKER_FILES_DIR"payments.tab"
#define CURR_PAYS_TAB         BANKER_FILES_DIR"currPays.tab"
#define NOT_PAID_PAYS_TAB     BANKER_FILES_DIR"notPaidPays.tab"
#define PAY_ID_GEN_TAB        BANKER_FILES_DIR"payIdGen.tab"
  /*@}*/

  /**@name table fields' names */
  /*@{*/
#define TF_FILE_ID            "File-ID"
#define TF_STATE              "State"
#define TF_NUM_OF_TRIES       "Num-Of-Tries"
#define TF_REASON             "Reason"
  /*@}*/

  /// gMessage fields' names
#define GM_STATE              TF_STATE

  /// states of 'current payment'
  /**@name states of 'current payment' */
  /*@{*/
#define REQUEST               1
#define ANSWER                2
  /*@}*/

  /*@}*/
  
#endif
