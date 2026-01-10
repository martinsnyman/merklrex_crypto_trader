#include "User.h"

// Store user identity and hashed credentials.
User::User(std::string _username,
           std::string _fullName,
           std::string _email,
           std::string _passwordHash)
: username(_username),
  fullName(_fullName),
  email(_email),
  passwordHash(_passwordHash)
{
}
