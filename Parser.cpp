#include "Parser.h"
#include <iostream>
#include <sstream>
#include <iterator>
#include <cmath>

#define PI 3.14159265

namespace simpleparser {

    using namespace std;

    Func Parser::declare(const vector<Token> &tokens, vector<Token>::iterator begin, vector<Token>::iterator end) {
        mEndToken = end;
        mCurrentToken = begin;
        Func new_usr_func;
        vector<Token> new_func_expr;

        while (mCurrentToken != mEndToken) {
            if (mCurrentToken->mType == FUNC_NAME) {
                new_usr_func.Func_name = *mCurrentToken;
                break;
            } else { ++mCurrentToken; }
        }
        while (mCurrentToken != mEndToken) {
            if (mCurrentToken->mType == FUNC_VAR_NAME) {
                new_usr_func.Func_var_name = *mCurrentToken;
                break;
            } else { ++mCurrentToken; }
        }
        while (mCurrentToken != mEndToken) {
            if (expectEqual().has_value()) {
                Token tempToken;
                while (mCurrentToken != mEndToken) {
                    tempToken = *mCurrentToken;
                    new_func_expr.push_back(tempToken);
                    ++mCurrentToken;
                }
                break;
            } else { ++mCurrentToken; }
        }
        new_usr_func.Func_expres = new_func_expr;
        return new_usr_func;
    }

