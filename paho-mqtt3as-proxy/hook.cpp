#include <windows.h>
#include <fstream>
#include "MinHook.h"
#include <shlobj.h>
#include <filesystem>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QtWebSockets/QWebSocket>

#include <QtCore/QUrl>
#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

static std::string GetLogPath()
{
	char localAppData[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPathA(
			NULL,
			CSIDL_LOCAL_APPDATA,
			NULL,
			0,
			localAppData)))
	{
		std::filesystem::path dir =
			std::filesystem::path(localAppData) / "RMHook";
		std::filesystem::create_directories(dir);
		return (dir / "rmhook.log").string();
	}
	return "rmhook.log";
}


static void Log(const std::string& msg)
{
	std::ofstream file(GetLogPath(), std::ios::app);
	if (file.is_open())
	{
		file << msg << std::endl;
	}
}


static std::string gConfiguredHost = "example.com";

static int gConfiguredPort = 443;

static void LoadConfig()
{
	char localAppData[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, localAppData)))
	{
		std::filesystem::path configPath = std::filesystem::path(localAppData) / "RMHook" / "config.json";
		QString qConfigPath = QString::fromStdString(configPath.string());

		QFile file(qConfigPath);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			QByteArray data = file.readAll();
			file.close();

			QJsonDocument doc = QJsonDocument::fromJson(data);
			if (!doc.isNull() && doc.isObject())
			{
				QJsonObject obj = doc.object();
				if (obj.contains("host"))
				{
					gConfiguredHost = obj["host"].toString().toStdString();
				}
				if (obj.contains("port"))
				{
					gConfiguredPort = obj["port"].toInt();
				}
				Log("[*] Loaded config: host=" + gConfiguredHost + ", port=" + std::to_string(gConfiguredPort));
				return;
			}
		}

		MessageBoxA(NULL, "First launch detected.\nUsing default config (example.com:443).\nYou can edit configuration in %LOCALAPPDATA%\\RMHook\\config.json", "RMHook Configuration", MB_OK | MB_ICONINFORMATION);

		std::filesystem::create_directories(configPath.parent_path());
		if (file.open(QIODevice::WriteOnly))
		{
			QJsonObject obj;
			obj["host"] = QString::fromStdString(gConfiguredHost);
			obj["port"] = gConfiguredPort;

			QJsonDocument doc(obj);
			file.write(doc.toJson());
			file.close();
		}
	}
}

static inline bool shouldPatchURL(const QString& host)
{
	if (host.isEmpty())
	{
		return false;
	}

	return QString(R"""(
		hwr-production-dot-remarkable-production.appspot.com
		service-manager-production-dot-remarkable-production.appspot.com
		local.appspot.com
		my.remarkable.com
		ping.remarkable.com
		internal.cloud.remarkable.com
		eu.tectonic.remarkable.com
		backtrace-proxy.cloud.remarkable.engineering
		dev.ping.remarkable.com
		dev.tectonic.remarkable.com
		dev.internal.cloud.remarkable.com
		eu.internal.tctn.cloud.remarkable.com
		webapp-prod.cloud.remarkable.engineering
		vernemq-prod.cloud.remarkable.engineering
		vernemq-dev.cloud.remarkable.engineering
	)""")
		.contains(host, Qt::CaseInsensitive);
}


typedef QNetworkReply* (__fastcall* QNAM_CreateRequest_t)(
	QNetworkAccessManager* self,
	QNetworkAccessManager::Operation op,
	const QNetworkRequest& req,
	QIODevice* outgoingData
);

typedef void (__fastcall* QWebSocket_Open_t)(
	QWebSocket* self,
	const QNetworkRequest& req
);

static QNAM_CreateRequest_t originalCreateRequest = nullptr;
static QWebSocket_Open_t originalWebSocketOpen = nullptr;

QNetworkReply* __fastcall hookedCreateRequest(
	QNetworkAccessManager* self,
	QNetworkAccessManager::Operation op,
	const QNetworkRequest& req,
	QIODevice* outgoingData)
{
	Log("[*] Intercepted createRequest");
	const QUrl url = req.url();
	const QString host = url.host();

	Log("[HTTP] Request to: " + url.toString().toStdString() + " (Method: " + std::to_string((int)op) + ")");

	if (shouldPatchURL(host)) {
		QNetworkRequest newReq(req);
		QUrl newUrl = url;
		newUrl.setHost(QString::fromStdString(gConfiguredHost));
		newUrl.setPort(gConfiguredPort);
		newReq.setUrl(newUrl);
		Log("[HTTP PATCHED] " + host.toStdString() + ":" + std::to_string(gConfiguredPort) + " -> " + newUrl.toString().toStdString());
		if (originalCreateRequest) {
			return originalCreateRequest(self, op, newReq, outgoingData);
		}
		return nullptr;
	}
	
	if (originalCreateRequest) {
			return originalCreateRequest(self, op, req, outgoingData);
	}
	return nullptr;
}


