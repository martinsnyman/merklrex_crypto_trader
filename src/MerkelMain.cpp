#include "MerkelMain.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include "Candlestick.h"
#include "Transaction.h"

namespace
{
    std::string toLower(std::string input)
    {
        std::transform(input.begin(), input.end(), input.begin(),
                       [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
        return input;
    }

    // System timestamp for simulated user orders.
    std::string getCurrentTimestamp()
    {
        auto now = std::chrono::system_clock::now();
        auto timeNow = std::chrono::system_clock::to_time_t(now);
        auto micros = std::chrono::duration_cast<std::chrono::microseconds>(
            now.time_since_epoch()) % 1000000;

        std::tm tmNow{};
#ifdef _WIN32
        localtime_s(&tmNow, &timeNow);
#else
        localtime_r(&timeNow, &tmNow);
#endif

        std::ostringstream oss;
        oss << std::put_time(&tmNow, "%Y/%m/%d %H:%M:%S")
            << "."
            << std::setw(6)
            << std::setfill('0')
            << micros.count();
        return oss.str();
    }
}

MerkelMain::MerkelMain()
{

}

void MerkelMain::init()
{
    int input;
    currentTime = orderBook.getEarliestTime();

    // Require login/registration before using the platform.
    userManager.load();
    if (!handleLogin())
    {
        return;
    }

    while(true)
    {
        printMenu();
        input = getUserOption();
        processUserOption(input);
    }
}


void MerkelMain::printMenu()
{
    // 1 print help
    std::cout << "1: Print help " << std::endl;
    // 2 print exchange stats
    std::cout << "2: Print exchange stats" << std::endl;
    // 3 make an offer
    std::cout << "3: Make an offer " << std::endl;
    // 4 make a bid 
    std::cout << "4: Make a bid " << std::endl;
    // 5 print wallet
    std::cout << "5: Print wallet " << std::endl;
    // 6 continue   
    std::cout << "6: Continue " << std::endl;
    // 7 candlestick data
    std::cout << "7: Print candlestick data " << std::endl;
    // Wallet utilities and history.
    // 8 deposit/withdraw
    std::cout << "8: Manage funds " << std::endl;
    // 9 recent transactions
    std::cout << "9: Recent transactions " << std::endl;
    // 10 summary statistics
    std::cout << "10: User activity summary " << std::endl;

    std::cout << "============== " << std::endl;

    std::cout << "Current time is: " << currentTime << std::endl;
}

void MerkelMain::printLoginMenu()
{
    std::cout << "1: Login" << std::endl;
    std::cout << "2: Register" << std::endl;
    std::cout << "3: Forgot login/reset password" << std::endl;
    std::cout << "4: Quit" << std::endl;
}

void MerkelMain::printHelp()
{
    std::cout << "Help - your aim is to make money. Analyse the market and make bids and offers. " << std::endl;
}

void MerkelMain::printMarketStats()
{
    for (std::string const& p : orderBook.getKnownProducts())
    {
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, 
                                                                p, currentTime);
        std::cout << "Asks seen: " << entries.size() << std::endl;
        std::cout << "Max ask: " << OrderBook::getHighPrice(entries) << std::endl;
        std::cout << "Min ask: " << OrderBook::getLowPrice(entries) << std::endl;



    }
    // std::cout << "OrderBook contains :  " << orders.size() << " entries" << std::endl;
    // unsigned int bids = 0;
    // unsigned int asks = 0;
    // for (OrderBookEntry& e : orders)
    // {
    //     if (e.orderType == OrderBookType::ask)
    //     {
    //         asks ++;
    //     }
    //     if (e.orderType == OrderBookType::bid)
    //     {
    //         bids ++;
    //     }  
    // }    
    // std::cout << "OrderBook asks:  " << asks << " bids:" << bids << std::endl;

}

void MerkelMain::enterAsk()
{
    std::cout << "Make an ask - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterAsk Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::ask 
            );
            obe.username = "simuser";
            if (getWallet().canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
                logTransaction("ask", obe.product, obe.price, obe.amount);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterAsk Bad input " << std::endl;
        }   
    }
}

