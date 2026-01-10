# Merklerex (C++ Console Crypto Exchange Simulator)

A small console program that simulates a basic crypto exchange: you can register/login, manage a wallet, place bids/asks on products like `ETH/BTC`, advance the market through time, and view simple candlestick summaries.

## Requirements

- A C++ compiler (`g++`/MinGW-w64, or `clang++`) and a terminal.
- Run the program from the `src/` folder so the CSV data files can be found.

## Build and run

### Option A: Build with `g++` (recommended)

From the repository root:

```powershell
cd src
# simplest build (creates a default executable, e.g. a.exe on Windows)
g++ *.cpp
# run
./a.exe
```

If you want a named binary:

```powershell
cd src
g++ -std=c++17 -O2 -o merklerex *.cpp
./merklerex.exe
```

## Data files and persistence

The app loads and writes files using relative paths, so keep these files next to the executable (the `src/` folder already contains them):

- Market dataset: `src/20200601.csv` (selected in `src/MerkelMain.h`)
  - You can switch to the smaller `src/20200317.csv` by changing the filename in `src/MerkelMain.h`.
- User accounts: `src/users.csv` (username, full name, email, hashed password)
- Wallet balances: `src/wallets.csv` (username, currency, amount)
- Transaction history: `src/transactions.csv` (username, timestamp, type, product/currency, price, amount, wallet snapshot)

## How to use

### 1) Login / Register

When the program starts, you must choose one of:

1. Login
2. Register
3. Forgot login / reset password
4. Quit

Notes:
- Registering creates a random numeric username and gives the new user an initial balance of `BTC = 10`.
- On registration the program also seeds a few initial bids/asks (for each known product) and logs them.

### 2) Main menu

After login, the app loops over this menu (shown with the current market timestamp):

1. Print help
2. Print exchange stats (per product: ask count, min ask, max ask at the current time)
3. Make an offer (ask)
4. Make a bid
5. Print wallet
6. Continue (match asks<->bids for each product, update wallet for any sales, then move to the next timestamp)
7. Print candlestick data (OHLC aggregated by day/month/year)
8. Manage funds (deposit/withdraw)
9. Recent transactions (shows the last 5 trade-related entries)
10. User activity summary (counts asks/bids; optional product filter; spending by timeframe)

### Placing orders (options 3 and 4)

You are prompted to enter:

`product,price,amount`

Example:

`ETH/BTC,200,0.5`

How the wallet check works:
- **Ask** (`ETH/BTC`): you must have at least `amount` of the *base* currency (`ETH`).
- **Bid** (`ETH/BTC`): you must have at least `amount * price` of the *quote* currency (`BTC`).

### Candlesticks (option 7)

- Choose a product (e.g. `ETH/BTC`)
- Choose an order type (`ask` or `bid`)
- Choose timeframe: `daily`, `monthly`, or `yearly`

The program prints `Date Open High Low Close` for each period.

## Main learning points (first C++ project)

- **OOP decomposition**: separating responsibilities into classes (`MerkelMain`, `OrderBook`, `Wallet`, `CSVReader`, `UserManager`, `WalletStore`, `TransactionLog`).
- **STL data structures**: using `std::vector` for collections and `std::map` for balances / product sets.
- **Parsing and validation**: tokenising CSV/input strings, converting to numbers, and validating menu + numeric input.
- **Sorting/algorithms**: ordering entries by timestamp and matching asks to bids using sorted price lists.
- **File I/O and persistence**: reading a large dataset and writing user/wallet/transaction state back to CSV.
- **Time handling**: working with timestamps in the dataset and generating system timestamps for seeded orders.
- **Debugging and iteration**: building a text UI loop, testing edge cases (bad input, insufficient funds), and refining features.

## Notes / limitations

- `users.csv` stores passwords using `std::hash` (fine for learning, not secure for real authentication).
- This is a learning simulator; it is not a real exchange and does not model networking, concurrency, or realistic order books.

