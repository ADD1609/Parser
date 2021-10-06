//
// Created by add on 8/21/21.
//

#include "Tokenizer.h"
#include <iostream>

namespace simpleparser {
    using namespace std;

    vector<Token> Tokenizer::parse(const string &inProgram) {
        vector<Token> tokens;
        vector<Token> vars;
        Token currentToken;

        for (char currCh: inProgram) {
            switch (currCh) {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    if (currentToken.mType == NEG) {
                        currentToken.mType = INT_LITERAL;
                        currentToken.mText.append(1, '-');
                        currentToken.mText.append(1, currCh);
                    } else if (currentToken.mType == WHITESPACE) {
                        currentToken.mType = INT_LITERAL;
                        currentToken.mText.append(1, currCh);
                    } else if (currentToken.mType == POTENTIAL_DOUBLE) {
                        currentToken.mType = DOUBLE_LITERAL;
                        currentToken.mText.append(1, currCh);
                    } else if (currentToken.mType == FUNC_NAME) {
                        endToken(currentToken, tokens);
                        currentToken.mFuncState = "FUNC_IN";
                        currentToken.mType = INT_LITERAL;
                        currentToken.mText.append(1, currCh);
                    } else {
                        currentToken.mText.append(1, currCh);
                    }
                    break;
                case '.':
                    if (currentToken.mType == WHITESPACE) {
                        currentToken.mType = POTENTIAL_DOUBLE;
                        currentToken.mText.append(1, currCh);
                    } else if (currentToken.mType == INT_LITERAL) {
                        currentToken.mType = DOUBLE_LITERAL;
                        currentToken.mText.append(1, currCh);
                    } else if (currentToken.mType == STRING_LITERAL) {
                        currentToken.mText.append(1, currCh);
                    } else {
                        endToken(currentToken, tokens);
                        currentToken.mType = OPERATOR;
                        currentToken.mText.append(1, currCh);
                        endToken(currentToken, tokens);
                    }
                    break;
                case '(':
                    if (currentToken.mText == "sin" || currentToken.mText == "asin") {
                        string temp = (currentToken.mText == "sin") ? "SIN" : "ASIN";
                        currentToken.mType = STRING_LITERAL;
                        endToken(currentToken, tokens);
                        currentToken.mType = (temp == "SIN") ? SIN_START : ASIN_START;
                        currentToken.mState.push_back(temp);
                        currentToken.mText.append(1, currCh);
                        endToken(currentToken, tokens);
                    } else if (currentToken.mText == "cos" || currentToken.mText == "acos") {
                        string temp = (currentToken.mText == "cos") ? "COS" : "ACOS";
                        currentToken.mType = STRING_LITERAL;
                        endToken(currentToken, tokens);
                        currentToken.mType = (temp == "COS") ? COS_START : ACOS_START;
                        currentToken.mState.push_back(temp);
                        currentToken.mText.append(1, currCh);
                        endToken(currentToken, tokens);
                    } else if (currentToken.mText == "root" || currentToken.mText == "Root") {
                        currentToken.mType = STRING_LITERAL;
                        endToken(currentToken, tokens);
                        currentToken.mType = ROOT_START;
                        currentToken.mState.emplace_back("ROOT");
                        currentToken.mText.append(1, currCh);
                        endToken(currentToken, tokens);
                    } else if (currentToken.mType == IDENTIFIER && currCh != currentToken.mVar) {
                        currentToken.mType = FUNC_NAME;
                        break;
                    } else if (currentToken.mType != STRING_LITERAL) {
                        endToken(currentToken, tokens);
                        currentToken.mType = BRACKET_START;
                        currentToken.mText.append(1, currCh);
                        endToken(currentToken, tokens);
                    } else {
                        currentToken.mText.append(1, currCh);
                    }
                    break;
                case ')':
                    if (currentToken.mType == FUNC_VAR_NAME) {
                        vars.push_back(currentToken);
                        endToken(currentToken, tokens);
                    } else if (currentToken.mFuncState == "FUNC_IN") {
                        endToken(currentToken, tokens);
                        currentToken.mFuncState = string();
                    } else if (!currentToken.mState.empty()) {
                        if (currentToken.mState.back() == "SIN" || currentToken.mState.back() == "ASIN") {
                            string temp = currentToken.mState.back();
                            endToken(currentToken, tokens);
                            currentToken.mType = (temp == "SIN") ? SIN_END : ASIN_END;
                            currentToken.mText.append(1, currCh);
                            endToken(currentToken, tokens);
                            currentToken.mState.pop_back();
                        } else if (currentToken.mState.back() == "COS" || currentToken.mState.back() == "ACOS") {
                            string temp = currentToken.mState.back();
                            endToken(currentToken, tokens);
                            currentToken.mType = (temp == "COS") ? COS_END : ACOS_END;
                            currentToken.mText.append(1, currCh);
                            endToken(currentToken, tokens);
                            currentToken.mState.pop_back();
                        } else if (currentToken.mState.back() == "ROOT") {
                            endToken(currentToken, tokens);
                            currentToken.mType = ROOT_END;
                            currentToken.mText.append(1, currCh);
                            endToken(currentToken, tokens);
                            currentToken.mState.pop_back();
                        }
                    } else if (currentToken.mType != STRING_LITERAL) {
                        endToken(currentToken, tokens);
                        currentToken.mType = BRACKET_END;
                        currentToken.mText.append(1, currCh);
                        endToken(currentToken, tokens);
                    } else {
                        currentToken.mText.append(1, currCh);
                    }
                    break;
                case '=':
                    if (currentToken.mType != STRING_LITERAL) {
                        endToken(currentToken, tokens);
                        currentToken.mType = EQUAL;
                        currentToken.mText.append(1, currCh);
                        endToken(currentToken, tokens);
                    } else {
                        currentToken.mText.append(1, currCh);
                    }
                    break;
                case '+':
                    if (currentToken.mType != STRING_LITERAL) {
                        endToken(currentToken, tokens);
                        currentToken.mType = PLUS;
                        currentToken.mText.append(1, currCh);
                        endToken(currentToken, tokens);
                    } else {
                        currentToken.mText.append(1, currCh);
                    }
                    break;
                case '*':
                    if (currentToken.mType != STRING_LITERAL) {
                        endToken(currentToken, tokens);
                        currentToken.mType = MULT;
                        currentToken.mText.append(1, currCh);
                        currentToken.multState = 1;
                        endToken(currentToken, tokens);
                    } else {
                        currentToken.mText.append(1, currCh);
                    }
                    break;
                case '/':
                    if (currentToken.mType != STRING_LITERAL) {
                        endToken(currentToken, tokens);
                        currentToken.mType = DIV;
                        currentToken.mText.append(1, currCh);
                        endToken(currentToken, tokens);
                    } else {
                        currentToken.mText.append(1, currCh);
                    }
                    break;
                case '-':
                    if (currentToken.mType == WHITESPACE || currentToken.mType == FUNC_NAME) {
                        auto temp = currentToken.mType;
                        endToken(currentToken, tokens);
                        if (temp == FUNC_NAME)
                            currentToken.mFuncState = "FUNC_IN";
                        currentToken.mType = NEG;
                        break;
                    }
                    if (currentToken.mType != STRING_LITERAL) {
                        endToken(currentToken, tokens);
                        currentToken.mType = MINUS;
                        currentToken.mText.append(1, currCh);
                        endToken(currentToken, tokens);
                    } else {
                        currentToken.mText.append(1, currCh);
                    }
                    break;
                case '^':
                    if (currentToken.mType != STRING_LITERAL) {
                        endToken(currentToken, tokens);
                        currentToken.mType = POWER;
                        currentToken.mText.append(1, currCh);
                        endToken(currentToken, tokens);
                    }
                    break;
                case ',':
                    if (currentToken.mState.back() == "ROOT") {
                        endToken(currentToken, tokens);
                        currentToken.mType = ROOT_COMMA;
                        currentToken.mText.append(1, currCh);
                        endToken(currentToken, tokens);
                    }
                    break;
                case ' ':
                    if (currentToken.mType == NEG) {
                        currentToken.mType = MINUS;
                        currentToken.mText = "-";
                        endToken(currentToken, tokens);
                    } else endToken(currentToken, tokens);
                    break;
                default:
                    if (currentToken.mType == FUNC_NAME) {
                        endToken(currentToken, tokens);
                        currentToken.mType = FUNC_VAR_NAME;
                        currentToken.mVar = currCh;
                        currentToken.mText.append(1, currCh);
                    } else if (currCh == currentToken.mVar && currentToken.multState == 0) {
                        if (currentToken.mType != INT_LITERAL && currentToken.mType != DOUBLE_LITERAL
                            && currentToken.mType != IDENTIFIER) {
                            if (currentToken.mType == NEG) {
                                currentToken.mType = INT_LITERAL;
                                currentToken.mText.append(1, '-');
                                currentToken.mText.append(1, '1');
                            } else {
                                endToken(currentToken, tokens);
                                currentToken.mType = INT_LITERAL;
                                currentToken.mText.append(1, '1');
                            }
                        }
                        if (currentToken.mType == IDENTIFIER)
                            currentToken.mType = COEF;
                        endToken(currentToken, tokens);
                        currentToken.mType = MULT;
                        currentToken.mText.append(1, '*');
                        endToken(currentToken, tokens);
                        currentToken.mType = FUNC_VAR;
                        currentToken.mText.append(1, currCh);
                    } else if (currCh == currentToken.mVar) {
                        endToken(currentToken, tokens);
                        currentToken.mType = FUNC_VAR;
                        currentToken.mText.append(1, currCh);
                    } else if (currentToken.mType == WHITESPACE ||
                               currentToken.mType == INT_LITERAL ||
                               currentToken.mType == DOUBLE_LITERAL) {
                        endToken(currentToken, tokens);
                        currentToken.mType = IDENTIFIER;
                        currentToken.mText.append(1, currCh);
                    } else {
                        currentToken.mText.append(1, currCh);
                    }
                    break;
            }

        }

        endToken(currentToken, tokens);

        return tokens;


    }

    void Tokenizer::endToken(Token &token, vector<Token> &tokens) {
        if (token.mType != WHITESPACE) {
            tokens.push_back(token);
        }
        if (token.mType == POTENTIAL_DOUBLE) {
            token.mType = (token.mText.compare(".") == 0 ? OPERATOR : DOUBLE_LITERAL);
        }
        token.mType = WHITESPACE;
        token.mText.erase();
    }

    void Token::debugPrint() const {
        cout << "Token: " << sTokenTypeStrings[mType] << " " << mText << "\n";
    }
}
