#include "cramer.h"

int main() {
 
    matrix eq;
    
    eq = generateRandomMatrix(3);
    printMatrix(eq);
    cout << solveCramer(eq) << '\n';

    eq = generateRandomMatrix(5);
    printMatrix(eq);
    cout << solveCramer(eq) << '\n';


    eq = generateRandomMatrix(7);
    printMatrix(eq);
    cout << solveCramer(eq) << '\n';


    for (int i = 9; i < 30; i++) {
        eq = generateRandomMatrix(i);
        printMatrix(eq);
        cout << solveCramer(eq) << '\n';
    }



    return 0;
}
