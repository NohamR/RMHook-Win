#include <windows.h>
#include <fstream>
#include <iterator>
#include <shlobj.h>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <string>
#include <system_error>

#include "MinHook.h"
#include "common.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QtWebSockets/QWebSocket>

#include <QtCore/QUrl>
#include <QtCore/QString>
#include <QtCore/QByteArray>

static bool GetLocalAppDataPath(std::filesystem::path& path)
{
	char localAppData[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPathA(
			NULL,
			CSIDL_LOCAL_APPDATA,
			NULL,
			0,
			localAppData)))
	{
		path = localAppData;
		return true;
	}
	return false;
}

static std::string GetLogPath()
{
	std::filesystem::path localAppData;
	if (GetLocalAppDataPath(localAppData))
	{
		std::filesystem::path dir = localAppData / "RMHook";
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

static std::string SafeCString(const char* value)
{
	return value ? std::string(value) : std::string("(null)");
}

static std::string QStringToUtf8String(const QString& value)
{
	const QByteArray utf8 = value.toUtf8();
	if (utf8.isEmpty())
	{
		return {};
	}
	return std::string(utf8.constData(), static_cast<size_t>(utf8.size()));
}

static QString QStringFromUtf8String(const std::string& value)
{
	return QString::fromUtf8(value.data(), static_cast<qsizetype>(value.size()));
}

static void LogHttpRequest(const char* prefix, const QUrl& url, QNetworkAccessManager::Operation op)
{
	std::string message(prefix);
	message += QStringToUtf8String(url.toString());
	message += " (Method: ";
	message += std::to_string(static_cast<int>(op));
	message += ")";
	Log(message);
}

static void LogUrlPatch(const char* prefix, const QString& originalHost, int port, const QUrl& newUrl)
{
	std::string message(prefix);
	message += QStringToUtf8String(originalHost);
	message += ":";
	message += std::to_string(port);
	message += " -> ";
	message += QStringToUtf8String(newUrl.toString());
	Log(message);
}


static constexpr const char* kDefaultHost = "example.com";
static constexpr int kDefaultPort = 443;

static std::string gConfiguredHost = kDefaultHost;
static int gConfiguredPort = kDefaultPort;

static size_t SkipJsonWhitespace(const std::string& text, size_t pos)
{
	while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos])))
	{
		++pos;
	}
	return pos;
}

static bool FindJsonValueStart(const std::string& text, const char* key, size_t& valueStart)
{
	const std::string quotedKey = std::string("\"") + key + "\"";
	size_t keyPos = text.find(quotedKey);
	if (keyPos == std::string::npos)
	{
		return false;
	}

	size_t colonPos = text.find(':', keyPos + quotedKey.size());
	if (colonPos == std::string::npos)
	{
		return false;
	}

	valueStart = SkipJsonWhitespace(text, colonPos + 1);
	return valueStart < text.size();
}

static bool ReadJsonStringValue(const std::string& text, const char* key, std::string& value)
{
	size_t pos = 0;
	if (!FindJsonValueStart(text, key, pos) || text[pos] != '"')
	{
		return false;
	}

	++pos;
	std::string parsed;
	while (pos < text.size())
	{
		char ch = text[pos++];
		if (ch == '"')
		{
			value = parsed;
			return true;
		}

		if (ch == '\\' && pos < text.size())
		{
			char escaped = text[pos++];
			switch (escaped)
			{
				case '"':
				case '\\':
				case '/':
					parsed.push_back(escaped);
					break;
				case 'b':
					parsed.push_back('\b');
					break;
				case 'f':
					parsed.push_back('\f');
					break;
				case 'n':
					parsed.push_back('\n');
					break;
				case 'r':
					parsed.push_back('\r');
					break;
				case 't':
					parsed.push_back('\t');
					break;
				default:
					parsed.push_back(escaped);
					break;
			}
		}
		else
		{
			parsed.push_back(ch);
		}
	}

	return false;
}

