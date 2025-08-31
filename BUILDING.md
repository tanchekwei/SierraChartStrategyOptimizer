# Building the Strategy Optimizer

This guide provides instructions on how to build the Strategy Optimizer DLL from the source code.

## Prerequisites

-   **Visual Studio 2022**: You must have Visual Studio 2022 installed with the C++ desktop development workload.
-   **Sierra Chart**: The project must be located within the Sierra Chart `ACS_Source` directory.

## Directory Structure

To ensure that the project can locate the `sierrachart.h` header file, you must place the repository inside a subfolder within your Sierra Chart `ACS_Source` directory.

For example:

```
C:\SierraChart\ACS_Source\StrategyOptimizer\
```

## Building the DLL

The repository is configured with Visual Studio Code tasks to simplify the build process.

### Steps

1.  **Open the repository folder in Visual Studio Code.**
2.  **Open the `StrategyOptimizer.cpp` file.**
3.  **Press `Ctrl+Shift+P`** to open the Command Palette.
4.  **Type `Run Task`** and select **Tasks: Run Task** from the list.
5.  **Choose a build task**:
    *   Select **Build SierraChart DLL (Release)** to build the optimized release version.
    *   Select **Build SierraChart DLL (Debug)** to build the debug version.

The compiled DLL will be placed in your Sierra Chart `Data` directory.

### Configuring Build Scripts

If your environment differs from the default settings (e.g., your Visual Studio installation path is different), you may need to update the `build_sierra_release.ps1` or `build_sierra_debug.ps1` scripts.

You can modify the following parameters at the top of the scripts:

```powershell
param (
    [string]$SourceFile,
    [string]$OutDir = "C:\SierraChart\Data",
    [string]$VCVarsPath = "D:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
)