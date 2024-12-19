#include <iostream>  
#include "solver.h"

using namespace std;

int main() {
    Instance instance;
    if (!instance.loadFromFile("1.txt")) {
        cerr << "Failed to load data.\n";
        return 1;
    }
    //instance.displayData();
    Solver solver(instance);
    solver.solve();
    solver.displaySolution();

    return 0;
}