static bool ReadJsonIntValue(const std::string& text, const char* key, int& value)
{
	size_t pos = 0;
	if (!FindJsonValueStart(text, key, pos))
	{
		return false;
	}

	char* end = nullptr;
	long parsed = std::strtol(text.c_str() + pos, &end, 10);
	if (end == text.c_str() + pos)
	{
		return false;
	}

	value = static_cast<int>(parsed);
	return true;
}

static std::string JsonEscapeString(const std::string& value)
{
	std::string escaped;
	escaped.reserve(value.size());
	for (char ch : value)
	{
		switch (ch)
		{
			case '"':
				escaped += "\\\"";
				break;
			case '\\':
				escaped += "\\\\";
				break;
			case '\b':
				escaped += "\\b";
				break;
			case '\f':
				escaped += "\\f";
				break;
			case '\n':
				escaped += "\\n";
				break;
			case '\r':
				escaped += "\\r";
				break;
			case '\t':
				escaped += "\\t";
				break;
			default:
				escaped.push_back(ch);
				break;
		}
	}
	return escaped;
}

static void LoadConfig()
{
	std::filesystem::path localAppData;
	if (!GetLocalAppDataPath(localAppData))
	{
		Log("[ERROR] Failed to resolve LOCALAPPDATA for config");
		return;
	}

	std::filesystem::path configPath = localAppData / "RMHook" / "config.json";
	Log("[*] Config path: " + configPath.string());

	std::error_code ec;
	if (std::filesystem::exists(configPath, ec))
	{
		std::ifstream file(configPath, std::ios::binary);
		if (!file.is_open())
		{
			Log("[ERROR] Failed to open config for reading: " + configPath.string());
		}
		else
		{
			const std::string data(
				(std::istreambuf_iterator<char>(file)),
				std::istreambuf_iterator<char>());

			std::string host;
			int port = 0;
			bool readAnySetting = false;
			if (ReadJsonStringValue(data, "host", host) && !host.empty())
			{
				gConfiguredHost = host;
				readAnySetting = true;
			}
			if (ReadJsonIntValue(data, "port", port) && port > 0 && port <= 65535)
			{
				gConfiguredPort = port;
				readAnySetting = true;
			}
			if (readAnySetting)
			{
				Log("[*] Loaded config: host=" + gConfiguredHost + ", port=" + std::to_string(gConfiguredPort));
				return;
			}

			Log("[ERROR] Config exists but no valid host or port was found");
		}
	}
	else if (ec)
	{
		Log("[ERROR] Failed to check config existence: " + ec.message());
	}

	MessageBoxA(NULL, "First launch detected.\nUsing default config (example.com:443).\nYou can edit configuration in %LOCALAPPDATA%\\RMHook\\config.json", "RMHook Configuration", MB_OK | MB_ICONINFORMATION);

	std::filesystem::create_directories(configPath.parent_path(), ec);
	if (ec)
	{
		Log("[ERROR] Failed to create config directory: " + ec.message());
		return;
	}

	const std::string json =
		"{\n"
		"    \"host\": \"" + JsonEscapeString(gConfiguredHost) + "\",\n"
		"    \"port\": " + std::to_string(gConfiguredPort) + "\n"
		"}\n";
	std::ofstream file(configPath, std::ios::binary | std::ios::trunc);
	if (!file.is_open())
	{
		Log("[ERROR] Failed to open config for writing: " + configPath.string());
		return;
	}

	file.write(json.data(), static_cast<std::streamsize>(json.size()));
	file.close();
	Log("[*] Created default config: " + configPath.string());
}

