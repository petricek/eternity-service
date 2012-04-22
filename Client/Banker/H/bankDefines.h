#ifndef CLIENT_BANK_DEFINES_H
#define CLIENT_BANK_DEFINES_H

#include "./../../../Common/Payments/H/payDefines.h"

  /**@name BankerDefines */
  /*@{*/

  /// Banker's files dir
  /*@{*/
#define BANKER_FILES_DIR      "Banker/"
  /*@}*/

  /// Bankers' files
  /*@{*/
#define PAY_PLAN_TAB          BANKER_FILES_DIR"payPlan.tab"
#define BANK_PPLAN_TAB        BANKER_FILES_DIR"bankPPlan.tab"
#define ESO_TMP_TAB           BANKER_FILES_DIR"esoTmp.tab"
#define BANK_TMP_TAB          BANKER_FILES_DIR"bankTmp.tab"
  /*@}*/

  /// Banker's table fields' names
  /*@{*/
#define TF_BANK_P_PLAN        "BankPPlan"
  /*@}*/

  /*@}*/

#endif
