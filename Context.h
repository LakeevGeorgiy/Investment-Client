#pragma once

#include "Models/User.h"

class Context {
public:

    User user_;
    bool authorized_;

public:
    
    Context();
};