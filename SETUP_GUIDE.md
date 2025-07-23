# TradeStation Black-Scholes Algorithm Setup Guide

This guide will help you set up and deploy the Black-Scholes trading algorithm with Brownian motion simulation on TradeStation.

## 📋 Prerequisites

### Software Requirements:
- **TradeStation Desktop** (Version 10.0 or higher)
- **Microsoft Visual Studio** (2019 or 2022) with C++ support
- **Windows SDK** (for DLL compilation)
- **TradeStation SDK** (optional, for advanced features)

### TradeStation Account:
- **Active TradeStation account** with trading permissions
- **Real-time data subscription** (recommended)
- **Sufficient account balance** for testing

## 🔧 Installation Steps

### Step 1: Compile the C++ DLL

1. **Open Visual Studio**
2. **Create New Project**: 
   - File → New → Project
   - Select "Dynamic-Link Library (DLL)" 
   - Name: `BlackScholesTradeStation`

3. **Add the source code**:
   - Copy `BlackScholesTradeStation.cpp` to your project
   - Add `#define TRADESTATION_DLL` at the top
   - Configure project for Release mode (x64)

4. **Build Settings**:
   ```cpp
   // Project Properties → C/C++ → Preprocessor → Preprocessor Definitions
   TRADESTATION_DLL;_WINDOWS;_USRDLL;BLACKSCHOLESTRADESTATION_EXPORTS
   
   // Project Properties → Linker → General → Output File
   $(OutDir)BlackScholesTradeStation.dll
   ```

5. **Build the DLL**:
   - Build → Build Solution (Ctrl+Shift+B)
   - Locate the generated `BlackScholesTradeStation.dll`

### Step 2: Install DLL in TradeStation

1. **Copy DLL file** to TradeStation directory:
   ```
   C:\Program Files (x86)\TradeStation 10.0\Program\
   ```

2. **Register the DLL** (if required):
   - Open Command Prompt as Administrator
   - Run: `regsvr32 "C:\Program Files (x86)\TradeStation 10.0\Program\BlackScholesTradeStation.dll"`

### Step 3: Import EasyLanguage Strategy

1. **Open TradeStation**
2. **Go to**: EasyLanguage → Strategy
3. **Create New Strategy**:
   - File → New → Strategy
   - Name: `BlackScholesStrategy`

4. **Copy EasyLanguage Code**:
   - Copy contents from `BlackScholesTradeStation.ELD`
   - Paste into the strategy editor

5. **Verify and Compile**:
   - Tools → Verify and Compile (F3)
   - Fix any syntax errors if they appear

### Step 4: Apply Strategy to Chart

1. **Open a Chart**:
   - File → New → Chart
   - Select symbol (e.g., SPY, AAPL, QQQ)
   - Set timeframe (Daily recommended)

2. **Apply Strategy**:
   - Right-click chart → Insert Study → Strategy
   - Select `BlackScholesStrategy`
   - Configure parameters (see below)

3. **Configure Parameters**:
   ```
   RiskFreeRate: 0.02 (2% - adjust based on current rates)
   MaxPositionSize: 0.10 (10% of account per trade)
   StopLossPercent: 0.05 (5% stop loss)
   TakeProfitPercent: 0.15 (15% take profit)
   LookbackPeriod: 252 (1 year of data)
   MonteCarloSims: 1000 (simulation iterations)
   MinConfidence: 0.5 (minimum confidence threshold)
   MinSignalStrength: 0.3 (minimum signal strength)
   ```

## 📊 Strategy Parameters Explained

### Core Parameters:
- **RiskFreeRate**: Current risk-free interest rate (Treasury rate)
- **MaxPositionSize**: Maximum percentage of account per position (10%)
- **StopLossPercent**: Automatic stop-loss percentage
- **TakeProfitPercent**: Automatic take-profit percentage

### Algorithm Parameters:
- **LookbackPeriod**: Days of historical data for calculations
- **MonteCarloSims**: Number of price simulations (more = better accuracy)
- **MinConfidence**: Minimum confidence level to enter trades
- **MinSignalStrength**: Minimum signal strength threshold

## 🎯 Optimization Guide

### Conservative Settings (Lower Risk):
```
StopLossPercent: 0.03 (3%)
TakeProfitPercent: 0.10 (10%)
MaxPositionSize: 0.05 (5%)
MinConfidence: 0.7 (70%)
MinSignalStrength: 0.5 (50%)
```

