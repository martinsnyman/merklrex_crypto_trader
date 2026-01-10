#pragma once

#include <vector>
#include <map>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"
#include "UserManager.h"
#include "WalletStore.h"
#include "TransactionLog.h"


class MerkelMain
{
    public:
        MerkelMain();
        /** Call this to start the sim */
        void init();
    private: 
        void printMenu();
        void printHelp();
        void printMarketStats();
        void enterAsk();
        void enterBid();
        void printWallet();
        // Deposit/withdraw funds and log wallet changes.
        void manageFunds();
        // Show recent trading transactions.
        void printRecentTransactions();
        // Summarize user trading activity and spend.
        void printUserSummary();
        // Login/register flow before using the platform.
        bool handleLogin();
        void printLoginMenu();
        void printCandlesticks();
        void gotoNextTimeframe();
        int getUserOption();
        // Centralized input validation for menus.
        int readIntInRange(int minValue, int maxValue, const std::string& prompt);
        // Centralized numeric input validation.
        double readPositiveDouble(const std::string& prompt);
        void processUserOption(int userOption);
        Wallet& getWallet();
        // Log any user action that changes state.
        void logTransaction(const std::string& type,
                            const std::string& product,
                            double price,
                            double amount);
        // Log simulated orders using system timestamp.
        void logTransactionAt(const std::string& type,
                              const std::string& product,
                              double price,
                              double amount,
                              const std::string& timestamp);
        // Seed initial asks/bids on registration.
        void seedUserOrdersOnRegister();

        std::string currentTime;
        std::string currentUser;
        std::map<std::string, Wallet> userWallets;
        Wallet* activeWallet = nullptr;
        // Users CSV store.
        UserManager userManager{"users.csv"};
        // Wallet and transaction CSV stores.
        WalletStore walletStore{"wallets.csv"};
        TransactionLog transactionLog{"transactions.csv"};

//        OrderBook orderBook{"20200317.csv"};
        OrderBook orderBook{"20200601.csv"};

};
