#pragma once
#include <windows.h>

#include "MinHook.h"

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

#define PAHO_FORWARDER_EXPORTS(X) \
	X(MQTTAsync_connect) \
	X(MQTTAsync_create) \
	X(MQTTAsync_destroy) \
	X(MQTTAsync_disconnect) \
	X(MQTTAsync_free) \
	X(MQTTAsync_freeMessage) \
	X(MQTTAsync_getPendingTokens) \
	X(MQTTAsync_getVersionInfo) \
	X(MQTTAsync_global_init) \
	X(MQTTAsync_isComplete) \
	X(MQTTAsync_isConnected) \
	X(MQTTAsync_malloc) \
	X(MQTTAsync_reconnect) \
	X(MQTTAsync_send) \
	X(MQTTAsync_sendMessage) \
	X(MQTTAsync_setAfterPersistenceRead) \
	X(MQTTAsync_setBeforePersistenceWrite) \
	X(MQTTAsync_setCallbacks) \
	X(MQTTAsync_setConnected) \
	X(MQTTAsync_setConnectionLostCallback) \
	X(MQTTAsync_setDeliveryCompleteCallback) \
	X(MQTTAsync_setDisconnected) \
	X(MQTTAsync_setMessageArrivedCallback) \
	X(MQTTAsync_setTraceCallback) \
	X(MQTTAsync_setTraceLevel) \
	X(MQTTAsync_setUpdateConnectOptions) \
	X(MQTTAsync_strerror) \
	X(MQTTAsync_subscribe) \
	X(MQTTAsync_subscribeMany) \
	X(MQTTAsync_unsubscribe) \
	X(MQTTAsync_unsubscribeMany) \
	X(MQTTAsync_waitForCompletion) \
	X(MQTTProperties_add) \
	X(MQTTProperties_copy) \
	X(MQTTProperties_free) \
	X(MQTTProperties_getNumericValue) \
	X(MQTTProperties_getNumericValueAt) \
	X(MQTTProperties_getProperty) \
	X(MQTTProperties_getPropertyAt) \
	X(MQTTProperties_hasProperty) \
	X(MQTTProperties_propertyCount) \
	X(MQTTPropertyName) \
	X(MQTTProperty_getType) \
	X(MQTTReasonCode_toString) \
	X(Thread_create_mutex) \
	X(Thread_getid) \
	X(Thread_lock_mutex) \
	X(Thread_start) \
	X(Thread_unlock_mutex)

extern "C" {
#define DECLARE_FORWARDER_TARGET(name) extern FARPROC Original_##name;
	PAHO_FORWARDER_EXPORTS(DECLARE_FORWARDER_TARGET)
#undef DECLARE_FORWARDER_TARGET
}

void InstallHooks();
