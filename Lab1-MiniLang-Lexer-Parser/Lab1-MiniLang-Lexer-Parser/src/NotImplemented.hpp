#pragma once

#include <stdexcept>
#include <string>

// Thrown by the starter code at every place where YOU must write code (the TODO markers).
// The driver (Main.cpp) turns it into exit code 70, so an unfinished part can never be
// mistaken for a passing test. Delete the `throw NotImplementedError(...)` lines as you
// replace them with real code.
class NotImplementedError : public std::logic_error {
public:
    explicit NotImplementedError(const std::string& where)
        : std::logic_error(where + ": not implemented yet (look for the TODO marker)")
    {}
};
