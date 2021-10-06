#include "gauss_ext.h"
#include <iostream>
#include <vector>

using namespace N;
using namespace std;

void print_banner() {
    cout
            << "\n""      ***            * *        *           *          **          **     \n""    **   **         *   *       *           *        **  *       **  *    \n""   *       *       *     *      *           *       **         **          **\n"" *                *        *    *           *      **         **          *??*\n""*                *         *    *           *        **         **         **\n""*       **       *=========*    *           *         **         **    \n""*          *     *         *    *           *           *          *       **\n""*           *    *         *    *           *           *          *      *??*\n""*          **    *         *     *         *           *          *        **\n"" **       **     *         *      *       *     *   **      *  **         \n""   *******       *         *        *****        ***         ***          \n""\n""================================================================================\n""\n";
}

void print_matrix(vector<vector<double>> &matrix) {
    // output each line of the matrix
    for (int i = 0; i < matrix.size(); i++) {
        cout << "\n" << i + 1 << ": ";
        for (int j = 0; j < matrix[i].size(); j++) {
            cout << matrix[i][j] << " ";
        }
    }
    cout << "\n\n\n";
}

void print_sol(vector<vector<double>> &matrix) {
    for (int i = 0; i < matrix.size(); i++) {
        cout << "x" << i + 1 << ": " << matrix[i][matrix.size()] << "\n";
    }
}


int current_column(vector<vector<double>> &matrix, int iter) {
    /*
    Diese Funktion returned den index der ersten Spalte von links,
    in der mindestens ein Wert ungleich Null ist.
    */
    int idx = iter;
    for (int i = 0; i < matrix.size(); i++) {
        if (matrix[i][iter] != 0)
            return idx;
    }
    return current_column(matrix, iter + 1);
}

vector<vector<double>> swap_rows(vector<vector<double>> &matrix, int idx, int iter) {
    if (matrix[iter][idx] == 0) {
        for (int i = 0; i < matrix[iter].size(); ++i) {
            if (matrix[i][idx] != 0) {
                vector<double> first = matrix[i];
                vector<double> second = matrix[0];

                matrix[i] = second;
                matrix[0] = first;

                return matrix;
            }
        }
    }
    return matrix;
}

vector<vector<double>> make_key(vector<vector<double>> &matrix, int iter) {
    /*
    Diese Funktion dividiert die Nte Zeile durch das
    nun oberste Element der gewählten Spalte.
    int iter ist die gewählte Spalte
    */
    vector<vector<double>> matrix_out(matrix.size(), vector<double>(matrix[0].size()));
    matrix_out = matrix;
    for (int i = 0; i < matrix[0].size(); i++) {
        if (matrix[iter][i] == 0)
            matrix[iter][i] = 0;
        else {
            matrix_out[iter][i] = matrix[iter][i] / matrix[iter][iter];
        }
    }
    return matrix_out;
}

vector<vector<double>> elemin_down(vector<vector<double>> &matrix, int iter) {
    vector<vector<double>> matrix_out(matrix.size(), vector<double>(matrix[0].size()));
    matrix_out = matrix;

    for (int i = 0; i < matrix.size(); i++) {
        if (i != iter) {
            for (int j = 0; j < matrix[i].size(); j++) {
                matrix_out[i][j] = matrix_out[i][j] - (matrix[iter][j] * matrix[i][iter]);
            }
        } else {
            matrix_out[i] = matrix[i];
        }
    }
    return matrix_out;
}

vector<vector<double>> Gauss::gauss(vector<vector<double>> &matrix) {
    //print_banner();
    //print_matrix(matrix);
    for (int i = 0; i < matrix.size(); i++) {
        int idx = current_column(matrix, i);
        matrix = swap_rows(matrix, idx, i);
        matrix = make_key(matrix, i);
        matrix = elemin_down(matrix, i);
        //print_matrix(matrix);
    }
    cout << "\n\n";
    print_sol(matrix);
    return matrix;
}
