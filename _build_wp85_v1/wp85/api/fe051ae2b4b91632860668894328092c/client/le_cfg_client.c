/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

#include "le_cfg_interface.h"
#include "le_cfg_messages.h"
#include "le_cfg_service.h"


//--------------------------------------------------------------------------------------------------
// Generic Client Types, Variables and Functions
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Client Thread Objects
 *
 * This object is used to contain thread specific data for each IPC client.
 */
//--------------------------------------------------------------------------------------------------
typedef struct
{
    le_msg_SessionRef_t sessionRef;     ///< Client Session Reference
    int                 clientCount;    ///< Number of clients sharing this thread
    le_cfg_DisconnectHandler_t disconnectHandler; ///< Disconnect handler for this thread
    void*               contextPtr;     ///< Context for disconnect handler
}
_ClientThreadData_t;


//--------------------------------------------------------------------------------------------------
/**
 * Static pool for client threads.
 */
//--------------------------------------------------------------------------------------------------
LE_MEM_DEFINE_STATIC_POOL(le_cfg_ClientThreadData,
                          LE_CDATA_COMPONENT_COUNT,
                          sizeof(_ClientThreadData_t));


//--------------------------------------------------------------------------------------------------
/**
 * The memory pool for client thread objects
 */
//--------------------------------------------------------------------------------------------------
static le_mem_PoolRef_t _ClientThreadDataPool;


//--------------------------------------------------------------------------------------------------
/**
 * Key under which the pointer to the Thread Object (_ClientThreadData_t) will be kept in
 * thread-local storage.  This allows a thread to quickly get a pointer to its own Thread Object.
 */
//--------------------------------------------------------------------------------------------------
static pthread_key_t _ThreadDataKey;


//--------------------------------------------------------------------------------------------------
/**
 * This global flag is shared by all client threads, and is used to indicate whether the common
 * data has been initialized.
 *
 * @warning Use InitMutex, defined below, to protect accesses to this data.
 */
//--------------------------------------------------------------------------------------------------
static bool CommonDataInitialized = false;


//--------------------------------------------------------------------------------------------------
/**
 * Mutex and associated macros for use with the above CommonDataInitialized.
 */
//--------------------------------------------------------------------------------------------------
extern le_mutex_Ref_t le_ifgen_InitMutexRef;

/// Locks the mutex.
#define LOCK_INIT    le_mutex_Lock(le_ifgen_InitMutexRef);

/// Unlocks the mutex.
#define UNLOCK_INIT  le_mutex_Unlock(le_ifgen_InitMutexRef);

//--------------------------------------------------------------------------------------------------
/**
 * Initialize thread specific data, and connect to the service for the current thread.
 *
 * @return
 *  - LE_OK if the client connected successfully to the service.
 *  - LE_UNAVAILABLE if the server is not currently offering the service to which the client is
 *    bound.
 *  - LE_NOT_PERMITTED if the client interface is not bound to any service (doesn't have a binding).
 *  - LE_COMM_ERROR if the Service Directory cannot be reached.
 */
//--------------------------------------------------------------------------------------------------
static le_result_t InitClientForThread
(
    bool isBlocking
)
{
    // Open a session.
    le_msg_SessionRef_t sessionRef;
    le_msg_ProtocolRef_t protocolRef;

    protocolRef = le_msg_GetProtocolRef(PROTOCOL_ID_STR, sizeof(_Message_t));
    sessionRef = le_msg_CreateSession(protocolRef, SERVICE_INSTANCE_NAME);
    le_result_t result = ifgen_le_cfg_OpenSession(sessionRef, isBlocking);
    if (result != LE_OK)
    {
        LE_DEBUG("Could not connect to '%s' service", SERVICE_INSTANCE_NAME);

        return result;
    }

    // Store the client sessionRef in thread-local storage, since each thread requires
    // its own sessionRef.
    _ClientThreadData_t* clientThreadPtr = le_mem_ForceAlloc(_ClientThreadDataPool);
    memset(clientThreadPtr, 0, sizeof(_ClientThreadData_t));
    clientThreadPtr->sessionRef = sessionRef;
    if (pthread_setspecific(_ThreadDataKey, clientThreadPtr) != 0)
    {
        LE_FATAL("pthread_setspecific() failed!");
    }

    // This is the first client for the current thread
    clientThreadPtr->clientCount = 1;

    return LE_OK;
}


//--------------------------------------------------------------------------------------------------
/**
 * Get a pointer to the client thread data for the current thread.
 *
 * If the current thread does not have client data, then NULL is returned
 */
