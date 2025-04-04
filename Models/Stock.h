#pragma once

#include <cstdint>
#include <string>

class Stock {
public:

    uint64_t id_;
    uint32_t cost_;
    uint32_t count_;
    std::string company_name_;

public:

    Stock(uint64_t id, uint32_t cost, uint32_t count, std::string company_name);

    Stock(const Stock& other);

    Stock& operator=(const Stock& other);

    friend bool operator==(const Stock& lhs, const Stock& rhs);
};