# Parameter Optimization Guide

This guide provides systematic approaches to optimize the Black-Scholes algorithm parameters for different market conditions and trading objectives.

## 🎯 Optimization Objectives

### Primary Goals:
- **Maximize Sharpe Ratio**: Risk-adjusted returns
- **Minimize Maximum Drawdown**: Capital preservation
- **Optimize Win Rate**: Consistency of profits
- **Balance Risk/Reward**: Sustainable performance

## 📊 Parameter Categories

### 1. Risk Management Parameters

#### StopLossPercent (Default: 0.05)
- **Conservative**: 0.02-0.03 (2-3%)
- **Moderate**: 0.04-0.06 (4-6%)
- **Aggressive**: 0.07-0.10 (7-10%)

**Optimization Tips:**
- Lower values = More frequent exits, lower drawdown
- Higher values = Fewer whipsaws, larger potential losses
- Optimize based on symbol volatility

#### TakeProfitPercent (Default: 0.15)
- **Conservative**: 0.08-0.12 (8-12%)
- **Moderate**: 0.12-0.20 (12-20%)
- **Aggressive**: 0.20-0.30 (20-30%)

**Optimization Tips:**
- Should be 2-4x the stop loss for positive expectancy
- Higher values work better in trending markets
- Lower values better for range-bound markets

#### MaxPositionSize (Default: 0.10)
- **Conservative**: 0.05-0.08 (5-8%)
- **Moderate**: 0.08-0.12 (8-12%)
- **Aggressive**: 0.12-0.20 (12-20%)

**Optimization Tips:**
- Smaller positions = Lower volatility, slower growth
- Larger positions = Higher volatility, faster growth
- Consider correlation between traded symbols

### 2. Algorithm Parameters

#### LookbackPeriod (Default: 252)
- **Short-term**: 63-126 days (3-6 months)
- **Medium-term**: 126-252 days (6-12 months)
- **Long-term**: 252-504 days (1-2 years)

**Optimization Tips:**
- Shorter periods = More responsive to recent changes
- Longer periods = More stable, less noise
- Match to your trading timeframe

#### MonteCarloSims (Default: 1000)
- **Fast**: 500-750 simulations
- **Balanced**: 750-1250 simulations
- **Accurate**: 1250-2000 simulations

**Optimization Tips:**
- More simulations = Better accuracy, slower execution
- Fewer simulations = Faster execution, more noise
- Balance based on available computing power

#### MinConfidence (Default: 0.5)
- **Conservative**: 0.6-0.8 (60-80%)
- **Moderate**: 0.4-0.6 (40-60%)
- **Aggressive**: 0.2-0.4 (20-40%)

**Optimization Tips:**
- Higher confidence = Fewer but higher quality trades
- Lower confidence = More trades, potentially more noise
- Adjust based on market volatility

#### MinSignalStrength (Default: 0.3)
- **Conservative**: 0.4-0.6 (40-60%)
- **Moderate**: 0.2-0.4 (20-40%)
- **Aggressive**: 0.1-0.3 (10-30%)

**Optimization Tips:**
- Higher strength = Stronger signals, fewer trades
- Lower strength = More signals, potential false positives
- Correlates with confidence settings

## 🔧 Optimization Methodology

### Step 1: Market Regime Analysis
```
Bull Market (Trending Up):
- Higher TakeProfitPercent (0.20-0.30)
- Lower StopLossPercent (0.03-0.05)
- Higher MaxPositionSize (0.12-0.15)
- Lower MinConfidence (0.3-0.5)

Bear Market (Trending Down):
- Lower TakeProfitPercent (0.08-0.15)
- Higher StopLossPercent (0.05-0.08)
- Lower MaxPositionSize (0.05-0.08)
- Higher MinConfidence (0.6-0.8)

Sideways Market (Range-bound):
- Moderate TakeProfitPercent (0.10-0.18)
- Moderate StopLossPercent (0.04-0.06)
- Moderate MaxPositionSize (0.08-0.12)
- Higher MinSignalStrength (0.4-0.6)
```

### Step 2: Symbol-Specific Optimization

#### High Volatility Stocks (σ > 0.4)
```
StopLossPercent: 0.06-0.10
TakeProfitPercent: 0.18-0.30
MaxPositionSize: 0.05-0.08
MinConfidence: 0.6-0.8
```

#### Low Volatility Stocks (σ < 0.2)
```
StopLossPercent: 0.02-0.04
TakeProfitPercent: 0.08-0.15
MaxPositionSize: 0.10-0.15
MinConfidence: 0.3-0.5
```

