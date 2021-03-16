#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <thread>
#include <time.h>


using namespace std;

template<typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
    auto it = v.cbegin();
    auto end = v.cend();

    os << '[';
    if (it != end) {
        os << *it++;
    }
    while (it != end) {
        os << ", " << *it++;
    }

    return os << ']';
}

const double SMALL = 1.0E-30;

using matrix = vector<vector<double>>;

double determinant(matrix A) {
    int n = A.size();

    double det = 1;

    // Row operations for i = 0, ,,,, n - 2 (n-1 not needed)
    for (int i = 0; i < n - 1; i++) {
        // Частичный поворот: найдите строку r ниже с наибольшим элементом в столбце i
        int r = i;
        double maxA = abs(A[i][i]);
        for (int k = i + 1; k < n; k++) {
            double val = abs(A[k][i]);
            if (val > maxA) {
                r = k;
                maxA = val;
            }
        }
        if (r != i) {
            for (int j = i; j < n; j++) swap(A[i][j], A[r][j]);
            det = -det;
        }

        // Рядные операции для получения верхнетреугольной формы
        double pivot = A[i][i];
        if (abs(pivot) < SMALL) return 0.0;              // Сингулярная матрица

        for (int r = i + 1; r < n; r++)                    // На нижних рядах
        {
            double multiple = A[r][i] / pivot;                // Multiple of row i to clear element in ith column
            for (int j = i; j < n; j++) A[r][j] -= multiple * A[i][j];
        }
        det *= pivot;                                        // Determinant - произведение диагонали
    }

    det *= A[n - 1][n - 1];

    return det;
}

matrix insertInTerms(matrix &sourceMatrix, matrix tmpMatrix, vector<double> ins, int at) {
    for (int i = 0; i < sourceMatrix.size(); i++) {
        tmpMatrix[i][at] = ins[i];
    }
    return tmpMatrix;
}

void solve(vector<double> &answer, matrix &A, int at, vector<double> &b, double &det) {
    vector<vector<double>> tmpMatrix = insertInTerms(A, A, b, at);
    answer[at] = determinant(tmpMatrix) / det;
}

vector<double> solveCramer(matrix &equations) {
    int size = equations.size();

    vector<vector<double>> matrix(size);
    vector<double> column(size);
    for (int r = 0; r < size; ++r) {
        column[r] = equations[r][size];
        matrix[r].resize(size);
        for (int c = 0; c < size; ++c) {
            matrix[r][c] = equations[r][c];
        }
    }

    double det = determinant(matrix);
    if (det == 0.0) {
        throw std::runtime_error("The determinant is zero.");
    }

    vector<double> answer(matrix.size());
    vector<std::thread> vecOfThreads;
    for (int i = 0; i < matrix.size(); i++)
    {
        std::thread th(solve, ref(answer), ref(matrix), i, ref(column), ref(det));
        vecOfThreads.push_back(move(th));
    }

    for (std::thread & th : vecOfThreads)
    {
        if (th.joinable())
            th.join();
    }

    return answer;
}