### Aggressive Settings (Higher Risk/Reward):
```
StopLossPercent: 0.08 (8%)
TakeProfitPercent: 0.25 (25%)
MaxPositionSize: 0.15 (15%)
MinConfidence: 0.4 (40%)
MinSignalStrength: 0.2 (20%)
```

### High-Frequency Settings:
```
LookbackPeriod: 126 (6 months)
MonteCarloSims: 500 (faster execution)
MinConfidence: 0.3 (30%)
```

## 📈 Backtesting Instructions

### Step 1: Historical Testing
1. **Set Date Range**:
   - Right-click chart → Format Symbol
   - Set start date (e.g., 2020-01-01)
   - Set end date (recent date)

2. **Run Backtest**:
   - Strategy → Strategy Performance Report
   - Analyze results and metrics

### Step 2: Walk-Forward Analysis
1. **Divide time periods**:
   - Training: 2020-2022
   - Testing: 2023-present

2. **Optimize parameters** on training data
3. **Test performance** on out-of-sample data

### Step 3: Performance Metrics to Monitor
- **Net Profit**: Total profit/loss
- **Profit Factor**: Gross profit / Gross loss
- **Sharpe Ratio**: Risk-adjusted returns
- **Maximum Drawdown**: Worst peak-to-trough decline
- **Win Rate**: Percentage of winning trades
- **Average Trade**: Average profit per trade

## 🔍 Monitoring and Alerts

### Real-Time Monitoring:
1. **Strategy Performance Window**:
   - View → Strategy Performance
   - Monitor real-time P&L

2. **Trade Log**:
   - View → TradeManager
   - Track all entries and exits

3. **Custom Alerts**:
   ```easylanguage
   // Add to strategy for position alerts
   if MarketPosition <> MarketPosition[1] then
       Alert("Position changed: " + NumToStr(MarketPosition, 0));
   ```

### Risk Monitoring:
- **Daily P&L limits**
- **Maximum position size checks**
- **Volatility spike alerts**
- **Correlation monitoring** (if trading multiple symbols)

## 🚨 Troubleshooting

### Common Issues:

1. **DLL Not Found Error**:
   - Verify DLL is in correct TradeStation directory
   - Check DLL architecture (x64 vs x86)
   - Ensure all dependencies are installed

2. **Compilation Errors**:
   - Check EasyLanguage syntax
   - Verify all DLL functions are declared correctly
   - Update TradeStation to latest version

3. **No Signals Generated**:
   - Lower MinConfidence and MinSignalStrength
   - Check if enough historical data is available
   - Verify symbol has sufficient liquidity

4. **Poor Performance**:
   - Optimize parameters using Strategy Optimization
   - Test on different symbols and timeframes
   - Adjust risk management settings

### Performance Issues:
- **Slow execution**: Reduce MonteCarloSims
- **Memory usage**: Decrease LookbackPeriod
- **Signal lag**: Optimize C++ code compilation

## 📞 Support and Resources

### TradeStation Resources:
- **Help Documentation**: F1 in TradeStation
- **EasyLanguage Reference**: Help → EasyLanguage
- **Community Forums**: TradeStation Community
- **Customer Support**: 1-800-TRADESTATION

### Algorithm Support:
- **Code Documentation**: Comments in source files
- **Parameter Guides**: This document
- **Optimization Tips**: Backtesting section above

## ⚠️ Risk Disclaimers

1. **Past Performance**: Does not guarantee future results
2. **Market Risk**: Algorithm performance depends on market conditions
3. **Technology Risk**: DLL or system failures can impact trading
4. **Capital Risk**: Only trade with risk capital you can afford to lose
5. **Testing Required**: Always backtest thoroughly before live trading

## 🔄 Updates and Maintenance

### Regular Maintenance:
- **Monthly**: Review performance and adjust parameters
- **Quarterly**: Reoptimize on recent data
- **Annually**: Full strategy review and updates

### Version Control:
- Keep backup copies of working DLL and ELD files
- Document parameter changes and performance impact
- Test updates in simulation before live deployment

This algorithm combines advanced mathematical modeling with practical risk management for systematic trading on TradeStation. Always test thoroughly and understand the risks before deploying with real capital.