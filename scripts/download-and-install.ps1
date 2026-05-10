$DllUrl = "https://github.com/NohamR/RMHook-Win/releases/download/v1.0/paho-mqtt3as.dll"
$ScriptUrl = "https://raw.githubusercontent.com/NohamR/RMHook-Win/main/scripts/install-hook.ps1"

$TempDir = [System.IO.Path]::GetTempPath()
$DestPath = Join-Path $TempDir "paho-mqtt3as.dll"
$InstallScriptPath = Join-Path $TempDir "install-hook.ps1"

[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12

Write-Host "Downloading DLL from $DllUrl to $DestPath..."
Invoke-WebRequest -Uri $DllUrl -OutFile $DestPath -UseBasicParsing

Write-Host "Downloading install script..."
Invoke-WebRequest -Uri $ScriptUrl -OutFile $InstallScriptPath -UseBasicParsing

Write-Host "Installing hook with downloaded DLL..."
# Run the downloaded script
& $InstallScriptPath -Action install -SourcePath $DestPath

Write-Host "Done. You can safely close this window."