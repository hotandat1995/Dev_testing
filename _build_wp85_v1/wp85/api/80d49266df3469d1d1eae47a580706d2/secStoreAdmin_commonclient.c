/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

#include "secStoreAdmin_common.h"
#include "secStoreAdmin_messages.h"


//--------------------------------------------------------------------------------------------------
/**
 * Client Data Objects
 *
 * This object is used for each registered handler.  This is needed since we are not using
 * events, but are instead queueing functions directly with the event loop.
 */
//--------------------------------------------------------------------------------------------------
typedef struct
{
    le_event_HandlerFunc_t handlerPtr;          ///< Registered handler function
    void*                  contextPtr;          ///< ContextPtr registered with handler
    le_event_HandlerRef_t  handlerRef;          ///< HandlerRef for the registered handler
    le_thread_Ref_t        callersThreadRef;    ///< Caller's thread.
}
_ClientData_t;


//--------------------------------------------------------------------------------------------------
/**
 * Expected maximum simultaneous client data items.
 */
//--------------------------------------------------------------------------------------------------
#define HIGH_CLIENT_DATA_COUNT   1


//--------------------------------------------------------------------------------------------------
/**
 * Static memory pool for client data
 */
//--------------------------------------------------------------------------------------------------
LE_MEM_DEFINE_STATIC_POOL(secStoreAdmin_ClientData,
                          HIGH_CLIENT_DATA_COUNT,
                          sizeof(_ClientData_t));


//--------------------------------------------------------------------------------------------------
/**
 * The memory pool for client data objects
 */
//--------------------------------------------------------------------------------------------------
static le_mem_PoolRef_t _ClientDataPool;


//--------------------------------------------------------------------------------------------------
/**
 * Static safe reference map for use with Add/Remove handler references
 */
//--------------------------------------------------------------------------------------------------
LE_REF_DEFINE_STATIC_MAP(secStoreAdmin_ClientHandlers, HIGH_CLIENT_DATA_COUNT);


//--------------------------------------------------------------------------------------------------
/**
 * Safe Reference Map for use with Add/Remove handler references
 *
 * @warning Use _Mutex, defined below, to protect accesses to this data.
 */
//--------------------------------------------------------------------------------------------------
static le_ref_MapRef_t _HandlerRefMap;


//--------------------------------------------------------------------------------------------------
/**
 * Mutex and associated macros for use with the above HandlerRefMap.
 *
 * Unused attribute is needed because this variable may not always get used.
 */
//--------------------------------------------------------------------------------------------------
__attribute__((unused)) static pthread_mutex_t _Mutex = PTHREAD_MUTEX_INITIALIZER;   // POSIX "Fast" mutex.

/// Locks the mutex.
#define _LOCK    LE_ASSERT(pthread_mutex_lock(&_Mutex) == 0);

/// Unlocks the mutex.
#define _UNLOCK  LE_ASSERT(pthread_mutex_unlock(&_Mutex) == 0);


//--------------------------------------------------------------------------------------------------
/**
 * Trace reference used for controlling tracing in this module.
 */
//--------------------------------------------------------------------------------------------------
#if defined(MK_TOOLS_BUILD) && !defined(NO_LOG_SESSION)

static le_log_TraceRef_t TraceRef;

/// Macro used to generate trace output in this module.
/// Takes the same parameters as LE_DEBUG() et. al.
#define TRACE(...) LE_TRACE(TraceRef, ##__VA_ARGS__)

/// Macro used to query current trace state in this module
#define IS_TRACE_ENABLED LE_IS_TRACE_ENABLED(TraceRef)

#else

#define TRACE(...)
#define IS_TRACE_ENABLED 0

#endif

//--------------------------------------------------------------------------------------------------
/**
 * Message to call when unsolicited message (e.g. callback) is received from server.
 */
//--------------------------------------------------------------------------------------------------
static void ClientIndicationRecvHandler(le_msg_MessageRef_t  msgRef,
                                        void*                contextPtr);