void MerkelMain::enterBid()
{
    std::cout << "Make an bid - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterBid Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::bid 
            );
            obe.username = "simuser";

            if (getWallet().canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
                logTransaction("bid", obe.product, obe.price, obe.amount);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterBid Bad input " << std::endl;
        }   
    }
}

void MerkelMain::printWallet()
{
    std::cout << getWallet().toString() << std::endl;
}

void MerkelMain::manageFunds()
{
    // Deposit/withdraw flow with validation.
    std::cout << "Deposit or withdraw? (d/w)" << std::endl;
    std::string action;
    std::getline(std::cin, action);
    action = toLower(action);

    bool deposit = (action == "d" || action == "deposit");
    bool withdraw = (action == "w" || action == "withdraw");
    if (!deposit && !withdraw)
    {
        std::cout << "Invalid choice." << std::endl;
        return;
    }

    std::cout << "Enter currency (e.g., BTC): " << std::endl;
    std::string currency;
    std::getline(std::cin, currency);
    if (currency.empty())
    {
        std::cout << "No currency entered." << std::endl;
        return;
    }

    double amount = readPositiveDouble("Enter amount: ");

    if (deposit)
    {
        getWallet().insertCurrency(currency, amount);
        walletStore.saveWallet(currentUser, getWallet());
        logTransaction("deposit", currency, 0.0, amount);
        std::cout << "Deposit successful." << std::endl;
    }
    else
    {
        if (!getWallet().removeCurrency(currency, amount))
        {
            std::cout << "Insufficient funds." << std::endl;
            return;
        }
        walletStore.saveWallet(currentUser, getWallet());
        logTransaction("withdraw", currency, 0.0, amount);
        std::cout << "Withdrawal successful." << std::endl;
    }
}

void MerkelMain::printCandlesticks()
{
    // Candlestick summary by product/type/timeframe.
    std::cout << "Enter product (e.g., ETH/BTC): " << std::endl;
    std::string product;
    std::getline(std::cin, product);

    if (product.empty())
    {
        std::cout << "No product entered." << std::endl;
        return;
    }

    bool knownProduct = false;
    for (const std::string& p : orderBook.getKnownProducts())
    {
        if (p == product)
        {
            knownProduct = true;
            break;
        }
    }
    if (!knownProduct)
    {
        std::cout << "Unknown product: " << product << std::endl;
        return;
    }

    std::cout << "Enter order type (ask/bid): " << std::endl;
    std::string typeInput;
    std::getline(std::cin, typeInput);
    typeInput = toLower(typeInput);

    OrderBookType type = OrderBookType::unknown;
    if (typeInput == "ask")
    {
        type = OrderBookType::ask;
    }
    else if (typeInput == "bid")
    {
        type = OrderBookType::bid;
    }
    else
    {
        std::cout << "Invalid order type: " << typeInput << std::endl;
        return;
    }

    std::cout << "Enter timeframe (daily/monthly/yearly). Default yearly: " << std::endl;
    std::string timeframeInput;
    std::getline(std::cin, timeframeInput);
    timeframeInput = toLower(timeframeInput);

    CandlestickGranularity granularity = CandlestickGranularity::yearly;
    if (timeframeInput == "daily")
    {
        granularity = CandlestickGranularity::daily;
    }
    else if (timeframeInput == "monthly")
    {
        granularity = CandlestickGranularity::monthly;
    }
    else if (timeframeInput == "yearly" || timeframeInput.empty())
    {
        granularity = CandlestickGranularity::yearly;
    }
    else
    {
        std::cout << "Invalid timeframe: " << timeframeInput << std::endl;
        return;
    }

    std::vector<Candlestick> sticks = orderBook.getCandlesticks(type, product, granularity);
    if (sticks.empty())
    {
        std::cout << "No data for " << product << std::endl;
        return;
    }

    std::cout << "Date Open High Low Close" << std::endl;
    for (const Candlestick& stick : sticks)
    {
        std::cout << stick.period << " "
                  << stick.open << " "
                  << stick.high << " "
                  << stick.low << " "
                  << stick.close << std::endl;
    }
}
        
