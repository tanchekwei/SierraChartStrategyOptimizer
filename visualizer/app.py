# Copyright (c) 2025 Chek Wei Tan
# Licensed under the MIT License. See LICENSE file in the project root for full license information.

import streamlit as st
import pandas as pd
import os
import json
import numpy as np
from datetime import datetime
import plotly.express as px
import plotly.graph_objects as go

# Function to calculate metrics from trades data
def calculate_metrics(trades):
    if not trades:
        return {
            'Total Profit/Loss': 0,
            'Number of trades': 0,
            'Average trade duration': 0,
            'Sharpe Ratio': 0,
            'Maximum Drawdown': 0,
            'Win Rate': 0
        }

    df = pd.DataFrame(trades)
    total_profit_loss = df['TradeProfitLoss'].sum()
    num_trades = len(df)
    
    # Calculate average trade duration
    df['OpenDateTime'] = pd.to_datetime(df['OpenDateTime'])
    df['CloseDateTime'] = pd.to_datetime(df['CloseDateTime'])
    df['Duration'] = (df['CloseDateTime'] - df['OpenDateTime']).dt.total_seconds()
    avg_duration = df['Duration'].mean()
    
    # Calculate Sharpe Ratio (assuming risk-free rate is 0)
    returns = df['TradeProfitLoss']
    sharpe_ratio = returns.mean() / returns.std() if returns.std() != 0 else 0
    
    # Maximum Drawdown calculation
    cumulative_profit = returns.cumsum()
    peak = cumulative_profit.cummax()
    drawdown = (cumulative_profit - peak)
    max_drawdown = drawdown.min()

    # Calculate Win Rate
    profitable_trades = df[df['TradeProfitLoss'] > 0]
    win_rate = (len(profitable_trades) / num_trades) * 100 if num_trades > 0 else 0

    return {
        'Total Profit/Loss': total_profit_loss,
        'Number of trades': num_trades,
        'Average trade duration': avg_duration,
        'Sharpe Ratio': sharpe_ratio,
        'Maximum Drawdown': max_drawdown if not np.isnan(max_drawdown) else 0,
        'Win Rate': win_rate
    }

# Function to generate equity curve data
def generate_equity_curve(trades):
    if not trades:
        return pd.DataFrame(columns=['DateTime', 'Equity'])

    df_trades = pd.DataFrame(trades)
    df_trades['CloseDateTime'] = pd.to_datetime(df_trades['CloseDateTime'])
    df_trades = df_trades.sort_values(by='CloseDateTime')
    
    # Calculate cumulative profit/loss
    df_trades['CumulativeProfitLoss'] = df_trades['TradeProfitLoss'].cumsum()
    
    # The equity curve should start at 0 before the first trade
    initial_equity_point = pd.DataFrame([{'DateTime': df_trades['CloseDateTime'].min() - pd.Timedelta(seconds=1), 'Equity': 0}])
    
    equity_curve = pd.DataFrame({
        'DateTime': df_trades['CloseDateTime'],
        'Equity': df_trades['CumulativeProfitLoss']
    })
    
    return pd.concat([initial_equity_point, equity_curve]).sort_values(by='DateTime').reset_index(drop=True)

# Function to analyze profit/loss by entry minute
def analyze_profit_loss_by_entry_minute(trades_df):
    if trades_df.empty:
        return pd.DataFrame(columns=['EntryMinute', 'Profitable Trades', 'Losing Trades'])

    df = trades_df.copy()
    df['OpenDateTime'] = pd.to_datetime(df['OpenDateTime'])
    df['EntryMinute'] = df['OpenDateTime'].dt.minute
    
    # Determine if trade was profitable or losing
    df['IsProfitable'] = df['TradeProfitLoss'] > 0
    df['IsLosing'] = df['TradeProfitLoss'] < 0

    # Group by EntryMinute and count profitable/losing trades
    minute_analysis = df.groupby('EntryMinute').agg(
        Profitable_Trades=('IsProfitable', lambda x: x.sum()),
        Losing_Trades=('IsLosing', lambda x: x.sum())
    ).reset_index()

    # Ensure all minutes from 0 to 59 are present, filling missing with 0
    all_minutes = pd.DataFrame({'EntryMinute': range(60)})
    minute_analysis = pd.merge(all_minutes, minute_analysis, on='EntryMinute', how='left').fillna(0)
    minute_analysis['Profitable_Trades'] = minute_analysis['Profitable_Trades'].astype(int)
    minute_analysis['Losing_Trades'] = minute_analysis['Losing_Trades'].astype(int)

    return minute_analysis

