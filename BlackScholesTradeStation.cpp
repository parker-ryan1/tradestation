#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <map>
#include <string>

// TradeStation C++ DLL Interface Headers
#ifdef TRADESTATION_DLL
#include "TSAnalysisTechnique.h"
#include "TSStudyInfo.h"
#endif

class BlackScholesTradeStation {
private:
    // Algorithm parameters
    double riskFreeRate;
    double maxPositionSize;
    double stopLossPercent;
    double takeProfitPercent;
    int lookbackPeriod;
    int monteCarloSimulations;
    
    // Data storage
    std::vector<double> priceHistory;
    std::vector<double> volatilityHistory;
    std::vector<double> returns;
    
    // Random number generation
    std::mt19937 generator;
    std::normal_distribution<double> normalDist;
    
    // Position tracking
    struct Position {
        double entryPrice = 0.0;
        int quantity = 0;
        double unrealizedPnL = 0.0;
        bool isLong = false;
    };
    Position currentPosition;
    
public:
    BlackScholesTradeStation() 
        : riskFreeRate(0.02),
          maxPositionSize(0.1),
          stopLossPercent(0.05),
          takeProfitPercent(0.15),
          lookbackPeriod(252),
          monteCarloSimulations(1000),
          generator(std::random_device{}()),
          normalDist(0.0, 1.0) {
        
        priceHistory.reserve(lookbackPeriod);
        volatilityHistory.reserve(lookbackPeriod);
        returns.reserve(lookbackPeriod);
    }
    
    // Main analysis function called by TradeStation
    int AnalyzeBar(double open, double high, double low, double close, double volume, 
                   int barNumber, double& buySignal, double& sellSignal, double& confidence) {
        
        // Update price history
        UpdatePriceHistory(close);
        
        // Need minimum data for analysis
        if (priceHistory.size() < 30) {
            buySignal = 0.0;
            sellSignal = 0.0;
            confidence = 0.0;
            return 0;
        }
        
        // Calculate current volatility
        double currentVolatility = CalculateVolatility();
        volatilityHistory.push_back(currentVolatility);
        
        // Keep volatility history within bounds
        if (volatilityHistory.size() > lookbackPeriod) {
            volatilityHistory.erase(volatilityHistory.begin());
        }
        
        // Generate trading signals using Black-Scholes and Monte Carlo
        auto signal = GenerateTradingSignal(close, currentVolatility);
        
        buySignal = signal.buyStrength;
        sellSignal = signal.sellStrength;
        confidence = signal.confidence;
        
        // Update position tracking
        UpdatePosition(close);
        
        return signal.action; // 1 = Buy, -1 = Sell, 0 = Hold
    }
    
private:
    struct TradingSignal {
        double buyStrength = 0.0;
        double sellStrength = 0.0;
        double confidence = 0.0;
        int action = 0; // 1 = Buy, -1 = Sell, 0 = Hold
    };
    
    void UpdatePriceHistory(double price) {
        priceHistory.push_back(price);
        
        // Maintain lookback period
        if (priceHistory.size() > lookbackPeriod) {
            priceHistory.erase(priceHistory.begin());
        }
        
        // Calculate returns if we have enough data
        if (priceHistory.size() >= 2) {
            double dailyReturn = std::log(price / priceHistory[priceHistory.size() - 2]);
            returns.push_back(dailyReturn);
            
            if (returns.size() > lookbackPeriod) {
                returns.erase(returns.begin());
            }
        }
    }
    
    double CalculateVolatility() {
        if (returns.size() < 10) return 0.2; // Default volatility
        
        // Calculate mean return
        double meanReturn = 0.0;
        for (double ret : returns) {
            meanReturn += ret;
        }
        meanReturn /= returns.size();
        
        // Calculate variance
        double variance = 0.0;
        for (double ret : returns) {
            variance += std::pow(ret - meanReturn, 2);
        }
        variance /= (returns.size() - 1);
        
        // Annualized volatility
        return std::sqrt(variance * 252);
    }
    
    double BlackScholesCall(double S, double K, double T, double r, double sigma) {
        if (T <= 0 || sigma <= 0) return std::max(S - K, 0.0);
        
        double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
        double d2 = d1 - sigma * std::sqrt(T);
        
        double callPrice = S * NormalCDF(d1) - K * std::exp(-r * T) * NormalCDF(d2);
        return std::max(callPrice, 0.0);
    }
    
    double BlackScholesPut(double S, double K, double T, double r, double sigma) {
        if (T <= 0 || sigma <= 0) return std::max(K - S, 0.0);
        
        double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
        double d2 = d1 - sigma * std::sqrt(T);
        
        double putPrice = K * std::exp(-r * T) * NormalCDF(-d2) - S * NormalCDF(-d1);
        return std::max(putPrice, 0.0);
    }
    
