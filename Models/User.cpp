#include "User.h"

User::User(uint64_t id, QString name, QString password, uint64_t balance):
    id_(id)
    , name_(name)
    , password_(password)
    , balance_(balance)
    , stocks_()
{}

User::User(const User& other): id_(other.id_), name_(other.name_), password_(other.password_), balance_(other.balance_), stocks_(other.stocks_) {}

User& User::operator=(const User& other) {
    if (this == &other) {
        return *this;
    }

    id_ = other.id_;
    name_ = other.name_;
    password_ = other.password_;
    balance_ = other.balance_;
    stocks_ = other.stocks_;
    return *this;
}

bool operator==(const User& lhs, const User& rhs) {
    return lhs.id_ == rhs.id_ && lhs.name_ == rhs.name_ && lhs.password_ == rhs.password_ && lhs.balance_ == rhs.balance_ && lhs.stocks_ == rhs.stocks_;
}
