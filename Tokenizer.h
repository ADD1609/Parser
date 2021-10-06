#pragma once

#include <vector>
#include <string>

namespace simpleparser {
    using namespace std;

    enum TokenType {
        WHITESPACE,
        POTENIAL_ID,
        IDENTIFIER,
        COEF,
        INT_LITERAL,
        POTENTIAL_DOUBLE,
        DOUBLE_LITERAL,
        STRING_LITERAL,
        OPERATOR,
        BRACKET_START,
        BRACKET_END,
        PLUS,
        MINUS,
        MULT,
        DIV,
        EQUAL,
        NEG,
        POWER,
        ROOT_START,
        ROOT_END,
        ROOT_COMMA,
        SIN_START,
        ASIN_START,
        COS_START,
        ACOS_START,
        SIN_END,
        ASIN_END,
        COS_END,
        ACOS_END,
        FUNC_NAME,
        FUNC_VAR,
        FUNC_VAR_NAME,
        FUNC_IN
    };

    static const char *sTokenTypeStrings[] = {
            "WHITESPACE",
            "POTENIAL_ID",
            "IDENTIFIER",
            "COEF",
            "INT_LITERAL",
            "POTENTIAL_DOUBLE",
            "DOUBLE_LITERAL",
            "STRING_LITERAL",
            "OPERATOR",
            "BRACKET_START",
            "BRACKET_END",
            "PLUS",
            "MINUS",
            "MULT",
            "DIV",
            "EQUAL",
            "NEG",
            "POWER",
            "ROOT_START",
            "ROOT_END",
            "ROOT_COMMA",
            "SIN_START",
            "ASIN_START",
            "COS_START",
            "ACOS_START",
            "SIN_END",
            "ASIN_END",
            "COS_END",
            "ACOS_END",
            "FUNC_NAME",
            "FUNC_VAR",
            "FUNC_VAR_NAME",
            "FUNC_IN"
    };

    class Token {
    public:
        enum TokenType mType{WHITESPACE};

        string mText;
        vector<string> mState;
        string mFuncState;
        int multState = 0;
        char mVar;

        void debugPrint() const;
    };


    class Tokenizer {
    public:
        vector<Token> parse(const string &inProgram);

    private:
        static void endToken(Token &token, vector<Token> &tokens);
    };
}
