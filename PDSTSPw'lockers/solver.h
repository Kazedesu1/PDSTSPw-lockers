#ifndef SOLVER_H
#define SOLVER_H

#include "instance.h"
#include <vector>
#include <set>

struct Drones {
    double total_time = 0;
    std::vector<int> route;
    std::vector<int> Customersroute;
    double Dronecost = 0;
    Drones() : total_time(0), Customersroute() , route() {}
};

class Solver {
public:
    Solver(const Instance& instance);
    void solve();
    void displaySolution() const;
    double Truckcost = 0;
    const Instance& instance;
    std::vector<Drones> drones;
    std::vector<int> truckCustomersRoute = { -1, -1 };
    std::vector<int> truckRoute = { 0, 0 };

    double tinhCostTruckTang(const std::vector<int>& route, const std::vector<std::vector<double>>& Ctruck, int node, int insertPos, int discount) const;
    double tinhCostTruck(const std::vector<int>& route, const std::vector<std::vector<double>>& Ctruck) const;
    double tinhCostDrone(const std::vector<int>& route, const std::vector<double>& Cdrone) const;
};

#endif // SOLVER_H
