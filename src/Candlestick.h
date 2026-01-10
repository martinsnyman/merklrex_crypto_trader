#pragma once

#include <string>

enum class CandlestickGranularity { daily, monthly, yearly };

class Candlestick
{
    public:
        // Candlestick model for OHLC aggregation by period.
        Candlestick(std::string _period, double _open);

        void addPrice(double price);

        std::string period;
        double open;
        double high;
        double low;
        double close;
};