    double NormalCDF(double x) {
        return 0.5 * (1.0 + std::erf(x / std::sqrt(2.0)));
    }
    
    double SimulatePriceWithBrownianMotion(double currentPrice, double drift, 
                                          double volatility, double timeStep) {
        double randomShock = normalDist(generator);
        
        // Geometric Brownian Motion
        double logReturn = (drift - 0.5 * volatility * volatility) * timeStep + 
                          volatility * std::sqrt(timeStep) * randomShock;
        
        return currentPrice * std::exp(logReturn);
    }
    
    std::vector<double> MonteCarloSimulation(double currentPrice, double drift, 
                                           double volatility, int days) {
        std::vector<double> finalPrices;
        finalPrices.reserve(monteCarloSimulations);
        
        double timeStep = 1.0 / 252.0; // Daily time step
        
        for (int sim = 0; sim < monteCarloSimulations; ++sim) {
            double price = currentPrice;
            
            // Simulate price path
            for (int day = 0; day < days; ++day) {
                price = SimulatePriceWithBrownianMotion(price, drift, volatility, timeStep);
            }
            
            finalPrices.push_back(price);
        }
        
        return finalPrices;
    }
    
    double CalculateExpectedReturn() {
        if (returns.size() < 21) return 0.0;
        
        // Calculate recent average return (last 21 days)
        double totalReturn = 0.0;
        int count = std::min(21, static_cast<int>(returns.size()));
        
        for (int i = returns.size() - count; i < returns.size(); ++i) {
            totalReturn += returns[i];
        }
        
        return (totalReturn / count) * 252; // Annualized
    }
    
    TradingSignal GenerateTradingSignal(double currentPrice, double volatility) {
        TradingSignal signal;
        
        if (priceHistory.size() < 30) return signal;
        
        // Calculate expected drift
        double drift = CalculateExpectedReturn();
        
        // Monte Carlo simulation for 21-day price prediction
        auto simulatedPrices = MonteCarloSimulation(currentPrice, drift, volatility, 21);
        
        // Calculate statistics from simulation
        double meanPrice = 0.0;
        for (double price : simulatedPrices) {
            meanPrice += price;
        }
        meanPrice /= simulatedPrices.size();
        
        // Calculate probability of profit (5% threshold)
        int profitableOutcomes = 0;
        int lossOutcomes = 0;
        
        for (double price : simulatedPrices) {
            if (price > currentPrice * 1.05) {
                profitableOutcomes++;
            } else if (price < currentPrice * 0.95) {
                lossOutcomes++;
            }
        }
        
        double profitProbability = static_cast<double>(profitableOutcomes) / simulatedPrices.size();
        double lossProbability = static_cast<double>(lossOutcomes) / simulatedPrices.size();
        
        // Calculate expected return
        double expectedReturn = (meanPrice - currentPrice) / currentPrice;
        
        // Use Black-Scholes for additional insight
        double timeToExpiry = 30.0 / 365.0; // 30 days
        double callStrike = currentPrice * 1.05; // 5% OTM call
        double putStrike = currentPrice * 0.95;  // 5% OTM put
        
        double callValue = BlackScholesCall(currentPrice, callStrike, timeToExpiry, riskFreeRate, volatility);
        double putValue = BlackScholesPut(currentPrice, putStrike, timeToExpiry, riskFreeRate, volatility);
        
        // Normalize option values for signal strength
        double callSignal = callValue / (currentPrice * 0.05);
        double putSignal = putValue / (currentPrice * 0.05);
        
        // Generate signals based on multiple factors
        signal.confidence = std::min(1.0, static_cast<double>(simulatedPrices.size()) / 1000.0);
        
        // Buy signal conditions
        if (expectedReturn > 0.08 && profitProbability > 0.6 && volatility < 0.4 && callSignal > 0.3) {
            signal.buyStrength = std::min(1.0, (expectedReturn * profitProbability * callSignal) / 0.15);
            signal.action = 1;
        }
        // Sell signal conditions
        else if (expectedReturn < -0.05 || lossProbability > 0.6 || volatility > 0.6 || putSignal > 0.4) {
            signal.sellStrength = std::min(1.0, (std::abs(expectedReturn) * lossProbability * putSignal) / 0.15);
            signal.action = -1;
        }
        
        return signal;
    }
    
