#pragma once
#include <windows.h>

struct paho_mqtt3as_dll {
	HMODULE dll;
	FARPROC OrignalMQTTAsync_connect;
	FARPROC OrignalMQTTAsync_create;
	FARPROC OrignalMQTTAsync_createWithOptions;
	FARPROC OrignalMQTTAsync_destroy;
	FARPROC OrignalMQTTAsync_disconnect;
	FARPROC OrignalMQTTAsync_free;
	FARPROC OrignalMQTTAsync_freeMessage;
	FARPROC OrignalMQTTAsync_getPendingTokens;
	FARPROC OrignalMQTTAsync_getVersionInfo;
	FARPROC OrignalMQTTAsync_global_init;
	FARPROC OrignalMQTTAsync_isComplete;
	FARPROC OrignalMQTTAsync_isConnected;
	FARPROC OrignalMQTTAsync_malloc;
	FARPROC OrignalMQTTAsync_reconnect;
	FARPROC OrignalMQTTAsync_send;
	FARPROC OrignalMQTTAsync_sendMessage;
	FARPROC OrignalMQTTAsync_setAfterPersistenceRead;
	FARPROC OrignalMQTTAsync_setBeforePersistenceWrite;
	FARPROC OrignalMQTTAsync_setCallbacks;
	FARPROC OrignalMQTTAsync_setConnected;
	FARPROC OrignalMQTTAsync_setConnectionLostCallback;
	FARPROC OrignalMQTTAsync_setDeliveryCompleteCallback;
	FARPROC OrignalMQTTAsync_setDisconnected;
	FARPROC OrignalMQTTAsync_setMessageArrivedCallback;
	FARPROC OrignalMQTTAsync_setTraceCallback;
	FARPROC OrignalMQTTAsync_setTraceLevel;
	FARPROC OrignalMQTTAsync_setUpdateConnectOptions;
	FARPROC OrignalMQTTAsync_strerror;
	FARPROC OrignalMQTTAsync_subscribe;
	FARPROC OrignalMQTTAsync_subscribeMany;
	FARPROC OrignalMQTTAsync_unsubscribe;
	FARPROC OrignalMQTTAsync_unsubscribeMany;
	FARPROC OrignalMQTTAsync_waitForCompletion;
	FARPROC OrignalMQTTProperties_add;
	FARPROC OrignalMQTTProperties_copy;
	FARPROC OrignalMQTTProperties_free;
	FARPROC OrignalMQTTProperties_getNumericValue;
	FARPROC OrignalMQTTProperties_getNumericValueAt;
	FARPROC OrignalMQTTProperties_getProperty;
	FARPROC OrignalMQTTProperties_getPropertyAt;
	FARPROC OrignalMQTTProperties_hasProperty;
	FARPROC OrignalMQTTProperties_propertyCount;
	FARPROC OrignalMQTTPropertyName;
	FARPROC OrignalMQTTProperty_getType;
	FARPROC OrignalMQTTReasonCode_toString;
	FARPROC OrignalThread_create_mutex;
	FARPROC OrignalThread_getid;
	FARPROC OrignalThread_lock_mutex;
	FARPROC OrignalThread_start;
	FARPROC OrignalThread_unlock_mutex;
};

extern paho_mqtt3as_dll paho_mqtt3as;
extern "C" FARPROC PA;
