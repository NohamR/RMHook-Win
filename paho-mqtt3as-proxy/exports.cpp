#include "common.h"

paho_mqtt3as_dll paho_mqtt3as;

extern "C" {
#define DEFINE_FORWARDER_TARGET(name) FARPROC Original_##name = nullptr;
	PAHO_FORWARDER_EXPORTS(DEFINE_FORWARDER_TARGET)
#undef DEFINE_FORWARDER_TARGET
}
