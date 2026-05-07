Set-ExecutionPolicy
Run from an elevated PowerShell (script will attempt to relaunch elevated if not already) or right-click -> Run with PowerShell.
Example: .\scripts\install-hook.ps1 -Action install
To use a specific DLL: .\scripts\install-hook.ps1 -Action install -SourcePath "C:\path\to\your.dll"
To restore original: .\scripts\install-hook.ps1 -Action restore


install-hook.bat -Action install
install-hook.bat -Action restore