    double
    Parser::calc(const vector<Token> &tokens, vector<Token>::iterator begin, vector<Token>::iterator end, double output,
                 double var) {
        mEndToken = end;
        mCurrentToken = begin;
        vector<int> mScopes = {0, 0, 0}; // [0]=>insideBracket  [1]=>insideRoot   [2]=>insidePeri

        // WERTE FÜR VAR EINSETZEN
        if (var != -0.0101010101696961420) {
            while (mCurrentToken != mEndToken) {
                if (mCurrentToken->mType == FUNC_VAR) {
                    string temp = to_string(var);
                    mCurrentToken->mText = temp;
                    mCurrentToken->mType = DOUBLE_LITERAL;
                } else {
                    ++mCurrentToken;
                }
            }
            mCurrentToken = begin;
        }

        // ROOT
        while (mCurrentToken != mEndToken) {
            checkScope(mScopes);
            if (expectRootComma().has_value() && !mScopes[0] && !mScopes[1] &&
                !mScopes[2]) {// Wir haben ein Komma in root(n,m).
                auto rootCommaState = mCurrentToken;
                auto right = calc(tokens, rootCommaState, mEndToken, output);// Rechts von ","
                --rootCommaState;
                --rootCommaState;
                auto left = calc(tokens, begin, rootCommaState, output);// Links von ","

                return pow(left, 1.0 / right);
            } else {
                ++mCurrentToken;
            }
        }
        mCurrentToken = begin;
        mScopes = {0, 0, 0};

        // ADDITION
        while (mCurrentToken != mEndToken) {
            checkScope(mScopes);
            if (expectPlus().has_value() && !mScopes[0] && !mScopes[1] && !mScopes[2]) {// Wir haben eine Addition.
                auto plusState = mCurrentToken;
                auto right = calc(tokens, plusState, mEndToken, output);// Rechts von "+"
                --plusState;
                --plusState;
                auto left = calc(tokens, begin, plusState, output);// Links von "+"

                return left + right;
            } else {
                ++mCurrentToken;
            }
        }
        mCurrentToken = begin;
        mScopes = {0, 0, 0};

        while (mCurrentToken != mEndToken) {
            checkScope(mScopes);
            if (expectMinus().has_value() && !mScopes[0] && !mScopes[1] && !mScopes[2]) {// Wir haben eine Subtraktion.
                auto minusState = mCurrentToken;
                auto right = calc(tokens, minusState, mEndToken, output);// Rechts von "-"
                --minusState;
                --minusState;
                auto left = calc(tokens, begin, minusState, output);// Links von "-"

                return left - right;
            } else {
                ++mCurrentToken;
            }
        }
        mCurrentToken = begin;
        mScopes = {0, 0, 0};

        while (mCurrentToken != mEndToken) {
            checkScope(mScopes);
            if (expectMult().has_value() && !mScopes[0] && !mScopes[1] &&
                !mScopes[2]) {// Wir haben eine Multiplikation.
                auto multState = mCurrentToken;
                auto right = calc(tokens, multState, mEndToken, output);// Rechts von "*"
                --multState;
                --multState;
                auto left = calc(tokens, begin, multState, output);// Links von "*"

                return left * right;
            } else {
                ++mCurrentToken;
            }
        }
        mCurrentToken = begin;
        mScopes = {0, 0, 0};


        while (mCurrentToken != mEndToken) {
            checkScope(mScopes);
            if (expectDiv().has_value() && !mScopes[0] && !mScopes[1] && !mScopes[2]) {// Wir haben eine Division.
                auto divState = mCurrentToken;
                auto right = calc(tokens, divState, mEndToken, output);// Rechts von "/"
                --divState;
                --divState;
                auto left = calc(tokens, begin, divState, output);// Links von "/"

                return left / right;
            } else {
                ++mCurrentToken;
            }
        }
        mCurrentToken = begin;
        mScopes = {0, 0, 0};

        while (mCurrentToken != mEndToken) {
            checkScope(mScopes);
            if (expectPow().has_value() && !mScopes[0] && !mScopes[1] && !mScopes[2]) {// Wir haben eine Potenz.
                auto powState = mCurrentToken;
                auto right = calc(tokens, powState, mEndToken, output);// Rechts von "^"
                --powState;
                --powState;
                auto left = calc(tokens, begin, powState, output);// Links von "^"

                return pow(left, right);
            } else {
                ++mCurrentToken;
            }
        }
        mCurrentToken = begin;
        mScopes = {0, 0, 0};

        while (mCurrentToken != mEndToken) {
            checkScope(mScopes);
            if (expectSinStart().has_value() && !mScopes[0] && !mScopes[1]) {// Wir haben sin(.
                auto right = calc(tokens, mCurrentToken, mEndToken, output);// Rechts von "sin("

                //return sin(right*PI/180);
                return sin(right);
            } else {
                ++mCurrentToken;
            }
        }
        mCurrentToken = begin;
        mScopes = {0, 0, 0};

        while (mCurrentToken != mEndToken) {
            checkScope(mScopes);
            if (expectAsinStart().has_value() && !mScopes[0] && !mScopes[1]) {// Wir haben Asin(.
                auto right = calc(tokens, mCurrentToken, mEndToken, output);// Rechts von "Asin("

                return asin(right);
            } else {
                ++mCurrentToken;
            }
        }
        mCurrentToken = begin;
        mScopes = {0, 0, 0};

        while (mCurrentToken != mEndToken) {
            checkScope(mScopes);
            if (expectCosStart().has_value() && !mScopes[0] && !mScopes[1]) {// Wir haben cos(.
                auto right = calc(tokens, mCurrentToken, mEndToken, output);// Rechts von "cos("

                return cos(right);
            } else {
                ++mCurrentToken;
            }
        }
        mCurrentToken = begin;
        mScopes = {0, 0, 0};

        while (mCurrentToken != mEndToken) {
            checkScope(mScopes);
            if (expectAcosStart().has_value() && !mScopes[0] && !mScopes[1]) {// Wir haben Acos(.
                auto right = calc(tokens, mCurrentToken, mEndToken, output);// Rechts von "Acos("

                return acos(right);
            } else {
                ++mCurrentToken;
            }
        }
        mCurrentToken = begin;
        mScopes = {0, 0, 0};

        while (mCurrentToken != mEndToken) {
            checkScope(mScopes);
            if (expectRootStart().has_value() && !mScopes[0]) {// Wir haben eine Wurzel "root(".
                auto right = calc(tokens, mCurrentToken, mEndToken, output);// Rechts von "root("

                return right;
            } else {
                ++mCurrentToken;
            }
        }
        mCurrentToken = begin;

        while (mCurrentToken != mEndToken) {
            if (expectBracket_start().has_value()) {// Wir haben eine Klammer.
                auto right = calc(tokens, mCurrentToken, mEndToken, output);// Rechts von "("

                return right;
            } else {
                ++mCurrentToken;
            }
        }
        mCurrentToken = begin;

        if (mCurrentToken->mType == DOUBLE_LITERAL) {// Wir haben einen Double.
            return stod(mCurrentToken->mText);
        }
        if (mCurrentToken->mType == INT_LITERAL) {// Wir haben einen Int.
            return stoi(mCurrentToken->mText);
        } else { cout << "Kein Argument erkannt\n"; }
        return -0;
    }


