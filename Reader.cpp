#include "Reader.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "gauss_ext.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace simpleparser;
using namespace std;

const string plotFileName = "currplot.dat";
const string plotConfigName = "currplot.gp";

const char alphabet[24] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
                           's', 't', 'u', 'v', 'w', 'z'};

vector<Func> usr_funcs;

ofstream os(plotFileName);
ofstream gp(plotConfigName);

void Reader::listen() {
    while (true) {
        string v = string();
        cout << "\nModus wählen: ";
        if (!getline(cin, v)) { break; }
        Actions action = Reader::resolveActions(v);
        switch (action) {
            case EXIT:
                cout << "Programm wird runtergefahren...\n";
                return;
            case ACTION_INVALID:
                cout << "Diesen Modus kenn ich nicht!\n";
                break;
            case DEBUG:
                Reader::debug(1);
                break;
            case PRINT:
                Reader::debug(0);
                break;
            case CALC:
                Reader::calc();
                break;
            case SOLVE:
                Reader::solve();
                break;
            case DECLARE:
                Reader::declare();
                break;
            case ABLEIT:
                Reader::ableit();
                break;
            case HELP:
                cout << "\nFolgende Modi können genutzt werden:\n";
                for (const auto &name: ActionsStrings) {
                    cout << name.first << " ";
                }
                cout << "\n\n";
                break;
            case SHOW_ALL:
                show_all_funcs();
                break;
            case TABLE:
                table();
                break;
            case NULLSTELLEN:
                nullstellen();
                break;
            case DELETE_ALL:
                delAll();
                break;
            case EXTREM:
                extrem();
                break;
            case PLOT:
                plot();
                break;
            case TRASS:
                trass();
                break;
        }
    }
}


Actions Reader::resolveActions(const std::string &input) {

    auto itr = ActionsStrings.find(input);
    if (itr != ActionsStrings.end()) {
        return itr->second;
    }
    return ACTION_INVALID;
}

int Reader::check(const string &func_name, int mode) {
    if (usr_funcs.empty() && mode != 0)return -1;
    int idx = 0;
    for (const Func &currFunc: usr_funcs) {
        if (currFunc.Func_name.mText == func_name) {
            switch (mode) {
                case 0:
                    cout << currFunc.Func_name.mText << "(" << currFunc.Func_var_name.mText << ") = ";
                    for (const Token &token: currFunc.Func_expres) {
                        cout << token.mText;
                    }
                    cout << "\n";
                    return 0;
                case 1:
                    return -2;
                case 2:
                    usr_funcs.erase(usr_funcs.begin() + idx);
                    cout << "\nFunktion wurde erfolgreich gelöscht\n";
                    return 0;
                case 3:
                    return idx;
                default:
                    return -1;
            }
        }
        ++idx;
    }
    return -1;
}


void Reader::debug(int mode) {
    while (true) {
        string c;
        cout << "Eingabe: ";
        getline(cin, c);
        if (c == "back" || c == "break") { break; }
        Tokenizer tokenizer;
        vector<Token> tokens = tokenizer.parse(c);
        if (mode == 0) {
            for (const Token &token: tokens) {
                cout << token.mText;
            }
        } else if (mode == 1) {
            for (const Token &token: tokens) {
                token.debugPrint();
            }
        }
        cout << "\n\n";
    }
}


void Reader::calc() {
    while (true) {
        string c = string();
        cout << "\n\nRechnung eingeben: ";
        getline(cin, c);
        if (c == "back" || c == "break") { break; }
        Tokenizer tokenizer;
        Parser parser;
        vector<Token> tokens = tokenizer.parse(c);
        double sol = parser.calc(tokens, tokens.begin(), tokens.end(), 0.0);
        parser.round_nplaces(sol, 4);
        cout << c << " = " << sol;
    }
}

void Reader::declare() {
    while (true) {
        string c = string();
        cout << "\nNeue Funktion definieren: ";
        getline(cin, c);
        if (c == "back" || c == "break") { break; }
        Tokenizer tokenizer;
        Parser parser;
        vector<Token> tokens = tokenizer.parse(c);
        if (check(tokens.front().mText, 1) == -2) {
            cout << "\nDieser Funktionsname ist bereits belegt\nAlte Funktion löschen?\n[ja/nein]:";
            getline(cin, c);
            if (c == "ja") { check(tokens.front().mText, 2); } else return;
            cout << "\nNeue Definiton von [" << tokens.front().mText << "] speichern?\n[ja/nein]:";
            getline(cin, c);
            if (c == "nein") { return; }
        }
        Func my_func = parser.declare(tokens, tokens.begin(), tokens.end());
        usr_funcs.push_back(my_func);
        show_all_funcs();
    }
}