//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 *
 * If using this version of the function, it's a remote binding.
 */
//--------------------------------------------------------------------------------------------------
__attribute__((weak))
LE_SHARED bool ifgen_secStoreAdmin_HasLocalBinding
(
    void
)
{
    return false;
}

//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
__attribute__((weak))
LE_SHARED void ifgen_secStoreAdmin_InitCommonData
(
    void
)
{
    // Make sure each entry is only initialized once
    _LOCK;
    {
        if (!_ClientDataPool)
        {
            // Allocate the client data pool
            _ClientDataPool = le_mem_InitStaticPool(secStoreAdmin_ClientData,
                                                    HIGH_CLIENT_DATA_COUNT,
                                                    sizeof(_ClientData_t));
        }


        if (!_HandlerRefMap)
        {
            // Create safe reference map for handler references.
            // The size of the map should be based on the number of handlers defined multiplied by
            // the number of client threads.  Since this number can't be completely determined at
            // build time, just make a reasonable guess.
            _HandlerRefMap = le_ref_InitStaticMap(secStoreAdmin_ClientHandlers,
                                                  HIGH_CLIENT_DATA_COUNT);
        }

#if defined(MK_TOOLS_BUILD) && !defined(NO_LOG_SESSION)
        // Get a reference to the trace keyword that is used to control tracing in this module.
        if (!TraceRef)
        {
            TraceRef = le_log_GetTraceRef("ipc");
        }
#endif
    }
    _UNLOCK;
}

//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
__attribute__((weak))
LE_SHARED le_result_t ifgen_secStoreAdmin_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
)
{
    le_msg_SetSessionRecvHandler(_ifgen_sessionRef, ClientIndicationRecvHandler, NULL);

    if ( isBlocking )
    {
        le_msg_OpenSessionSync(_ifgen_sessionRef);
    }
    else
    {
        le_result_t result;

        result = le_msg_TryOpenSessionSync(_ifgen_sessionRef);
        if ( result != LE_OK )
        {
            le_msg_DeleteSession(_ifgen_sessionRef);

            switch (result)
            {
                case LE_UNAVAILABLE:
                    LE_DEBUG("Service not offered");
                    break;

                case LE_NOT_PERMITTED:
                    LE_DEBUG("Missing binding");
                    break;

                case LE_COMM_ERROR:
                    LE_DEBUG("Can't reach ServiceDirectory");
                    break;

                default:
                    LE_CRIT("le_msg_TryOpenSessionSync() returned unexpected result code %d (%s)",
                            result,
                            LE_RESULT_TXT(result));
                    break;
            }

            return result;
        }
    }

    return LE_OK;
}

//--------------------------------------------------------------------------------------------------
// Client Specific Client Code
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Create an iterator for listing entries in secure storage under the specified path.
 *
 * @return
 *      An iterator reference if successful.
 *      NULL if the secure storage is currently unavailable.
 */
//--------------------------------------------------------------------------------------------------
__attribute__((weak))
LE_SHARED secStoreAdmin_IterRef_t ifgen_secStoreAdmin_CreateIter
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    const char* LE_NONNULL path
        ///< [IN] Path to iterate over.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;

    secStoreAdmin_IterRef_t _result =
        NULL;

    // Range check values, if appropriate
    if ( strnlen(path, 511) > 511 )
    {
        LE_FATAL("strnlen(path, 511) > 511");
    }


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(_ifgen_sessionRef);
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_secStoreAdmin_CreateIter;
    _msgBufPtr = _msgPtr->buffer;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                  path, 511 ));

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        le_msg_SessionEventHandler_t sessionCloseHandler = NULL;
        void*                        closeContextPtr = NULL;

        le_msg_GetSessionCloseHandler(_ifgen_sessionRef,
                                      &sessionCloseHandler,
                                      &closeContextPtr);
        if (sessionCloseHandler)
        {
            sessionCloseHandler(_ifgen_sessionRef, closeContextPtr);
        }

        LE_FATAL("Error receiving response from server");
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;

    // Unpack the result first
    if (!le_pack_UnpackReference( &_msgBufPtr, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Deletes an iterator.
 */
//--------------------------------------------------------------------------------------------------
__attribute__((weak))
LE_SHARED void ifgen_secStoreAdmin_DeleteIter
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    secStoreAdmin_IterRef_t iterRef
        ///< [IN] Iterator reference.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(_ifgen_sessionRef);
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_secStoreAdmin_DeleteIter;
    _msgBufPtr = _msgPtr->buffer;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr,
                                                  iterRef ));

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        le_msg_SessionEventHandler_t sessionCloseHandler = NULL;
        void*                        closeContextPtr = NULL;

        le_msg_GetSessionCloseHandler(_ifgen_sessionRef,
                                      &sessionCloseHandler,
                                      &closeContextPtr);
        if (sessionCloseHandler)
        {
            sessionCloseHandler(_ifgen_sessionRef, closeContextPtr);
        }

        LE_FATAL("Error receiving response from server");
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;

    // Unpack any "out" parameters


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);

    return;
}


