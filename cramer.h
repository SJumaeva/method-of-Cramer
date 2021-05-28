#include <iostream>
#include <sstream> 
#include <vector>
#include <cmath>
#include <algorithm>
#include <thread>
#include <time.h>
#include <ctime>
#include <atomic>
#include <chrono>
#include <mutex>
#include <random>

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

template<typename Vector>
auto split_vector(const Vector& v, unsigned number_lines) {
  using Iterator = typename Vector::const_iterator;
  std::vector<Vector> rtn;
  Iterator it = v.cbegin();
  const Iterator end = v.cend();

  while (it != end) {
    Vector v;
    std::back_insert_iterator<Vector> inserter(v);
    const auto num_to_copy = std::min(static_cast<unsigned>(
        std::distance(it, end)), number_lines);
    std::copy(it, it + num_to_copy, inserter);
    rtn.push_back(std::move(v));
    std::advance(it, num_to_copy);
  }

  return rtn;
}

// stopwatch. Returns time in seconds
class timer {
public:
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
    timer() : lastTime(std::chrono::high_resolution_clock::now()) {}
    inline double elapsed() {
        std::chrono::time_point<std::chrono::high_resolution_clock> thisTime=std::chrono::high_resolution_clock::now();
        double deltaTime = std::chrono::duration<double>(thisTime-lastTime).count();
        lastTime = thisTime;
        return deltaTime;
    }
};


/** Thread safe cout class
  * Exemple of use:
  *    PrintThread{} << "Hello world!" << std::endl;
  */
class PrintThread: public std::ostringstream
{
public:
    PrintThread() = default;

    ~PrintThread()
    {
        std::lock_guard<std::mutex> guard(_mutexPrint);
        std::cout << this->str();
    }

private:
    static std::mutex _mutexPrint;
};

std::mutex PrintThread::_mutexPrint{};

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
        if (abs(pivot) < SMALL) {
            PrintThread{} << "singularity" << std::endl;
            return 0.0;
        }              // Сингулярная матрица

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


void solveA(vector<vector<int>> &workArray, int workArrayAt, vector<double> &answer, matrix &matrix,  vector<double> &column, double &det) {
    timer stopwatch;
    auto arr = workArray[workArrayAt];

    for (int i = 0; i < arr.size(); i++) {
        solve(answer, matrix, arr[i], column, det);
    }
    
    double elapsedTime = stopwatch.elapsed();
    PrintThread{} << "thread[" << workArrayAt+1 << "] = " << elapsedTime << " sec" << std::endl;
}

vector<double> solveCramer(matrix &equations, const int& threadNum) {
    PrintThread{} << "------------------" << std::endl;
    timer stopwatch;
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
    vector<int> tmpArray;
    
    // vector1.insert( vector1.end(), vector2.begin(), vector2.end() );
    for (int i = 0; i < matrix.size(); i++)
    {   
        tmpArray.push_back(i);
    }


    cout << "threadNum: " << threadNum << endl;

    if (threadNum == 1 && (matrix.size() < threadNum)) {
        for (int i = 0; i < matrix.size(); i++) {
            solve(answer, matrix, i, column, det);
        }            
    } else if (threadNum >= 1) {
        // cout << "tmpArray:: " << tmpArray << endl;
        auto workerArr = split_vector(tmpArray, (matrix.size() / threadNum));
        // cout << "workArr:: " << workerArr << endl; 
        vector<std::thread> vecOfThreads;
        for (int i = 0; i < workerArr.size(); i++) {
            std::thread th(solveA, ref(workerArr), i, ref(answer), ref(matrix), ref(column), ref(det));
            vecOfThreads.push_back(move(th));
        }

        for (std::thread & th : vecOfThreads)
        {
            if (th.joinable())
                th.join();
        }
    }

    double elapsedTime = stopwatch.elapsed();

    PrintThread{} << "overall elapsed time of equation: " << elapsedTime << " sec" << std::endl;
    return answer;
}

matrix generateRandomMatrix(const int& size) {
    matrix eq;

    // random sleep для srand(clock())
    std::mt19937_64 eng{std::random_device{}()};  // 
    std::uniform_int_distribution<> dist{10, 100};
    std::this_thread::sleep_for(std::chrono::milliseconds{dist(eng)});

    srand ( clock() );  
    for (int i = 0; i < size; i++) {
        vector<double> vecOfRandomNums(size);
        generate(vecOfRandomNums.begin(), vecOfRandomNums.end(), []() {
            return rand() % 100;
        });

        eq.push_back(vecOfRandomNums);
    }
    

    return eq;
}

void printMatrix(matrix& eq) {
    cout << "-------------" << endl;
    
    for (int i = 0; i < eq.size(); i++) {
        for (int j = 0; j < eq.size(); j++) {
            cout << eq[i][j] << ' ';
        }

        cout << "" << endl;
    }
}
