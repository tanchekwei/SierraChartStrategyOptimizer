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
](https://github.com/user-attachments/assets/39058e51-d398-479b-85dd-c1b960acd997)

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

6.  **Generate Configuration**:
    *   Click the **CS6** (Generate) button on your control bar.
    *   This will create a new JSON configuration file in the `StrategyOptimizerGeneratedConfig` folder inside your Sierra Chart `Data` directory.
    *   Rename and move the file to the path `C:\SierraChart\Data\StrategyOptimizerConfig.json` (configurable in the setting)
7.  **Verify Configuration**:
    *   Before running a full optimization, click the **CS7** (Verify Config) button on your control bar.
    *   Check the on-chart log to ensure that your configuration is loaded correctly and the inputs are displayed as expected.
8.  **Run the Optimization**:
    *   Click the **CS8** (Start) button on your control bar to begin the optimization process.

## Stopping the Optimization Process

To stop the optimization process entirely, you must use the **Reset / Stop** button (**CS9**). If you click the "Stop" button in the Sierra Chart replay window, the optimizer will simply move on to the next combination. The **Reset / Stop** button ensures that the entire process is halted and the optimizer's state is cleared.

## JSON Configuration

| Property                          | Type    | Description                                                                                                                              |
| --------------------------------- | ------- | ---------------------------------------------------------------------------------------------------------------------------------------- |
| `_customStudyFileAndFunctionName` | string  | **For display purposes only.** The name of the custom study file and function.                                                           |
| `openResultsFolder`               | boolean | If `true`, the folder containing the optimization results will be opened automatically when the process is complete.                       |
| `replayConfig`                    | object  | An object containing settings for the chart replay.                                                                                      |
| `logConfig`                       | object  | An object containing settings for the on-chart logger.                                                                                   |
| `paramConfigs`                    | array   | An array of objects, where each object defines a parameter to be optimized.                                                              |

### `replayConfig`

| Property                                      | Type    | Description                                                                                                                                                                |
| --------------------------------------------- | ------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `replaySpeed`                                 | number  | The speed of the chart replay.                                                                                                                                             |
| `startDate`                                   | string  | The start date for the replay, in `YYYY-MM-DD` format.                                                                                                                     |
| `startTime`                                   | string  | The start time for the replay, in `HH:MM:SS.sss` format.                                                                                                                   |
| `replayMode`                                  | number  | The replay mode. Possible values: `1` (Standard), `2` (Accurate Trading System Back Test), `3` (Calculate at Every Tick), `4` (Calculate Same as Real-Time).                 |
| `chartsToReplay`                              | number  | The charts to replay. Possible values: `0` (Single Chart), `1` (All Charts in Chartbook), `2` (Charts with Same Link Number).                                               |
| `clearExistingTradeSimulationDataForSymbolAndTradeAccount` | number  | If set to `1`, any existing trade simulation data for the symbol and trade account will be cleared before the replay starts.                                        |

### `logConfig`

| Property               | Type    | Description                                                                 |
| ---------------------- | ------- | --------------------------------------------------------------------------- |
| `enableLog`            | boolean | If `true`, logging will be enabled.                                         |
| `enableShowLogOnChart` | boolean | If `true`, log messages will be displayed directly on the chart.              |
| `maxLogLines`          | number  | The maximum number of log lines to display on the chart.                    |

### `paramConfigs`

| Property    | Type   | Description                                                                                                |
| ----------- | ------ | ---------------------------------------------------------------------------------------------------------- |
| `_name`     | string | **For display purposes only.** The name of the input as it appears in the study's settings.                  |
| `index`     | number | The zero-based index of the study input.                                                                   |
| `type`      | string | The data type of the input. Possible values: `int`, `float`, `bool`.                                       |
| `min`       | number | The minimum value to be tested for this parameter.                                                         |
| `max`       | number | The maximum value to be tested for this parameter.                                                         |
| `increment` | number | The amount to increment the value between `min` and `max`. If `0`, the parameter will be fixed at `min`. |

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

## ⭐ Support the Project

If you find this Strategy Optimizer useful, please consider giving the repository a star on GitHub! Your support helps in maintaining and improving this tool. Thank you for helping this project grow! 🚀

## Disclaimer

This tool is intended for experienced developers and traders familiar with Sierra Chart and ACSIL. While it has been designed to be robust and reliable, it is your responsibility to ensure it is configured correctly for your specific needs.
