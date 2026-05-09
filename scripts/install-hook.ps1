<#
Installs or restores a paho-mqtt3as proxy DLL into "C:\Program Files\reMarkable".
Usage examples:
  .\install-hook.ps1 -Action install
  .\install-hook.ps1 -Action install -SourcePath "C:\some\path\my.dll"
  .\install-hook.ps1 -Action restore
#>

param(
    [ValidateSet("install","restore","help")]
    [string]$Action = "help",

    [string]$SourcePath = "",

    [switch]$Force
)

$InstallDir   = "C:\Program Files\reMarkable"
$TargetName   = "paho-mqtt3as.dll"
$OrigName     = "paho-mqtt3as_orig.dll"
$TargetPath   = Join-Path $InstallDir $TargetName
$OrigPath     = Join-Path $InstallDir $OrigName

$RepoRoot       = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$DefaultDebug   = Join-Path $RepoRoot "paho-mqtt3as-proxy\x64\Debug\paho-mqtt3as.dll"
$DefaultRelease = Join-Path $RepoRoot "paho-mqtt3as-proxy\x64\Release\paho-mqtt3as.dll"
$LegacyDebug    = Join-Path $RepoRoot "x64\Debug\paho-mqtt3as.dll"
$LegacyRelease  = Join-Path $RepoRoot "x64\Release\paho-mqtt3as.dll"

function Show-Help {
    "Actions:"
    "  install    - rename existing paho-mqtt3as.dll -> paho-mqtt3as_orig.dll (if present) and copy proxy DLL into place."
    "  restore    - remove proxy DLL (if present) and rename paho-mqtt3as_orig.dll back to paho-mqtt3as.dll."
    ""
    "Examples:"
    "  .\install-hook.ps1 -Action install"
    "  .\install-hook.ps1 -Action install -SourcePath `"$DefaultRelease`""
    "  .\install-hook.ps1 -Action restore"
}

function Ensure-Admin {
    $isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)

    if (-not $isAdmin) {
        Write-Host "Requesting administrator privileges..."

        $args = @(
            "-NoProfile",
            "-ExecutionPolicy", "Bypass",
            "-File", "`"$PSCommandPath`""
        )

        if ($MyInvocation.UnboundArguments -and $MyInvocation.UnboundArguments.Count -gt 0) {
            $args += $MyInvocation.UnboundArguments
        }

        Start-Process -FilePath "powershell.exe" -Verb RunAs -ArgumentList $args

        exit
    }
}

function Install-Proxy {
    Ensure-Admin

    $resolvedSource = $SourcePath

    if (-not $resolvedSource) {
        if (Test-Path $DefaultRelease) {
            $resolvedSource = $DefaultRelease
        }
        elseif (Test-Path $DefaultDebug) {
            $resolvedSource = $DefaultDebug
        }
        elseif (Test-Path $LegacyRelease) {
            $resolvedSource = $LegacyRelease
        }
        elseif (Test-Path $LegacyDebug) {
            $resolvedSource = $LegacyDebug
        }
        else {
            Write-Error "No source DLL supplied and none found in default Debug/Release paths."
            exit 2
        }
    }

    if (-not (Test-Path $resolvedSource)) {
        Write-Error "Source DLL not found: $resolvedSource"
        exit 3
    }

    if (-not (Test-Path $InstallDir)) {
        Write-Error "Install directory not found: $InstallDir"
        exit 4
    }

    if (-not (Test-Path $OrigPath) -and (Test-Path $TargetPath)) {
        Move-Item $TargetPath $OrigPath -Force
        Write-Host "Backed up original DLL"
    }

    Copy-Item $resolvedSource $TargetPath -Force

    Write-Host ""
    Write-Host "Hook installed successfully."
}

function Restore-Original {
    Ensure-Admin

    if (Test-Path $TargetPath) {
        Remove-Item -Path $TargetPath -Force
        Write-Host "Removed proxy DLL at $TargetPath"
    } else {
        Write-Host "No proxy DLL found at $TargetPath"
    }

    if (Test-Path $OrigPath) {
        Move-Item -Path $OrigPath -Destination $TargetPath -Force
        Write-Host "Restored original DLL: $OrigName -> $TargetName"
    } else {
        Write-Host "No backup original found at $OrigPath"
    }
}

switch ($Action) {
    "install" { Install-Proxy; break }
    "restore" { Restore-Original; break }
    "help"    { Show-Help; break }
    default   { Show-Help; break }
}