//--------------------------------------------------------------------------------------------------
/**
 * Go to the next entry in the iterator.  This should be called at least once before accessing the
 * entry.  After the first time this function is called successfully on an iterator the first entry
 * will be available.
 *
 * @return
 *      LE_OK if successful.
 *      LE_NOT_FOUND if there are no more entries available.
 */
//--------------------------------------------------------------------------------------------------
__attribute__((weak))
LE_SHARED le_result_t ifgen_secStoreAdmin_Next
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    secStoreAdmin_IterRef_t iterRef
        ///< [IN] Iterator reference.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;

    le_result_t _result =
        LE_OK;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(_ifgen_sessionRef);
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_secStoreAdmin_Next;
    _msgBufPtr = _msgPtr->buffer;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr,
                                                  iterRef ));

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        le_msg_SessionEventHandler_t sessionCloseHandler = NULL;
        void*                        closeContextPtr = NULL;

        le_msg_GetSessionCloseHandler(_ifgen_sessionRef,
                                      &sessionCloseHandler,
                                      &closeContextPtr);
        if (sessionCloseHandler)
        {
            sessionCloseHandler(_ifgen_sessionRef, closeContextPtr);
        }

        LE_FATAL("Error receiving response from server");
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Get the current entry's name.
 *
 * @return
 *      LE_OK if successful.
 *      LE_OVERFLOW if the buffer is too small to hold the entry name.
 *      LE_UNAVAILABLE if the secure storage is currently unavailable.
 */
