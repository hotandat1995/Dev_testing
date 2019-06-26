

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

/**
 * @page c_le_wifi_client WiFi Client Service
 *
 * @ref le_wifiClient_interface.h "API Reference"
 *
 * <HR>
 *
 * This API provides WiFi Client setup.
 * Please note that the WiFi Client cannot be used at the same time as the WiFi Access Points
 * service, due to the sharing of same wifi hardware.
 *
 * @section le_wifi_binding IPC interfaces binding
 *
 *
 * Here's a code sample binding to WiFi service:
 * @verbatim
   bindings:
   {
      clientExe.clientComponent.le_wifiClient -> wifiService.le_wifiClient
   }
   @endverbatim
 *
 * @section le_wifiClient_Start Starting the WiFi Client
 *
 * First of all the function le_wifiClient_Start() must be called to start the WiFi Service.
 * - le_wifiClient_Start(): returns LE_OK if the call went ok.
 *   If WiFi Access Point is active, this will fail.
 *
 *
 * To subscribe to wifi events le_wifiClient_AddConnectionEventHandler() is to be called. The event
 * indication contains the event type, interface name, AP bssid, and disconnection cause.
 * - le_wifiClient_AddConnectionEventHandler(): returns the handler reference if the call went ok.
 *
 * @code
 *
 * static void EventHandler
 * (
 *     const le_wifiClient_EventInd_t* wifiEventPtr,
 *     void *contextPtr
 * )
 * {
 *     switch( wifiEventPtr->event )
 *     {
 *          case LE_WIFICLIENT_EVENT_CONNECTED:
 *          {
 *              LE_INFO("WiFi Client Connected.");
 *              LE_INFO("Interface: %s, bssid: %s",
 *                      &wifiEventPtr->ifName[0],
 *                      &wifiEventPtr->apBssid[0]);
 *          }
 *          break;
 *          case LE_WIFICLIENT_EVENT_DISCONNECTED:
 *          {
 *              LE_INFO("WiFi Client Disconnected.");
 *              LE_INFO("Interface: %s, disconnectCause: %d",
 *                      &wifiEventPtr->ifName[0],
 *                      wifiEventPtr->cause);
 *          }
 *          break;
 *          case LE_WIFICLIENT_EVENT_SCAN_DONE:
 *          {
 *              LE_INFO("WiFi Client Scan is done.");
 *              MyHandleScanResult();
 *          }
 *          break;
 *     }
 * }
 *
 * le_wifiClient_ConnectionEventHandlerRef_t WifiEventHandlerRef = NULL;
 *
 * static void MyInit
 * (
 *     void
 * )
 * {
 *     le_result_t result = le_wifiClient_start();
 *
 *     if ( LE_OK == result )
 *     {
 *         LE_INFO("WiFi Client started.");
 *         WiFiEventHandlerRef = le_wifiClient_AddConnectionEventHandler( EventHandler, NULL );
 *     }
 *     else if ( LE_BUSY == result )
 *     {
 *         LE_INFO("ERROR: WiFi Client already started.");
 *     }
 *     else
 *     {
 *         LE_INFO("ERROR: WiFi Client not started.");
 *     }
 *
 * }
 *
 * @endcode
 *
 *
 * To subscribe to wifi events le_wifiClient_AddNewEventHandler() is to be called.
 * - le_wifiClient_AddNewEventHandler(): returns the handler reference if the call went ok.
 *
 * @deprecated le_wifiClient_AddNewEventHandler() will be removed in near future.
 * It will be replaced by le_wifiClient_AddConnectionEventHandler().
 *
 * @code
 *
 * static void EventHandler
 * (
 *     le_wifiClient_Event_t clientEvent,
 *     void *contextPtr
 * )
 * {
 *     switch( clientEvent )
 *     {
 *          case LE_WIFICLIENT_EVENT_CONNECTED:
 *          {
 *              LE_INFO("WiFi Client Connected.");
 *          }
 *          break;
 *          case LE_WIFICLIENT_EVENT_DISCONNECTED:
 *          {
 *              LE_INFO("WiFi Client Disconnected.");
 *          }
 *          break;
 *          case LE_WIFICLIENT_EVENT_SCAN_DONE:
 *          {
 *              LE_INFO("WiFi Client Scan is done.");
 *              MyHandleScanResult();
 *          }
 *          break;
 *     }
 * }
 *
 * le_wifiClient_NewEventHandler WiFiEventHandlerRef = NULL;
 *
 * static void MyInit
 * (
 *     void
 * )
 * {
 *     le_result_t result = le_wifiClient_start();
 *
 *     if ( LE_OK == result )
 *     {
 *         LE_INFO("WiFi Client started.");
 *         WiFiEventHandlerRef = le_wifiClient_AddNewEventHandler( EventHandler, NULL );
 *     }
 *     else if ( LE_BUSY == result )
 *     {
 *         LE_INFO("ERROR: WiFi Client already started.");
 *     }
 *     else
 *     {
 *         LE_INFO("ERROR: WiFi Client not started.");
 *     }
 *
 * }
 *
 * @endcode
 *
 *
 * @section le_wifiClient_scan Scanning Access Points with WiFi Client
 *
 * To start a scan for Access Points, the le_wifiClient_Scan() should be called.
 * - le_wifiClient_Scan(): returns the LE_OK if the call went ok.
 *
 *
 * @section le_wifiClient_scan_result Processing the WiFi scan results
 *
 * Once the scan results are available, the event LE_WIFICLIENT_EVENT_SCAN_DONE is received.
 * The found Access Points can then be gotten with
 * - le_wifiClient_GetFirstAccessPoint():  returns the Access Point if found. Else NULL.
 * - le_wifiClient_GetNextAccessPoint(): returns the next Access Point if found. Else NULL.
 *
 * The Access Points SSID, Service Set Identifier, is not a string.
 * It does however often contain human readable ASCII values.
 * It can be read with the following function:
 * - le_wifiClient_GetSsid() : returns the LE_OK if the SSID was read ok.
 *
 * The Access Points signal strength can be read with the following function:
 * - le_wifiClient_GetSignalStrength() : returns the signal strength in dBm of the AccessPoint
 *
 * @code
 *
 * static void MyHandleScanResult
 * (
 *     void
 * )
 * {
 *     uint8 ssid[MAX_SSID_BYTES];
 *     le_wifiClient_AccessPointRef_t accessPointRef = le_wifiClient_GetFirstAccessPoint();
 *
 *     while( NULL != accessPointRef )
 *     {
 *          result = le_wifiClient_GetSsid( accessPointRef, ssid, MAX_SSID_BYTES );
 *          if (( result == LE_OK ) && ( memcmp( ssid, "MySSID", 6) == 0 ))
 *          {
 *               LE_INFO("WiFi Client found.");
 *               break;
 *          }
 *          accessPointRef = le_wifiClient_GetNextAccessPoint();
 *     }
 * }
 *
 * @endcode
 *
 * @section le_wifiClient_connect_to_ap Connecting to Access Point
 *
 * First of all, an Access Point reference should be created using the SSID of the target Access
 * Point. Use the following function to create a reference:
 * - le_wifiClient_Create(): returns Access Point reference
 *
 * To set the pass phrase prior for the Access Point use the function:
 * - le_wifiClient_SetPassphrase(): returns the function execution status.
 *
 * WPA-Enterprise requires a username and password to authenticate.
 * To set them use the function:
 * - le_wifiClient_SetUserCredentials():  returns the function execution status.
 *
 * If an Access Point is hidden, it does not announce its presence and will not show up in scan.
 * So, the SSID of this Access Point must be known in advance. Then, use the following function to
 * allow connections to hidden Access Points:
 * le_wifiClient_SetHiddenNetworkAttribute(): returns the function execution status.
 *
 * Finally and when the Access Point parameters have been configured, use the following function to
 * attempt a connection:
 * - le_wifiClient_Connect():  returns the function execution status.
 *
 * @code
 *
 * static void MyConnectTo
 * (
 *     le_wifiClient_AccessPointRef_t accessPointRef
 * )
 * {
 *     le_result_t result;
 *     le_wifiClient_SetPassphrase ( accessPointRef, "Secret1" );
 *     result = le_wifiClient_Connect( accessPointRef );
 *     if (result == LE_OK)
 *     {
 *          LE_INFO("Connecting to AP.");
 *     }
 * }
 *
 * @endcode
 *
 * @section le_wifiClient_config_ssid_security Configure security protocol
 *
 * The Wifi security configurations are SSID-based so that different SSIDs or Wifi networks can
 * employ different security protocols independently. The following subsections explain the
 * wifiClient APIs that can be used to configure the supported security protocols onto an SSID.
 * The currently supported Wifi security protocols are:
 * - No or open security.
 * - WEP,
 * - WPA PSK and WPA2 PSK,
 * - WPA EAP PEAP and WPA2 EAP PEAP.
 *
 * To ensure high confidentiality, all configured credentials including any keys, usernames,
 * passwords, phrases, secrets, etc., are stored in Legato's secured storage. There they are
 * encrypted and can only be accessed by the wifiClient component. But to allow easy cross-checking
 * and debugging of non-confidential Wifi configurations, an SSID's configured security protocol is
 * still saved on the config tree under the same path as for other Wifi configurations, i.e.
 * "dataConnectionService:/wifi/channel/<ssid>/secProtocol" as in the following example. Note that
 * an application doesn't need to explicitly set this onto the config tree itself, but can use the
 * APIs to be explained in the upcoming subsections for configuring security to complete this step.
 *
 * root@swi-mdm9x28:~# config get wifiService:/wifi/
 *  wifi/
 *    channel/
 *      MY-MOBILE/
 *        secProtocol<string> == 2
 *        hidden<bool> == false
 *      MY-WLAN/
 *        secProtocol<string> == 3
 *        hidden<bool> == true
 *
 * @subsection le_wifiClient_config_ssid_nosecurity Clear security
 *
 * The le_wifiClient_RemoveSsidSecurityConfigs() function can be used for the following purposes:
 * - Reset the security protocol type to use no security, i.e. LE_WIFICLIENT_SECURITY_NONE,
 * - Remove any previously installed Wifi security configurations including security protocol
 * type and any user credentials.
 *
 * The following is an example of how to use this API:
 *
 * @code
 *    le_result_t ret = le_wifiClient_RemoveSsidSecurityConfigs(ssid, sizeof(ssid));
 *    if ((ret != LE_OK) && (ret != LE_NOT_FOUND))
 *    {
 *        LE_ERROR("Failed to clear Wifi security configs; retcode %d", ret);
 *    }
 *    else
 *    {
 *        LE_INFO("Succeeded clearing Wifi security configs");
 *    }
 * @endcode
 *
 * @subsection le_wifiClient_config_ssid_wep Configure an SSID with WEP
 *
 * The API for configuring a given SSID to use WEP is le_wifiClient_ConfigureWep(). It will set
 * WEP as the security protocol used for this SSID and saved the provided WEP key into secured
 * storage for its later use via le_wifiClient_LoadSsid().
 *
 * The following is an example of how to use this API. The wepKey input has to be non-null.
 *
 * @code
 *    le_wifiClient_AccessPointRef_t ref;
 *    ret = le_wifiClient_ConfigureWep(ssid, sizeof(ssid), wepKey, sizeof(wepKey));
 *    if (ret != LE_OK)
 *    {
 *        LE_ERROR("Failed to configure WEP into secStore; retcode %d", ret);
 *    }
 *    else
 *    {
 *        ret = le_wifiClient_LoadSsid(ssid, sizeof(ssid), &ref);
 *        if (ret != LE_OK)
 *        {
 *            LE_ERROR("LoadSsid failed over WEP; retcode %d", ret);
 *        }
 *        else
 *        {
 *            le_wifiClient_Delete(ref);
 *            ref = NULL;
 *        }
 *    }
 * @endcode
 *
 * @subsection le_wifiClient_config_ssid_psk Configure an SSID with PSK
 *
 * The API for configuring a given SSID to use WPA or WPA2 PSK is le_wifiClient_ConfigurePsk().
 * It will set this security protocoly type as the one used for this SSID and saved the provided
 * credentials into secured storage for its later use via le_wifiClient_LoadSsid().
 *
 * With this protocol type, either a pass phrase or a pre-shared key has to be provided. If both
 * are provided, pass-phrase has precedence and will be used. But it fails to authenticate during
 * a connect attempt, wifiClient will not do a second attempt using an available pre-shared key.
 *
 * The following is an example of how to use this API. The passphrase input has to be non-null.
 *
 * @code
 *    le_wifiClient_AccessPointRef_t ref;
 *    ret = le_wifiClient_ConfigurePsk(ssid, sizeof(ssid), LE_WIFICLIENT_SECURITY_WPA_PSK_PERSONAL,
 *                                     passphrase, sizeof(passphrase), NULL, 0);
 *    if (ret != LE_OK)
 *    {
 *        LE_ERROR("Failed to configure WPA passphrase into secStore; retcode %d", ret);
 *    }
 *    else
 *    {
 *        ret = le_wifiClient_LoadSsid(ssid, sizeof(ssid), &ref);
 *        if (ret != LE_OK)
 *        {
 *            LE_ERROR("LoadSsid failed over WPA passphrase; retcode %d", ret);
 *        }
 *        else
 *        {
 *            le_wifiClient_Delete(ref);
 *            ref = NULL;
 *        }
 *    }
 * @endcode
 *
 * @subsection le_wifiClient_config_ssid_eap Configure an SSID with EAP
 *
 * The API for configuring a given SSID to use WPA or WPA2 EAP is le_wifiClient_ConfigureEap().
 * It will set this security protocoly type as the one used for this SSID and saved the provided
 * user name and password into secured storage for its later use via le_wifiClient_LoadSsid().
 *
 * The following is an example of how to use this API. Both the user name and password inputs have
 * to be non-null.
 *
 * @code
 *    le_wifiClient_AccessPointRef_t ref;
 *    ret = le_wifiClient_ConfigureEap(ssid, sizeof(ssid),
 *                                     LE_WIFICLIENT_SECURITY_WPA2_EAP_PEAP0_ENTERPRISE,
 *                                     username, sizeof(username), password, sizeof(password));
 *    if (ret != LE_OK)
 *    {
 *        LE_ERROR("Failed to configure WPA2 EAP into secStore; retcode %d", ret);
 *    }
 *    else
 *    {
 *        ret = le_wifiClient_LoadSsid(ssid, sizeof(ssid), &ref);
 *        if (ret != LE_OK)
 *        {
 *            LE_ERROR("LoadSsid failed over WPA2 EAP; retcode %d", ret);
 *        }
 *        else
 *        {
 *            le_wifiClient_Delete(ref);
 *            ref = NULL;
 *        }
 *    }
 * @endcode
 *
 * @section le_wifiClient_config_ssid Configure Wifi client with an SSID
 *
 * Before a Wifi connection can be established over an SSID via le_wifiClient_Connect(), a few
 * preparations have to be first done. The following are the ones that can be done via the
 * le_wifiClient_LoadSsid() function in this component:
 * - Have the SSID selected: The SSID provided in this function's input argument is considered
 * the one selected for establishing the Wifi connection.
 * - Load the pre-configured Wifi security configurations into wifiClient: These include the
 * security protocol type and its involved user credentials, e.g. WEP key for WEP, pass phrase or
 * pre-shared key for PSK, user name and password for EAP.
 * - Load other pre-configured Wifi configuration: So far there is only the Wifi hidden attribute
 * pre-configured via le_wifiClient_SetHiddenNetworkAttribute().
 * - Create an AP object reference: This reference is created for this given and configured SSID.
 * And it is to be returned to the API caller in its output argument so that it can be used as
 * the reference for subsequent connection operations including connecting, disconnecting, etc.
 *
 * The following is a sample code to illustrate how this API can be used:
 *
 * @code
 *
 *    le_result_t ret = le_wifiClient_LoadSsid(ssid, strlen(ssid), &apRef);
 *    if (ret == LE_OK)
 *    {
 *        LE_DEBUG("Wifi configs installed to connect over SSID %s with AP reference %p",
 *                 ssid, apRef);
 *    }
 *    else
 *    {
 *        LE_ERROR("Failed to install wifi configs to connect over SSID %s", ssid);
 *    }
 *
 * @endcode
 *
 * @section le_wifiClient_get_current_connect Get the currently selected connection
 *
 * A selected SSID via its AP reference is set for use in Wifi connection establishment since
 * the API call to le_wifiClient_Connect(). Note that while the input argument is actually an
 * Access Point reference, this reference specifically refers to a given SSID on the device.
 * This is considered the selected connection for use until le_wifiClient_Disconnect() is called
 * to deselect it.
 *
 * During the time when this AP reference is set for use, there comes the need to be able to query
 * le_wifiClient for it back. This is what this le_wifiClient_GetCurrentConnection() API seeks to
 * return. The following is a sample code to illustrate how it can be used. The retrieved AP
 * reference is returned in the output argument.
 *
 * @code
 *
 *    le_wifiClient_GetCurrentConnection(&apRef);
 *    if (!apRef)
 *    {
 *        return;
 *    }
 *    ret = le_wifiClient_GetSsid(apRef, &ssid[0], &ssidSize);
 *    if (LE_OK != ret)
 *    {
 *        LE_ERROR("Failed to find SSID of AP reference %p", apRef);
 *        return;
 *    }
 *    ssid[ssidSize] = '\0';
 *    LE_DEBUG("Found currently selected Wifi connection to get established: %s, reference %p",
 *             ssid, apRef);
 *
 * @endcode
 *
 *
 * <HR>
 *
 * Copyright (C) Sierra Wireless Inc.
 */
