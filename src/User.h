#pragma once

#include <string>

class User
{
    public:
        // User model stored in users.csv with hashed password.
        User(std::string _username,
             std::string _fullName,
             std::string _email,
             std::string _passwordHash);

        std::string username;
        std::string fullName;
        std::string email;
        std::string passwordHash;
};