//--------------------------------------------------------------------------------------------------
__attribute__((weak))
LE_SHARED le_result_t ifgen_secStoreAdmin_GetEntry
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    secStoreAdmin_IterRef_t iterRef,
        ///< [IN] Iterator reference.
    char* name,
        ///< [OUT] Buffer to store the entry name.
    size_t nameSize,
        ///< [IN]
    bool* isDirPtr
        ///< [OUT] True if the entry is a directory, false otherwise.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;

    le_result_t _result =
        LE_OK;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(_ifgen_sessionRef);
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_secStoreAdmin_GetEntry;
    _msgBufPtr = _msgPtr->buffer;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(name)) << 0);
    _requiredOutputs |= ((!!(isDirPtr)) << 1);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr,
                                                  iterRef ));
    if (name)
    {
        LE_ASSERT(le_pack_PackSize( &_msgBufPtr, (nameSize-1) ));
    }

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        le_msg_SessionEventHandler_t sessionCloseHandler = NULL;
        void*                        closeContextPtr = NULL;

        le_msg_GetSessionCloseHandler(_ifgen_sessionRef,
                                      &sessionCloseHandler,
                                      &closeContextPtr);
        if (sessionCloseHandler)
        {
            sessionCloseHandler(_ifgen_sessionRef, closeContextPtr);
        }

        LE_FATAL("Error receiving response from server");
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters
    if (name &&
        (!le_pack_UnpackString( &_msgBufPtr,
                               name,
                               nameSize,
                               511 )))
    {
        goto error_unpack;
    }
    if (isDirPtr &&
        (!le_pack_UnpackBool( &_msgBufPtr,
                                               isDirPtr )))
    {
        goto error_unpack;
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Writes a buffer of data into the specified path in secure storage.  If the item already exists,
 * it'll be overwritten with the new value. If the item doesn't already exist, it'll be created.
 *
 * @note
 *      The specified path must be an absolute path.
 *
 * @return
 *      LE_OK if successful.
 *      LE_NO_MEMORY if there isn't enough memory to store the item.
 *      LE_UNAVAILABLE if the secure storage is currently unavailable.
 *      LE_FAULT if there was some other error.
 */
//--------------------------------------------------------------------------------------------------
__attribute__((weak))
LE_SHARED le_result_t ifgen_secStoreAdmin_Write
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    const char* LE_NONNULL path,
        ///< [IN] Path of the secure storage item.
    const uint8_t* bufPtr,
        ///< [IN] Buffer containing the data to store.
    size_t bufSize
        ///< [IN]
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;

    le_result_t _result =
        LE_OK;

    // Range check values, if appropriate
    if ( strnlen(path, 511) > 511 )
    {
        LE_FATAL("strnlen(path, 511) > 511");
    }
    if ( (NULL == bufPtr) &&
         (0 != bufSize) )
    {
        LE_FATAL("If bufPtr is NULL "
                 "bufSize must be zero");
    }
    if ( bufSize > 8192 )
    {
        LE_FATAL("bufSize > 8192");
    }


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(_ifgen_sessionRef);
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_secStoreAdmin_Write;
    _msgBufPtr = _msgPtr->buffer;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                  path, 511 ));
    bool bufResult;
            LE_PACK_PACKARRAY( &_msgBufPtr,
                       bufPtr, bufSize,
                       8192, le_pack_PackUint8,
                       &bufResult );
    LE_ASSERT(bufResult);

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        le_msg_SessionEventHandler_t sessionCloseHandler = NULL;
        void*                        closeContextPtr = NULL;

        le_msg_GetSessionCloseHandler(_ifgen_sessionRef,
                                      &sessionCloseHandler,
                                      &closeContextPtr);
        if (sessionCloseHandler)
        {
            sessionCloseHandler(_ifgen_sessionRef, closeContextPtr);
        }

        LE_FATAL("Error receiving response from server");
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Reads an item from secure storage.
 *
 * @note
 *      The specified path must be an absolute path.
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
__attribute__((weak))
LE_SHARED le_result_t ifgen_secStoreAdmin_Read
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    const char* LE_NONNULL path,
        ///< [IN] Path of the secure storage item.
    uint8_t* bufPtr,
        ///< [OUT] Buffer to store the data in.
    size_t* bufSizePtr
        ///< [INOUT]
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;

    le_result_t _result =
        LE_OK;

    // Range check values, if appropriate
    if ( strnlen(path, 511) > 511 )
    {
        LE_FATAL("strnlen(path, 511) > 511");
    }


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(_ifgen_sessionRef);
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_secStoreAdmin_Read;
    _msgBufPtr = _msgPtr->buffer;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(bufPtr)) << 0);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                  path, 511 ));
    if (bufPtr)
    {
        LE_ASSERT(le_pack_PackSize( &_msgBufPtr, (*bufSizePtr) ));
    }

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        le_msg_SessionEventHandler_t sessionCloseHandler = NULL;
        void*                        closeContextPtr = NULL;

        le_msg_GetSessionCloseHandler(_ifgen_sessionRef,
                                      &sessionCloseHandler,
                                      &closeContextPtr);
        if (sessionCloseHandler)
        {
            sessionCloseHandler(_ifgen_sessionRef, closeContextPtr);
        }

        LE_FATAL("Error receiving response from server");
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters
    bool bufResult;
    if (bufPtr)
    {
            LE_PACK_UNPACKARRAY( &_msgBufPtr,
                             bufPtr, bufSizePtr,
                             8192, le_pack_UnpackUint8,
                             &bufResult );
        if (!bufResult)
        {
        goto error_unpack;
        }
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Copy the meta file to the specified path.
 *
 * @return
 *      LE_OK if successful.
 *      LE_NOT_FOUND if the meta file does not exist.
 *      LE_UNAVAILABLE if the sfs is currently unavailable.
 *      LE_FAULT if there was some other error.
 */
//--------------------------------------------------------------------------------------------------
__attribute__((weak))
LE_SHARED le_result_t ifgen_secStoreAdmin_CopyMetaTo
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    const char* LE_NONNULL path
        ///< [IN] Destination path of meta file copy.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;

    le_result_t _result =
        LE_OK;

    // Range check values, if appropriate
    if ( strnlen(path, 511) > 511 )
    {
        LE_FATAL("strnlen(path, 511) > 511");
    }


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(_ifgen_sessionRef);
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_secStoreAdmin_CopyMetaTo;
    _msgBufPtr = _msgPtr->buffer;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                  path, 511 ));

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        le_msg_SessionEventHandler_t sessionCloseHandler = NULL;
        void*                        closeContextPtr = NULL;

        le_msg_GetSessionCloseHandler(_ifgen_sessionRef,
                                      &sessionCloseHandler,
                                      &closeContextPtr);
        if (sessionCloseHandler)
        {
            sessionCloseHandler(_ifgen_sessionRef, closeContextPtr);
        }

        LE_FATAL("Error receiving response from server");
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Recursively deletes all items under the specified path and the specified path from secure
 * storage.
 *
 * @note
 *      The specified path must be an absolute path.
 *
 * @return
 *      LE_OK if successful.
 *      LE_NOT_FOUND if the path doesn't exist.
 *      LE_UNAVAILABLE if the secure storage is currently unavailable.
 *      LE_FAULT if there was some other error.
 */