/**
 * @file le_wifiClient_interface.h
 *
 * Legato @ref c_le_wifi_client include file.
 *
 * Copyright (C) Sierra Wireless Inc.
 */

#ifndef LE_WIFICLIENT_INTERFACE_H_INCLUDE_GUARD
#define LE_WIFICLIENT_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"

// Interface specific includes
#include "le_wifiDefs_interface.h"

// Internal includes for this interface
#include "le_wifiClient_common.h"
//--------------------------------------------------------------------------------------------------
/**
 * Type for handler called when a server disconnects.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_wifiClient_DisconnectHandler_t)(void *);

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
void le_wifiClient_ConnectService
(
    void
);

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
le_result_t le_wifiClient_TryConnectService
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Set handler called when server disconnection is detected.
 *
 * When a server connection is lost, call this handler then exit with LE_FATAL.  If a program wants
 * to continue without exiting, it should call longjmp() from inside the handler.
 */
//--------------------------------------------------------------------------------------------------
LE_FULL_API void le_wifiClient_SetServerDisconnectHandler
(
    le_wifiClient_DisconnectHandler_t disconnectHandler,
    void *contextPtr
);

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
void le_wifiClient_DisconnectService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Reference type for AccessPoint that is returned by the WiFi Scan.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * WiFi Client Events.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * WiFi Client disconnect cause type enum.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * WiFi Client Security Protocol for connection
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Handler for WiFi Client changes
 * @deprecated le_wifiClient_AddNewEventHandler() will be removed in near future.
 * It is replaced by le_wifiClient_AddConnectionEventHandler().
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_wifiClient_NewEvent'
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * WiFi event indication structure. The disconnectionCause only applies to event EVENT_DISCONNECTED.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Handler for WiFi Client connection changes
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_wifiClient_ConnectionEvent'
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_wifiClient_NewEvent'
 *
 * This event provide information on WiFi Client event changes.
 * NewEvent will be deprecated.
 *
 */