# Function to analyze profit/loss by entry hour
def analyze_profit_loss_by_entry_hour(trades_df):
    if trades_df.empty:
        return pd.DataFrame(columns=['EntryHour', 'Profitable Trades', 'Losing Trades'])

    df = trades_df.copy()
    df['OpenDateTime'] = pd.to_datetime(df['OpenDateTime'])
    df['EntryHour'] = df['OpenDateTime'].dt.hour
    
    df['IsProfitable'] = df['TradeProfitLoss'] > 0
    df['IsLosing'] = df['TradeProfitLoss'] < 0

    hour_analysis = df.groupby('EntryHour').agg(
        Profitable_Trades=('IsProfitable', lambda x: x.sum()),
        Losing_Trades=('IsLosing', lambda x: x.sum())
    ).reset_index()

    all_hours = pd.DataFrame({'EntryHour': range(24)})
    hour_analysis = pd.merge(all_hours, hour_analysis, on='EntryHour', how='left').fillna(0)
    hour_analysis['Profitable_Trades'] = hour_analysis['Profitable_Trades'].astype(int)
    hour_analysis['Losing_Trades'] = hour_analysis['Losing_Trades'].astype(int)

    return hour_analysis

# Function to analyze profit/loss by session (RTH vs ETH)
def analyze_profit_loss_by_session(trades_df, rth_start_hour=9, rth_start_minute=30, rth_end_hour=16, rth_end_minute=0):
    if trades_df.empty:
        return pd.DataFrame(columns=['Session', 'Profitable Trades', 'Losing Trades'])

    df = trades_df.copy()
    df['OpenDateTime'] = pd.to_datetime(df['OpenDateTime'])

    # Determine session (RTH or ETH)
    def get_session(dt):
        if (dt.hour > rth_start_hour or (dt.hour == rth_start_hour and dt.minute >= rth_start_minute)) and \
           (dt.hour < rth_end_hour or (dt.hour == rth_end_hour and dt.minute <= rth_end_minute)):
            return 'RTH'
        return 'ETH'
    
    df['Session'] = df['OpenDateTime'].apply(get_session)

    df['IsProfitable'] = df['TradeProfitLoss'] > 0
    df['IsLosing'] = df['TradeProfitLoss'] < 0

    session_analysis = df.groupby('Session').agg(
        Profitable_Trades=('IsProfitable', lambda x: x.sum()),
        Losing_Trades=('IsLosing', lambda x: x.sum())
    ).reset_index()

    # Ensure both RTH and ETH are present even if no trades occurred in one
    all_sessions = pd.DataFrame({'Session': ['RTH', 'ETH']})
    session_analysis = pd.merge(all_sessions, session_analysis, on='Session', how='left').fillna(0)
    session_analysis['Profitable_Trades'] = session_analysis['Profitable_Trades'].astype(int)
    session_analysis['Losing_Trades'] = session_analysis['Losing_Trades'].astype(int)

    return session_analysis


