#pragma once

#include "Tokenizer.h"
#include <vector>
#include <string>

namespace simpleparser {
    using namespace std;

    class Func {
    public:
        Token Func_name;
        Token Func_var_name;
        vector<Token> Func_expres;

        ~Func();
    };
}