void MerkelMain::gotoNextTimeframe()
{
    std::cout << "Going to next time frame. " << std::endl;
    for (std::string p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales =  orderBook.matchAsksToBids(p, currentTime);
        std::cout << "Sales: " << sales.size() << std::endl;
        for (OrderBookEntry& sale : sales)
        {
            std::cout << "Sale price: " << sale.price << " amount " << sale.amount << std::endl; 
            if (sale.username == "simuser")
            {
                // update the wallet
                getWallet().processSale(sale);
                // Persist wallet and log sales.
                walletStore.saveWallet(currentUser, getWallet());
                std::string saleType = (sale.orderType == OrderBookType::bidsale) ? "bidsale" : "asksale";
                logTransaction(saleType, sale.product, sale.price, sale.amount);
            }
        }
        
    }

    currentTime = orderBook.getNextTime(currentTime);
}
 
int MerkelMain::getUserOption()
{
    return readIntInRange(1, 10, "Type in 1-10");
}

Wallet& MerkelMain::getWallet()
{
    return *activeWallet;
}

bool MerkelMain::handleLogin()
{
    while (true)
    {
        // Login/register/reset menu.
        printLoginMenu();
        std::string line;
        std::getline(std::cin, line);

        int choice = 0;
        try
        {
            choice = std::stoi(line);
        }
        catch (const std::exception&)
        {
            choice = 0;
        }

        if (choice == 1)
        {
            std::cout << "Enter username: " << std::endl;
            std::string username;
            std::getline(std::cin, username);
            std::cout << "Enter password: " << std::endl;
            std::string password;
            std::getline(std::cin, password);

            if (userManager.login(username, password))
            {
                currentUser = username;
                if (userWallets.count(username) == 0)
                {
                    // Load wallet from CSV or initialize.
                    Wallet loadedWallet = walletStore.loadWallet(username);
                    if (loadedWallet.empty())
                    {
                        loadedWallet.insertCurrency("BTC", 10);
                        walletStore.saveWallet(username, loadedWallet);
                    }
                    userWallets[username] = loadedWallet;
                }
                activeWallet = &userWallets[username];
                std::cout << "Login successful." << std::endl;
                return true;
            }
            std::cout << "Invalid username or password." << std::endl;
        }
        else if (choice == 2)
        {
            std::cout << "Enter full name: " << std::endl;
            std::string fullName;
            std::getline(std::cin, fullName);
            std::cout << "Enter email address: " << std::endl;
            std::string email;
            std::getline(std::cin, email);
            std::cout << "Enter password: " << std::endl;
            std::string password;
            std::getline(std::cin, password);

            std::string username;
            if (userManager.registerUser(fullName, email, password, username))
            {
                std::cout << "Registered. Your username is: " << username << std::endl;
                currentUser = username;
                Wallet newWallet;
                newWallet.insertCurrency("BTC", 10);
                userWallets[username] = newWallet;
                walletStore.saveWallet(username, newWallet);
                activeWallet = &userWallets[username];
                // Seed initial asks/bids and log them.
                seedUserOrdersOnRegister();
                return true;
            }
            std::cout << "User already exists with that name and email." << std::endl;
        }
        else if (choice == 3)
        {
            std::cout << "Enter full name: " << std::endl;
            std::string fullName;
            std::getline(std::cin, fullName);
            std::cout << "Enter email address: " << std::endl;
            std::string email;
            std::getline(std::cin, email);

            std::string username = userManager.findUsername(fullName, email);
            if (username.empty())
            {
                std::cout << "No matching user found." << std::endl;
                continue;
            }
            std::cout << "Your username is: " << username << std::endl;
            std::cout << "Reset password? (y/n)" << std::endl;
            std::string resetChoice;
            std::getline(std::cin, resetChoice);
            resetChoice = toLower(resetChoice);
            if (resetChoice == "y" || resetChoice == "yes")
            {
                std::cout << "Enter new password: " << std::endl;
                std::string newPassword;
                std::getline(std::cin, newPassword);
                if (userManager.resetPassword(fullName, email, newPassword))
                {
                    std::cout << "Password reset successful." << std::endl;
                }
                else
                {
                    std::cout << "Password reset failed." << std::endl;
                }
            }
        }
        else if (choice == 4)
        {
            return false;
        }
        else
        {
            std::cout << "Invalid choice." << std::endl;
        }
    }
}

