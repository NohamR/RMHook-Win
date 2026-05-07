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

// ------------------------------------------------------------
// Logging
// ------------------------------------------------------------

static void Log(const std::string& msg)
{
    std::ofstream file(GetLogPath(), std::ios::app);
    if (file.is_open())
    {
        file << msg << std::endl;
    }
}

// ------------------------------------------------------------
// Configuration
// ------------------------------------------------------------

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
                if (obj.contains("host")) gConfiguredHost = obj["host"].toString().toStdString();
                if (obj.contains("port")) gConfiguredPort = obj["port"].toInt();
                return;
            }
        }

        // If we reach here, no config exists or it failed to load.
        MessageBoxA(NULL, "First launch detected.\nUsing default config (example.com:443).\nYou can edit configuration in %LOCALAPPDATA%\\RMHook\\config.json", "RMHook Configuration", MB_OK | MB_ICONINFORMATION);

        // Save defaults as JSON
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

static inline bool shouldPatchURL(const QString& host) {
    if (host.isEmpty()) {
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
    )""")
        .contains(host, Qt::CaseInsensitive);
}

// ------------------------------------------------------------
// Original typedefs
// ------------------------------------------------------------

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

// ------------------------------------------------------------
// Originals
// ------------------------------------------------------------

static QNAM_CreateRequest_t originalCreateRequest = nullptr;
static QWebSocket_Open_t originalWebSocketOpen = nullptr;

// ------------------------------------------------------------
// Hooked createRequest
// ------------------------------------------------------------

QNetworkReply* __fastcall hookedCreateRequest(
    QNetworkAccessManager* self,
    QNetworkAccessManager::Operation op,
    const QNetworkRequest& req,
    QIODevice* outgoingData
)
{
    const QString host = req.url().host();
    if (shouldPatchURL(host)) {
        QNetworkRequest newReq(req);
        QUrl newUrl = req.url();
        newUrl.setHost(QString::fromStdString(gConfiguredHost));
        newUrl.setPort(gConfiguredPort);
        newReq.setUrl(newUrl);

        Log("[HTTP PATCHED] " + host.toStdString() + " -> " + newUrl.toString().toStdString());

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

// ------------------------------------------------------------
// Hooked websocket open
// ------------------------------------------------------------

void __fastcall hookedWebSocketOpen(
    QWebSocket* self,
    const QNetworkRequest& req
)
{
    if (!originalWebSocketOpen) {
        return;
    }

    const QString host = req.url().host();
    if (shouldPatchURL(host)) {
        QUrl newUrl = req.url();
        newUrl.setHost(QString::fromStdString(gConfiguredHost));
        newUrl.setPort(gConfiguredPort);

        QNetworkRequest newReq(req);
        newReq.setUrl(newUrl);

        Log("[WS PATCHED] " + host.toStdString() + " -> " + newUrl.toString().toStdString());

        originalWebSocketOpen(self, newReq);
        return;
    }

    originalWebSocketOpen(self, req);
}

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

void* ResolveExport(HMODULE module, const char* symbol)
{
    void* addr = (void*)GetProcAddress(module, symbol);

    if (!addr)
    {
        Log("[ERROR] Failed to resolve symbol");
        Log(symbol);
    }

    return addr;
}

// ------------------------------------------------------------
// InstallHooks
// ------------------------------------------------------------

void InstallHooks()
{
    LoadConfig();

    // std::string logPath = GetLogPath();
    // std::string message = "Proxy Hook Started.\nLog file: " + logPath;
    // MessageBoxA(NULL, message.c_str(), "reMarkable Proxy", MB_OK | MB_ICONINFORMATION);

    Log("[*] Initializing MinHook");

    if (MH_Initialize() != MH_OK)
    {
        Log("[ERROR] MH_Initialize failed");
        return;
    }

    // --------------------------------------------------------
    // Wait for Qt DLLs
    // --------------------------------------------------------

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

    // --------------------------------------------------------
    // Resolve symbols
    // --------------------------------------------------------

    void* createRequestAddr = ResolveExport(
        qtNetwork,
        "?createRequest@QNetworkAccessManager@@MEAAPEAVQNetworkReply@@W4Operation@1@AEBVQNetworkRequest@@PEAVQIODevice@@@Z"
    );

    void* webSocketOpenAddr = ResolveExport(
        qtWebSockets,
        "?open@QWebSocket@@QEAAXAEBVQNetworkRequest@@@Z"
    );

    if (!createRequestAddr || !webSocketOpenAddr)
    {
        Log("[ERROR] Failed to resolve one or more symbols");
        return;
    }

    Log("[+] Symbols resolved");

    // --------------------------------------------------------
    // Hook QNetworkAccessManager::createRequest
    // --------------------------------------------------------

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

    // --------------------------------------------------------
    // Hook QWebSocket::open
    // --------------------------------------------------------

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

    // --------------------------------------------------------
    // Enable all hooks
    // --------------------------------------------------------

    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
    {
        Log("[ERROR] Failed to enable hooks");
    }
    else
    {
        Log("[+] Hooks enabled");
    }
}