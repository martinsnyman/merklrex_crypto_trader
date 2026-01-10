#pragma once

#include <string>
#include "Wallet.h"

class WalletStore
{
    public:
        // CSV-backed wallet persistence.
        explicit WalletStore(std::string _csvPath);

        Wallet loadWallet(const std::string& username) const;
        bool saveWallet(const std::string& username, const Wallet& wallet) const;

    private:
        std::string csvPath;
};
