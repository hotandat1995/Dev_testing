
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_SECSTORE_COMMON_H_INCLUDE_GUARD
#define LE_SECSTORE_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_SECSTORE_PROTOCOL_ID "98c2a4dd482018a94c642390891be24f"
#define IFGEN_LE_SECSTORE_MSG_SIZE 8463



//--------------------------------------------------------------------------------------------------
/**
 * Maximum number of characters and byte storage size permitted for a secure storage item name.
 */
//--------------------------------------------------------------------------------------------------
#define LE_SECSTORE_MAX_NAME_SIZE 255

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
#define LE_SECSTORE_MAX_NAME_BYTES 256

//--------------------------------------------------------------------------------------------------
/**
 * Maximum number of bytes for each item in secure storage.
 */
//--------------------------------------------------------------------------------------------------
#define LE_SECSTORE_MAX_ITEM_SIZE 8192


//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_secStore_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_secStore_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_secStore_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Writes an item to secure storage. If the item already exists, it'll be overwritten with
 * the new value. If the item doesn't already exist, it'll be created.
 * If the item name is not valid or the buffer is NULL, this function will kill the calling client.
 *
 * @return
 *      LE_OK if successful.
 *      LE_NO_MEMORY if there isn't enough memory to store the item.
 *      LE_UNAVAILABLE if the secure storage is currently unavailable.
 *      LE_FAULT if there was some other error.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_secStore_Write
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL name,
        ///< [IN] Name of the secure storage item.
        const uint8_t* bufPtr,
        ///< [IN] Buffer containing the data to store.
        size_t bufSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Reads an item from secure storage.
 * If the item name is not valid or the buffer is NULL, this function will kill the calling client.
 *
 * @return
 *      LE_OK if successful.
 *      LE_OVERFLOW if the buffer is too small to hold the entire item. No data will be written to
 *                  the buffer in this case.
 *      LE_NOT_FOUND if the item doesn't exist.
 *      LE_UNAVAILABLE if the secure storage is currently unavailable.
 *      LE_FAULT if there was some other error.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_secStore_Read
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL name,
        ///< [IN] Name of the secure storage item.
        uint8_t* bufPtr,
        ///< [OUT] Buffer to store the data in.
        size_t* bufSizePtr
        ///< [INOUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * Deletes an item from secure storage.
 * If the item name is not valid, this function will kill the calling client.
 *
 * @return
 *      LE_OK if successful.
 *      LE_NOT_FOUND if the item doesn't exist.
 *      LE_UNAVAILABLE if the secure storage is currently unavailable.
 *      LE_FAULT if there was some other error.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_secStore_Delete
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL name
        ///< [IN] Name of the secure storage item.
);

#endif // LE_SECSTORE_COMMON_H_INCLUDE_GUARD