    optional<Token> Parser::expectIdentifier(const string &name) {
        if (mCurrentToken == mEndToken) { return nullopt; }
        if (mCurrentToken->mType != IDENTIFIER) { return nullopt; }
        if (!name.empty() && mCurrentToken->mText != name) { return nullopt; }

        Token returnToken = *mCurrentToken;
        ++mCurrentToken;
        return returnToken;
    }

    optional<Token> Parser::expectEqual(const string &name) {
        if (mCurrentToken == mEndToken) { return nullopt; }
        if (mCurrentToken->mType != EQUAL) { return nullopt; }
        if (!name.empty() && mCurrentToken->mText != name) { return nullopt; }

        Token returnToken = *mCurrentToken;
        ++mCurrentToken;
        return returnToken;
    }

    optional<Token> Parser::expectDouble(const string &name) {
        if (mCurrentToken == mEndToken) { return nullopt; }
        if (mCurrentToken->mType != DOUBLE_LITERAL) { return nullopt; }
        if (!name.empty() && mCurrentToken->mText != name) { return nullopt; }

        Token returnToken = *mCurrentToken;
        ++mCurrentToken;
        return returnToken;
    }

    optional<Token> Parser::expectInt(const string &name) {
        if (mCurrentToken == mEndToken) { return nullopt; }
        if (mCurrentToken->mType != INT_LITERAL) { return nullopt; }
        if (!name.empty() && mCurrentToken->mText != name) { return nullopt; }

        Token returnToken = *mCurrentToken;
        ++mCurrentToken;
        return returnToken;
    }

    optional<Token> Parser::expectBracket_start(const string &name) {
        if (mCurrentToken == mEndToken) { return nullopt; }
        if (mCurrentToken->mType != BRACKET_START) { return nullopt; }
        if (!name.empty() && mCurrentToken->mText != name) { return nullopt; }

        Token returnToken = *mCurrentToken;
        ++mCurrentToken;
        return returnToken;
    }

    optional<Token> Parser::expectBracket_end(const string &name) {
        if (mCurrentToken == mEndToken) { return nullopt; }
        if (mCurrentToken->mType != BRACKET_END) { return nullopt; }
        if (!name.empty() && mCurrentToken->mText != name) { return nullopt; }

        Token returnToken = *mCurrentToken;
        ++mCurrentToken;
        return returnToken;
    }

    optional<Token> Parser::expectPlus(const string &name) {
        if (mCurrentToken == mEndToken) { return nullopt; }
        if (mCurrentToken->mType != PLUS) { return nullopt; }
        if (!name.empty() && mCurrentToken->mText != name) { return nullopt; }

        Token returnToken = *mCurrentToken;
        ++mCurrentToken;
        return returnToken;
    }

    optional<Token> Parser::expectMinus(const string &name) {
        if (mCurrentToken == mEndToken) { return nullopt; }
        if (mCurrentToken->mType != MINUS) { return nullopt; }
        if (!name.empty() && mCurrentToken->mText != name) { return nullopt; }

        Token returnToken = *mCurrentToken;
        ++mCurrentToken;
        return returnToken;
    }

    optional<Token> Parser::expectMult(const string &name) {
        if (mCurrentToken == mEndToken) { return nullopt; }
        if (mCurrentToken->mType != MULT) { return nullopt; }
        if (!name.empty() && mCurrentToken->mText != name) { return nullopt; }

        Token returnToken = *mCurrentToken;
        ++mCurrentToken;
        return returnToken;
    }

    optional<Token> Parser::expectDiv(const string &name) {
        if (mCurrentToken == mEndToken) { return nullopt; }
        if (mCurrentToken->mType != DIV) { return nullopt; }
        if (!name.empty() && mCurrentToken->mText != name) { return nullopt; }

        Token returnToken = *mCurrentToken;
        ++mCurrentToken;
        return returnToken;
    }

