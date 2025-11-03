#pragma once

#if __cplusplus >= 201703L // C++17 and later 
#include <string_view>

bool ends_with(std::string_view str, std::string_view suffix);
// static bool ends_with(std::string_view str, std::string suffix);

bool starts_with(std::string_view str, std::string_view prefix);
#endif // C++17
