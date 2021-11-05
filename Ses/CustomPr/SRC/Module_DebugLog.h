#ifndef MODULE_DEBUGLOG_H
#define MODULE_DEBUGLOG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "string.h"

	void debugLog_init();

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // MODULE_DEBUGLOG_H