#include "UserManager.h"
#include "CSVReader.h"
#include <fstream>
#include <functional>
#include <random>

// Load/store users in a temporary CSV file.
UserManager::UserManager(std::string _csvPath)
: csvPath(_csvPath)
{
}

bool UserManager::load()
{
    users.clear();
    std::ifstream file{csvPath};
    if (!file.is_open())
    {
        return true;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::vector<std::string> tokens = CSVReader::tokenise(line, ',');
        if (tokens.size() != 4)
        {
            continue;
        }
        users.emplace_back(tokens[0], tokens[1], tokens[2], tokens[3]);
    }
    return true;
}

// Enforce unique full name + email; store hashed password.
bool UserManager::registerUser(const std::string& fullName,
                               const std::string& email,
                               const std::string& password,
                               std::string& outUsername)
{
    if (userExists(fullName, email))
    {
        return false;
    }

    outUsername = generateUsername();
    std::string passwordHash = hashPassword(password);
    users.emplace_back(outUsername, fullName, email, passwordHash);
    return save();
}

// Compare hashed password during login.
bool UserManager::login(const std::string& username, const std::string& password) const
{
    std::string passwordHash = hashPassword(password);
    for (const User& user : users)
    {
        if (user.username == username && user.passwordHash == passwordHash)
        {
            return true;
        }
    }
    return false;
}

// Password reset after identity check.
bool UserManager::resetPassword(const std::string& fullName,
                                const std::string& email,
                                const std::string& newPassword)
{
    std::string passwordHash = hashPassword(newPassword);
    for (User& user : users)
    {
        if (user.fullName == fullName && user.email == email)
        {
            user.passwordHash = passwordHash;
            return save();
        }
    }
    return false;
}

std::string UserManager::findUsername(const std::string& fullName,
                                      const std::string& email) const
{
    for (const User& user : users)
    {
        if (user.fullName == fullName && user.email == email)
        {
            return user.username;
        }
    }
    return "";
}

// Hash passwords with std::hash for storage.
std::string UserManager::hashPassword(const std::string& password) const
{
    std::hash<std::string> hasher;
    return std::to_string(hasher(password));
}

std::string UserManager::generateUsername() const
{
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<long long> dist(1000000000LL, 9999999999LL);

    std::string candidate;
    bool unique = false;
    while (!unique)
    {
        candidate = std::to_string(dist(gen));
        unique = true;
        for (const User& user : users)
        {
            if (user.username == candidate)
            {
                unique = false;
                break;
            }
        }
    }
    return candidate;
}

bool UserManager::save() const
{
    std::ofstream file{csvPath, std::ios::trunc};
    if (!file.is_open())
    {
        return false;
    }

    for (const User& user : users)
    {
        file << user.username << ","
             << user.fullName << ","
             << user.email << ","
             << user.passwordHash << "\n";
    }
    return true;
}

bool UserManager::userExists(const std::string& fullName, const std::string& email) const
{
    for (const User& user : users)
    {
        if (user.fullName == fullName && user.email == email)
        {
            return true;
        }
    }
    return false;
}
