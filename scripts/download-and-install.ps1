$REPO = "NohamR/RMHook-Win"
$FILE = "paho-mqtt3as.dll"

$TempDir = [System.IO.Path]::GetTempPath()
$DestPath = Join-Path $TempDir $FILE
$InstallScriptPath = Join-Path $TempDir "install-hook.ps1"

[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12

Write-Host "[INFO] Downloading $FILE..."
Invoke-WebRequest -Uri "https://github.com/$REPO/releases/latest/download/$FILE" -OutFile $DestPath -UseBasicParsing

Write-Host "[INFO] Downloading install script..."
Invoke-WebRequest -Uri "https://raw.githubusercontent.com/$REPO/refs/heads/main/scripts/install-hook.ps1" -OutFile $InstallScriptPath -UseBasicParsing

Write-Host "[INFO] Running install script..."
# Run the downloaded script, bypassing execution policies
powershell.exe -NoProfile -ExecutionPolicy Bypass -File $InstallScriptPath -Action install -SourcePath $DestPath

Write-Host "Done. You can safely close this window."