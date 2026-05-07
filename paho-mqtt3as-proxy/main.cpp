#include "common.h"

DWORD WINAPI DelayedHelloThread(LPVOID lpParam)
{
	Sleep(10000); // 10 seconds
	MessageBox(0, "Hello :)", "Proxy", MB_OK | MB_ICONINFORMATION);
	return 0;
}

void LoadOriginalDllFunctions() {
	paho_mqtt3as.OrignalMQTTAsync_connect = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_connect");
	paho_mqtt3as.OrignalMQTTAsync_create = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_create");
	paho_mqtt3as.OrignalMQTTAsync_createWithOptions = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_createWithOptions");
	paho_mqtt3as.OrignalMQTTAsync_destroy = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_destroy");
	paho_mqtt3as.OrignalMQTTAsync_disconnect = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_disconnect");
	paho_mqtt3as.OrignalMQTTAsync_free = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_free");
	paho_mqtt3as.OrignalMQTTAsync_freeMessage = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_freeMessage");
	paho_mqtt3as.OrignalMQTTAsync_getPendingTokens = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_getPendingTokens");
	paho_mqtt3as.OrignalMQTTAsync_getVersionInfo = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_getVersionInfo");
	paho_mqtt3as.OrignalMQTTAsync_global_init = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_global_init");
	paho_mqtt3as.OrignalMQTTAsync_isComplete = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_isComplete");
	paho_mqtt3as.OrignalMQTTAsync_isConnected = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_isConnected");
	paho_mqtt3as.OrignalMQTTAsync_malloc = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_malloc");
	paho_mqtt3as.OrignalMQTTAsync_reconnect = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_reconnect");
	paho_mqtt3as.OrignalMQTTAsync_send = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_send");
	paho_mqtt3as.OrignalMQTTAsync_sendMessage = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_sendMessage");
	paho_mqtt3as.OrignalMQTTAsync_setAfterPersistenceRead = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_setAfterPersistenceRead");
	paho_mqtt3as.OrignalMQTTAsync_setBeforePersistenceWrite = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_setBeforePersistenceWrite");
	paho_mqtt3as.OrignalMQTTAsync_setCallbacks = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_setCallbacks");
	paho_mqtt3as.OrignalMQTTAsync_setConnected = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_setConnected");
	paho_mqtt3as.OrignalMQTTAsync_setConnectionLostCallback = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_setConnectionLostCallback");
	paho_mqtt3as.OrignalMQTTAsync_setDeliveryCompleteCallback = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_setDeliveryCompleteCallback");
	paho_mqtt3as.OrignalMQTTAsync_setDisconnected = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_setDisconnected");
	paho_mqtt3as.OrignalMQTTAsync_setMessageArrivedCallback = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_setMessageArrivedCallback");
	paho_mqtt3as.OrignalMQTTAsync_setTraceCallback = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_setTraceCallback");
	paho_mqtt3as.OrignalMQTTAsync_setTraceLevel = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_setTraceLevel");
	paho_mqtt3as.OrignalMQTTAsync_setUpdateConnectOptions = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_setUpdateConnectOptions");
	paho_mqtt3as.OrignalMQTTAsync_strerror = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_strerror");
	paho_mqtt3as.OrignalMQTTAsync_subscribe = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_subscribe");
	paho_mqtt3as.OrignalMQTTAsync_subscribeMany = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_subscribeMany");
	paho_mqtt3as.OrignalMQTTAsync_unsubscribe = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_unsubscribe");
	paho_mqtt3as.OrignalMQTTAsync_unsubscribeMany = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_unsubscribeMany");
	paho_mqtt3as.OrignalMQTTAsync_waitForCompletion = GetProcAddress(paho_mqtt3as.dll, "MQTTAsync_waitForCompletion");
	paho_mqtt3as.OrignalMQTTProperties_add = GetProcAddress(paho_mqtt3as.dll, "MQTTProperties_add");
	paho_mqtt3as.OrignalMQTTProperties_copy = GetProcAddress(paho_mqtt3as.dll, "MQTTProperties_copy");
	paho_mqtt3as.OrignalMQTTProperties_free = GetProcAddress(paho_mqtt3as.dll, "MQTTProperties_free");
	paho_mqtt3as.OrignalMQTTProperties_getNumericValue = GetProcAddress(paho_mqtt3as.dll, "MQTTProperties_getNumericValue");
	paho_mqtt3as.OrignalMQTTProperties_getNumericValueAt = GetProcAddress(paho_mqtt3as.dll, "MQTTProperties_getNumericValueAt");
	paho_mqtt3as.OrignalMQTTProperties_getProperty = GetProcAddress(paho_mqtt3as.dll, "MQTTProperties_getProperty");
	paho_mqtt3as.OrignalMQTTProperties_getPropertyAt = GetProcAddress(paho_mqtt3as.dll, "MQTTProperties_getPropertyAt");
	paho_mqtt3as.OrignalMQTTProperties_hasProperty = GetProcAddress(paho_mqtt3as.dll, "MQTTProperties_hasProperty");
	paho_mqtt3as.OrignalMQTTProperties_propertyCount = GetProcAddress(paho_mqtt3as.dll, "MQTTProperties_propertyCount");
	paho_mqtt3as.OrignalMQTTPropertyName = GetProcAddress(paho_mqtt3as.dll, "MQTTPropertyName");
	paho_mqtt3as.OrignalMQTTProperty_getType = GetProcAddress(paho_mqtt3as.dll, "MQTTProperty_getType");
	paho_mqtt3as.OrignalMQTTReasonCode_toString = GetProcAddress(paho_mqtt3as.dll, "MQTTReasonCode_toString");
	paho_mqtt3as.OrignalThread_create_mutex = GetProcAddress(paho_mqtt3as.dll, "Thread_create_mutex");
	paho_mqtt3as.OrignalThread_getid = GetProcAddress(paho_mqtt3as.dll, "Thread_getid");
	paho_mqtt3as.OrignalThread_lock_mutex = GetProcAddress(paho_mqtt3as.dll, "Thread_lock_mutex");
	paho_mqtt3as.OrignalThread_start = GetProcAddress(paho_mqtt3as.dll, "Thread_start");
	paho_mqtt3as.OrignalThread_unlock_mutex = GetProcAddress(paho_mqtt3as.dll, "Thread_unlock_mutex");
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		DisableThreadLibraryCalls(hModule);
        
        MH_Initialize();

		paho_mqtt3as.dll = LoadLibrary("paho-mqtt3as_orig.dll");
		if (paho_mqtt3as.dll == NULL)
		{
			MessageBox(0, "Cannot load original paho_mqtt3as.dll library", "Proxy", MB_ICONERROR);
			ExitProcess(0);
		}
		
		LoadOriginalDllFunctions();
        InstallHooks();

		break;
	}
	case DLL_PROCESS_DETACH:
	{
		FreeLibrary(paho_mqtt3as.dll);
	}
	break;
	}
	return TRUE;
}
