

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_AVDATA_INTERFACE_H_INCLUDE_GUARD
#define LE_AVDATA_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"

// Internal includes for this interface
#include "le_avdata_common.h"
//--------------------------------------------------------------------------------------------------
/**
 * Get the server service reference
 */
//--------------------------------------------------------------------------------------------------
le_msg_ServiceRef_t le_avdata_GetServiceRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the client session reference for the current message
 */
//--------------------------------------------------------------------------------------------------
le_msg_SessionRef_t le_avdata_GetClientSessionRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Initialize the server and advertise the service.
 */
//--------------------------------------------------------------------------------------------------
void le_avdata_AdvertiseService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Resource access modes:
 *   - Variable: read (server), read/write (client)
 *   - Setting: read/write (server), read/write (client)
 *   - Command: execute (server)
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Resource access types
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Resource namespace
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Data types
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Status of the data push
 * @todo Provide additional status to troubleshoot delivery problems
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Argument list reference, for command only.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Handler for resource activity. Note that the path returned by this event follows the unix format
 * (e.g. "/a/b/c") even if the app uses the URL format (e.g "a.b.c") when registering
 * the handler.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Handler for pushing data result.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_avdata_ResourceEvent'
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * A record reference
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Reference returned by RequestSession function and used by ReleaseSession function
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * AVMS session state
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Handler for AV session changes
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_avdata_SessionState'
 */
//--------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_avdata_ResourceEvent'
 *
 * Upon resource access on the server side, the registered handler is called.
 *
 * For "settings" (read/write), the same handler is called for both read and write access.
 *
 * For "commands", the handler function must call the "ReplyExecResult" function to send the command
 * execution result back to the AVC daemon (which then sends the proper response back to the AV
 * server).
 */