# Function to load and process data from JSON files
def load_and_process_data(folder_path):
    all_data = []
    raw_data = {}
    strategy_name = "N/A"
    
    json_files = [f for f in os.listdir(folder_path) if f.endswith('.json')]
    
    for i, filename in enumerate(json_files):
        file_path = os.path.join(folder_path, filename)
        with open(file_path, 'r') as f:
            data = json.load(f)
            
            # Extract strategy name from the first file
            if i == 0:
                strategy_name = data.get('customStudyInformation', {}).get('StudyOriginalName', 'N/A')

            combination = data.get('combination', {})
            combination_id = ", ".join([f"{k}: {v}" for k, v in combination.items()])
            
            trades_data = data.get('tradesData', [])
            trade_statistics = data.get('tradeStatistics', {})

            # Generate equity curve for the current combination
            equity_curve_df = generate_equity_curve(trades_data)

            raw_data[combination_id] = {
                "tradeStatistics": trade_statistics,
                "tradesData": trades_data,
                "equityCurve": equity_curve_df # Store the equity curve DataFrame
            }
            
            long_trades = [t for t in trades_data if t['TradeType'] == 1]
            short_trades = [t for t in trades_data if t['TradeType'] == -1]
            
            combined_metrics = calculate_metrics(trades_data)
            long_metrics = calculate_metrics(long_trades)
            short_metrics = calculate_metrics(short_trades)
            
            row = {**combination}
            row['combination_id'] = combination_id
            for k, v in combined_metrics.items():
                row[f'Combined_{k.replace(" ", "_")}'] = v
            for k, v in long_metrics.items():
                row[f'Long_{k.replace(" ", "_")}'] = v
            for k, v in short_metrics.items():
                row[f'Short_{k.replace(" ", "_")}'] = v
            
            all_data.append(row)
                
    return pd.DataFrame(all_data), raw_data, strategy_name

# --- Visualization Functions ---
def create_heatmap(df, value_col, x_col, y_col):
    if x_col and y_col and value_col and x_col != y_col:
        try:
            pivot_table = df.pivot_table(values=value_col, index=y_col, columns=x_col)
            fig = px.imshow(pivot_table, text_auto=True, aspect="auto",
                            labels=dict(x=x_col, y=y_col, color=value_col),
                            title=f'Heatmap of {value_col}')
            return fig
        except Exception as e:
            st.warning(f"Could not create heatmap: {e}")
    return None

def create_scatter_plot(df, x_col, y_col, color_col, title):
    if x_col and y_col and color_col:
        fig = px.scatter(df, x=x_col, y=y_col, color=color_col,
                         title=title, hover_data=df.columns)
        return fig
    return None

# Streamlit UI
st.set_page_config(layout="wide")
st.title('Trading Backtest Analysis')

# Use a default folder that exists in the repo for demonstration
default_folder = 'src/data' if os.path.isdir('src/data') else '.'
folder_path = st.text_input('Enter the path to the folder containing JSON files:', default_folder)

