#pragma once

#include <string>
#include <vector>
#include "Transaction.h"

class TransactionLog
{
    public:
        // Append and query transactions in CSV.
        explicit TransactionLog(std::string _csvPath);

        bool append(const Transaction& transaction) const;
        std::vector<Transaction> loadUser(const std::string& username) const;

    private:
        std::string csvPath;
        Transaction fromTokens(const std::vector<std::string>& tokens) const;
};
