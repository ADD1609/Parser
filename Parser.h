#pragma once

#include "Tokenizer.h"
#include "User_funcs.h"
#include <optional>

namespace simpleparser {
    using namespace std;

    class Parser {
    public:

        vector<Token> ableiten(vector<Token> tokens);

        Func declare(const vector<Token> &tokens, vector<Token>::iterator begin, vector<Token>::iterator end);

        double
        calc(const vector<Token> &tokens, vector<Token>::iterator begin, vector<Token>::iterator end, double output,
             double var = -0.0101010101696961420);

        void makeEqn(const vector<Token> &tokens, vector<Token>::iterator begin, vector<Token>::iterator end,
                     vector<double> &eqn, double var);

        void round_nplaces(double &value, const uint32_t &to);

    private:
        //! Empty string means match any of your type.
        optional<Token> expectIdentifier(const string &name = string());

        optional<Token> expectEqual(const string &name = string());

        optional<Token> expectInt(const string &name = string());

        optional<Token> expectDouble(const string &name = string());

        optional<Token> expectBracket_start(const string &name = string());

        optional<Token> expectBracket_end(const string &name = string());

        optional<Token> expectPlus(const string &name = string());

        optional<Token> expectMinus(const string &name = string());

        optional<Token> expectMult(const string &name = string());

        optional<Token> expectDiv(const string &name = string());

        optional<Token> expectPow(const string &name = string());

        optional<Token> expectRootStart(const string &name = string());

        optional<Token> expectRootComma(const string &name = string());

        optional<Token> expectSinStart(const string &name = string());

        optional<Token> expectAsinStart(const string &name = string());

        optional<Token> expectCosStart(const string &name = string());

        optional<Token> expectAcosStart(const string &name = string());

        void checkScope(vector<int> &mScopes);

        static void endTerm(vector<Token> &Term, int &PowIdx, int &VarIdx);

        vector<Token>::iterator mCurrentToken;
        vector<Token>::iterator mEndToken;
    };
}


