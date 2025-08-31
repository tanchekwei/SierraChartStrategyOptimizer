# Sierra Chart ACSIL Strategy Optimizer
![alt text](images/Report.png)
## Features

-   **Automated Backtesting**: Executes your trading strategy across all possible parameter combinations within user-defined ranges.
-   **Parameter Optimization**: Helps you discover the most profitable and robust settings for your strategy.
-   **JSON-Based Configuration**: Easily configure backtest parameters, including study inputs, replay settings, and start date.
-   **Detailed Logging**: Provides real-time feedback and detailed logs of the backtesting process, displayed directly on the chart.
-   **Result Analysis**: Automatically generates a summary CSV file from results of all backtest runs, sorted by profitability.
-   **Configuration Verification**: Allows you to verify your configuration settings and see the applied study inputs before starting a full backtest.

## How It Works

The Strategy Optimizer reads a configuration file (e.g., `StrategyOptimizerConfig.json`) that defines the parameters for the backtest. This file includes the name of the study to be tested, the input ranges for each parameter, and the replay settings.

You can use the "Verify Config" button to load your configuration and display the initial study inputs on the chart, allowing you to confirm that your settings are correctly interpreted before running a full optimization.

When you click the "Start" button, the optimizer generates all possible parameter combinations and begins the backtesting process. It systematically applies each combination to the target study, runs a chart replay, and logs the performance metrics for each run into individual JSON files.

After all combinations have been tested, the optimizer generated JSON files and creates a summary CSV report, with the results sorted by total profit/loss.

The "Reset / Stop" button allows you to clear the current state and stop the optimization process entirely.

[<video controls src="images/2025-08-27 05-33-51-1.mp4" title=""></video>
](https://github.com/user-attachments/assets/fb0005f0-b4c7-4264-9b12-fdf3115ef22b)

## Quick Start Guide

### Prerequisites

*   You must have a working ACSIL strategy study loaded in a Sierra Chart chart.
*   The chart should be configured with the instrument and timeframe you wish to use for the backtest replay.

### Steps

1.  **Download DLL**: Download the compiled DLL file for this study, or compile it yourself.
2.  **Install DLL**: Place the downloaded DLL file into your Sierra Chart `Data` directory (usually `C:\SierraChart\Data`).
3.  **Add the Optimizer Study**:
    *   In Sierra Chart, open the chart where your strategy is applied.
    *   Press `F6` to open the **Chart Studies** window.
    *   Click the **Add Custom Study** button.
    *   Find and select **Strategy Optimizer** from the list and click **Add**.
4.  **Configure the Optimizer Study**:
    *   In the **Chart Studies** window, select the **Strategy Optimizer** study and go to its **Settings**.
    *   Select the study you want to optimize from the **Target Study** dropdown list.
5.  **Add Control Bar Buttons**:
    *   Right-click on a control bar in Sierra Chart (e.g., Control Bar 1).
    *   Select **Customize Control Bar...**.
    *   In the **Available Control Bar Commands**, expand **Advanced Custom Study Button**.
    *   Add **ACS6 - Generate Config | CS6**, **ACS7 - Verify Config | CS7**, **ACS8 - Start | CS8** and **ACS9 - Reset | CS9** to your control bar.
    *   If any of these button numbers are in use, update the corresponding button numbers in the Strategy Optimizer settings.
    *   **Generate (CS6)**: Generates a configuration file for the selected study.
    *   **Verify Config (CS7)**: Loads and displays the configuration parameters on the chart.
    *   **Start (CS8)**: Begins the full backtesting and optimization process.
    *   **Reset / Stop (CS9)**: Clears the current state of the optimizer.

    ![ACS6 or ACS7](images/Button.png)

6.  **Generate Configuration**:
    *   Click the **CS6** (Generate) button on your control bar.
    *   This will create a new JSON configuration file in the `StrategyOptimizerGeneratedConfig` folder inside your Sierra Chart `Data` directory.
7.  **Verify Configuration**:
    *   Before running a full optimization, click the **CS7** (Verify Config) button on your control bar.
    *   Check the on-chart log to ensure that your configuration is loaded correctly and the inputs are displayed as expected.
8.  **Run the Optimization**:
    *   Click the **CS8** (Start) button on your control bar to begin the optimization process.

## Stopping the Optimization Process

To stop the optimization process entirely, you must use the **Reset / Stop** button (**CS9**). If you click the "Stop" button in the Sierra Chart replay window, the optimizer will simply move on to the next combination. The **Reset / Stop** button ensures that the entire process is halted and the optimizer's state is cleared.

## JSON Configuration Example

Here is an example of a generated configuration file.

```json
{
    "_customStudyFileAndFunctionName": "VolumeSpikeStrategy_250830_2320_release.scsf_VolumeSpikeStrategy",
    "openResultsFolder": true,
    "replayConfig": {
        "replaySpeed": 15360,
        "startDate": "2025-08-29",
        "startTime": "00:00:00.000",
        "replayMode": 2,
        "chartsToReplay": 0,
        "clearExistingTradeSimulationDataForSymbolAndTradeAccount": 1
    },
    "logConfig": {
        "enableLog": true,
        "enableShowLogOnChart": true,
        "maxLogLines": 25
    },
    "paramConfigs": [
        {
            "_name": "Enable Trading",
            "index": 0,
            "type": "bool",
            "min": 1,
            "max": 1,
            "increment": 0
        },
        {
            "_name": "Enable Long Trades",
            "index": 1,
            "type": "bool",
            "min": 1,
            "max": 1,
            "increment": 0
        }
    ]
}
```
-   **`_customStudyFileAndFunctionName`**: For display purposes only.
-   **`_name`**: (For display purposes only) The name of the input.
-   **`increment`**: If set to `0`, the parameter will be fixed and not optimized.

## Support the Project

If you find this Strategy Optimizer useful, please consider giving the repository a star on GitHub! Your support helps in maintaining and improving this tool.

## Results and Reports

After the optimization process completes, the optimizer generates a set of report files. If `openResultsFolder` is set to `true` in your configuration, the folder containing these reports will open automatically.

Each optimization run creates a new folder named with a timestamp. Inside this folder, you will find:
-   **A json and csv file for each parameter combination tested.**
-   **A `...summary.csv` file.**

### Summary Report
The `summary.csv` file provides a high-level overview of all the backtest runs, with each row representing a different parameter combination. The results are sorted by `Total P/L`, allowing you to quickly identify the best-performing settings. Key columns include:
-   **Total P/L**: The total profit or loss for the backtest run.
-   **Profit Factor**: The ratio of gross profit to gross loss.
-   **Total Trades**: The total number of trades executed.
-   **Win Rate (%)**: The percentage of winning trades.
-   **Max Drawdown**: The maximum peak-to-trough decline in equity.

### Detailed Run Data
-   **A `.json` file**: Contains detailed trade-by-trade data, including entry/exit times, prices, and profit/loss for each trade. This file is useful for in-depth analysis.
-   **A `.csv` file**: A CSV representation of the trade data for easy viewing.

## Disclaimer

This tool is intended for experienced developers and traders familiar with Sierra Chart and ACSIL. While it has been designed to be robust and reliable, it is your responsibility to ensure it is configured correctly for your specific needs.