void Reader::solve() {
    while (true) {
        if (usr_funcs.empty()) {
            cout << "Sie haben noch keine Funktionen definiert!\n";
            return;
        }
        string c = string();
        cout << "\nFunktion und Stelle a(n): ";
        getline(cin, c);
        if (c == "back" || c == "break") { break; }
        Tokenizer tokenizer;
        Parser parser;
        vector<Token> tokens = tokenizer.parse(c);
        if (tokens.empty()) {
            cout << "\nKeine Eingabe erkannt";
            return;
        }
        auto mCurrentToken = tokens.begin();
        auto mEndToken = tokens.end();
        while (mCurrentToken != mEndToken) {
            if (mCurrentToken->mType == FUNC_NAME) {
                int idx = check(mCurrentToken->mText, 3);
                if (idx != -1) {
                    Func temp_func_copy = usr_funcs[idx];
                    ++mCurrentToken;
                    if (mCurrentToken->mType != INT_LITERAL && mCurrentToken->mType != DOUBLE_LITERAL) {
                        cout << "Kein x Wert erkannt";
                        return;
                    }
                    double xValIn = stod(mCurrentToken->mText);
                    double sol = parser.calc(usr_funcs[idx].Func_expres, usr_funcs[idx].Func_expres.begin(),
                                             usr_funcs[idx].Func_expres.end(), 0.0, xValIn);
                    parser.round_nplaces(sol, 4);
                    cout << usr_funcs[idx].Func_name.mText << "(" << xValIn << ") = " << sol;
                    usr_funcs[idx] = temp_func_copy;
                } else {
                    cout << "Keine Passende Funktion gefunden\n";
                    break;
                }
            } else { ++mCurrentToken; }
        }
    }
}

void Reader::table() {
    while (true) {
        if (usr_funcs.empty()) {
            cout << "Sie haben noch keine Funktionen definiert!\n";
            return;
        }
        string c = string();
        show_all_funcs();
        cout << "\nIndex wählen: ";
        getline(cin, c);
        if (c.empty()) {
            cout << "\nKeine Eingabe erkannt";
            return;
        } else if (c == "back" || c == "break") { break; }
        Parser parser;
        int idx = stoi(c);
        if (idx < usr_funcs.size()) {
            double sol = 0;
            Func temp_func_copy = usr_funcs[idx];
            cout << "\nStartwert?: ";
            getline(cin, c);
            double xStart = stod(c);

            cout << "\nEndwert?: ";
            getline(cin, c);
            double xEnd = stod(c);

            cout << "\nSchrittweite?: ";
            getline(cin, c);
            double xStep = stod(c);

            for (double i = xStart; i <= xEnd; i += xStep) {
                sol = parser.calc(usr_funcs[idx].Func_expres, usr_funcs[idx].Func_expres.begin(),
                                  usr_funcs[idx].Func_expres.end(), 0.0, i);
                parser.round_nplaces(sol, 4);
                cout << usr_funcs[idx].Func_name.mText << "(" << i << ") = " << sol << "\n";
                usr_funcs[idx] = temp_func_copy;
            }
            return;
        } else {
            cout << "Keine Passende Funktion gefunden\n";
            return;
        }
    }
}

void Reader::nullstellen() {
    while (true) {
        vector<double> xNullStellenList;
        if (usr_funcs.empty()) {
            cout << "Sie haben noch keine Funktionen definiert!\n";
            return;
        }
        string c = string();
        show_all_funcs();
        cout << "\nIndex wählen: ";
        getline(cin, c);
        if (c.empty()) {
            cout << "\nKeine Eingabe erkannt";
            return;
        } else if (c == "back" || c == "break") { break; }
        Parser parser;
        int idx = stoi(c);
        if (idx < usr_funcs.size()) {
            cout << "\nNullstellen von " << usr_funcs[idx].Func_name.mText << "(" << usr_funcs[idx].Func_var_name.mText
                 << "):\n";
            nullStellenCalc(xNullStellenList, idx, -180, 180, 0.51);
            if (xNullStellenList.empty()) {
                cout << "\nKeine Nullstellen gefunden";
                return;
            }
            for (double stelle: xNullStellenList) {
                cout << usr_funcs[idx].Func_name.mText << "(" << stelle << ") = " << 0 << "\n";
            }
            return;
        } else { cout << "Keine passende Funktion gefunden\n"; }
    }
}