//--------------------------------------------------------------------------------------------------
static _ClientThreadData_t* GetClientThreadDataPtr
(
    void
)
{
    return pthread_getspecific(_ThreadDataKey);
}


//--------------------------------------------------------------------------------------------------
/**
 * Return the sessionRef for the current thread.
 *
 * If the current thread does not have a session ref, then this is a fatal error.
 */
//--------------------------------------------------------------------------------------------------
__attribute__((unused)) static le_msg_SessionRef_t GetCurrentSessionRef
(
    void
)
{
    if (ifgen_le_cfg_HasLocalBinding())
    {
        return NULL;
    }
    else
    {
        _ClientThreadData_t* clientThreadPtr = GetClientThreadDataPtr();

        // If the thread specific data is NULL, then the session ref has not been created.
        LE_FATAL_IF(clientThreadPtr==NULL,
                    "le_cfg_ConnectService() not called for current thread");

        return clientThreadPtr->sessionRef;
    }
}


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads.
 */
//--------------------------------------------------------------------------------------------------
static void InitCommonData(void)
{
    // Perform common initialization across all instances of this API.
    ifgen_le_cfg_InitCommonData();

    // Allocate the client thread pool
    _ClientThreadDataPool = le_mem_InitStaticPool(le_cfg_ClientThreadData,
                                                  LE_CDATA_COMPONENT_COUNT,
                                                  sizeof(_ClientThreadData_t));

    // Create the thread-local data key to be used to store a pointer to each thread object.
    LE_ASSERT(pthread_key_create(&_ThreadDataKey, NULL) == 0);
}


//--------------------------------------------------------------------------------------------------
/**
 * Connect to the service, using either blocking or non-blocking calls.
 *
 * This function implements the details of the public ConnectService functions.
 *
 * @return
 *  - LE_OK if the client connected successfully to the service.
 *  - LE_UNAVAILABLE if the server is not currently offering the service to which the client is
 *    bound.
 *  - LE_NOT_PERMITTED if the client interface is not bound to any service (doesn't have a binding).
 *  - LE_COMM_ERROR if the Service Directory cannot be reached.
 */
//--------------------------------------------------------------------------------------------------
static le_result_t DoConnectService
(
    bool isBlocking
)
{
    // If this is the first time the function is called, init the client common data.
    LOCK_INIT
    if ( ! CommonDataInitialized )
    {
        InitCommonData();
        CommonDataInitialized = true;
    }
    UNLOCK_INIT

    _ClientThreadData_t* clientThreadPtr = GetClientThreadDataPtr();

    // If the thread specific data is NULL, then there is no current client session.
    if (clientThreadPtr == NULL)
    {
        le_result_t result;

        result = InitClientForThread(isBlocking);
        if ( result != LE_OK )
        {
            // Note that the blocking call will always return LE_OK
            return result;
        }

        LE_DEBUG("======= Starting client for '%s' service ========", SERVICE_INSTANCE_NAME);
    }
    else
    {
        // Keep track of the number of clients for the current thread.  There is only one
        // connection per thread, and it is shared by all clients.
        clientThreadPtr->clientCount++;
        LE_DEBUG("======= Starting another client for '%s' service ========",
                 SERVICE_INSTANCE_NAME);
    }

    return LE_OK;
}


