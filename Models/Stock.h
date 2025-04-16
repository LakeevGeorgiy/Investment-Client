#pragma once

#include <cstdint>
#include <QString>

class Stock {
public:

    uint64_t id_;
    uint32_t cost_;
    uint32_t count_;
    QString company_name_;
    QString image_url_;

public:

    Stock(uint64_t id, uint32_t cost, uint32_t count, QString company_name, QString image_url);

    Stock(const Stock& other);

    Stock& operator=(const Stock& other);

    friend bool operator==(const Stock& lhs, const Stock& rhs);
};