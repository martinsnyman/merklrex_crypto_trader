#pragma once

#include <string>
#include <vector>
#include "User.h"

class UserManager
{
    public:
        // CSV-backed user registration, login, and password reset.
        explicit UserManager(std::string _csvPath);

        bool load();
        bool registerUser(const std::string& fullName,
                          const std::string& email,
                          const std::string& password,
                          std::string& outUsername);
        bool login(const std::string& username, const std::string& password) const;
        bool resetPassword(const std::string& fullName,
                           const std::string& email,
                           const std::string& newPassword);
        std::string findUsername(const std::string& fullName,
                                 const std::string& email) const;

    private:
        std::string csvPath;
        std::vector<User> users;

        std::string hashPassword(const std::string& password) const;
        std::string generateUsername() const;
        bool save() const;
        bool userExists(const std::string& fullName, const std::string& email) const;
};
