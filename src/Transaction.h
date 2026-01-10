#pragma once

#include <string>

class Transaction
{
    public:
        // Transaction record persisted to CSV.
        Transaction(std::string _username,
                    std::string _timestamp,
                    std::string _type,
                    std::string _product,
                    double _price,
                    double _amount,
                    std::string _walletSnapshot);

        std::string username;
        std::string timestamp;
        std::string type;
        std::string product;
        double price;
        double amount;
        std::string walletSnapshot;
};