//--------------------------------------------------------------------------------------------------
/**
 *
 * Connect the current client thread to the service providing this API. Block until the service is
 * available.
 *
 * For each thread that wants to use this API, either ConnectService or TryConnectService must be
 * called before any other functions in this API.  Normally, ConnectService is automatically called
 * for the main thread, but not for any other thread. For details, see @ref apiFilesC_client.
 *
 * This function is created automatically.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_ConnectService
(
    void
)
{
    if (!ifgen_le_cfg_HasLocalBinding())
    {
        // Connect to the service; block until connected.
        DoConnectService(true);
    }
}

//--------------------------------------------------------------------------------------------------
/**
 *
 * Try to connect the current client thread to the service providing this API. Return with an error
 * if the service is not available.
 *
 * For each thread that wants to use this API, either ConnectService or TryConnectService must be
 * called before any other functions in this API.  Normally, ConnectService is automatically called
 * for the main thread, but not for any other thread. For details, see @ref apiFilesC_client.
 *
 * This function is created automatically.
 *
 * @return
 *  - LE_OK if the client connected successfully to the service.
 *  - LE_UNAVAILABLE if the server is not currently offering the service to which the client is
 *    bound.
 *  - LE_NOT_PERMITTED if the client interface is not bound to any service (doesn't have a binding).
 *  - LE_COMM_ERROR if the Service Directory cannot be reached.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_cfg_TryConnectService
(
    void
)
{
    if (ifgen_le_cfg_HasLocalBinding())
    {
        return LE_OK;
    }
    else
    {
        // Connect to the service; return with an error if not connected.
        return DoConnectService(false);
    }
}

//--------------------------------------------------------------------------------------------------
// Session close handler.
//
// Dispatches session close notifications to the registered client handler function (if any)
//--------------------------------------------------------------------------------------------------
static void SessionCloseHandler
(
    le_msg_SessionRef_t sessionRef,
    void *contextPtr
)
{
    _ClientThreadData_t* clientThreadPtr = contextPtr;

    le_msg_DeleteSession( clientThreadPtr->sessionRef );

    // Need to delete the thread specific data, since it is no longer valid.  If a new
    // client session is started, new thread specific data will be allocated.
    le_mem_Release(clientThreadPtr);
    if (pthread_setspecific(_ThreadDataKey, NULL) != 0)
    {
        LE_FATAL("pthread_setspecific() failed!");
    }

    LE_DEBUG("======= '%s' service spontaneously disconnected ========", SERVICE_INSTANCE_NAME);

    if (clientThreadPtr->disconnectHandler)
    {
        clientThreadPtr->disconnectHandler(clientThreadPtr->contextPtr);
    }

    LE_FATAL("Component for le_cfg disconnected\n");
}

//--------------------------------------------------------------------------------------------------
/**
 * Set handler called when server disconnection is detected.
 *
 * When a server connection is lost, call this handler then exit with LE_FATAL.  If a program wants
 * to continue without exiting, it should call longjmp() from inside the handler.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_SetServerDisconnectHandler
(
    le_cfg_DisconnectHandler_t disconnectHandler,
    void *contextPtr
)
{
    if (ifgen_le_cfg_HasLocalBinding())
    {
        // Local bindings don't disconnect
        return;
    }

    _ClientThreadData_t* clientThreadPtr = GetClientThreadDataPtr();

    if (NULL == clientThreadPtr)
    {
        LE_CRIT("Trying to set disconnect handler for non-existent client session for '%s' service",
                SERVICE_INSTANCE_NAME);
    }
    else
    {
        clientThreadPtr->disconnectHandler = disconnectHandler;
        clientThreadPtr->contextPtr = contextPtr;

        if (disconnectHandler)
        {
            le_msg_SetSessionCloseHandler(clientThreadPtr->sessionRef,
                                          SessionCloseHandler,
                                          clientThreadPtr);
        }
    }
}


//--------------------------------------------------------------------------------------------------
/**
 *
 * Disconnect the current client thread from the service providing this API.
 *
 * Normally, this function doesn't need to be called. After this function is called, there's no
 * longer a connection to the service, and the functions in this API can't be used. For details, see
 * @ref apiFilesC_client.
 *
 * This function is created automatically.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_DisconnectService
(
    void
)
{
    if (ifgen_le_cfg_HasLocalBinding())
    {
        // Cannot disconnect local bindings
        return;
    }

    _ClientThreadData_t* clientThreadPtr = GetClientThreadDataPtr();

    // If the thread specific data is NULL, then there is no current client session.
    if (clientThreadPtr == NULL)
    {
        LE_CRIT("Trying to stop non-existent client session for '%s' service",
                SERVICE_INSTANCE_NAME);
    }
    else
    {
        // This is the last client for this thread, so close the session.
        if ( clientThreadPtr->clientCount == 1 )
        {
            le_msg_DeleteSession( clientThreadPtr->sessionRef );

            // Need to delete the thread specific data, since it is no longer valid.  If a new
            // client session is started, new thread specific data will be allocated.
            le_mem_Release(clientThreadPtr);
            if (pthread_setspecific(_ThreadDataKey, NULL) != 0)
            {
                LE_FATAL("pthread_setspecific() failed!");
            }

            LE_DEBUG("======= Stopping client for '%s' service ========", SERVICE_INSTANCE_NAME);
        }
        else
        {
            // There is one less client sharing this thread's connection.
            clientThreadPtr->clientCount--;

            LE_DEBUG("======= Stopping another client for '%s' service ========",
                     SERVICE_INSTANCE_NAME);
        }
    }
}

//--------------------------------------------------------------------------------------------------
/**
 * Create a read transaction and open a new iterator for traversing the config tree.
 *
 * This action creates a read lock on the given tree, which will start a read-timeout.
 * Once the read timeout expires, all active read iterators on that tree will be
 * expired and their clients will be killed.
 *
 * @note A tree transaction is global to that tree; a long-held read transaction will block other
 *        user's write transactions from being committed.
 *
 * @return This will return the newly created iterator reference.
 */
