#include "common.h"

paho_mqtt3as_dll paho_mqtt3as;
extern "C" FARPROC PA = NULL;

extern "C"
{
	extern "C" void FakeMQTTAsync_connect() {}
	extern "C" void FakeMQTTAsync_create() {}
	extern "C" void FakeMQTTAsync_createWithOptions() {}
	extern "C" void FakeMQTTAsync_destroy() {}
	extern "C" void FakeMQTTAsync_disconnect() {}
	extern "C" void FakeMQTTAsync_free() {}
	extern "C" void FakeMQTTAsync_freeMessage() {}
	extern "C" void FakeMQTTAsync_getPendingTokens() {}
	extern "C" void FakeMQTTAsync_getVersionInfo() {}
	extern "C" void FakeMQTTAsync_global_init() {}
	extern "C" void FakeMQTTAsync_isComplete() {}
	extern "C" void FakeMQTTAsync_isConnected() {}
	extern "C" void FakeMQTTAsync_malloc() {}
	extern "C" void FakeMQTTAsync_reconnect() {}
	extern "C" void FakeMQTTAsync_send() {}
	extern "C" void FakeMQTTAsync_sendMessage() {}
	extern "C" void FakeMQTTAsync_setAfterPersistenceRead() {}
	extern "C" void FakeMQTTAsync_setBeforePersistenceWrite() {}
	extern "C" void FakeMQTTAsync_setCallbacks() {}
	extern "C" void FakeMQTTAsync_setConnected() {}
	extern "C" void FakeMQTTAsync_setConnectionLostCallback() {}
	extern "C" void FakeMQTTAsync_setDeliveryCompleteCallback() {}
	extern "C" void FakeMQTTAsync_setDisconnected() {}
	extern "C" void FakeMQTTAsync_setMessageArrivedCallback() {}
	extern "C" void FakeMQTTAsync_setTraceCallback() {}
	extern "C" void FakeMQTTAsync_setTraceLevel() {}
	extern "C" void FakeMQTTAsync_setUpdateConnectOptions() {}
	extern "C" void FakeMQTTAsync_strerror() {}
	extern "C" void FakeMQTTAsync_subscribe() {}
	extern "C" void FakeMQTTAsync_subscribeMany() {}
	extern "C" void FakeMQTTAsync_unsubscribe() {}
	extern "C" void FakeMQTTAsync_unsubscribeMany() {}
	extern "C" void FakeMQTTAsync_waitForCompletion() {}
	extern "C" void FakeMQTTProperties_add() {}
	extern "C" void FakeMQTTProperties_copy() {}
	extern "C" void FakeMQTTProperties_free() {}
	extern "C" void FakeMQTTProperties_getNumericValue() {}
	extern "C" void FakeMQTTProperties_getNumericValueAt() {}
	extern "C" void FakeMQTTProperties_getProperty() {}
	extern "C" void FakeMQTTProperties_getPropertyAt() {}
	extern "C" void FakeMQTTProperties_hasProperty() {}
	extern "C" void FakeMQTTProperties_propertyCount() {}
	extern "C" void FakeMQTTPropertyName() {}
	extern "C" void FakeMQTTProperty_getType() {}
	extern "C" void FakeMQTTReasonCode_toString() {}
	extern "C" void FakeThread_create_mutex() {}
	extern "C" void FakeThread_getid() {}
	extern "C" void FakeThread_lock_mutex() {}
	extern "C" void FakeThread_start() {}
	extern "C" void FakeThread_unlock_mutex() {}
}