    optional<Token> Parser::expectPow(const string &name) {
        if (mCurrentToken == mEndToken) { return nullopt; }
        if (mCurrentToken->mType != POWER) { return nullopt; }
        if (!name.empty() && mCurrentToken->mText != name) { return nullopt; }

        Token returnToken = *mCurrentToken;
        ++mCurrentToken;
        return returnToken;
    }

    optional<Token> Parser::expectRootStart(const string &name) {
        if (mCurrentToken == mEndToken) { return nullopt; }
        if (mCurrentToken->mType != ROOT_START) { return nullopt; }
        if (!name.empty() && mCurrentToken->mText != name) { return nullopt; }

        Token returnToken = *mCurrentToken;
        ++mCurrentToken;
        return returnToken;
    }

    optional<Token> Parser::expectRootComma(const string &name) {
        if (mCurrentToken == mEndToken) { return nullopt; }
        if (mCurrentToken->mType != ROOT_COMMA) { return nullopt; }
        if (!name.empty() && mCurrentToken->mText != name) { return nullopt; }

        Token returnToken = *mCurrentToken;
        ++mCurrentToken;
        return returnToken;
    }

    optional<Token> Parser::expectSinStart(const string &name) {
        if (mCurrentToken == mEndToken) { return nullopt; }
        if (mCurrentToken->mType != SIN_START) { return nullopt; }
        if (!name.empty() && mCurrentToken->mText != name) { return nullopt; }

        Token returnToken = *mCurrentToken;
        ++mCurrentToken;
        return returnToken;
    }

    optional<Token> Parser::expectAsinStart(const string &name) {
        if (mCurrentToken == mEndToken) { return nullopt; }
        if (mCurrentToken->mType != ASIN_START) { return nullopt; }
        if (!name.empty() && mCurrentToken->mText != name) { return nullopt; }

        Token returnToken = *mCurrentToken;
        ++mCurrentToken;
        return returnToken;
    }

    optional<Token> Parser::expectCosStart(const string &name) {
        if (mCurrentToken == mEndToken) { return nullopt; }
        if (mCurrentToken->mType != COS_START) { return nullopt; }
        if (!name.empty() && mCurrentToken->mText != name) { return nullopt; }

        Token returnToken = *mCurrentToken;
        ++mCurrentToken;
        return returnToken;
    }

    optional<Token> Parser::expectAcosStart(const string &name) {
        if (mCurrentToken == mEndToken) { return nullopt; }
        if (mCurrentToken->mType != ACOS_START) { return nullopt; }
        if (!name.empty() && mCurrentToken->mText != name) { return nullopt; }

        Token returnToken = *mCurrentToken;
        ++mCurrentToken;
        return returnToken;
    }

    void Parser::checkScope(vector<int> &mScopes) {
        //Prüft ob entweder ein Ausdruck in einer Klammer Anfängt oder endet.
        if (mCurrentToken->mType == BRACKET_START)mScopes[0] = 1;
        if (mCurrentToken->mType == BRACKET_END)mScopes[0] = 0;
        if (mCurrentToken->mType == ROOT_START)mScopes[1] = 1;
        if (mCurrentToken->mType == ROOT_END)mScopes[1] = 0;
        //Prüft ob ein Periodischer Ausdruck entweder beginnt oder endet.
        if (mCurrentToken->mType == SIN_START || mCurrentToken->mType == ASIN_START
            || mCurrentToken->mType == COS_START || mCurrentToken->mType == ACOS_START)
            mScopes[2] = 1;
        if (mCurrentToken->mType == SIN_END || mCurrentToken->mType == ASIN_END
            || mCurrentToken->mType == COS_END || mCurrentToken->mType == ACOS_END)
            mScopes[2] = 0;
    }

    void Parser::round_nplaces(double &value, const uint32_t &to) {
        uint32_t places = 1, whole = *(&value);
        for (uint32_t i = 0; i < to; i++) places *= 10;

        value -= whole; //leave decimals

        value *= places;  //0.1234 -> 123.4
        value = round(value);//123.4 -> 123
        value /= places;  //123 -> .123

        value += whole; //bring the whole value back
    }

