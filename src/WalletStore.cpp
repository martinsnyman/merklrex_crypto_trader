#include "WalletStore.h"
#include "CSVReader.h"
#include <fstream>

// Persist wallet balances to CSV.
WalletStore::WalletStore(std::string _csvPath)
: csvPath(_csvPath)
{
}

// Load wallet balances for a user from CSV.
Wallet WalletStore::loadWallet(const std::string& username) const
{
    Wallet wallet;
    std::ifstream file{csvPath};
    if (!file.is_open())
    {
        return wallet;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::vector<std::string> tokens = CSVReader::tokenise(line, ',');
        if (tokens.size() != 3)
        {
            continue;
        }
        if (tokens[0] != username)
        {
            continue;
        }
        try
        {
            double amount = std::stod(tokens[2]);
            wallet.insertCurrency(tokens[1], amount);
        }
        catch (const std::exception&)
        {
        }
    }
    return wallet;
}

// Save wallet balances for a user to CSV.
bool WalletStore::saveWallet(const std::string& username, const Wallet& wallet) const
{
    std::vector<std::string> lines;
    std::ifstream input{csvPath};
    std::string line;
    while (input.is_open() && std::getline(input, line))
    {
        std::vector<std::string> tokens = CSVReader::tokenise(line, ',');
        if (tokens.size() != 3)
        {
            continue;
        }
        if (tokens[0] != username)
        {
            lines.push_back(line);
        }
    }

    for (const auto& pair : wallet.getCurrencies())
    {
        lines.push_back(username + "," + pair.first + "," + std::to_string(pair.second));
    }

    std::ofstream output{csvPath, std::ios::trunc};
    if (!output.is_open())
    {
        return false;
    }
    for (const std::string& storedLine : lines)
    {
        output << storedLine << "\n";
    }
    return true;
}
