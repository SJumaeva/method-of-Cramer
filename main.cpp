#include "cramer.h"

int main() {
 
    matrix eq;
    
    eq = generateRandomMatrix(1);
    printMatrix(eq);
    cout << solveCramer(eq) << '\n';

    eq = generateRandomMatrix(2);
    printMatrix(eq);
    cout << solveCramer(eq) << '\n';


    eq = generateRandomMatrix(4);
    printMatrix(eq);
    cout << solveCramer(eq) << '\n';

    eq = generateRandomMatrix(8);
    printMatrix(eq);
    cout << solveCramer(eq) << '\n';
 
 
    for (int i = 9; i < 30; i++) {
        eq = generateRandomMatrix(i);
        printMatrix(eq);
        cout << solveCramer(eq) << '\n';
    }



    return 0;
}