void __fastcall hookedWebSocketOpen(
	QWebSocket* self,
	const QNetworkRequest& req)
{
	Log("[*] Intercepted QWebSocket::open");
	if (!originalWebSocketOpen) {
		return;
	}

	const QUrl url = req.url();
	const QString host = url.host();

	Log("[WS] Opening: " + url.toString().toStdString());

	if (shouldPatchURL(host)) {
		QUrl newUrl = url;
		newUrl.setHost(QString::fromStdString(gConfiguredHost));
		newUrl.setPort(gConfiguredPort);
		QNetworkRequest newReq(req);
		newReq.setUrl(newUrl);
		Log("[WS PATCHED] " + host.toStdString() + ":" + std::to_string(gConfiguredPort) + " -> " + newUrl.toString().toStdString());
		originalWebSocketOpen(self, newReq);
		return;
	}
	originalWebSocketOpen(self, req);
}


typedef int(__cdecl* MQTTAsync_createWithOptions_t)(
    void**      handle,
    const char* serverURI,
    const char* clientId,
    int         persistence_type,
    void*       persistence_context,
    void*       options
);
static MQTTAsync_createWithOptions_t originalMQTTAsyncCreate = nullptr;

// typedef void* (__fastcall* async_client_ctor_t)(void* self, void* serverURI, void* clientId, void* persistence);
// static async_client_ctor_t originalAsyncClientCtor = nullptr;

// void* __fastcall hookedAsyncClientCtor(void* self, void* serverURI, void* clientId, void* persistence)
// {
//     Log("[MQTT] Intercepted mqtt::async_client constructor");
//     return self;
// }

// Patch a paho URI: "ssl://host.remarkable.com:port" -> "ssl://proxy:port"
// Returns patched string, or empty if no patch needed.
static std::string PatchMqttUri(const char* uri)
{
    if (!uri) return {};
    const std::string original(uri);

    size_t schemeEnd = original.find("://");
    size_t hostStart = (schemeEnd != std::string::npos) ? schemeEnd + 3 : 0;
    size_t hostEnd   = original.find_first_of(":/", hostStart);
    if (hostEnd == std::string::npos) hostEnd = original.size();

    const std::string origHost = original.substr(hostStart, hostEnd - hostStart);

    // Match any *.remarkable.com or *.remarkable.engineering host
    static const char* kSuffixes[] = {
        ".remarkable.com",
        ".remarkable.engineering",
        nullptr
    };
    bool shouldPatch = false;
    for (int i = 0; kSuffixes[i]; ++i)
    {
        const std::string suffix(kSuffixes[i]);
        if (origHost.size() >= suffix.size() &&
            origHost.compare(origHost.size() - suffix.size(),
                             suffix.size(), suffix) == 0)
        {
            shouldPatch = true;
            break;
        }
    }
    if (!shouldPatch) return {};

    std::string patched = original;
    patched.replace(hostStart, hostEnd - hostStart, gConfiguredHost);

    // Fix port
    size_t colonPos = patched.find(':', hostStart + gConfiguredHost.size());
    if (colonPos != std::string::npos)
    {
        size_t numEnd = patched.find_first_not_of("0123456789", colonPos + 1);
        if (numEnd == std::string::npos) numEnd = patched.size();
        patched.replace(colonPos + 1, numEnd - colonPos - 1,
                        std::to_string(gConfiguredPort));
    }
    return patched;
}

int __cdecl hookedMQTTAsyncCreate(
    void**      handle,
    const char* serverURI,
    const char* clientId,
    int         persistence_type,
    void*       persistence_context,
    void*       options)
{
    Log("[MQTT] MQTTAsync_createWithOptions URI: " + std::string(serverURI ? serverURI : "(null)") + " ClientId: " + (clientId ? clientId : "(null)") + " PersistenceType: " + std::to_string(persistence_type));

    const char* finalUri = serverURI;
	std::string patched;
	patched = PatchMqttUri(serverURI);
	if (!patched.empty())
	{
		finalUri = patched.c_str();
	}
	int ret = originalMQTTAsyncCreate(
		handle,
		finalUri,
		clientId,
		persistence_type,
		persistence_context,
		options
	);
	Log("[MQTT] originalMQTTAsyncCreate returned " + std::to_string(ret) + " Final URI: " + std::string(finalUri));
	return ret;
    // return -1; // Return error to prevent actual MQTT connection attempts until we implement the full patch.
}