void Reader::nullStellenCalc(vector<double> &xNullStellen, int idx, double xStart = -100, double xEnd = 100,
                             double xStep = 1) {
    double sol = 0;
    double last_sol;
    Parser parser;
    Func temp_func_copy = usr_funcs[idx];

    for (double i = xStart; i <= xEnd; i += xStep) {
        last_sol = sol;
        sol = -0.00182347324128494756287562376;
        sol = parser.calc(usr_funcs[idx].Func_expres, usr_funcs[idx].Func_expres.begin(),
                          usr_funcs[idx].Func_expres.end(), 0.0, i);
        parser.round_nplaces(sol, 4);
        parser.round_nplaces(i, 4);
        usr_funcs[idx] = temp_func_copy;
        if (sol == 0) {
            xNullStellen.push_back(i);
            ++i;
        }
        if (sol > -0.005 && sol < 0.005) {
            xNullStellen.push_back(i);
            ++i;
        }
        if ((last_sol < 0 && sol > 0) || (last_sol > 0 && sol < 0)) {
            nullStellenCalc(xNullStellen, idx, i - xStep, i + xStep, xStep * 0.1);
        }
    }
}

void Reader::ableit() {
    while (true) {
        vector<double> xNullStellenList;
        if (usr_funcs.empty()) {
            cout << "Sie haben noch keine Funktionen definiert!\n";
            return;
        }
        string c = string();
        show_all_funcs();
        cout << "\nIndex wählen: ";
        getline(cin, c);
        if (c.empty()) {
            cout << "\nKeine Eingabe erkannt";
            return;
        } else if (c == "back" || c == "break") { break; }
        Parser parser;
        int idx = stoi(c);
        if (idx < usr_funcs.size()) {
            Func new_ableit;
            new_ableit = usr_funcs[idx];
            new_ableit.Func_name.mText.append(1, '`');
            int checkDuplicate = check(new_ableit.Func_name.mText, 3);
            if (checkDuplicate == -1) {
                new_ableit.Func_expres = parser.ableiten(new_ableit.Func_expres);
                usr_funcs.push_back(new_ableit);
                show_all_funcs();
            } else {
                show_all_funcs();
            }
            return;
        }
    }
}

void Reader::extrem() {
    while (true) {
        vector<double> xNullStellenList1;
        vector<double> xNullStellenList2;
        double checkTpHp = 0;
        double sol = 0;
        if (usr_funcs.empty()) {
            cout << "Sie haben noch keine Funktionen definiert!\n";
            return;
        }
        string c = string();
        show_all_funcs();
        cout << "\nIndex wählen: ";
        getline(cin, c);
        if (c.empty()) {
            cout << "\nKeine Eingabe erkannt";
            return;
        } else if (c == "back" || c == "break") { break; }
        Parser parser;
        int idx = stoi(c);
        if (idx < usr_funcs.size()) {
            const Func func0Copy = usr_funcs[idx];

            Func ableit1 = usr_funcs[idx];
            ableit1.Func_expres = parser.ableiten(usr_funcs[idx].Func_expres);
            ableit1.Func_name.mText.append(1, '`');
            usr_funcs.push_back(ableit1);

            Func ableit2 = ableit1;
            ableit2.Func_expres = parser.ableiten(ableit1.Func_expres);
            ableit2.Func_name.mText.append(1, '`');
            usr_funcs.push_back(ableit2);

            const Func func2Copy = ableit2;

            nullStellenCalc(xNullStellenList1, idx + 1, -180, 180, 0.51);
            if (xNullStellenList1.empty()) {
                cout << "\nKeine Nullstellen gefunden";
                return;
            }
            for (double stelle: xNullStellenList1) {
                sol = parser.calc(usr_funcs[idx].Func_expres, usr_funcs[idx].Func_expres.begin(),
                                  usr_funcs[idx].Func_expres.end(), 0.0, stelle);
                parser.round_nplaces(sol, 4);
                checkTpHp = parser.calc(ableit2.Func_expres, ableit2.Func_expres.begin(), ableit2.Func_expres.end(),
                                        checkTpHp, stelle);
                if (checkTpHp > 0) {
                    cout << "\nTP: " << "(" << stelle << "|" << sol << ")";
                } else {
                    cout << "\nHP: " << "(" << stelle << "|" << sol << ")";
                }
                usr_funcs[idx] = func0Copy;
                ableit2 = func2Copy;
            }
            usr_funcs.pop_back();
            usr_funcs.pop_back();
            cout << "\n";
            return;
        } else { cout << "Keine passende Funktion gefunden\n"; }
    }
}