static bool ShouldPatchHost(const QString& host)
{
	if (host.isEmpty())
	{
		return false;
	}

	static constexpr const char* kPatchHosts[] = {
		"hwr-production-dot-remarkable-production.appspot.com",
		"service-manager-production-dot-remarkable-production.appspot.com",
		"local.appspot.com",
		"my.remarkable.com",
		"ping.remarkable.com",
		"internal.cloud.remarkable.com",
		"eu.tectonic.remarkable.com",
		"backtrace-proxy.cloud.remarkable.engineering",
		"dev.ping.remarkable.com",
		"dev.tectonic.remarkable.com",
		"dev.internal.cloud.remarkable.com",
		"eu.internal.tctn.cloud.remarkable.com",
		"webapp-prod.cloud.remarkable.engineering",
		"vernemq-prod.cloud.remarkable.engineering",
		"vernemq-dev.cloud.remarkable.engineering",
	};

	for (const char* patchHost : kPatchHosts)
	{
		if (host.compare(QString::fromLatin1(patchHost), Qt::CaseInsensitive) == 0)
		{
			return true;
		}
	}
	return false;
}

static void ApplyConfiguredEndpoint(QUrl& url)
{
	url.setHost(QStringFromUtf8String(gConfiguredHost));
	url.setPort(gConfiguredPort);
}


using QNAM_CreateRequest_t = QNetworkReply* (__fastcall*)(
	QNetworkAccessManager* self,
	QNetworkAccessManager::Operation op,
	const QNetworkRequest& req,
	QIODevice* outgoingData
);

using QWebSocket_Open_t = void (__fastcall*)(
	QWebSocket* self,
	const QNetworkRequest& req
);

static QNAM_CreateRequest_t originalCreateRequest = nullptr;
static QWebSocket_Open_t originalWebSocketOpen = nullptr;

static QNetworkReply* __fastcall hookedCreateRequest(
	QNetworkAccessManager* self,
	QNetworkAccessManager::Operation op,
	const QNetworkRequest& req,
	QIODevice* outgoingData)
{
	Log("[*] Intercepted createRequest");
	const QUrl url = req.url();
	const QString host = url.host();

	LogHttpRequest("[HTTP] Request to: ", url, op);

	if (ShouldPatchHost(host))
	{
		QNetworkRequest newReq(req);
		QUrl newUrl = url;
		ApplyConfiguredEndpoint(newUrl);
		newReq.setUrl(newUrl);
		LogUrlPatch("[HTTP PATCHED] ", host, gConfiguredPort, newUrl);
		if (originalCreateRequest)
		{
			return originalCreateRequest(self, op, newReq, outgoingData);
		}
		return nullptr;
	}
	
	if (originalCreateRequest)
	{
		return originalCreateRequest(self, op, req, outgoingData);
	}
	return nullptr;
}


static void __fastcall hookedWebSocketOpen(
	QWebSocket* self,
	const QNetworkRequest& req)
{
	Log("[*] Intercepted QWebSocket::open");
	if (!originalWebSocketOpen)
	{
		return;
	}

	const QUrl url = req.url();
	const QString host = url.host();

	std::string openMessage("[WS] Opening: ");
	openMessage += QStringToUtf8String(url.toString());
	Log(openMessage);

	if (ShouldPatchHost(host))
	{
		QUrl newUrl = url;
		ApplyConfiguredEndpoint(newUrl);
		QNetworkRequest newReq(req);
		newReq.setUrl(newUrl);
		LogUrlPatch("[WS PATCHED] ", host, gConfiguredPort, newUrl);
		originalWebSocketOpen(self, newReq);
		return;
	}
	originalWebSocketOpen(self, req);
}


using MQTTAsync_createWithOptions_t = int (__cdecl*)(
	void** handle,
	const char* serverURI,
	const char* clientId,
	int persistence_type,
	void* persistence_context,
	void* options
);

