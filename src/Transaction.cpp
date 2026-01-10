#include "Transaction.h"

// Store transaction details for audit/history.
Transaction::Transaction(std::string _username,
                         std::string _timestamp,
                         std::string _type,
                         std::string _product,
                         double _price,
                         double _amount,
                         std::string _walletSnapshot)
: username(_username),
  timestamp(_timestamp),
  type(_type),
  product(_product),
  price(_price),
  amount(_amount),
  walletSnapshot(_walletSnapshot)
{
}
