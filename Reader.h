#pragma once

#include <vector>
#include <string>
#include <map>

namespace simpleparser {
    using namespace std;

    enum Actions {
        ACTION_INVALID,
        HELP,
        EXIT,
        DEBUG,
        PRINT,
        CALC,
        SOLVE,
        DECLARE,
        ABLEIT,
        TABLE,
        TRASS,
        NULLSTELLEN,
        EXTREM,
        SHOW_ALL,
        DELETE_ALL,
        PLOT
    };

    static const map<std::string, Actions> ActionsStrings{
            {"help",   HELP},
            {"exit",   EXIT},
            {"debug",  DEBUG},
            {"print",  PRINT},
            {"calc",   CALC},
            {"solve",  SOLVE},
            {"def",    DECLARE},
            {"ableit", ABLEIT},
            {"extrem", EXTREM},
            {"table",  TABLE},
            {"trass",  TRASS},
            {"null",   NULLSTELLEN},
            {"list",   SHOW_ALL},
            {"delAll", DELETE_ALL},
            {"plot",   PLOT}
    };

    class Reader {
    public:

        static void listen();

        static Actions resolveActions(const std::string &input);

        static void debug(int mode);

        static int check(const string &func_name, int mode);

        static void show_all_funcs();

        static void declare();

        static void solve();

        static void table();

        static void
        nullStellenCalc(vector<double> &xNullStellenList, int idx, double xStart, double xEnd, double xStep);

        static void nullstellen();

        static void extrem();

        static void trass();

        static void calc();

        static void ableit();

        static void delAll();

        static void plot();

    };
}