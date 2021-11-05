#include "Module_DebugLog.h"

void debugLog_init()
{
	uint32_t err_code;

	err_code = NRF_LOG_INIT(NULL);
	APP_ERROR_CHECK(err_code);

	NRF_LOG_DEFAULT_BACKENDS_INIT();
}

//void debugLog_Error(char const * const p_str)
//{
//	NRF_LOG_ERROR(p_str);
//	NRF_LOG_FLUSH();
//}

//void debugLog_Warning(char const * const p_str)
//{
//	NRF_LOG_WARNING(p_str);
//	NRF_LOG_FLUSH();
//}

//void debugLog_Info(char const * const p_str)
//{
//	NRF_LOG_INFO(p_str);
//	NRF_LOG_FLUSH();
//}

//void debugLog_Debug(char const * const p_str)
//{
//	NRF_LOG_DEBUG(p_str);
//	NRF_LOG_FLUSH();
//}