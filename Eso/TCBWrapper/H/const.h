#ifndef TCBW_CONST_H
#define TCBW_CONST_H

///@name TCBWrapper paths
/*@{*/
#define TCBW_DIR_ROOT     "TCB/"
#define TCBW_DIR_TEMP     TCBW_DIR_ROOT"Temp/"
#define TCBW_DIR_STORAGE  TCBW_DIR_ROOT"Storage/"
/*@}*/

///@name  File names of files TCBWrapper uses
/*@{*/
#define TCBW_MASTER_KEY_FILE           TCBW_DIR_ROOT"master.key"
#define TCBW_RING_KEYS_FILE            TCBW_DIR_ROOT"ring.keys"
#define TCBW_STORAGE_TABLE_FILE        TCBW_DIR_ROOT"storage.tab"
#define TCBW_CERTIFICATES_TABLE_FILE   TCBW_DIR_ROOT"certificates.tab"
/*@}*/

///@name  Ring key names
/*@{*/
#define TCBW_RING_KEY_STORAGE        "storageTable ring key"
#define TCBW_RING_KEY_CERTIFICATES   "acTable ring key"
/*@}*/

///@name  StorageTable and  acTable field names
/*@{*/
#define TCBW_FLD_FID           "FID"
#define TCBW_FLD_FFID          "FFID"
#define TCBW_FLD_FILE_NAME     "FName"
#define TCBW_FLD_STORAGE_KEY   "Storage key"
#define TCBW_FLD_KEYWORDS      "Keywords"
#define TCBW_FLD_FILE_SIZE     "FSize"
#define TCBW_FLD_ACID          "ACID"
#define TCBW_FLD_AC_PUB_KEY    "ACPubKey"
#define TCBW_FLD_AC_PRIV_KEY   "ACPrivKey"
#define TCBW_FLD_TCB_PUB_KEY   "TCBPubKey"
#define TCBW_FLD_TCB_PRIV_KEY  "TCBPrivKey"
/*@}*/

#endif