//--------------------------------------------------------------------------------------------------
le_cfg_IteratorRef_t le_cfg_CreateReadTxn
(
    const char* LE_NONNULL basePath
        ///< [IN] Path to the location to create the new iterator.
)
{
    return ifgen_le_cfg_CreateReadTxn(
        GetCurrentSessionRef(),
        basePath
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Create a write transaction and open a new iterator for both reading and writing.
 *
 * This action creates a write transaction. If the app holds the iterator for
 * longer than the configured write transaction timeout, the iterator will cancel the
 * transaction. Other reads will fail to return data, and all writes will be thrown away.
 *
 * @note A tree transaction is global to that tree; a long-held write transaction will block
 *       other user's write transactions from being started. Other trees in the system
 *       won't be affected.
 *
 * @return This will return a newly created iterator reference.
 */
//--------------------------------------------------------------------------------------------------
le_cfg_IteratorRef_t le_cfg_CreateWriteTxn
(
    const char* LE_NONNULL basePath
        ///< [IN] Path to the location to create the new iterator.
)
{
    return ifgen_le_cfg_CreateWriteTxn(
        GetCurrentSessionRef(),
        basePath
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Closes the write iterator and commits the write transaction. This updates the config tree
 * with all of the writes that occurred within the iterator.
 *
 * @note This operation will also delete the iterator object.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_CommitTxn
(
    le_cfg_IteratorRef_t iteratorRef
        ///< [IN] Iterator object to commit.
)
{
     ifgen_le_cfg_CommitTxn(
        GetCurrentSessionRef(),
        iteratorRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Closes and frees the given iterator object. If the iterator is a write iterator, the transaction
 * will be canceled. If the iterator is a read iterator, the transaction will be closed. No data is
 * written to the tree
 *
 * @note This operation will also delete the iterator object.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_CancelTxn
(
    le_cfg_IteratorRef_t iteratorRef
        ///< [IN] Iterator object to close.
)
{
     ifgen_le_cfg_CancelTxn(
        GetCurrentSessionRef(),
        iteratorRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Changes the location of iterator. The path passed can be an absolute or a
 * relative path from the iterators current location.
 *
 * The target node does not need to exist. Writing a value to a non-existent node will
 * automatically create that node and any ancestor nodes (parent, parent's parent, etc.) that
 * also don't exist.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_GoToNode
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to move.
    const char* LE_NONNULL newPath
        ///< [IN] Absolute or relative path from the current location.
)
{
     ifgen_le_cfg_GoToNode(
        GetCurrentSessionRef(),
        iteratorRef,
        newPath
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Move the iterator to the parent of the current node (moves up the tree).
 *
 * @return Return code will be one of the following values:
 *
 *         - LE_OK        - Commit was completed successfully.
 *         - LE_NOT_FOUND - Current node is the root node: has no parent.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_cfg_GoToParent
(
    le_cfg_IteratorRef_t iteratorRef
        ///< [IN] Iterator to move.
)
{
    return ifgen_le_cfg_GoToParent(
        GetCurrentSessionRef(),
        iteratorRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Moves the iterator to the the first child of the node from the current location.
 *
 * For read iterators without children, this function will fail. If the iterator is a write
 * iterator, then a new node is automatically created. If this node or newly created
 * children of this node are not written to, then this node will not persist even if the iterator is
 * committed.
 *
 * @return Return code will be one of the following values:
 *
 *         - LE_OK        - Move was completed successfully.
 *         - LE_NOT_FOUND - The given node has no children.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_cfg_GoToFirstChild
(
    le_cfg_IteratorRef_t iteratorRef
        ///< [IN] Iterator object to move.
)
{
    return ifgen_le_cfg_GoToFirstChild(
        GetCurrentSessionRef(),
        iteratorRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Jumps the iterator to the next child node of the current node. Assuming the following tree:
 *
 * @code
 * baseNode
 *      |
 *      +childA
 *          |
 *          +valueA
 *          |
 *          +valueB
 * @endcode
 *
 * If the iterator is moved to the path, "/baseNode/childA/valueA". After the first
 * GoToNextSibling the iterator will be pointing at valueB. A second call to GoToNextSibling
 * will cause the function to return LE_NOT_FOUND.
 *
 * @return Returns one of the following values:
 *
 *         - LE_OK            - Commit was completed successfully.
 *         - LE_NOT_FOUND     - Iterator has reached the end of the current list of siblings.
 *                              Also returned if the the current node has no siblings.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_cfg_GoToNextSibling
(
    le_cfg_IteratorRef_t iteratorRef
        ///< [IN] Iterator to iterate.
)
{
    return ifgen_le_cfg_GoToNextSibling(
        GetCurrentSessionRef(),
        iteratorRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get path to the node where the iterator is currently pointed.
 *
 * Assuming the following tree:
 *
 * @code
 * baseNode
 *      |
 *      +childA
 *          |
 *          +valueA
 *          |
 *          +valueB
 * @endcode
 *
 * If the iterator was currently pointing at valueA, GetPath would return the following path:
 *
 * @code
 * /baseNode/childA/valueA
 * @endcode
 *
 * Optionally, a path to another node can be supplied to this function. So, if the iterator is
 * again on valueA and the relative path ".." is supplied then this function will return the
 * the path relative to the node given:
 *
 * @code
 * /baseNode/childA/
 * @endcode
 *
 * @return - LE_OK            - The write was completed successfully.
 *         - LE_OVERFLOW      - The supplied string buffer was not large enough to hold the value.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_cfg_GetPath
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to move.
    const char* LE_NONNULL path,
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///< a path relative from the iterator's current position.
    char* pathBuffer,
        ///< [OUT] Absolute path to the iterator's current node.
    size_t pathBufferSize
        ///< [IN]
)
{
    return ifgen_le_cfg_GetPath(
        GetCurrentSessionRef(),
        iteratorRef,
        path,
        pathBuffer,
        pathBufferSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the data type of node where the iterator is currently pointing.
 *
 * @return le_cfg_nodeType_t value indicating the stored value.
 */
//--------------------------------------------------------------------------------------------------
le_cfg_nodeType_t le_cfg_GetNodeType
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator object to use to read from the tree.
    const char* LE_NONNULL path
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///< a path relative from the iterator's current position.
)
{
    return ifgen_le_cfg_GetNodeType(
        GetCurrentSessionRef(),
        iteratorRef,
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the name of the node where the iterator is currently pointing.
 *
 * @return - LE_OK       Read was completed successfully.
 *         - LE_OVERFLOW Supplied string buffer was not large enough to hold the value.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_cfg_GetNodeName
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator object to use to read from the tree.
    const char* LE_NONNULL path,
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///< a path relative from the iterator's current position.
    char* name,
        ///< [OUT] Read the name of the node object.
    size_t nameSize
        ///< [IN]
)
{
    return ifgen_le_cfg_GetNodeName(
        GetCurrentSessionRef(),
        iteratorRef,
        path,
        name,
        nameSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_cfg_Change'
 *
 * This event provides information on changes to the given node object, or any of it's children,
 * where a change could be either a read, write, create or delete operation.
 */
//--------------------------------------------------------------------------------------------------
le_cfg_ChangeHandlerRef_t le_cfg_AddChangeHandler
(
    const char* LE_NONNULL newPath,
        ///< [IN] Path to the object to watch.
    le_cfg_ChangeHandlerFunc_t handlerPtr,
        ///< [IN] Handler to receive change notification
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_le_cfg_AddChangeHandler(
        GetCurrentSessionRef(),
        newPath,
        handlerPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_cfg_Change'
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_RemoveChangeHandler
(
    le_cfg_ChangeHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_le_cfg_RemoveChangeHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Deletes the node specified by the path. If the node doesn't exist, nothing happens. All child
 * nodes are also deleted.
 *
 * If the path is empty, the iterator's current node is deleted.
 *
 * This function is only valid during a write transaction.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_DeleteNode
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.
    const char* LE_NONNULL path
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///< a path relative from the iterator's current position.
)
{
     ifgen_le_cfg_DeleteNode(
        GetCurrentSessionRef(),
        iteratorRef,
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Check if the given node is empty. A node is also considered empty if it doesn't yet exist. A
 * node is also considered empty if it has no value or is a stem with no children.
 *
 * If the path is empty, the iterator's current node is queried for emptiness.
 *
 * Valid for both read and write transactions.
 *
 * @return A true if the node is considered empty, false if not.
 */
//--------------------------------------------------------------------------------------------------
bool le_cfg_IsEmpty
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.
    const char* LE_NONNULL path
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///< a path relative from the iterator's current position.
)
{
    return ifgen_le_cfg_IsEmpty(
        GetCurrentSessionRef(),
        iteratorRef,
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Clears out the node's value. If the node doesn't exist it will be created, and have no value.
 *
 * If the path is empty, the iterator's current node will be cleared. If the node is a stem
 * then all children will be removed from the tree.
 *
 * Only valid during a write transaction.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_SetEmpty
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.
    const char* LE_NONNULL path
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///< a path relative from the iterator's current position.
)
{
     ifgen_le_cfg_SetEmpty(
        GetCurrentSessionRef(),
        iteratorRef,
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Checks to see if a given node in the config tree exists.
 *
 * @return True if the specified node exists in the tree. False if not.
 */
//--------------------------------------------------------------------------------------------------
bool le_cfg_NodeExists
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.
    const char* LE_NONNULL path
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///< a path relative from the iterator's current position.
)
{
    return ifgen_le_cfg_NodeExists(
        GetCurrentSessionRef(),
        iteratorRef,
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Reads a string value from the config tree. If the value isn't a string, or if the node is
 * empty or doesn't exist, the default value will be returned.
 *
 * Valid for both read and write transactions.
 *
 * If the path is empty, the iterator's current node will be read.
 *
 * @return - LE_OK       - Read was completed successfully.
 *         - LE_OVERFLOW - Supplied string buffer was not large enough to hold the value.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_cfg_GetString
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.
    const char* LE_NONNULL path,
        ///< [IN] Path to the target node. Can be an absolute path,
        ///< or a path relative from the iterator's current
        ///< position.
    char* value,
        ///< [OUT] Buffer to write the value into.
    size_t valueSize,
        ///< [IN]
    const char* LE_NONNULL defaultValue
        ///< [IN] Default value to use if the original can't be
        ///<   read.
)
{
    return ifgen_le_cfg_GetString(
        GetCurrentSessionRef(),
        iteratorRef,
        path,
        value,
        valueSize,
        defaultValue
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Writes a string value to the config tree. Only valid during a write
 * transaction.
 *
 * If the path is empty, the iterator's current node will be set.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_SetString
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.
    const char* LE_NONNULL path,
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///< a path relative from the iterator's current position.
    const char* LE_NONNULL value
        ///< [IN] Value to write.
)
{
     ifgen_le_cfg_SetString(
        GetCurrentSessionRef(),
        iteratorRef,
        path,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 *  Read a binary data from the config tree.  If the the node has a wrong type, is
 *  empty or doesn't exist, the default value will be returned.
 *
 *  Valid for both read and write transactions.
 *
 *  If the path is empty, the iterator's current node will be read.
 *
 *  \b Responds \b With:
 *
 *  This function will respond with one of the following values:
 *
 *          - LE_OK             - Read was completed successfully.
 *          - LE_FORMAT_ERROR   - if data can't be decoded.
 *          - LE_OVERFLOW       - Supplied buffer was not large enough to hold the value.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_cfg_GetBinary
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.
    const char* LE_NONNULL path,
        ///< [IN] Path to the target node. Can be an absolute path,
        ///< or a path relative from the iterator's current
        ///< position.
    uint8_t* valuePtr,
        ///< [OUT] Buffer to write the value into.
    size_t* valueSizePtr,
        ///< [INOUT]
    const uint8_t* defaultValuePtr,
        ///< [IN] Default value to use if the original can't be
        ///<   read.
    size_t defaultValueSize
        ///< [IN]
)
{
    return ifgen_le_cfg_GetBinary(
        GetCurrentSessionRef(),
        iteratorRef,
        path,
        valuePtr,
        valueSizePtr,
        defaultValuePtr,
        defaultValueSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 *  Write a binary data to the config tree.  Only valid during a write
 *  transaction.
 *
 *  If the path is empty, the iterator's current node will be set.
 *
 *  @note Binary data cannot be written to the 'system' tree.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_SetBinary
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.
    const char* LE_NONNULL path,
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///< a path relative from the iterator's current position.
    const uint8_t* valuePtr,
        ///< [IN] Value to write.
    size_t valueSize
        ///< [IN]
)
{
     ifgen_le_cfg_SetBinary(
        GetCurrentSessionRef(),
        iteratorRef,
        path,
        valuePtr,
        valueSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Reads a signed integer value from the config tree.
 *
 * If the underlying value is not an integer, the default value will be returned instead. The
 * default value is also returned if the node does not exist or if it's empty.
 *
 * If the value is a floating point value, then it will be rounded and returned as an integer.
 *
 * Valid for both read and write transactions.
 *
 * If the path is empty, the iterator's current node will be read.
 */
//--------------------------------------------------------------------------------------------------
int32_t le_cfg_GetInt
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.
    const char* LE_NONNULL path,
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///< a path relative from the iterator's current position.
    int32_t defaultValue
        ///< [IN] Default value to use if the original can't be
        ///<   read.
)
{
    return ifgen_le_cfg_GetInt(
        GetCurrentSessionRef(),
        iteratorRef,
        path,
        defaultValue
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Writes a signed integer value to the config tree. Only valid during a
 * write transaction.
 *
 * If the path is empty, the iterator's current node will be set.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_SetInt
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.
    const char* LE_NONNULL path,
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///< a path relative from the iterator's current position.
    int32_t value
        ///< [IN] Value to write.
)
{
     ifgen_le_cfg_SetInt(
        GetCurrentSessionRef(),
        iteratorRef,
        path,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Reads a 64-bit floating point value from the config tree.
 *
 * If the value is an integer then the value will be promoted to a float. Otherwise, if the
 * underlying value is not a float or integer, the default value will be returned.
 *
 * If the path is empty, the iterator's current node will be read.
 *
 * @note Floating point values will only be stored up to 6 digits of precision.
 */
//--------------------------------------------------------------------------------------------------
double le_cfg_GetFloat
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.
    const char* LE_NONNULL path,
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///< a path relative from the iterator's current position.
    double defaultValue
        ///< [IN] Default value to use if the original can't be
        ///<   read.
)
{
    return ifgen_le_cfg_GetFloat(
        GetCurrentSessionRef(),
        iteratorRef,
        path,
        defaultValue
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Writes a 64-bit floating point value to the config tree. Only valid
 * during a write transaction.
 *
 * If the path is empty, the iterator's current node will be set.
 *
 * @note Floating point values will only be stored up to 6 digits of precision.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_SetFloat
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.
    const char* LE_NONNULL path,
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///< a path relative from the iterator's current position.
    double value
        ///< [IN] Value to write.
)
{
     ifgen_le_cfg_SetFloat(
        GetCurrentSessionRef(),
        iteratorRef,
        path,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Reads a value from the tree as a boolean. If the node is empty or doesn't exist, the default
 * value is returned. Default value is also returned if the node is a different type than
 * expected.
 *
 * Valid for both read and write transactions.
 *
 * If the path is empty, the iterator's current node will be read.
 */
//--------------------------------------------------------------------------------------------------
bool le_cfg_GetBool
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.
    const char* LE_NONNULL path,
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///< a path relative from the iterator's current position.
    bool defaultValue
        ///< [IN] Default value to use if the original can't be
        ///<   read.
)
{
    return ifgen_le_cfg_GetBool(
        GetCurrentSessionRef(),
        iteratorRef,
        path,
        defaultValue
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Writes a boolean value to the config tree. Only valid during a write
 * transaction.
 *
 * If the path is empty, the iterator's current node will be set.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_SetBool
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.
    const char* LE_NONNULL path,
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///< a path relative from the iterator's current position.
    bool value
        ///< [IN] Value to write.
)
{
     ifgen_le_cfg_SetBool(
        GetCurrentSessionRef(),
        iteratorRef,
        path,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Deletes the node specified by the path. If the node doesn't exist, nothing happens. All child
 * nodes are also deleted.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_QuickDeleteNode
(
    const char* LE_NONNULL path
        ///< [IN] Path to the node to delete.
)
{
     ifgen_le_cfg_QuickDeleteNode(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Clears the current value of a node. If the node doesn't currently exist then it is created as a
 * new empty node.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_QuickSetEmpty
(
    const char* LE_NONNULL path
        ///< [IN] Absolute or relative path to read from.
)
{
     ifgen_le_cfg_QuickSetEmpty(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Reads a string value from the config tree. If the value isn't a string, or if the node is
 * empty or doesn't exist, the default value will be returned.
 *
 * @return - LE_OK       - Commit was completed successfully.
 *         - LE_OVERFLOW - Supplied string buffer was not large enough to hold the value.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_cfg_QuickGetString
(
    const char* LE_NONNULL path,
        ///< [IN] Path to read from.
    char* value,
        ///< [OUT] Value read from the requested node.
    size_t valueSize,
        ///< [IN]
    const char* LE_NONNULL defaultValue
        ///< [IN] Default value to use if the original can't be read.
)
{
    return ifgen_le_cfg_QuickGetString(
        GetCurrentSessionRef(),
        path,
        value,
        valueSize,
        defaultValue
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Writes a string value to the config tree.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_QuickSetString
(
    const char* LE_NONNULL path,
        ///< [IN] Path to the value to write.
    const char* LE_NONNULL value
        ///< [IN] Value to write.
)
{
     ifgen_le_cfg_QuickSetString(
        GetCurrentSessionRef(),
        path,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Reads a binary data from the config tree. If the node type is different, or if the node is
 * empty or doesn't exist, the default value will be returned.
 *
 * @return - LE_OK              - Commit was completed successfully.
 *         - LE_FORMAT_ERROR    - if data can't be decoded.
 *         - LE_OVERFLOW        - Supplied buffer was not large enough to hold the value.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_cfg_QuickGetBinary
(
    const char* LE_NONNULL path,
        ///< [IN] Path to the target node.
    uint8_t* valuePtr,
        ///< [OUT] Buffer to write the value into.
    size_t* valueSizePtr,
        ///< [INOUT]
    const uint8_t* defaultValuePtr,
        ///< [IN] Default value to use if the original can't be
        ///<   read.
    size_t defaultValueSize
        ///< [IN]
)
{
    return ifgen_le_cfg_QuickGetBinary(
        GetCurrentSessionRef(),
        path,
        valuePtr,
        valueSizePtr,
        defaultValuePtr,
        defaultValueSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Writes a binary data to the config tree.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_QuickSetBinary
(
    const char* LE_NONNULL path,
        ///< [IN] Path to the target node.
    const uint8_t* valuePtr,
        ///< [IN] Value to write.
    size_t valueSize
        ///< [IN]
)
{
     ifgen_le_cfg_QuickSetBinary(
        GetCurrentSessionRef(),
        path,
        valuePtr,
        valueSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Reads a signed integer value from the config tree. If the value is a floating point
 * value, then it will be rounded and returned as an integer. Otherwise If the underlying value is
 * not an integer or a float, the default value will be returned instead.
 *
 * If the value is empty or the node doesn't exist, the default value is returned instead.
 */
//--------------------------------------------------------------------------------------------------
int32_t le_cfg_QuickGetInt
(
    const char* LE_NONNULL path,
        ///< [IN] Path to the value to write.
    int32_t defaultValue
        ///< [IN] Default value to use if the original can't be read.
)
{
    return ifgen_le_cfg_QuickGetInt(
        GetCurrentSessionRef(),
        path,
        defaultValue
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Writes a signed integer value to the config tree.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_QuickSetInt
(
    const char* LE_NONNULL path,
        ///< [IN] Path to the value to write.
    int32_t value
        ///< [IN] Value to write.
)
{
     ifgen_le_cfg_QuickSetInt(
        GetCurrentSessionRef(),
        path,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Reads a 64-bit floating point value from the config tree. If the value is an integer,
 * then it is promoted to a float. Otherwise, if the underlying value is not a float, or an
 * integer the default value will be returned.
 *
 * If the value is empty or the node doesn't exist, the default value is returned.
 *
 * @note Floating point values will only be stored up to 6 digits of precision.
 */
//--------------------------------------------------------------------------------------------------
double le_cfg_QuickGetFloat
(
    const char* LE_NONNULL path,
        ///< [IN] Path to the value to write.
    double defaultValue
        ///< [IN] Default value to use if the original can't be read.
)
{
    return ifgen_le_cfg_QuickGetFloat(
        GetCurrentSessionRef(),
        path,
        defaultValue
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Writes a 64-bit floating point value to the config tree.
 *
 * @note Floating point values will only be stored up to 6 digits of precision.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_QuickSetFloat
(
    const char* LE_NONNULL path,
        ///< [IN] Path to the value to write.
    double value
        ///< [IN] Value to write.
)
{
     ifgen_le_cfg_QuickSetFloat(
        GetCurrentSessionRef(),
        path,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Reads a value from the tree as a boolean. If the node is empty or doesn't exist, the default
 * value is returned. This is also true if the node is a different type than expected.
 *
 * If the value is empty or the node doesn't exist, the default value is returned instead.
 */
//--------------------------------------------------------------------------------------------------
bool le_cfg_QuickGetBool
(
    const char* LE_NONNULL path,
        ///< [IN] Path to the value to write.
    bool defaultValue
        ///< [IN] Default value to use if the original can't be read.
)
{
    return ifgen_le_cfg_QuickGetBool(
        GetCurrentSessionRef(),
        path,
        defaultValue
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Writes a boolean value to the config tree.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_QuickSetBool
(
    const char* LE_NONNULL path,
        ///< [IN] Path to the value to write.
    bool value
        ///< [IN] Value to write.
)
{
     ifgen_le_cfg_QuickSetBool(
        GetCurrentSessionRef(),
        path,
        value
    );
}