//--------------------------------------------------------------------------------------------------
le_avdata_ResourceEventHandlerRef_t le_avdata_AddResourceEventHandler
(
    const char* LE_NONNULL path,
        ///< [IN]
    le_avdata_ResourceHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_avdata_ResourceEvent'
 */
//--------------------------------------------------------------------------------------------------
void le_avdata_RemoveResourceEventHandler
(
    le_avdata_ResourceEventHandlerRef_t handlerRef
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Create an asset data with the provided path. Note that asset data type and value are determined
 * upon the first call to a Set function. When an asset data is created, it contains a null value,
 * represented by the data type of none.
 *
 * @return:
 *      - LE_OK on success
 *      - LE_DUPLICATE if path has already been called by CreateResource before, or path is parent
 *        or child to an existing Asset Data path.
 *      - LE_FAULT on any other error.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_CreateResource
(
    const char* LE_NONNULL path,
        ///< [IN]
    le_avdata_AccessMode_t accessMode
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Sets the namespace for asset data.
 *
 * @return:
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if the namespace is unknown
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_SetNamespace
(
    le_avdata_Namespace_t _namespace
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Sets an asset data to contain a null value, represented by the data type of none.
 *
 * @return:
 *      - LE_NOT_FOUND - if the path is invalid and does not point to an asset data
 *      - LE_NOT_PERMITTED - asset data being accessed does not have the right permission
 *      - LE_OK - access successful.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_SetNull
(
    const char* LE_NONNULL path
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Gets the integer value of an asset data.
 *
 * @return:
 *      - LE_BAD_PARAMETER - asset data being accessed is of the wrong data type
 *      - LE_UNAVAILABLE - asset data contains null value
 *      - LE_NOT_FOUND - if the path is invalid and does not point to an asset data
 *      - LE_NOT_PERMITTED - asset data being accessed does not have the right permission
 *      - LE_OK - access successful.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_GetInt
(
    const char* LE_NONNULL path,
        ///< [IN]
    int32_t* valuePtr
        ///< [OUT]
);



//--------------------------------------------------------------------------------------------------
/**
 * Sets an asset data to an integer value.
 *
 * @return:
 *      - LE_NOT_FOUND - if the path is invalid and does not point to an asset data
 *      - LE_NOT_PERMITTED - asset data being accessed does not have the right permission
 *      - LE_OK - access successful.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_SetInt
(
    const char* LE_NONNULL path,
        ///< [IN]
    int32_t value
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Gets the float value of an asset data.
 *
 * @return:
 *      - LE_BAD_PARAMETER - asset data being accessed is of the wrong data type
 *      - LE_UNAVAILABLE - asset data contains null value
 *      - LE_NOT_FOUND - if the path is invalid and does not point to an asset data
 *      - LE_NOT_PERMITTED - asset data being accessed does not have the right permission
 *      - LE_OK - access successful.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_GetFloat
(
    const char* LE_NONNULL path,
        ///< [IN]
    double* valuePtr
        ///< [OUT]
);



//--------------------------------------------------------------------------------------------------
/**
 * Sets an asset data to a float value.
 *
 * @return:
 *      - LE_NOT_FOUND - if the path is invalid and does not point to an asset data
 *      - LE_NOT_PERMITTED - asset data being accessed does not have the right permission
 *      - LE_OK - access successful.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_SetFloat
(
    const char* LE_NONNULL path,
        ///< [IN]
    double value
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Gets the bool value of an asset data.
 *
 * @return:
 *      - LE_BAD_PARAMETER - asset data being accessed is of the wrong data type
 *      - LE_UNAVAILABLE - asset data contains null value
 *      - LE_NOT_FOUND - if the path is invalid and does not point to an asset data
 *      - LE_NOT_PERMITTED - asset data being accessed does not have the right permission
 *      - LE_OK - access successful.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_GetBool
(
    const char* LE_NONNULL path,
        ///< [IN]
    bool* valuePtr
        ///< [OUT]
);



//--------------------------------------------------------------------------------------------------
/**
 * Sets an asset data to a bool value.
 *
 * @return:
 *      - LE_NOT_FOUND - if the path is invalid and does not point to an asset data
 *      - LE_NOT_PERMITTED - asset data being accessed does not have the right permission
 *      - LE_OK - access successful.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_SetBool
(
    const char* LE_NONNULL path,
        ///< [IN]
    bool value
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Gets the string value of an asset data.
 *
 * @return:
 *      - LE_BAD_PARAMETER - asset data being accessed is of the wrong data type
 *      - LE_UNAVAILABLE - asset data contains null value
 *      - LE_NOT_FOUND - if the path is invalid and does not point to an asset data
 *      - LE_NOT_PERMITTED - asset data being accessed does not have the right permission
 *      - LE_OVERFLOW - asset data length exceeds the maximum length
 *      - LE_OK - access successful
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_GetString
(
    const char* LE_NONNULL path,
        ///< [IN]
    char* value,
        ///< [OUT]
    size_t valueSize
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Sets an asset data to a string value.
 *
 * @return:
 *      - LE_NOT_FOUND - if the path is invalid and does not point to an asset data
 *      - LE_NOT_PERMITTED - asset data being accessed does not have the right permission
 *      - LE_OK - access successful.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_SetString
(
    const char* LE_NONNULL path,
        ///< [IN]
    const char* LE_NONNULL value
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the bool argument with the specified name.
 *
 * @return:
 *      - LE_OK on success
 *      - LE_NOT_FOUND if argument doesn't exist, or its data type doesn't match the API.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_GetBoolArg
(
    le_avdata_ArgumentListRef_t argumentListRef,
        ///< [IN]
    const char* LE_NONNULL argName,
        ///< [IN]
    bool* boolArgPtr
        ///< [OUT]
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the float argument with the specified name.
 *
 * @return:
 *      - LE_OK on success
 *      - LE_NOT_FOUND if argument doesn't exist, or its data type doesn't match the API.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_GetFloatArg
(
    le_avdata_ArgumentListRef_t argumentListRef,
        ///< [IN]
    const char* LE_NONNULL argName,
        ///< [IN]
    double* floatArgPtr
        ///< [OUT]
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the int argument with the specified name.
 *
 * @return:
 *      - LE_OK on success
 *      - LE_NOT_FOUND if argument doesn't exist, or its data type doesn't match the API.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_GetIntArg
(
    le_avdata_ArgumentListRef_t argumentListRef,
        ///< [IN]
    const char* LE_NONNULL argName,
        ///< [IN]
    int32_t* intArgPtr
        ///< [OUT]
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the string argument with the specified name.
 *
 * @return:
 *      - LE_OK on success
 *      - LE_NOT_FOUND if argument doesn't exist, or its data type doesn't match the API
 *      - LE_OVERFLOW - argument length exceeds the maximum length
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_GetStringArg
(
    le_avdata_ArgumentListRef_t argumentListRef,
        ///< [IN]
    const char* LE_NONNULL argName,
        ///< [IN]
    char* strArg,
        ///< [OUT]
    size_t strArgSize
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the length (excluding terminating null byte) of the string argument of the specified name.
 *
 * @return:
 *      - LE_OK on success
 *      - LE_NOT_FOUND if argument doesn't exist, or its data type doesn't match the API.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_GetStringArgLength
(
    le_avdata_ArgumentListRef_t argumentListRef,
        ///< [IN]
    const char* LE_NONNULL argName,
        ///< [IN]
    int32_t* strArgLenPtr
        ///< [OUT]
);



//--------------------------------------------------------------------------------------------------
/**
 * Reply command execution result to AVC Daemon, which can then respond to AV server. This function
 * MUST be called at the end of a command execution, in order for AV server to be notified about the
 * execution status.
 */
//--------------------------------------------------------------------------------------------------
void le_avdata_ReplyExecResult
(
    le_avdata_ArgumentListRef_t argumentListRef,
        ///< [IN]
    le_result_t result
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Push asset data to the server.
 *
 * @return:
 *      - LE_OK on success
 *      - LE_NOT_FOUND if the provided path doesn't exist
 *      - LE_BUSY if push service is busy. Data added to queue list for later push
 *      - LE_OVERFLOW if data size exceeds the maximum allowed size
 *      - LE_NO_MEMORY if push queue is full, try again later
 *      - LE_FAULT on any other error
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_Push
(
    const char* LE_NONNULL path,
        ///< [IN]
    le_avdata_CallbackResultFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Push data dump to a specified path on the server.
 *
 * @return:
 *      - LE_OK on success
 *      - LE_BUSY if push service is busy. Data added to queue list for later push
 *      - LE_OVERFLOW if data size exceeds the maximum allowed size
 *      - LE_NO_MEMORY if push queue is full, try again later
 *      - LE_FAULT on any other error
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_PushStream
(
    const char* LE_NONNULL path,
        ///< [IN]
    int fd,
        ///< [IN]
    le_avdata_CallbackResultFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Create a timeseries record
 *
 * @return Reference to the record
 */
//--------------------------------------------------------------------------------------------------
le_avdata_RecordRef_t le_avdata_CreateRecord
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Delete a timeseries record
 */
//--------------------------------------------------------------------------------------------------
void le_avdata_DeleteRecord
(
    le_avdata_RecordRef_t recordRef
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Accumulate int data
 *
 * @note The client will be terminated if the recordRef is not valid, or the resource doesn't exist
 *
 * @return:
 *      - LE_OK on success
 *      - LE_NO_MEMORY if the current entry was NOT added because the time series buffer is full.
 *      - LE_FAULT on any other error
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_RecordInt
(
    le_avdata_RecordRef_t recordRef,
        ///< [IN]
    const char* LE_NONNULL path,
        ///< [IN]
    int32_t value,
        ///< [IN]
    uint64_t timestamp
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Accumulate float data
 *
 * @note The client will be terminated if the recordRef is not valid, or the resource doesn't exist
 *
 * @return:
 *      - LE_OK on success
 *      - LE_NO_MEMORY if the current entry was NOT added because the time series buffer is full.
 *      - LE_FAULT on any other error
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_RecordFloat
(
    le_avdata_RecordRef_t recordRef,
        ///< [IN]
    const char* LE_NONNULL path,
        ///< [IN]
    double value,
        ///< [IN]
    uint64_t timestamp
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Accumulate boolean data
 *
 * @note The client will be terminated if the recordRef is not valid, or the resource doesn't exist
 *
 * @return:
 *      - LE_OK on success
 *      - LE_NO_MEMORY if the current entry was NOT added because the time series buffer is full.
 *      - LE_FAULT on any other error
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_RecordBool
(
    le_avdata_RecordRef_t recordRef,
        ///< [IN]
    const char* LE_NONNULL path,
        ///< [IN]
    bool value,
        ///< [IN]
    uint64_t timestamp
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Accumulate string data
 *
 * @note The client will be terminated if the recordRef is not valid, or the resource doesn't exist
 *
 * @return:
 *      - LE_OK on success
 *      - LE_NO_MEMORY if the current entry was NOT added because the time series buffer is full.
 *      - LE_FAULT on any other error
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_RecordString
(
    le_avdata_RecordRef_t recordRef,
        ///< [IN]
    const char* LE_NONNULL path,
        ///< [IN]
    const char* LE_NONNULL value,
        ///< [IN]
    uint64_t timestamp
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Push record to the server
 *
 ** @return:
 *      - LE_OK on success
 *      - LE_BUSY if push service is busy. Data added to queue list for later push
 *      - LE_OVERFLOW if data size exceeds the maximum allowed size
 *      - LE_NO_MEMORY if push queue is full, try again later
 *      - LE_FAULT on any other error
 *
 * * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avdata_PushRecord
(
    le_avdata_RecordRef_t recordRef,
        ///< [IN]
    le_avdata_CallbackResultFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_avdata_SessionState'
 *
 * This event provides information on AV session state changes
 */
//--------------------------------------------------------------------------------------------------
le_avdata_SessionStateHandlerRef_t le_avdata_AddSessionStateHandler
(
    le_avdata_SessionStateHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_avdata_SessionState'
 */
//--------------------------------------------------------------------------------------------------
void le_avdata_RemoveSessionStateHandler
(
    le_avdata_SessionStateHandlerRef_t handlerRef
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Request the avcServer to open a session.
 *
 * @return
 *      - SessionRef if session request succeeded
 *      - NULL if session request failed
 */
//--------------------------------------------------------------------------------------------------
le_avdata_RequestSessionObjRef_t le_avdata_RequestSession
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Request the avcServer to close a session.
 *
 */
//--------------------------------------------------------------------------------------------------
void le_avdata_ReleaseSession
(
    le_avdata_RequestSessionObjRef_t requestRef
        ///< [IN] Reference to a previously opened AV session.
);


#endif // LE_AVDATA_INTERFACE_H_INCLUDE_GUARD