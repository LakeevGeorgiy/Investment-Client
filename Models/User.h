#include <cstdint>
#include <string>
#include <vector>
#include <QString>
#include <unordered_map>

class User {
public:

    uint64_t id_;
    QString name_;
    QString password_;
    std::unordered_map<uint64_t, uint32_t> stocks_;

public:

    User(uint64_t id, QString name, QString password);

    User(const User& other);

    User& operator=(const User& other);

    friend bool operator==(const User& lhs, const User& rhs);
};