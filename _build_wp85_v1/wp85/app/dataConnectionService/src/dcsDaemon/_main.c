
// Startup code for the executable 'dcsDaemon'.
// This is a generated file, do not edit.

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"
#include <dlfcn.h>


// Define IPC API interface names.
LE_SHARED const char* _watchdogChain_le_wdog_ServiceInstanceName = "dcsDaemon.watchdogChain.le_wdog";
LE_SHARED const char* _dcsDaemon_le_data_ServiceInstanceName = "le_data";
LE_SHARED const char* _dcsDaemon_le_mrc_ServiceInstanceName = "dcsDaemon.dcsDaemon.le_mrc";
LE_SHARED const char* _dcsDaemon_le_cfg_ServiceInstanceName = "dcsDaemon.dcsDaemon.le_cfg";
LE_SHARED const char* _dcsDaemon_le_wifiClient_ServiceInstanceName = "dcsDaemon.dcsDaemon.le_wifiClient";
LE_SHARED const char* _dcs_le_dcs_ServiceInstanceName = "le_dcs";
LE_SHARED const char* _dcs_le_mdc_ServiceInstanceName = "dcsDaemon.dcs.le_mdc";
LE_SHARED const char* _dcs_le_mrc_ServiceInstanceName = "dcsDaemon.dcs.le_mrc";
LE_SHARED const char* _dcs_le_cfg_ServiceInstanceName = "dcsDaemon.dcs.le_cfg";
LE_SHARED const char* _dcs_le_appInfo_ServiceInstanceName = "dcsDaemon.dcs.le_appInfo";
LE_SHARED const char* _dcsNet_le_net_ServiceInstanceName = "le_net";
LE_SHARED const char* _dcsCellular_le_mdc_ServiceInstanceName = "dcsDaemon.dcsCellular.le_mdc";
LE_SHARED const char* _dcsCellular_le_mrc_ServiceInstanceName = "dcsDaemon.dcsCellular.le_mrc";
LE_SHARED const char* _dcsCellular_le_cfg_ServiceInstanceName = "dcsDaemon.dcsCellular.le_cfg";
LE_SHARED const char* _dcsWifi_le_wifiClient_ServiceInstanceName = "dcsDaemon.dcsWifi.le_wifiClient";

// Define default component's log session variables.
LE_SHARED le_log_SessionRef_t dcsDaemon_exe_LogSession;
LE_SHARED le_log_Level_t* dcsDaemon_exe_LogLevelFilterPtr;

// Loads a library using dlopen().
__attribute__((unused)) static void LoadLib
(
    const char* libName
)
{
    dlopen(libName, RTLD_LAZY | RTLD_GLOBAL);
    const char* errorMsg = dlerror();
    LE_FATAL_IF(errorMsg != NULL,
                "Failed to load library '%s' (%s)",                libName,
                errorMsg);
}


int main(int argc, const char* argv[])
{
    // Pass the args to the Command Line Arguments API.
    le_arg_SetArgs((size_t)argc, argv);
    // Make stdout line buffered so printf shows up in logs without flushing.
    setlinebuf(stdout);

    dcsDaemon_exe_LogSession = log_RegComponent("dcsDaemon_exe", &dcsDaemon_exe_LogLevelFilterPtr);

    // Connect to the log control daemon.
    // Note that there are some rare cases where we don't want the
    // process to try to connect to the Log Control Daemon (e.g.,
    // the Supervisor and the Service Directory shouldn't).
    // The NO_LOG_CONTROL macro can be used to control that.
    #ifndef NO_LOG_CONTROL
        log_ConnectToControlDaemon();
    #else
        LE_DEBUG("Not connecting to the Log Control Daemon.");
    #endif

    // Load dynamic libraries.
    LoadLib("libComponent_le_pa_dcs_default.so");
    LoadLib("libComponent_le_pa_dcs.so");
    LoadLib("libComponent_watchdogChain.so");
    LoadLib("libComponent_dcsDaemon.so");
    LoadLib("libComponent_dcs.so");
    LoadLib("libComponent_dcsNet.so");
    LoadLib("libComponent_dcsCellular.so");
    LoadLib("libComponent_dcsWifi.so");

    // Set the Signal Fault handler
    le_sig_InstallShowStackHandler();

    // Set the Signal Term handler
    le_sig_InstallDefaultTermHandler();

    LE_DEBUG("== Starting Event Processing Loop ==");
    le_event_RunLoop();
    LE_FATAL("== SHOULDN'T GET HERE! ==");
}
