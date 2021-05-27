#include "cramer.h"


int main() {
 
    matrix eq;
    
    eq = generateRandomMatrix(24);
    printMatrix(eq);

    cout << solveCramer(eq, 1) << '\n';
    cout << solveCramer(eq, 2) << '\n';
    cout << solveCramer(eq, 4) << '\n';
    cout << solveCramer(eq, 8) << '\n';

    return 0;
}
