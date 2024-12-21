#include <iostream>  
#include "solver.h"

using namespace std;

int main() {
    Instance instance;
    instance.loadFromFile("16.txt");
    //instance.displayData();
    Solver solver(instance);
    solver.solve();
    solver.displaySolution();

    return 0;
}