void MerkelMain::logTransaction(const std::string& type,
                                const std::string& product,
                                double price,
                                double amount)
{
    // Default log uses current simulated time.
    logTransactionAt(type, product, price, amount, currentTime);
}

void MerkelMain::logTransactionAt(const std::string& type,
                                  const std::string& product,
                                  double price,
                                  double amount,
                                  const std::string& timestamp)
{
    Transaction transaction(currentUser,
                            timestamp,
                            type,
                            product,
                            price,
                            amount,
                            getWallet().toCompactString());
    transactionLog.append(transaction);
}

void MerkelMain::seedUserOrdersOnRegister()
{
    // Use current system time for simulated new orders.
    std::string timestamp = getCurrentTimestamp();
    std::vector<std::string> products = orderBook.getKnownProducts();
    for (const std::string& product : products)
    {
        std::vector<OrderBookEntry> asks = orderBook.getOrders(OrderBookType::ask, product, currentTime);
        if (asks.empty())
        {
            asks = orderBook.getOrders(OrderBookType::ask, product);
        }
        std::vector<OrderBookEntry> bids = orderBook.getOrders(OrderBookType::bid, product, currentTime);
        if (bids.empty())
        {
            bids = orderBook.getOrders(OrderBookType::bid, product);
        }

        double askBase = 1.0;
        if (!asks.empty())
        {
            askBase = (OrderBook::getLowPrice(asks) + OrderBook::getHighPrice(asks)) / 2.0;
        }
        double bidBase = askBase;
        if (!bids.empty())
        {
            bidBase = (OrderBook::getLowPrice(bids) + OrderBook::getHighPrice(bids)) / 2.0;
        }

        for (int i = 0; i < 5; ++i)
        {
            // Price offsets create a small spread around mid price.
            double step = 0.002 * (i + 1);
            double askPrice = askBase * (1.0 + step);
            double bidPrice = bidBase * (1.0 - step);
            if (bidPrice <= 0.0)
            {
                bidPrice = askBase * 0.99;
            }
            double amount = 0.1 * (i + 1);

            OrderBookEntry ask{askPrice, amount, timestamp, product, OrderBookType::ask, "simuser"};
            OrderBookEntry bid{bidPrice, amount, timestamp, product, OrderBookType::bid, "simuser"};

            orderBook.insertOrder(ask);
            orderBook.insertOrder(bid);
            // Persist seeded orders to the transaction log.
            logTransactionAt("ask", product, askPrice, amount, timestamp);
            logTransactionAt("bid", product, bidPrice, amount, timestamp);
        }
    }
}

void MerkelMain::printRecentTransactions()
{
    // Show last five trading transactions (optional product filter).
    std::vector<Transaction> transactions = transactionLog.loadUser(currentUser);
    if (transactions.empty())
    {
        std::cout << "No transactions found." << std::endl;
        return;
    }

    std::cout << "Filter by product (leave blank for all): " << std::endl;
    std::string productFilter;
    std::getline(std::cin, productFilter);

    std::vector<Transaction> filtered;
    for (const Transaction& t : transactions)
    {
        bool tradingType = (t.type == "ask" || t.type == "bid" ||
                            t.type == "asksale" || t.type == "bidsale");
        if (!tradingType)
        {
            continue;
        }
        if (!productFilter.empty() && t.product != productFilter)
        {
            continue;
        }
        filtered.push_back(t);
    }

    if (filtered.empty())
    {
        std::cout << "No matching transactions." << std::endl;
        return;
    }

    std::cout << "Showing last 5 transactions" << std::endl;
    int count = 0;
    for (auto it = filtered.rbegin(); it != filtered.rend() && count < 5; ++it, ++count)
    {
        std::cout << it->timestamp << " "
                  << it->type << " "
                  << it->product << " "
                  << it->price << " "
                  << it->amount << std::endl;
    }
}