//--------------------------------------------------------------------------------------------------
__attribute__((weak))
LE_SHARED le_result_t ifgen_secStoreAdmin_Delete
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    const char* LE_NONNULL path
        ///< [IN] Path of the secure storage item.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;

    le_result_t _result =
        LE_OK;

    // Range check values, if appropriate
    if ( strnlen(path, 511) > 511 )
    {
        LE_FATAL("strnlen(path, 511) > 511");
    }


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(_ifgen_sessionRef);
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_secStoreAdmin_Delete;
    _msgBufPtr = _msgPtr->buffer;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                  path, 511 ));

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        le_msg_SessionEventHandler_t sessionCloseHandler = NULL;
        void*                        closeContextPtr = NULL;

        le_msg_GetSessionCloseHandler(_ifgen_sessionRef,
                                      &sessionCloseHandler,
                                      &closeContextPtr);
        if (sessionCloseHandler)
        {
            sessionCloseHandler(_ifgen_sessionRef, closeContextPtr);
        }

        LE_FATAL("Error receiving response from server");
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Gets the size, in bytes, of all items under the specified path.
 *
 * @note
 *      The specified path must be an absolute path.
 *
 * @return
 *      LE_OK if successful.
 *      LE_NOT_FOUND if the path doesn't exist.
 *      LE_UNAVAILABLE if the secure storage is currently unavailable.
 *      LE_FAULT if there was some other error.
 */
