param (
    [string]$SourceFile,
    [string]$OutDir = "C:\SierraChart\Data",
    [string]$VCVarsPath = "D:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
)

# --- If the source ends with .hpp, switch to .cpp. If missing, stop with error. ---
$ext = [System.IO.Path]::GetExtension($SourceFile)
if ($ext -ieq '.hpp') {
    $candidate = [System.IO.Path]::ChangeExtension($SourceFile, '.cpp')
    if (-not (Test-Path -LiteralPath $candidate)) {
        Write-Error "Required .cpp counterpart not found: '$candidate'. Aborting."
        exit 1  # or: throw
    }
    $SourceFile = $candidate
}

# (Optional but robust) Ensure the final source file exists before proceeding
if (-not (Test-Path -LiteralPath $SourceFile)) {
    Write-Error "Source file not found: '$SourceFile'. Aborting."
    exit 1  # or: throw
}

$ts = Get-Date -Format "yyMMdd_HHmm"
$bat = Join-Path -Path $PSScriptRoot -ChildPath 'build_sierra_release.bat'

# Extract base name without extension
$baseName = [System.IO.Path]::GetFileNameWithoutExtension($SourceFile)


# Define the extra source files once
$extraSources = @(
    'ResultAnalyzer.cpp'
    'ReplayManager.cpp'
    'OnChartLogging.cpp'
    'Logging.cpp'
    'ConfigManager.cpp'
    'StrategyOptimizerHelpers.cpp'
    'CombinationGenerator.cpp'
    'InputParameter.cpp'
    'ReportGenerator.cpp'
)

# Join into a single space-separated string
$extra = $extraSources -join ' '

# Write batch file
Set-Content $bat "call `"$VCVarsPath`" amd64"
Add-Content $bat "cl /Zc:wchar_t /GS /GL /W3 /O2 /Zc:inline /D NDEBUG /D _WINDOWS /D _USRDLL /D _WINDLL /Oy /Gd /Gy /Oi /GR- /GF /Ot /fp:precise /MT /std:c++17 /LD /EHa /WX- /nologo `"$SourceFile`" $extra /link Gdi32.lib User32.lib Shell32.lib /DLL /DYNAMICBASE /INCREMENTAL:NO /OPT:REF /OPT:ICF /MACHINE:X64 /OUT:`"$OutDir\$baseName`_$ts`_release.dll`""

& $bat