    void Parser::makeEqn(const vector<Token> &tokens, vector<Token>::iterator begin, vector<Token>::iterator end,
                         vector<double> &eqn, double var) {
        eqn.clear();
        auto termStart = begin;
        auto eqnCurrentToken = begin;
        auto eqnEndToken = end;

        while (eqnCurrentToken != eqnEndToken) {
            if (eqnCurrentToken->mType == PLUS || eqnCurrentToken->mType == MINUS) {
                double coef = calc(tokens, termStart, eqnCurrentToken, 0.0, var);
                eqn.push_back(coef);
                ++eqnCurrentToken;
                termStart = eqnCurrentToken;
            }
            ++eqnCurrentToken;
        }
        --eqnCurrentToken;
        double lastCoef = calc(tokens, eqnCurrentToken, eqnEndToken, 0.0, var);
        eqn.push_back(lastCoef);
    }

    vector<Token> Parser::ableiten(vector<Token> tokens) {
        ostringstream streamObj;

        auto begin = tokens.begin();
        auto end = tokens.end();

        vector<Token> currentTerm;
        vector<Token> ableitExpres;
        int idx = 0;
        int currentCoefIdx = -1, currentPowIdx = -1, currentVarIdx = -1;
        double currentCoefVal = 0.0, currentPowVal = 0.0;

        mCurrentToken = begin;
        while (mCurrentToken != end) {
            if (mCurrentToken->mType == PLUS || mCurrentToken->mType == MINUS) {
                endTerm(currentTerm, currentPowIdx, currentVarIdx);
                currentTerm.push_back(*mCurrentToken);
                for (const Token &token: currentTerm) {
                    ableitExpres.push_back(token);
                }

                currentTerm.clear();
                idx = -1;
                currentCoefIdx = -1, currentPowIdx = -1, currentVarIdx = -1;
                currentCoefVal = 0.0, currentPowVal = 0.0;
            } else if (mCurrentToken->mType == INT_LITERAL || mCurrentToken->mType == DOUBLE_LITERAL) {
                currentCoefIdx = idx;
                currentCoefVal = stod(mCurrentToken->mText);

                currentTerm.push_back(*mCurrentToken);
            } else if (mCurrentToken->mType == FUNC_VAR) {
                currentVarIdx = idx;
                currentTerm.push_back(*mCurrentToken);
            } else if (mCurrentToken->mType == POWER) {
                ++mCurrentToken;
                ++idx;
                currentPowIdx = idx;
                currentPowVal = stod(mCurrentToken->mText);


                if (currentPowVal > 1) {
                    double newCoefVal = currentCoefVal * currentPowVal;
                    streamObj << newCoefVal;
                    string newCoefValStr = streamObj.str();
                    currentTerm[currentCoefIdx].mText = newCoefValStr;
                    streamObj.str("");
                }

                if (currentPowVal - 1 > 1) {
                    --mCurrentToken;
                    currentTerm.push_back(*mCurrentToken); // Das "^" Zeichen wird hinzugefügt
                    ++mCurrentToken;
                    currentTerm.push_back(*mCurrentToken);
                    streamObj << currentPowVal - 1;
                    string newPowValStr = streamObj.str();
                    currentTerm[currentPowIdx].mText = newPowValStr;
                    streamObj.str("");
                }
            } else {
                currentTerm.push_back(*mCurrentToken);
            }
            ++mCurrentToken;
            ++idx;
        }
        endTerm(currentTerm, currentPowIdx, currentVarIdx);
        if (currentTerm.back().mText == "0" && ableitExpres.size() > 1) {
            ableitExpres.pop_back();
        } else {
            for (const Token &token: currentTerm) {
                ableitExpres.push_back(token);
            }
        }
        return ableitExpres;
    }

    void Parser::endTerm(vector<Token> &Term, int &PowIdx, int &VarIdx) {

        if (PowIdx == -1) {

            if (VarIdx == -1) {
                Term.clear();
            } else {
                Term.erase(Term.begin() + VarIdx);
                Term.erase(Term.begin() + VarIdx - 1);
            }
        }

        if (Term.empty()) {
            Token empty;
            empty.mType = INT_LITERAL;
            empty.mText = "0";
            Term.push_back(empty);
        }
    }
}