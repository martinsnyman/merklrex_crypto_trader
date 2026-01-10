#include "Candlestick.h"

// Initialize OHLC with the first price in the period.
Candlestick::Candlestick(std::string _period, double _open)
: period(_period),
  open(_open),
  high(_open),
  low(_open),
  close(_open)
{
}

// Update high/low and close as new prices arrive.
void Candlestick::addPrice(double price)
{
    if (price > high)
    {
        high = price;
    }
    if (price < low)
    {
        low = price;
    }
    close = price;
}