void MerkelMain::printUserSummary()
{
    // Summaries for asks/bids and spending by timeframe.
    std::vector<Transaction> transactions = transactionLog.loadUser(currentUser);
    if (transactions.empty())
    {
        std::cout << "No transactions found." << std::endl;
        return;
    }

    int totalAsks = 0;
    int totalBids = 0;
    for (const Transaction& t : transactions)
    {
        if (t.type == "ask")
        {
            totalAsks++;
        }
        else if (t.type == "bid")
        {
            totalBids++;
        }
    }

    std::cout << "Total asks: " << totalAsks << std::endl;
    std::cout << "Total bids: " << totalBids << std::endl;

    std::cout << "Enter product for product-specific stats (leave blank for all): " << std::endl;
    std::string productFilter;
    std::getline(std::cin, productFilter);

    int productAsks = 0;
    int productBids = 0;
    for (const Transaction& t : transactions)
    {
        if (!productFilter.empty() && t.product != productFilter)
        {
            continue;
        }
        if (t.type == "ask")
        {
            productAsks++;
        }
        else if (t.type == "bid")
        {
            productBids++;
        }
    }

    if (!productFilter.empty())
    {
        std::cout << "Asks for " << productFilter << ": " << productAsks << std::endl;
        std::cout << "Bids for " << productFilter << ": " << productBids << std::endl;
    }

    std::cout << "Enter timeframe (daily/monthly/yearly): " << std::endl;
    std::string timeframeInput;
    std::getline(std::cin, timeframeInput);
    timeframeInput = toLower(timeframeInput);

    std::string periodPrefix;
    if (timeframeInput == "daily")
    {
        std::cout << "Enter date (YYYY-MM-DD): " << std::endl;
        std::getline(std::cin, periodPrefix);
    }
    else if (timeframeInput == "monthly")
    {
        std::cout << "Enter month (YYYY-MM): " << std::endl;
        std::getline(std::cin, periodPrefix);
    }
    else if (timeframeInput == "yearly")
    {
        std::cout << "Enter year (YYYY): " << std::endl;
        std::getline(std::cin, periodPrefix);
    }
    else
    {
        std::cout << "Invalid timeframe." << std::endl;
        return;
    }

    double totalSpent = 0.0;
    for (const Transaction& t : transactions)
    {
        if (t.type != "bidsale")
        {
            continue;
        }
        if (!productFilter.empty() && t.product != productFilter)
        {
            continue;
        }
        std::string dateKey = t.timestamp;
        if (dateKey.size() >= 10)
        {
            dateKey = dateKey.substr(0, 10);
            for (char& c : dateKey)
            {
                if (c == '/')
                {
                    c = '-';
                }
            }
        }
        if (dateKey.rfind(periodPrefix, 0) != 0)
        {
            continue;
        }
        totalSpent += t.price * t.amount;
    }

    std::cout << "Total spent in timeframe: " << totalSpent << std::endl;
}

void MerkelMain::processUserOption(int userOption)
{
    if (userOption == 1) 
    {
        printHelp();
    }
    if (userOption == 2) 
    {
        printMarketStats();
    }
    if (userOption == 3) 
    {
        enterAsk();
    }
    if (userOption == 4) 
    {
        enterBid();
    }
    if (userOption == 5) 
    {
        printWallet();
    }
    if (userOption == 6) 
    {
        gotoNextTimeframe();
    }
    if (userOption == 7)
    {
        printCandlesticks();
    }
    if (userOption == 8)
    {
        manageFunds();
    }
    if (userOption == 9)
    {
        printRecentTransactions();
    }
    if (userOption == 10)
    {
        printUserSummary();
    }       
}

int MerkelMain::readIntInRange(int minValue, int maxValue, const std::string& prompt)
{
    //: Validate menu input until valid.
    while (true)
    {
        std::cout << prompt << std::endl;
        std::string line;
        std::getline(std::cin, line);
        try
        {
            int value = std::stoi(line);
            if (value >= minValue && value <= maxValue)
            {
                std::cout << "You chose: " << value << std::endl;
                return value;
            }
        }
        catch (const std::exception&)
        {
        }
        std::cout << "Invalid choice. Choose " << minValue << "-" << maxValue << std::endl;
    }
}

double MerkelMain::readPositiveDouble(const std::string& prompt)
{
    //: Validate numeric input until positive.
    while (true)
    {
        std::cout << prompt << std::endl;
        std::string line;
        std::getline(std::cin, line);
        try
        {
            double value = std::stod(line);
            if (value > 0.0)
            {
                return value;
            }
        }
        catch (const std::exception&)
        {
        }
        std::cout << "Invalid amount. Enter a positive number." << std::endl;
    }
}
