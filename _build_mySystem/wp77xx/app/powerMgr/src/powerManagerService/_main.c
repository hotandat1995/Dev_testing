
// Startup code for the executable 'powerManagerService'.
// This is a generated file, do not edit.

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"
#include <dlfcn.h>


// Define IPC API interface names.
LE_SHARED const char* _watchdogChain_le_wdog_ServiceInstanceName = "powerManagerService.watchdogChain.le_wdog";
LE_SHARED const char* _powerMgr_le_pm_ServiceInstanceName = "le_pm";
LE_SHARED const char* _powerMgr_le_ulpm_ServiceInstanceName = "le_ulpm";
LE_SHARED const char* _powerMgr_le_bootReason_ServiceInstanceName = "le_bootReason";
LE_SHARED const char* _powerMgr_le_shutdown_ServiceInstanceName = "powerManagerService.powerMgr.le_shutdown";
LE_SHARED const char* _powerMgr_le_framework_ServiceInstanceName = "powerManagerService.powerMgr.le_framework";

// Define default component's log session variables.
LE_SHARED le_log_SessionRef_t powerManagerService_exe_LogSession;
LE_SHARED le_log_Level_t* powerManagerService_exe_LogLevelFilterPtr;

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

    powerManagerService_exe_LogSession = log_RegComponent("powerManagerService_exe", &powerManagerService_exe_LogLevelFilterPtr);

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
    LoadLib("libComponent_watchdogChain.so");
    LoadLib("libComponent_default.so");
    LoadLib("libComponent_powerMgr.so");

    // Set the Signal Fault handler
    le_sig_InstallShowStackHandler();

    // Set the Signal Term handler
    le_sig_InstallDefaultTermHandler();

    LE_DEBUG("== Starting Event Processing Loop ==");
    le_event_RunLoop();
    LE_FATAL("== SHOULDN'T GET HERE! ==");
}