void Reader::trass() {
    while (true) {
        vector<int> indxList; // gewählte indices
        vector<int> bedingList; // anzahl der Bedingungen zum index
        vector<double> xStelleList; // Verbindungsstellen der Funktionen
        if (usr_funcs.empty()) {
            cout << "Sie haben noch keine Funktionen definiert!\n";
            return;
        }
        string c = string();
        cout << "\nAnzahl: ";
        getline(cin, c);
        if (c.empty()) {
            cerr << "\nKeine Eingabe erkannt";
            return;
        } else if (c == "back" || c == "break") { break; }
        int nFuncs = stoi(c);
        for (int i = 0; i < nFuncs; ++i) {
            cout << "\n";
            show_all_funcs();
            cout << "\nIndex wählen: ";
            getline(cin, c);
            if (c.empty()) {
                cerr << "\nKeine Eingabe erkannt";
                return;
            } else if (c == "back" || c == "break") { break; }
            int idx = stoi(c);
            if (idx < usr_funcs.size()) {
                indxList.push_back(idx);

                cout << "\nAnzahl der Bedingungen: ";
                getline(cin, c);
                if (c.empty()) {
                    cerr << "\nKeine Eingabe erkannt";
                    return;
                } else if (c == "back" || c == "break") { break; }
                int nBedingung = stoi(c);
                if (nBedingung < 4 && nBedingung > 0) {
                    bedingList.push_back(nBedingung);

                    cout << "\nVerbindungsstelle: ";
                    getline(cin, c);
                    if (c.empty()) {
                        cerr << "\nKeine Eingabe erkannt";
                        return;
                    } else if (c == "back" || c == "break") { break; }
                    double xStelle = stod(c);

                    if (!xStelleList.empty()) {
                        if (xStelle <= xStelleList.back()) {
                            cout << "\nNur von Links nach Rechts!\n"
                                    "\nKehre zurück ins Hauptmenü!\n";
                            return;
                        }
                    }
                    xStelleList.push_back(xStelle);
                }
            }
        }
        if (indxList.size() > 1) {
            Tokenizer tokenizer;
            Parser parser;

            vector<Func> musterAbleitList;
            vector<vector<Func>> usrAbleitList;

            for (int i: indxList) {
                vector<Func> first;
                auto copy = usr_funcs[i];
                first.push_back(copy);
                usrAbleitList.push_back(first);
            }

            // Muster Polynom aufstellen:
            int polyGrad = 0;
            for (int currBeding: bedingList) {
                polyGrad += currBeding;
            }
            cout << "\nTrasse benennen [f]: ";
            getline(cin, c);
            if (c.empty()) {
                cerr << "\nKeine Eingabe erkannt";
                return;
            } else if (c == "back" || c == "break") { break; }

            string polyMuster = c;
            polyMuster += "(x) = ";

            for (int j = 0; j < polyGrad; ++j) {
                polyMuster += std::to_string(1);
                if (j < polyGrad - 1) {
                    polyMuster += 'x';
                    if (j < polyGrad - 2) {
                        polyMuster += '^';
                        polyMuster += std::to_string(polyGrad - 1 - j);
                    }
                    polyMuster += '+';
                }
            }
            vector<Token> tokens = tokenizer.parse(polyMuster);
            if (check(tokens.front().mText, 1) == -2) {
                cout << "\nDieser Funktionsname ist bereits belegt\nAlte Funktion löschen?\n[ja/nein]:";
                getline(cin, c);
                if (c == "ja") { check(tokens.front().mText, 2); } else return;
                cout << "\nNeue Definiton von [" << tokens.front().mText << "] speichern?\n[ja/nein]:";
                getline(cin, c);
                if (c == "nein") { return; }
            }
            Func startPolynom = parser.declare(tokens, tokens.begin(), tokens.end());
            usr_funcs.push_back(startPolynom);
            musterAbleitList.push_back(startPolynom);
            show_all_funcs();

            vector<vector<double>> lgs;
            for (int i = 0; i < 3; ++i) {
                switch (i) {
                    case 0:
                        cout << "\nSprungfreiheit:\n";
                        break;
                    case 1:
                        cout << "\nKnickfreiheit:\n";
                        if (musterAbleitList.size() < 2) {
                            Func tempCopy;
                            tempCopy = musterAbleitList.back();
                            tempCopy.Func_name.mText += '`';
                            tempCopy.Func_expres = parser.ableiten(tempCopy.Func_expres);
                            musterAbleitList.push_back(tempCopy);
                        }
                        break;
                    case 2:
                        cout << "\nRuckfreiheit:\n";
                        if (musterAbleitList.size() < 3) {
                            Func tempCopy;
                            tempCopy.Func_expres = musterAbleitList.back().Func_expres;
                            tempCopy.Func_name.mText += '`';
                            tempCopy.Func_expres = parser.ableiten(tempCopy.Func_expres);
                            musterAbleitList.push_back(tempCopy);
                        }
                        break;
                    default:
                        break;
                }
                for (int j = 0; j < indxList.size(); ++j) {
                    if (bedingList[j] > i) {
                        vector<double> currEqn;

                        if (i >= usrAbleitList[j].size()) {
                            Func tempCopy;
                            tempCopy = usrAbleitList[j].back();
                            tempCopy.Func_name.mText += '`';
                            tempCopy.Func_expres = parser.ableiten(tempCopy.Func_expres);
                            usrAbleitList[j].push_back(tempCopy);
                        }
                        Func musterCopy = musterAbleitList[i];
                        parser.makeEqn(musterAbleitList[i].Func_expres, musterAbleitList[i].Func_expres.begin(),
                                       musterAbleitList[i].Func_expres.end(), currEqn, xStelleList[j]);
                        musterAbleitList[i] = musterCopy;

                        Func usrCopy = usrAbleitList[j][i];
                        double yWert = parser.calc(usrAbleitList[j][i].Func_expres,
                                                   usrAbleitList[j][i].Func_expres.begin(),
                                                   usrAbleitList[j][i].Func_expres.end(), 0.0, xStelleList[j]);
                        usrAbleitList[j][i] = usrCopy;
                        cout << "\n" << usr_funcs[indxList[j]].Func_name.mText << "(" << xStelleList[j] << ") = "
                             << yWert;
                        while (currEqn.size() < polyGrad) {
                            currEqn.push_back(0);
                        }
                        currEqn.push_back(yWert);
                        lgs.push_back(currEqn);
                    }
                }
            }
            ostringstream streamObj;
            N::Gauss gauss;
            lgs = gauss.gauss(lgs);

            vector<double> coefWerte(lgs.size(), 0);
            for (int i = 0; i < lgs.size(); ++i) {
                coefWerte[i] = (lgs[i][lgs.size()]);
            }
            string finalTstring;
            finalTstring += usr_funcs.back().Func_name.mText + "(x) = ";
            for (int i = 0; i < coefWerte.size(); ++i) {
                streamObj << coefWerte[i];
                finalTstring += streamObj.str();
                streamObj.str("");
                if (i < polyGrad - 1) {
                    finalTstring += 'x';
                }
                if (i < polyGrad - 2) {
                    finalTstring += '^';
                    finalTstring += to_string(polyGrad - 1 - i);
                }
                if (i < polyGrad - 1) {
                    if (coefWerte[i + 1] >= 0) {
                        finalTstring += '+';
                    }
                }
            }
            usr_funcs.pop_back();
            vector<Token> finalTrassToken = tokenizer.parse(finalTstring);
            Func finalTrassFunc = parser.declare(finalTrassToken, finalTrassToken.begin(), finalTrassToken.end());
            usr_funcs.push_back(finalTrassFunc);
            show_all_funcs();
            break;
        }
    }
}