    void UpdatePosition(double currentPrice) {
        if (currentPosition.quantity != 0) {
            currentPosition.unrealizedPnL = 
                (currentPrice - currentPosition.entryPrice) * currentPosition.quantity;
            
            // Check stop loss and take profit
            double pnlPercent = currentPosition.unrealizedPnL / 
                               (currentPosition.entryPrice * std::abs(currentPosition.quantity));
            
            if (currentPosition.isLong) {
                if (pnlPercent <= -stopLossPercent || pnlPercent >= takeProfitPercent) {
                    // Signal to close position (would be handled by TradeStation)
                    currentPosition = Position(); // Reset position
                }
            } else {
                if (pnlPercent >= stopLossPercent || pnlPercent <= -takeProfitPercent) {
                    // Signal to close position
                    currentPosition = Position(); // Reset position
                }
            }
        }
    }
    
public:
    // TradeStation interface functions
    void SetPosition(double entryPrice, int quantity) {
        currentPosition.entryPrice = entryPrice;
        currentPosition.quantity = quantity;
        currentPosition.isLong = quantity > 0;
    }
    
    double GetUnrealizedPnL() const {
        return currentPosition.unrealizedPnL;
    }
    
    bool ShouldClosePosition() const {
        if (currentPosition.quantity == 0) return false;
        
        double pnlPercent = currentPosition.unrealizedPnL / 
                           (currentPosition.entryPrice * std::abs(currentPosition.quantity));
        
        return (std::abs(pnlPercent) >= stopLossPercent) || 
               (pnlPercent >= takeProfitPercent);
    }
    
    // Parameter setters for optimization
    void SetRiskFreeRate(double rate) { riskFreeRate = rate; }
    void SetMaxPositionSize(double size) { maxPositionSize = size; }
    void SetStopLoss(double percent) { stopLossPercent = percent; }
    void SetTakeProfit(double percent) { takeProfitPercent = percent; }
    void SetLookbackPeriod(int period) { lookbackPeriod = period; }
    void SetMonteCarloSimulations(int sims) { monteCarloSimulations = sims; }
};

// TradeStation DLL Export Functions
#ifdef TRADESTATION_DLL
extern "C" {
    static BlackScholesTradeStation* algorithm = nullptr;
    
    __declspec(dllexport) int InitializeAlgorithm() {
        if (algorithm) delete algorithm;
        algorithm = new BlackScholesTradeStation();
        return 1;
    }
    
    __declspec(dllexport) int AnalyzeBar(double open, double high, double low, 
                                        double close, double volume, int barNumber,
                                        double* buySignal, double* sellSignal, 
                                        double* confidence) {
        if (!algorithm) return 0;
        
        return algorithm->AnalyzeBar(open, high, low, close, volume, barNumber,
                                   *buySignal, *sellSignal, *confidence);
    }
    
    __declspec(dllexport) void SetPosition(double entryPrice, int quantity) {
        if (algorithm) algorithm->SetPosition(entryPrice, quantity);
    }
    
    __declspec(dllexport) double GetUnrealizedPnL() {
        return algorithm ? algorithm->GetUnrealizedPnL() : 0.0;
    }
    
    __declspec(dllexport) int ShouldClosePosition() {
        return algorithm ? (algorithm->ShouldClosePosition() ? 1 : 0) : 0;
    }
    
    __declspec(dllexport) void SetParameters(double riskFreeRate, double maxPositionSize,
                                           double stopLoss, double takeProfit,
                                           int lookbackPeriod, int monteCarloSims) {
        if (algorithm) {
            algorithm->SetRiskFreeRate(riskFreeRate);
            algorithm->SetMaxPositionSize(maxPositionSize);
            algorithm->SetStopLoss(stopLoss);
            algorithm->SetTakeProfit(takeProfit);
            algorithm->SetLookbackPeriod(lookbackPeriod);
            algorithm->SetMonteCarloSimulations(monteCarloSims);
        }
    }
    
    __declspec(dllexport) void CleanupAlgorithm() {
        if (algorithm) {
            delete algorithm;
            algorithm = nullptr;
        }
    }
}
#endif

// Standalone testing main function
#ifndef TRADESTATION_DLL
int main() {
    BlackScholesTradeStation algo;
    
    // Test with sample data
    std::vector<double> testPrices = {
        100.0, 101.5, 99.8, 102.3, 103.1, 101.9, 104.2, 105.8, 103.4, 106.1,
        107.3, 105.9, 108.2, 109.5, 107.8, 110.1, 108.7, 111.3, 109.9, 112.5
    };
    
    std::cout << "Black-Scholes TradeStation Algorithm Test\n";
    std::cout << "==========================================\n\n";
    
    for (size_t i = 0; i < testPrices.size(); ++i) {
        double price = testPrices[i];
        double buySignal, sellSignal, confidence;
        
        int action = algo.AnalyzeBar(price, price + 1, price - 1, price, 1000000, i + 1,
                                   buySignal, sellSignal, confidence);
        
        std::cout << "Bar " << (i + 1) << ": Price = $" << price 
                  << ", Action = " << action
                  << ", Buy = " << buySignal
                  << ", Sell = " << sellSignal
                  << ", Confidence = " << confidence << std::endl;
    }
    
    return 0;
}
#endif