#### ETFs and Index Funds
```
StopLossPercent: 0.03-0.06
TakeProfitPercent: 0.10-0.20
MaxPositionSize: 0.08-0.12
MinConfidence: 0.4-0.6
```

### Step 3: Walk-Forward Optimization

#### Training Period: 2 years
#### Testing Period: 6 months
#### Reoptimization: Every 3 months

```
Process:
1. Optimize parameters on training data
2. Test on out-of-sample data
3. Record performance metrics
4. Roll forward and repeat
5. Select parameters with consistent performance
```

## 📈 Optimization Examples

### Example 1: SPY (S&P 500 ETF)
```
Optimized Parameters:
RiskFreeRate: 0.025
MaxPositionSize: 0.12
StopLossPercent: 0.04
TakeProfitPercent: 0.16
LookbackPeriod: 189
MonteCarloSims: 1200
MinConfidence: 0.45
MinSignalStrength: 0.25

Expected Performance:
- Sharpe Ratio: 1.1-1.3
- Max Drawdown: 12-18%
- Win Rate: 58-65%
```

### Example 2: AAPL (High-Cap Growth)
```
Optimized Parameters:
RiskFreeRate: 0.025
MaxPositionSize: 0.08
StopLossPercent: 0.06
TakeProfitPercent: 0.22
LookbackPeriod: 126
MonteCarloSims: 1000
MinConfidence: 0.55
MinSignalStrength: 0.35

Expected Performance:
- Sharpe Ratio: 0.9-1.2
- Max Drawdown: 18-25%
- Win Rate: 55-62%
```

### Example 3: QQQ (NASDAQ ETF)
```
Optimized Parameters:
RiskFreeRate: 0.025
MaxPositionSize: 0.10
StopLossPercent: 0.05
TakeProfitPercent: 0.18
LookbackPeriod: 210
MonteCarloSims: 1100
MinConfidence: 0.50
MinSignalStrength: 0.30

Expected Performance:
- Sharpe Ratio: 1.0-1.4
- Max Drawdown: 15-22%
- Win Rate: 56-64%
```

## 🔍 Performance Monitoring

### Key Metrics to Track:
- **Sharpe Ratio**: Target > 1.0
- **Maximum Drawdown**: Target < 20%
- **Win Rate**: Target > 55%
- **Profit Factor**: Target > 1.3
- **Average Trade Duration**: Monitor for consistency

### Warning Signs:
- Sharpe ratio declining over time
- Increasing maximum drawdown
- Win rate falling below 50%
- Excessive trade frequency
- Large parameter sensitivity

## 🛠️ TradeStation Optimization

### Using TradeStation's Optimizer:
1. **Strategy → Optimize Strategy**
2. **Select parameters to optimize**
3. **Set optimization ranges**
4. **Choose fitness function** (Net Profit, Sharpe Ratio, etc.)
5. **Run optimization**
6. **Analyze results with walk-forward analysis**

### Optimization Settings:
```
Genetic Algorithm:
- Population Size: 100-200
- Generations: 50-100
- Mutation Rate: 0.1-0.2

Exhaustive Search:
- Use for 2-3 parameters max
- Set reasonable step sizes
- Monitor computation time
```

## ⚠️ Optimization Pitfalls

### Common Mistakes:
1. **Over-optimization**: Too many parameters, curve fitting
2. **Insufficient data**: Not enough historical data for optimization
3. **Look-ahead bias**: Using future information in optimization
4. **Parameter instability**: Parameters that work only in specific periods
5. **Ignoring transaction costs**: Not accounting for commissions and slippage

### Best Practices:
1. **Keep it simple**: Optimize only essential parameters
2. **Use out-of-sample testing**: Always validate on unseen data
3. **Regular reoptimization**: Update parameters quarterly
4. **Robustness testing**: Ensure parameters work across different periods
5. **Document everything**: Keep records of optimization results

## 📅 Optimization Schedule

### Monthly:
- Review performance metrics
- Check for parameter drift
- Monitor market regime changes

### Quarterly:
- Full parameter reoptimization
- Walk-forward analysis update
- Performance attribution analysis

### Annually:
- Complete strategy review
- Algorithm enhancement evaluation
- Risk management assessment

This systematic approach to parameter optimization will help you maintain consistent performance across different market conditions while avoiding the common pitfalls of over-optimization.