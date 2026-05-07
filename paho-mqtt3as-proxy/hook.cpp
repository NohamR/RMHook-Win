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
    QString host = req.url().host();

    Log("[HTTP] " + host.toStdString());

    return originalCreateRequest(
        self,
        op,
        req,
        outgoingData
    );
}

// ------------------------------------------------------------
// Hooked websocket open
// ------------------------------------------------------------

void __fastcall hookedWebSocketOpen(
    QWebSocket* self,
    const QNetworkRequest& req
)
{
    QString host = req.url().host();

    Log("[WS] " + host.toStdString());

    originalWebSocketOpen(
        self,
        req
    );
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