void Reader::show_all_funcs() {
    if (usr_funcs.empty()) {
        cout << "\nSie haben noch keine Funktionen definiert\n";
        return;
    }
    int idx = 0;
    for (const Func &currFunc: usr_funcs) { // Loopt durch alle definierten Funktionen
        cout << "\n[" << idx << "] " << currFunc.Func_name.mText << "(" << currFunc.Func_var_name.mText << ") = ";
        ++idx;
        for (const Token &currToken: currFunc.Func_expres) {//Loopt durch alle Teile des ausgewählten Ausdrucks
            cout << currToken.mText;
        }
    }
    cout << "\n";
}

void Reader::delAll() {
    usr_funcs.clear();
    if (usr_funcs.empty()) {
        cout << "\nErfolgreich alle Funktionen gelöscht!\n";
    }
}

void Reader::plot() {
    while (true) {
        vector<int> indxList;
        if (usr_funcs.empty()) {
            cout << "Sie haben noch keine Funktionen definiert!\n";
            return;
        }
        string c = string();
        show_all_funcs();
        cout << "\nAnzahl: ";
        getline(cin, c);
        if (c.empty()) {
            cerr << "\nKeine Eingabe erkannt";
            return;
        } else if (c == "back" || c == "break") { break; }
        int amount = stoi(c);
        for (int i = 0; i < amount; ++i) {
            cout << "\nIndex wählen: ";
            getline(cin, c);
            if (c.empty()) {
                cerr << "\nKeine Eingabe erkannt";
                return;
            } else if (c == "back" || c == "break") { break; }
            int idx = stoi(c);
            if (idx < usr_funcs.size()) {
                indxList.push_back(idx);
            }
        }
        if (!indxList.empty()) {
            Parser parser;
            double sol = 0;
            cout << "\nStartwert?: ";
            getline(cin, c);
            double xStart = stod(c);

            cout << "\nEndwert?: ";
            getline(cin, c);
            double xEnd = stod(c);

            cout << "\nSchrittweite?: ";
            getline(cin, c);
            double xStep = stod(c);

            if (!os.is_open()) {
                os.open(plotFileName);
            }
            if (!gp.is_open()) {
                gp.open(plotConfigName);
            }

            os.clear();
            gp.clear();
            os.flush();
            gp.flush();

            for (double i = xStart; i <= xEnd; i += xStep) {
                os << i;
                // Rechnet alle Werte im geforderten Bereich aus
                for (int currIdx: indxList) {
                    os << " ";
                    Func temp_func_copy = usr_funcs[currIdx];
                    sol = parser.calc(usr_funcs[currIdx].Func_expres, usr_funcs[currIdx].Func_expres.begin(),
                                      usr_funcs[currIdx].Func_expres.end(), 0.0, i);
                    parser.round_nplaces(sol, 4);
                    usr_funcs[currIdx] = temp_func_copy;
                    os << sol;
                }
                os << "\n";
                // Schreibt alle x werte und alle y werte in "currplot.txt"
            }
            os.close();

            // Wrtie config file:
            gp << "set zeroaxis linetype 1 linecolor rgb '#222222'\n"
               << "set xtics axis\n"
               << "set ytics axis\n"
               << "set border 0\n"
               << "set style line 12 lc rgb '#2D2D2A' lt 0 lw 1\n"
               << "set grid back ls 12\n"
               << "set xtics add ('' 0) font 'Times New Roman,12'\n"
               << "set ytics add ('' 0) font 'Times New Roman,12'\n"
               << "set label '{/:Italic x}' at first 0, graph 0 offset char 0, char -1 center font 'Times New Roman,14'\n"
               << "set label '{/:Italic y}' at graph 0, first 0 offset char -1, char 0 right font 'Times New Roman,14'\n"
               << "set lmargin 3\n"
               << "set bmargin 2\n"
               << "set offset graph 0,0.25, 0.25, 0.25\n"
               << "set term wxt size 1300,1200 lw 2\n"
               << "plot '" << plotFileName << "' using 1:2 title '" << usr_funcs[0].Func_name.mText << "("
               << usr_funcs[0].Func_var_name.mText << ")' w l lt 1 lc rgb 'red' lw 1.5";

            for (int i = 1; i < indxList.size(); ++i) {
                gp << ",\\\n'" << plotFileName << "' using 1:" << i + 2 << " title '" << usr_funcs[i].Func_name.mText
                   << "("
                   << usr_funcs[i].Func_var_name.mText << ")' w l lt 1 lc rgb '#440154' lw 1.5";
            }
            gp << "\n";
            gp.close();
            indxList.clear();

            string cmd = "gnuplot -p " + plotConfigName;
            int ret = system(cmd.c_str());
            if (ret != 0)cerr << "\nFehler beim plotten!\n";
            gp.clear();
            gp.flush();
            os.clear();
            os.flush();
            return;
        } else {
            cerr << "Keine Passende Funktion gefunden\n";
            return;
        }
    }
}
