# Define paths
$buildDir = "build"
$installDir = "install"
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$innoSetupPath = "C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
$issScriptPath = Join-Path -Path $scriptDir -ChildPath "installer_script_updated.iss"
$UpdateISSScript = Join-Path -Path $scriptDir -ChildPath "update_iss_script.ps1"
$outputDir = "Output"

# Check for required executables
$missingComponents = @()

if (-Not (Test-Path $innoSetupPath)) {
    $missingComponents += "Inno Setup (ISCC.exe)"
}

# Test cmake and nmake availability
try {
    $cmakePath = (Get-Command cmake).Source
} catch {
    $missingComponents += "CMake"
}
try {
    $makePath = (Get-Command make).Source
} catch {
    $missingComponents += "make"
}

# Warn if any component is missing
if ($missingComponents.Count -gt 0) {
    Write-Host "ERROR: The following required components are missing:" -ForegroundColor Red
    $missingComponents | ForEach-Object { Write-Host "- $_" -ForegroundColor Yellow }
    exit 1
}

# Create build directory and navigate to it
if (-Not (Test-Path $buildDir)) {
    New-Item -Path $buildDir -ItemType Directory
}
Set-Location -Path $buildDir

# Run CMake to configure the project
& $cmakePath -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install .. 

# Build and install the project using parallel jobs
Write-Host "Building project..."
& $cmakePath --build . --target install --config Release

# Navigate to the install directory
$navDir = Join-Path -Path $scriptDir -ChildPath $buildDir
$navDir = Join-Path -Path $navDir -ChildPath $installDir
Set-Location -Path $navDir

# Windeployqt
Write-Host "Running windeployqt..."
& windeployqt6.exe .\bin --qmldir ..\..\qml\

# Remove unnecessary files
Remove-Item -Path .\include\ -Recurse -Force -ErrorAction SilentlyContinue
Remove-Item -Path .\lib\ -Recurse -Force -ErrorAction SilentlyContinue
Remove-Item -Path .\XFingerCrack.desktop -Force -ErrorAction SilentlyContinue

# Remove old iss script
$oldISS = Join-Path -Path $scriptDir -ChildPath "installer_script_updated.iss"
Remove-Item $oldISS -Force -ErrorAction SilentlyContinue

# Run the PowerShell script to update the ISS script
Write-Host "Running update ISS script..."
& $UpdateISSScript

# Verify that the update script ran successfully
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to update the ISS script. Exiting."
    exit $LASTEXITCODE
}

# Compile the Inno Setup script
Write-Host "Compiling Inno Setup script..."
& $innoSetupPath /O"$outputDir" $issScriptPath

# Verify that the compilation succeeded
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to compile the Inno Setup script."
    exit $LASTEXITCODE
}

Write-Host "Script execution completed successfully."