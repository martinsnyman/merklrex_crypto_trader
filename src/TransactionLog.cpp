#include "TransactionLog.h"
#include "CSVReader.h"
#include <fstream>

// Lightweight CSV-backed transaction log.
TransactionLog::TransactionLog(std::string _csvPath)
: csvPath(_csvPath)
{
}

// Append transaction rows for persistence.
bool TransactionLog::append(const Transaction& transaction) const
{
    std::ofstream file{csvPath, std::ios::app};
    if (!file.is_open())
    {
        return false;
    }

    file << transaction.username << ","
         << transaction.timestamp << ","
         << transaction.type << ","
         << transaction.product << ","
         << transaction.price << ","
         << transaction.amount << ","
         << transaction.walletSnapshot << "\n";
    return true;
}

// Load a user's transaction history from CSV.
std::vector<Transaction> TransactionLog::loadUser(const std::string& username) const
{
    std::vector<Transaction> transactions;
    std::ifstream file{csvPath};
    if (!file.is_open())
    {
        return transactions;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::vector<std::string> tokens = CSVReader::tokenise(line, ',');
        if (tokens.size() != 7)
        {
            continue;
        }
        if (tokens[0] != username)
        {
            continue;
        }
        transactions.push_back(fromTokens(tokens));
    }
    return transactions;
}

Transaction TransactionLog::fromTokens(const std::vector<std::string>& tokens) const
{
    double price = 0.0;
    double amount = 0.0;
    try
    {
        price = std::stod(tokens[4]);
        amount = std::stod(tokens[5]);
    }
    catch (const std::exception&)
    {
        price = 0.0;
        amount = 0.0;
    }
    return Transaction(tokens[0], tokens[1], tokens[2], tokens[3], price, amount, tokens[6]);
}