//--------------------------------------------------------------------------------------------------
__attribute__((weak))
LE_SHARED le_result_t ifgen_secStoreAdmin_GetSize
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    const char* LE_NONNULL path,
        ///< [IN] Path of the secure storage item.
    uint64_t* sizePtr
        ///< [OUT] Size in bytes of all items in the path.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;

    le_result_t _result =
        LE_OK;

    // Range check values, if appropriate
    if ( strnlen(path, 511) > 511 )
    {
        LE_FATAL("strnlen(path, 511) > 511");
    }


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(_ifgen_sessionRef);
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_secStoreAdmin_GetSize;
    _msgBufPtr = _msgPtr->buffer;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(sizePtr)) << 0);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                  path, 511 ));

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        le_msg_SessionEventHandler_t sessionCloseHandler = NULL;
        void*                        closeContextPtr = NULL;

        le_msg_GetSessionCloseHandler(_ifgen_sessionRef,
                                      &sessionCloseHandler,
                                      &closeContextPtr);
        if (sessionCloseHandler)
        {
            sessionCloseHandler(_ifgen_sessionRef, closeContextPtr);
        }

        LE_FATAL("Error receiving response from server");
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters
    if (sizePtr &&
        (!le_pack_UnpackUint64( &_msgBufPtr,
                                               sizePtr )))
    {
        goto error_unpack;
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Gets the total space and the available free space in secure storage.
 *
 * @return
 *      LE_OK if successful.
 *      LE_UNAVAILABLE if the secure storage is currently unavailable.
 *      LE_FAULT if there was some other error.
 */
//--------------------------------------------------------------------------------------------------
__attribute__((weak))
LE_SHARED le_result_t ifgen_secStoreAdmin_GetTotalSpace
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    uint64_t* totalSizePtr,
        ///< [OUT] Total size, in bytes, of secure storage.
    uint64_t* freeSizePtr
        ///< [OUT] Free space, in bytes, in secure storage.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;

    le_result_t _result =
        LE_OK;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(_ifgen_sessionRef);
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_secStoreAdmin_GetTotalSpace;
    _msgBufPtr = _msgPtr->buffer;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(totalSizePtr)) << 0);
    _requiredOutputs |= ((!!(freeSizePtr)) << 1);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, _requiredOutputs));

    // Pack the input parameters

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        le_msg_SessionEventHandler_t sessionCloseHandler = NULL;
        void*                        closeContextPtr = NULL;

        le_msg_GetSessionCloseHandler(_ifgen_sessionRef,
                                      &sessionCloseHandler,
                                      &closeContextPtr);
        if (sessionCloseHandler)
        {
            sessionCloseHandler(_ifgen_sessionRef, closeContextPtr);
        }

        LE_FATAL("Error receiving response from server");
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters
    if (totalSizePtr &&
        (!le_pack_UnpackUint64( &_msgBufPtr,
                                               totalSizePtr )))
    {
        goto error_unpack;
    }
    if (freeSizePtr &&
        (!le_pack_UnpackUint64( &_msgBufPtr,
                                               freeSizePtr )))
    {
        goto error_unpack;
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


static void ClientIndicationRecvHandler
(
    le_msg_MessageRef_t  msgRef,
    void*                contextPtr
)
{
    // Get the message payload
    _Message_t* msgPtr = le_msg_GetPayloadPtr(msgRef);
    uint8_t* _msgBufPtr = msgPtr->buffer;

    // Have to partially unpack the received message in order to know which thread
    // the queued function should actually go to.
    void* clientContextPtr;
    if (!le_pack_UnpackReference( &_msgBufPtr, &clientContextPtr ))
    {
        LE_FATAL("Failed to unpack message from server.");
        return;
    }

    // The clientContextPtr is a safe reference for the client data object.  If the client data
    // pointer is NULL, this means the handler was removed before the event was reported to the
    // client. This is valid, and the event will be dropped.
    _LOCK
    _ClientData_t* clientDataPtr = le_ref_Lookup(_HandlerRefMap, clientContextPtr);
    _UNLOCK
    if ( clientDataPtr == NULL )
    {
        LE_DEBUG("Ignore reported event after handler removed");
        return;
    }

    // Pull out the callers thread
    le_thread_Ref_t callersThreadRef = clientDataPtr->callersThreadRef;

    // Trigger the appropriate event
    switch (msgPtr->id)
    {

        default:
            LE_FATAL("Unknowm msg id = %" PRIu32 " for client thread = %p",
                msgPtr->id, callersThreadRef);
    }
}