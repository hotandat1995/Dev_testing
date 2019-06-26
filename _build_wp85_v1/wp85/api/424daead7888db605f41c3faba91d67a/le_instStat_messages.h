/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_INSTSTAT_MESSAGES_H_INCLUDE_GUARD
#define LE_INSTSTAT_MESSAGES_H_INCLUDE_GUARD


#include "le_instStat_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_INSTSTAT_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_instStat_AddAppInstallEventHandler 0
#define _MSGID_le_instStat_RemoveAppInstallEventHandler 1
#define _MSGID_le_instStat_AddAppUninstallEventHandler 2
#define _MSGID_le_instStat_RemoveAppUninstallEventHandler 3


// Define type-safe pack/unpack functions for all enums, including included types

// Define pack/unpack functions for all structures, including included types


#endif // LE_INSTSTAT_MESSAGES_H_INCLUDE_GUARD