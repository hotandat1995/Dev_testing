

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef SECSTOREGLOBAL_INTERFACE_H_INCLUDE_GUARD
#define SECSTOREGLOBAL_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"

// Internal includes for this interface
#include "le_secStore_common.h"
//--------------------------------------------------------------------------------------------------
/**
 * Get the server service reference
 */
//--------------------------------------------------------------------------------------------------
le_msg_ServiceRef_t secStoreGlobal_GetServiceRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the client session reference for the current message
 */
//--------------------------------------------------------------------------------------------------
le_msg_SessionRef_t secStoreGlobal_GetClientSessionRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Initialize the server and advertise the service.
 */
//--------------------------------------------------------------------------------------------------
void secStoreGlobal_AdvertiseService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Maximum number of characters and byte storage size permitted for a secure storage item name.
 */
//--------------------------------------------------------------------------------------------------
#define SECSTOREGLOBAL_MAX_NAME_SIZE LE_SECSTORE_MAX_NAME_SIZE

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
#define SECSTOREGLOBAL_MAX_NAME_BYTES LE_SECSTORE_MAX_NAME_BYTES

//--------------------------------------------------------------------------------------------------
/**
 * Maximum number of bytes for each item in secure storage.
 */
//--------------------------------------------------------------------------------------------------
#define SECSTOREGLOBAL_MAX_ITEM_SIZE LE_SECSTORE_MAX_ITEM_SIZE


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
le_result_t secStoreGlobal_Write
(
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
le_result_t secStoreGlobal_Read
(
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
le_result_t secStoreGlobal_Delete
(
    const char* LE_NONNULL name
        ///< [IN] Name of the secure storage item.
);


#endif // SECSTOREGLOBAL_INTERFACE_H_INCLUDE_GUARD