//--------------------------------------------------------------------------------------------------
le_wifiClient_NewEventHandlerRef_t le_wifiClient_AddNewEventHandler
(
    le_wifiClient_NewEventHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_wifiClient_NewEvent'
 */
//--------------------------------------------------------------------------------------------------
void le_wifiClient_RemoveNewEventHandler
(
    le_wifiClient_NewEventHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_wifiClient_ConnectionEvent'
 *
 * This event provide information on WiFi Client connection event changes.
 *
 */
//--------------------------------------------------------------------------------------------------
le_wifiClient_ConnectionEventHandlerRef_t le_wifiClient_AddConnectionEventHandler
(
    le_wifiClient_ConnectionEventHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_wifiClient_ConnectionEvent'
 */
//--------------------------------------------------------------------------------------------------
void le_wifiClient_RemoveConnectionEventHandler
(
    le_wifiClient_ConnectionEventHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Start the WIFI device.
 *
 * @return
 *      - LE_OK     Function succeeded.
 *      - LE_FAULT  Function failed.
 *      - LE_BUSY   The WIFI device is already started.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_wifiClient_Start
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Stop the WIFI device.
 *
 * @return
 *      - LE_OK        Function succeeded.
 *      - LE_FAULT     Function failed.
 *      - LE_DUPLICATE The WIFI device is already stopped.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_wifiClient_Stop
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Start Scanning for WiFi Access points
 * Will result in event LE_WIFICLIENT_EVENT_SCAN_DONE when the scan results are available.
 *
 * @return
 *      - LE_OK     Function succeeded.
 *      - LE_FAULT  Function failed.
 *      - LE_BUSY   Scan already running.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_wifiClient_Scan
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the first WiFi Access Point found.
 *
 * @return
 *      - WiFi  Access Point reference if ok.
 *      - NULL  If no Access Point reference available.
 */
//--------------------------------------------------------------------------------------------------
le_wifiClient_AccessPointRef_t le_wifiClient_GetFirstAccessPoint
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the next WiFi Access Point.
 * Will return the Access Points in the order of found.
 * This function must be called in the same context as the GetFirstAccessPoint
 *
 * @return
 *      - WiFi  Access Point reference if ok.
 *      - NULL  If no Access Point reference available.
 */
//--------------------------------------------------------------------------------------------------
le_wifiClient_AccessPointRef_t le_wifiClient_GetNextAccessPoint
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the signal strength of the AccessPoint
 *
 * @return
 *      - Signal strength in dBm. Example -30 = -30dBm
 *      - If no signal available it will return LE_WIFICLIENT_NO_SIGNAL_STRENGTH
 *
 * @note The function returns the signal strength as reported at the time of the scan.
 */
//--------------------------------------------------------------------------------------------------
int16_t le_wifiClient_GetSignalStrength
(
    le_wifiClient_AccessPointRef_t accessPointRef
        ///< [IN] WiFi Access Point reference.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Basic Service set identifier (BSSID) of the AccessPoint
 *
 * @return
 *         LE_OK            Function succeeded.
 *         LE_FAULT         Function failed.
 *         LE_BAD_PARAMETER Invalid parameter.
 *         LE_OVERFLOW      bssid buffer is too small to contain the BSSID.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_wifiClient_GetBssid
(
    le_wifiClient_AccessPointRef_t accessPointRef,
        ///< [IN] WiFi Access Point reference.
    char* bssid,
        ///< [OUT] The BSSID
    size_t bssidSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Service set identification (SSID) of the AccessPoint
 *
 * @return
 *        LE_OK            Function succeeded.
 *        LE_FAULT         Function failed.
 *        LE_BAD_PARAMETER Invalid parameter.
 *        LE_OVERFLOW      ssid buffer is too small to contain the SSID.
 *
 * @note The SSID does not have to be human readable ASCII values, but often is.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_wifiClient_GetSsid
(
    le_wifiClient_AccessPointRef_t accessPointRef,
        ///< [IN] WiFi Access Point reference.
    uint8_t* ssidPtr,
        ///< [OUT] The SSID returned as a octet array.
    size_t* ssidSizePtr
        ///< [INOUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the currently selected connection to be established
 *
 * @return
 *      - LE_OK upon successful retrieval of the selected SSID to be connected
 *      - LE_FAULT upon failure to retrieve it
 */
//--------------------------------------------------------------------------------------------------
void le_wifiClient_GetCurrentConnection
(
    le_wifiClient_AccessPointRef_t* apRefPtr
        ///< [OUT] currently selected connection's AP reference
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the passphrase used to generate the PSK.
 *
 * @return
 *      - LE_OK            Function succeeded.
 *      - LE_FAULT         Function failed.
 *      - LE_BAD_PARAMETER Invalid parameter.
 *
 * @note The difference between le_wifiClient_SetPreSharedKey() and this function
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_wifiClient_SetPassphrase
(
    le_wifiClient_AccessPointRef_t accessPointRef,
        ///< [IN] WiFi Access Point reference.
    const char* LE_NONNULL PassPhrase
        ///< [IN] pass-phrase for PSK
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the Pre Shared Key, PSK.
 *
 * @return
 *      - LE_OK             Function succeeded.
 *      - LE_FAULT          Function failed.
 *      - LE_BAD_PARAMETER  Invalid parameter.
 *
 * @note This is one way to authenticate against the access point. The other one is provided by the
 * le_wifiClient_SetPassPhrase() function. Both ways are exclusive and are effective only when used
 * with WPA-personal authentication.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_wifiClient_SetPreSharedKey
(
    le_wifiClient_AccessPointRef_t accessPointRef,
        ///< [IN] WiFi Access Point reference.
    const char* LE_NONNULL PreSharedKey
        ///< [IN] PSK. Note the difference between PSK and
        ///< Pass Phrase.
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the security protocol for connection
 *
 * @return
 *      - LE_OK             Function succeeded.
 *      - LE_FAULT          Function failed.
 *      - LE_BAD_PARAMETER  Invalid parameter.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_wifiClient_SetSecurityProtocol
(
    le_wifiClient_AccessPointRef_t accessPointRef,
        ///< [IN] WiFi Access Point reference.
    le_wifiClient_SecurityProtocol_t securityProtocol
        ///< [IN] Security Mode
);

//--------------------------------------------------------------------------------------------------
/**
 * WPA-Enterprise requires a username and password to authenticate.
 * This function sets these parameters.
 *
 * @return
 *      - LE_OK             Function succeeded.
 *      - LE_FAULT          Function failed.
 *      - LE_BAD_PARAMETER  Invalid parameter.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_wifiClient_SetUserCredentials
(
    le_wifiClient_AccessPointRef_t accessPointRef,
        ///< [IN] WiFi Access Point reference.
    const char* LE_NONNULL userName,
        ///< [IN] UserName used for WPA-Enterprise.
    const char* LE_NONNULL password
        ///< [IN] Password used for WPA-Enterprise.
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the WEP key (WEP)
 *
 * @return
 *      - LE_OK     Function succeeded.
 *      - LE_FAULT  Function failed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_wifiClient_SetWepKey
(
    le_wifiClient_AccessPointRef_t accessPointRef,
        ///< [IN] WiFi Access Point reference.
    const char* LE_NONNULL wepKey
        ///< [IN] The WEP key
);

//--------------------------------------------------------------------------------------------------
/**
 * This function specifies whether the target Access Point is hiding its presence from clients or
 * not. When an Access Point is hidden, it cannot be discovered by a scan process.
 *
 * @return
 *      - LE_OK             Function succeeded.
 *      - LE_BAD_PARAMETER  Invalid parameter.
 *
 * @note By default, this attribute is not set which means that the client is unable to connect to
 * a hidden access point. When enabled, the client will be able to connect to the access point
 * whether it is hidden or not.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_wifiClient_SetHiddenNetworkAttribute
(
    le_wifiClient_AccessPointRef_t accessPointRef,
        ///< [IN] WiFi Access Point reference.
    bool hidden
        ///< [IN] If TRUE, the WIFI client will be able to connect to a hidden access point.
);

//--------------------------------------------------------------------------------------------------
/**
 * This function creates a reference to an Access Point given its SSID.
 * If an Access Point is hidden, it will not show up in the scan. So, its SSID must be known
 * in advance in order to create a reference.
 *
 * @return
 *      - AccessPoint reference to the current Access Point.
 *
 * @note This function fails if called while scan is running.
 */
//--------------------------------------------------------------------------------------------------
le_wifiClient_AccessPointRef_t le_wifiClient_Create
(
    const uint8_t* SsidPtr,
        ///< [IN] The SSID as a octet array.
    size_t SsidSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Deletes an accessPointRef.
 *
 * @return
 *      - LE_OK             Function succeeded.
 *      - LE_BAD_PARAMETER  Invalid parameter.
 *      - LE_BUSY           Function called during scan.
 *
 * @note The handle becomes invalid after it has been deleted.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_wifiClient_Delete
(
    le_wifiClient_AccessPointRef_t accessPointRef
        ///< [IN] WiFi Access Point reference.
);

//--------------------------------------------------------------------------------------------------
/**
 * Connect to the WiFi Access Point.
 * All authentication must be set prior to calling this function.
 *
 * @return
 *      - LE_OK             Function succeeded.
 *      - LE_BAD_PARAMETER  Invalid parameter.
 *
 * @note For PSK credentials see le_wifiClient_SetPassphrase() or le_wifiClient_SetPreSharedKey() .
 * @note For WPA-Enterprise credentials see le_wifiClient_SetUserCredentials()
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_wifiClient_Connect
(
    le_wifiClient_AccessPointRef_t accessPointRef
        ///< [IN] WiFi Access Point reference.
);

//--------------------------------------------------------------------------------------------------
/**
 * Disconnect from the current connected WiFi Access Point.
 *
 * @return
 *      - LE_OK     Function succeeded.
 *      - LE_FAULT  Function failed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_wifiClient_Disconnect
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Load the given SSID's configurations as it is selected as the connection to be established,
 * after creating for it an AP reference
 *
 * @return
 *      - LE_OK     Function succeeded.
 *      - LE_FAULT  Function failed.
 *      - LE_BAD_PARAMETER  Invalid parameter.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_wifiClient_LoadSsid
(
    const uint8_t* ssidPtr,
        ///< [IN] SSID which configs are to be installed
    size_t ssidSize,
        ///< [IN]
    le_wifiClient_AccessPointRef_t* apRefPtr
        ///< [OUT] reference to be created
);

//--------------------------------------------------------------------------------------------------
/**
 * Configure the given SSID to use WEP and the given WEP key in the respective input argument.
 * The WEP key is a mandatory input to be provided.
 *
 * @return
 *      - LE_OK     Succeeded to configure the givwn WEP key for the given SSID.
 *      - LE_FAULT  Failed to configure.
 *      - LE_BAD_PARAMETER  Invalid parameter.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_wifiClient_ConfigureWep
(
    const uint8_t* ssidPtr,
        ///< [IN] SSID which configs are to be installed
    size_t ssidSize,
        ///< [IN]
    const uint8_t* wepKeyPtr,
        ///< [IN] WEP key used for this SSID
    size_t wepKeySize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Configure the given SSID to use PSK and the given pass-phrase or pre-shared key in the
 * respective input arguments. The protocol input is mandatory and has to be set to either
 * LE_WIFICLIENT_SECURITY_WPA_PSK_PERSONAL or LE_WIFICLIENT_SECURITY_WPA2_PSK_PERSONAL.
 * Besides, it's mandatory to have at least one of the pass-phrase and pre-shared key supplied. If
 * both are provided as input, the pass-phrase has precedence and will be used. But it fails to
 * authenticate, a second attempt using the provided pre-shared key will not be done.
 *
 * @return
 *      - LE_OK     Succeeded to configure the given pass-phrase or pre-shared key for the given
 *                  SSID.
 *      - LE_FAULT  Failed to configure.
 *      - LE_BAD_PARAMETER  Invalid parameter.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_wifiClient_ConfigurePsk
(
    const uint8_t* ssidPtr,
        ///< [IN] SSID which configs are to be installed
    size_t ssidSize,
        ///< [IN]
    le_wifiClient_SecurityProtocol_t protocol,
        ///< [IN] security protocol WPA-PSK or WPA2-PSK
    const uint8_t* passPhrasePtr,
        ///< [IN] pass-phrase used for this SSID
    size_t passPhraseSize,
        ///< [IN]
    const uint8_t* pskPtr,
        ///< [IN] pre-shared key used for this SSID
    size_t pskSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Configure the given SSID to use EAP and the given EAP username and password in the respective
 * input arguments. The protocol input is mandatory and has to be set to either
 * LE_WIFICLIENT_SECURITY_WPA_EAP_PEAP0_ENTERPRISE or
 * LE_WIFICLIENT_SECURITY_WPA2_EAP_PEAP0_ENTERPRISE. Besides, both the username and password inputs
 * are mandatory.
 *
 * @return
 *      - LE_OK     Succeeded to configure the given EAP username and password for the given
 *                  SSID.
 *      - LE_FAULT  Failed to configure.
 *      - LE_BAD_PARAMETER  Invalid parameter.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_wifiClient_ConfigureEap
(
    const uint8_t* ssidPtr,
        ///< [IN] SSID which configs are to be installed
    size_t ssidSize,
        ///< [IN]
    le_wifiClient_SecurityProtocol_t protocol,
        ///< [IN] security protocol WPA-EAP or WPA2-EAP
    const uint8_t* usernamePtr,
        ///< [IN] EAP username used for this SSID
    size_t usernameSize,
        ///< [IN]
    const uint8_t* passwordPtr,
        ///< [IN] EAP password used for this SSID
    size_t passwordSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove and clear Wifi's security configurations to use with the given SSID from the config tree
 * and secured store. This includes the security protocol and all the username, password,
 * passphrase, pre-shared key, key, etc., previously configured via le_wifiClient_Configure APIs for
 * WEP, PSK and EAP.
 *
 * @return
 *      - LE_OK upon success to deconfigure the given SSID's configured user credentials;
 *        LE_FAULT otherwise
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_wifiClient_RemoveSsidSecurityConfigs
(
    const uint8_t* ssidPtr,
        ///< [IN]  SSID which user credentials to be deconfigured
    size_t ssidSize
        ///< [IN]
);

#endif // LE_WIFICLIENT_INTERFACE_H_INCLUDE_GUARD