// Patch a paho URI: "ssl://host.remarkable.com:port" -> "ssl://proxy:port"
// Returns patched string, or empty if no patch needed.
static std::string PatchMqttUri(const char* uri)
{
	if (!uri)
	{
		return {};
	}

	const std::string original(uri);
	const size_t schemeEnd = original.find("://");
	const size_t hostStart = (schemeEnd != std::string::npos) ? schemeEnd + 3 : 0;
	size_t hostEnd = original.find_first_of(":/", hostStart);
	if (hostEnd == std::string::npos)
	{
		hostEnd = original.size();
	}

	const std::string origHost = original.substr(hostStart, hostEnd - hostStart);
	std::string hostLower = origHost;
	std::transform(hostLower.begin(), hostLower.end(), hostLower.begin(),
		[](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

	static constexpr const char* kSuffixes[] = {
		".remarkable.com",
		".remarkable.engineering",
	};

	bool shouldPatch = false;
	for (const char* suffix : kSuffixes)
	{
		const size_t suffixLength = std::strlen(suffix);
		if (hostLower.size() >= suffixLength &&
			hostLower.compare(hostLower.size() - suffixLength, suffixLength, suffix) == 0)
		{
			shouldPatch = true;
			break;
		}
	}
	if (!shouldPatch)
	{
		return {};
	}

	std::string patched = original;
	patched.replace(hostStart, hostEnd - hostStart, gConfiguredHost);

	const std::string configuredPort = std::to_string(gConfiguredPort);
	const size_t hostAfter = hostStart + gConfiguredHost.size();
	if (hostAfter < patched.size() && patched[hostAfter] == ':')
	{
		const size_t colonPos = hostAfter;
		size_t numEnd = patched.find_first_not_of("0123456789", colonPos + 1);
		if (numEnd == std::string::npos)
		{
			numEnd = patched.size();
		}
		patched.replace(colonPos + 1, numEnd - colonPos - 1, configuredPort);
	}
	else
	{
		patched.insert(hostAfter, ":" + configuredPort);
	}
	return patched;
}

extern "C" int __cdecl FakeMQTTAsync_createWithOptions(
	void** handle,
	const char* serverURI,
	const char* clientId,
	int persistence_type,
	void* persistence_context,
	void* options)
{
	std::string message("[MQTT] MQTTAsync_createWithOptions URI: ");
	message += SafeCString(serverURI);
	message += " ClientId: ";
	message += SafeCString(clientId);
	message += " PersistenceType: ";
	message += std::to_string(persistence_type);
	Log(message);

	auto originalMQTTAsyncCreate =
		reinterpret_cast<MQTTAsync_createWithOptions_t>(paho_mqtt3as.OrignalMQTTAsync_createWithOptions);
	if (!originalMQTTAsyncCreate)
	{
		Log("[ERROR] Original MQTTAsync_createWithOptions is not loaded");
		return -1;
	}

	const char* finalUri = serverURI;
	std::string patched = PatchMqttUri(serverURI);
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
	Log("[MQTT] originalMQTTAsyncCreate returned " + std::to_string(ret) + " Final URI: " + SafeCString(finalUri));
	return ret;
}



static void* ResolveExport(HMODULE module, const char* symbol)
{
	if (!module || !symbol)
	{
		Log("[ERROR] ResolveExport called with null module or symbol");
		return nullptr;
	}

	void* addr = (void*)GetProcAddress(module, symbol);
	if (!addr)
	{
		Log(std::string("[ERROR] Failed to resolve symbol: ") + symbol);
		return nullptr;
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

	Log("[+] Qt DLLs loaded");

	void* createRequestAddr = ResolveExport(
		qtNetwork,
		"?createRequest@QNetworkAccessManager@@MEAAPEAVQNetworkReply@@W4Operation@1@AEBVQNetworkRequest@@PEAVQIODevice@@@Z"
	);

	void* webSocketOpenAddr = ResolveExport(
		qtWebSockets,
		"?open@QWebSocket@@QEAAXAEBVQNetworkRequest@@@Z"
	);

	if (!createRequestAddr)
	{
		Log("[ERROR] Skipping createRequest hook because the symbol was not resolved");
	}
	else if (MH_CreateHook(
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

	if (!webSocketOpenAddr)
	{
		Log("[ERROR] Skipping QWebSocket::open hook because the symbol was not resolved");
	}
	else if (MH_CreateHook(
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