if os.path.isdir(folder_path):
    df, raw_data, strategy_name = load_and_process_data(folder_path)
    
    if not df.empty:
        st.success(f'Successfully loaded and processed {len(df)} JSON files.')
        
        # Display Strategy Name and Combination Count
        st.info(f"**Strategy Name:** {strategy_name}  |  **Number of Combinations:** {len(df)}")

        # --- Export to CSV ---
        csv = df.to_csv(index=False).encode('utf-8')
        st.download_button(
            label="Download data as CSV",
            data=csv,
            file_name='backtest_analysis.csv',
            mime='text/csv',
        )
        
        param_cols = list(df.columns[:len(df.columns)-19]) # Adjusted for combination_id and new metrics
        
        tab1, tab2, tab3, tab4, tab5 = st.tabs(["Combined Trades", "Long Trades", "Short Trades", "Raw Data Explorer", "LLM Analysis"])
        
        def display_tab(tab_name, prefix):
            with tab_name:
                st.header(f"{prefix.replace('_',' ')} Analysis")
                
                display_cols = param_cols + [col for col in df.columns if prefix in col]
                st.dataframe(df[display_cols])
                
                st.header("Visualizations")
                
                # --- Heatmap ---
                st.subheader("Sharpe Ratio Heatmap")
                col1, col2 = st.columns(2)
                with col1:
                    x_axis = st.selectbox(f'Select X-axis for {prefix} Heatmap', options=param_cols, key=f'{prefix}_x')
                with col2:
                    y_axis = st.selectbox(f'Select Y-axis for {prefix} Heatmap', options=param_cols, index=1 if len(param_cols)>1 else 0, key=f'{prefix}_y')

                heatmap = create_heatmap(df, f'{prefix}Sharpe_Ratio', x_axis, y_axis)
                if heatmap:
                    st.plotly_chart(heatmap, use_container_width=True)
                    st.download_button(label=f"Save Heatmap as HTML", data=heatmap.to_html(), file_name=f'{prefix}heatmap.html', mime='text/html')
                    
                # --- Scatter Plots ---
                st.subheader("Performance Plots")
                scatter1 = create_scatter_plot(df, f'{prefix}Total_Profit/Loss', f'{prefix}Maximum_Drawdown', f'{prefix}Sharpe_Ratio', f'{prefix.replace("_"," ")} Profit vs. Drawdown')
                if scatter1:
                    st.plotly_chart(scatter1, use_container_width=True)
                    st.download_button(label=f"Save Profit vs Drawdown as HTML", data=scatter1.to_html(), file_name=f'{prefix}profit_vs_drawdown.html', mime='text/html')

                scatter2 = create_scatter_plot(df, f'{prefix}Number_of_trades', f'{prefix}Sharpe_Ratio', f'{prefix}Total_Profit/Loss', f'{prefix.replace("_"," ")} Trade Count vs. Sharpe Ratio')
                if scatter2:
                    st.plotly_chart(scatter2, use_container_width=True)
                    st.download_button(label=f"Save Trade Count vs Sharpe Ratio as HTML", data=scatter2.to_html(), file_name=f'{prefix}trade_count_vs_sharpe.html', mime='text/html')
                    
                # --- Top Performing Combinations ---
                st.subheader("Top Performing Combinations")
                sort_by_options = [f'{prefix}Sharpe_Ratio', f'{prefix}Total_Profit/Loss', f'{prefix}Maximum_Drawdown', f'{prefix}Win_Rate']
                sort_by = st.selectbox(f'Sort by for {prefix}', options=sort_by_options, key=f'{prefix}_sort')
                top_n = st.slider(f'Select number of top combinations for {prefix}', 1, len(df), 10, key=f'{prefix}_top_n')
                ascending_order = False if 'Profit' in sort_by or 'Sharpe' in sort_by or 'Win_Rate' in sort_by else True
                st.dataframe(df.sort_values(by=sort_by, ascending=ascending_order).head(top_n)[display_cols])

        display_tab(tab1, "Combined_")
        display_tab(tab2, "Long_")
        display_tab(tab3, "Short_")

        with tab4: # Raw Data Explorer
            st.header("Raw Data Explorer")
            
            selections = {}
            for param in param_cols:
                selections[param] = st.selectbox(
                    f'Select {param}',
                    options=sorted(df[param].unique())
                )

            # Filter the dataframe based on selections
            filtered_df = df.copy()
            for param, value in selections.items():
                filtered_df = filtered_df[filtered_df[param] == value]

            if len(filtered_df) == 1:
                FIRST_ROW_INDEX = 0
                selected_combination_id = filtered_df['combination_id'].iloc[FIRST_ROW_INDEX]
                
                st.subheader("Trade Statistics")
                stats_data = raw_data[selected_combination_id]['tradeStatistics']
                stats_df = pd.DataFrame.from_dict(stats_data, orient='index')
                st.dataframe(stats_df)

                st.subheader("Individual Trades")
                trades_df = pd.DataFrame(raw_data[selected_combination_id]['tradesData'])
                st.dataframe(trades_df)

                # Display Equity Curve for the selected combination
                st.subheader("Equity Curve")
                equity_curve_df = raw_data[selected_combination_id]['equityCurve']
                if not equity_curve_df.empty:
                    fig_equity = go.Figure()
                    fig_equity.add_trace(go.Scatter(x=equity_curve_df['DateTime'], y=equity_curve_df['Equity'],
                                                    mode='lines', name=selected_combination_id))
                    fig_equity.update_layout(
                        title=f'Equity Curve for Combination: {selected_combination_id}',
                        xaxis_title='Date/Time',
                        yaxis_title='Equity'
                    )
                    st.plotly_chart(fig_equity, use_container_width=True)
                    st.download_button(label=f"Save Equity Curve as HTML", data=fig_equity.to_html(), 
                                       file_name=f'equity_curve_{selected_combination_id.replace(": ", "_").replace(", ", "__")}.html', 
                                       mime='text/html')
                else:
                    st.info("No equity curve data available for this combination.")

                # Add section for profitable and losing periods
                st.subheader("Profitable and Losing Periods on Equity Curve")
                if not equity_curve_df.empty:
                    fig_pl_periods = go.Figure()

                    # Calculate daily changes
                    equity_curve_df['DailyChange'] = equity_curve_df['Equity'].diff().fillna(0)
                    
                    # Add base equity line
                    fig_pl_periods.add_trace(go.Scatter(x=equity_curve_df['DateTime'], y=equity_curve_df['Equity'],
                                                        mode='lines', name='Equity', line=dict(color='blue')))

                    # Add segments for profitable periods (green markers)
                    profitable_periods = equity_curve_df[equity_curve_df['DailyChange'] > 0]
                    if not profitable_periods.empty:
                        fig_pl_periods.add_trace(go.Scatter(x=profitable_periods['DateTime'], y=profitable_periods['Equity'],
                                                            mode='markers', marker=dict(color='green', size=4),
                                                            name='Profitable Period', showlegend=True))
                    
                    # Add segments for losing periods (red markers)
                    losing_periods = equity_curve_df[equity_curve_df['DailyChange'] < 0]
                    if not losing_periods.empty:
                        fig_pl_periods.add_trace(go.Scatter(x=losing_periods['DateTime'], y=losing_periods['Equity'],
                                                            mode='markers', marker=dict(color='red', size=4),
                                                            name='Losing Period', showlegend=True))

                    fig_pl_periods.update_layout(
                        title=f'Profitable and Losing Periods for Combination: {selected_combination_id}',
                        xaxis_title='Date/Time',
                        yaxis_title='Equity',
                        hovermode='x unified'
                    )
                    st.plotly_chart(fig_pl_periods, use_container_width=True)
                    st.download_button(label=f"Save P/L Periods as HTML", data=fig_pl_periods.to_html(), 
                                       file_name=f'pl_periods_{selected_combination_id.replace(": ", "_").replace(", ", "__")}.html', 
                                       mime='text/html')
                else:
                    st.info("No equity curve data to analyze for profitable and losing periods.")

                # Add section for P/L by Entry Minute
                st.subheader("Profit/Loss by Entry Minute")
                if not trades_df.empty:
                    minute_pl_analysis_df = analyze_profit_loss_by_entry_minute(trades_df)
                    st.dataframe(minute_pl_analysis_df)

                    fig_minute_pl = go.Figure(data=[
                        go.Bar(name='Profitable Trades', x=minute_pl_analysis_df['EntryMinute'], y=minute_pl_analysis_df['Profitable_Trades'], marker_color='green'),
                        go.Bar(name='Losing Trades', x=minute_pl_analysis_df['EntryMinute'], y=minute_pl_analysis_df['Losing_Trades'], marker_color='red')
                    ])
                    fig_minute_pl.update_layout(
                        barmode='group',
                        title=f'Profitable vs. Losing Trades by Entry Minute for {selected_combination_id}',
                        xaxis_title='Entry Minute (0-59)',
                        yaxis_title='Number of Trades'
                    )
                    st.plotly_chart(fig_minute_pl, use_container_width=True)
                    st.download_button(label=f"Save P/L by Entry Minute Chart as HTML", data=fig_minute_pl.to_html(), 
                                       file_name=f'pl_by_entry_minute_{selected_combination_id.replace(": ", "_").replace(", ", "__")}.html', 
                                       mime='text/html')

                else:
                    st.info("No individual trade data to analyze profit/loss by entry minute.")

                # Add section for P/L by Entry Hour
                st.subheader("Profit/Loss by Entry Hour")
                if not trades_df.empty:
                    hour_pl_analysis_df = analyze_profit_loss_by_entry_hour(trades_df)
                    st.dataframe(hour_pl_analysis_df)

                    fig_hour_pl = go.Figure(data=[
                        go.Bar(name='Profitable Trades', x=hour_pl_analysis_df['EntryHour'], y=hour_pl_analysis_df['Profitable_Trades'], marker_color='green'),
                        go.Bar(name='Losing Trades', x=hour_pl_analysis_df['EntryHour'], y=hour_pl_analysis_df['Losing_Trades'], marker_color='red')
                    ])
                    fig_hour_pl.update_layout(
                        barmode='group',
                        title=f'Profitable vs. Losing Trades by Entry Hour for {selected_combination_id}',
                        xaxis_title='Entry Hour (0-23)',
                        yaxis_title='Number of Trades'
                    )
                    st.plotly_chart(fig_hour_pl, use_container_width=True)
                    st.download_button(label=f"Save P/L by Entry Hour Chart as HTML", data=fig_hour_pl.to_html(), 
                                       file_name=f'pl_by_entry_hour_{selected_combination_id.replace(": ", "_").replace(", ", "__")}.html', 
                                       mime='text/html')
                else:
                    st.info("No individual trade data to analyze profit/loss by entry hour.")

                # Add section for P/L by Session (RTH vs ETH)
                st.subheader("Profit/Loss by Session (RTH vs ETH)")
                if not trades_df.empty:
                    session_pl_analysis_df = analyze_profit_loss_by_session(trades_df)
                    st.dataframe(session_pl_analysis_df)

                    fig_session_pl = go.Figure(data=[
                        go.Bar(name='Profitable Trades', x=session_pl_analysis_df['Session'], y=session_pl_analysis_df['Profitable_Trades'], marker_color='green'),
                        go.Bar(name='Losing Trades', x=session_pl_analysis_df['Session'], y=session_pl_analysis_df['Losing_Trades'], marker_color='red')
                    ])
                    fig_session_pl.update_layout(
                        barmode='group',
                        title=f'Profitable vs. Losing Trades by Session for {selected_combination_id}',
                        xaxis_title='Session',
                        yaxis_title='Number of Trades'
                    )
                    st.plotly_chart(fig_session_pl, use_container_width=True)
                    st.download_button(label=f"Save P/L by Session Chart as HTML", data=fig_session_pl.to_html(), 
                                       file_name=f'pl_by_session_{selected_combination_id.replace(": ", "_").replace(", ", "__")}.html', 
                                       mime='text/html')
                else:
                    st.info("No individual trade data to analyze profit/loss by session.")


            elif len(filtered_df) == 0:
                st.warning("No combination found for the selected parameters.")
            else:
                st.info("Multiple combinations match the current selection. Please refine.")

        with tab5:
            st.header("LLM Analysis Summary")

            # Let user select the metric to sort by
            llm_sort_by = st.selectbox(
                'Sort top 10 combinations by',
                options=['Combined_Sharpe_Ratio', 'Combined_Total_Profit/Loss', 'Combined_Maximum_Drawdown', 'Combined_Win_Rate']
            )

            # Determine sort order
            ascending_order = 'Drawdown' in llm_sort_by

            # Get top 10 combinations
            top_10_df = df.sort_values(by=llm_sort_by, ascending=ascending_order).head(10)

            # Define important columns for the summary
            important_cols = param_cols + [
                'Combined_Total_Profit/Loss',
                'Combined_Maximum_Drawdown',
                'Combined_Number_of_trades',
                'Combined_Sharpe_Ratio',
                'Combined_Win_Rate'
            ]
            
            summary_df = top_10_df[important_cols]

            # Format as markdown for easy copying
            summary_text = summary_df.to_markdown(index=False)
            
            st.text_area("Copy the summary below for LLM analysis", summary_text, height=300)

            
    else:
        st.warning('No JSON files found or processed in the specified folder.')
else:
    st.error('The specified path is not a valid directory.')