void* ResolveExport(HMODULE module, const char* symbol)
{
	if (!symbol) return nullptr;
	void* addr = (void*)GetProcAddress(module, symbol);
	if (!addr)
	{
		Log(std::string("[ERROR] Failed to resolve symbol: ") + symbol);
	}
	Log(std::string("[+] Resolved ") + symbol + " at " + std::to_string(reinterpret_cast<uintptr_t>(addr)));
	return addr;
}

void InstallHooks()
{
	LoadConfig();

	Log("[*] Initializing MinHook");

	if (MH_Initialize() != MH_OK)
	{
		Log("[ERROR] MH_Initialize failed");
		return;
	}

	HMODULE qtNetwork = nullptr;
	HMODULE qtWebSockets = nullptr;
	HMODULE mqttCLib = nullptr;
	// HMODULE mqttCppLib = nullptr;
	HMODULE mainModule = GetModuleHandleA(NULL);

	while (!qtNetwork)
	{
		qtNetwork = GetModuleHandleA("Qt6Network.dll");
		Sleep(100);
	}

	while (!qtWebSockets)
	{
		qtWebSockets = GetModuleHandleA("Qt6WebSockets.dll");
		Sleep(100);
	}

	while (!mqttCLib)
	{
		mqttCLib = GetModuleHandleA("paho-mqtt3as.dll");
		Sleep(100);
	}

	// while (!mqttCppLib)
	// {
	// 	mqttCppLib = GetModuleHandleA("paho-mqttpp3.dll");
	// 	Sleep(100);
	// }

	Log("[+] Qt and MQTT DLLs loaded");

	void* createRequestAddr = ResolveExport(
		qtNetwork,
		"?createRequest@QNetworkAccessManager@@MEAAPEAVQNetworkReply@@W4Operation@1@AEBVQNetworkRequest@@PEAVQIODevice@@@Z"
	);

	void* webSocketOpenAddr = ResolveExport(
		qtWebSockets,
		"?open@QWebSocket@@QEAAXAEBVQNetworkRequest@@@Z"
	);

	// void* asyncClientCtorAddr = ResolveExport(
	// 	mqttCppLib,
	// 	"??0async_client@mqtt@@QEAA@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@0PEAViclient_persistence@1@@Z"
	// );

	// if (MH_CreateHook(
	// 		asyncClientCtorAddr,
	// 		&hookedAsyncClientCtor,
	// 		reinterpret_cast<void**>(&originalAsyncClientCtor)
	// 	) != MH_OK)
	// 	Log("[ERROR] Failed to hook mqtt::async_client constructor");
	// else
	// 	Log("[+] Hooked mqtt::async_client constructor");

	void* mqttCreateAddr = ResolveExport(mqttCLib, "MQTTAsync_createWithOptions");

	if (MH_CreateHook(
			mqttCreateAddr,
			&hookedMQTTAsyncCreate,
			reinterpret_cast<void**>(&originalMQTTAsyncCreate)
		) != MH_OK)
		Log("[ERROR] Failed to hook MQTTAsync_createWithOptions");
	else
		Log("[+] Hooked MQTTAsync_createWithOptions");

	if (MH_CreateHook(
			createRequestAddr,
			&hookedCreateRequest,
			reinterpret_cast<void**>(&originalCreateRequest)
		) != MH_OK)
	{
		Log("[ERROR] Failed to hook createRequest");
	}
	else
	{
		Log("[+] Hooked createRequest");
	}

	if (MH_CreateHook(
			webSocketOpenAddr,
			&hookedWebSocketOpen,
			reinterpret_cast<void**>(&originalWebSocketOpen)
		) != MH_OK)
	{
		Log("[ERROR] Failed to hook QWebSocket::open");
	}
	else
	{
		Log("[+] Hooked QWebSocket::open");
	}

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
	{
		Log("[ERROR] Failed to enable hooks");
	}
	else
	{
		Log("[+] Hooks enabled");
	}
}