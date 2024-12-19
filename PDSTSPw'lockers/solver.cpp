#include "solver.h"
#include <iostream>
#include <algorithm>
#include <limits>
using namespace std;

Solver::Solver(const Instance& inst) : instance(inst) {}

void Solver::solve() {
    drones.resize(instance.num_drones); 
    vector<int> remainingCustomers = instance.C;

    while (!remainingCustomers.empty()) {
        int bestTruckCustomer = -1;
        int bestTruckNode = -1;
        double minTruckCostIncrease = numeric_limits<double>::infinity();
        int bestTruckPos = -1;

        // tìm node có cost thấp nhất của truck
        for (int cluster : remainingCustomers) {
            for (int j = 1; j < truckRoute.size(); ++j) {
                //duyet qua tung node cua clusters
                for (int i = 0; i < instance.clusters[cluster].nodes.size(); ++i) {
                    double costIncrease = tinhCostTruckTang(
                        truckRoute, instance.Ctruck, instance.clusters[cluster].nodes[i], j, instance.clusters[cluster].discounts[i]);
                    if (costIncrease < minTruckCostIncrease) {
                        minTruckCostIncrease = costIncrease;
                        bestTruckNode = instance.clusters[cluster].nodes[i];
                        bestTruckPos = j;
                        bestTruckCustomer = cluster;
                    }
                }
            }
        }

        // tìm node có cost thấp nhất của drone
        int bestDroneCustomer = -1;
        int bestDrone = -1;
        int bestDroneNode = -1;
        double minDroneCost = numeric_limits<double>::infinity();

        for (int i = 0; i < drones.size(); ++i) {
            for (int cluster : remainingCustomers) {
                if (find(instance.Cf.begin(), instance.Cf.end(), cluster) != instance.Cf.end()) {
                    for (int j = 0; j < instance.clusters[cluster].nodes.size(); ++j) {
                        //rang buoc khoang cach bay
                        if (instance.nodes[instance.clusters[cluster].nodes[j]].chargeStation == 1 ||
                            instance.nodes[instance.clusters[cluster].nodes[j]].euclidDist > instance.max_fly_distance / 2) {
                            double droneCost = instance.Cdrone[instance.clusters[cluster].nodes[j]];
                            if (droneCost < minDroneCost &&
                                drones[i].total_time + instance.Tdrone[instance.clusters[cluster].nodes[j]] <= instance.max_working_time) {
                                bestDroneCustomer = cluster;
                                minDroneCost = droneCost;
                                bestDrone = i;
                                bestDroneNode = instance.clusters[cluster].nodes[j];
                            }
                        }
                    }
                }
            }
        }

        // lựa chọn drone hay truck
        if (bestDroneNode != -1 && minDroneCost <= minTruckCostIncrease) {
            drones[bestDrone].route.push_back(bestDroneNode);
            drones[bestDrone].Customersroute.push_back(bestDroneCustomer);
            drones[bestDrone].total_time += instance.Tdrone[bestDroneNode];
            remainingCustomers.erase(remove(remainingCustomers.begin(), remainingCustomers.end(), bestDroneCustomer), remainingCustomers.end());
        }
        else if (bestTruckNode != -1) {
            truckRoute.insert(truckRoute.begin() + bestTruckPos, bestTruckNode);
            truckCustomersRoute.insert(truckCustomersRoute.begin() + bestTruckPos, bestTruckCustomer);
            remainingCustomers.erase(remove(remainingCustomers.begin(), remainingCustomers.end(), bestTruckCustomer), remainingCustomers.end());
        }
    }
}

// hàm tính cost tăng lên khi thêm 1 node vào vị trí bất kì trong route của truck
double Solver::tinhCostTruckTang(const vector<int>& route, const vector<vector<double>>& Ctruck, int node, int insertPos, int discount) const {
    int prevNode = route[insertPos - 1];
    int nextNode = route[insertPos];

    double totalCost = -Ctruck[prevNode][nextNode] +
        Ctruck[prevNode][node] +
        Ctruck[node][nextNode] +
        instance.discount_cost_factor * discount;
    return totalCost;
}

//hàm tính tổng cost truck và drone 
double Solver::tinhCostTruck(const vector<int>& route, const vector<vector<double>>& Ctruck) const {
    double totalCost = 0;
    int currentNode = 0;

    for (int nextNode : route) {
        totalCost += Ctruck[currentNode][nextNode];
        currentNode = nextNode;
    }

    totalCost += Ctruck[currentNode][0];
    return totalCost;
}

double Solver::tinhCostDrone(const vector<int>& route, const vector<double>& Cdrone) const {
    double cost = 0;
    for (int node : route) {
        cost += Cdrone[node];
    }
    return cost;
}

// hiển thị kết quả 
void Solver::displaySolution() const {
    double totalCost = 0;

    cout << "\nTruck                   Route: ";
    for (int node : truckRoute) {
        cout << node << " ";
    }
    /*cout << "\nCluster Route: ";
    for (int node : truckCustomersRoute) {
        cout << node << " ";
    }*/
    double truckCost = tinhCostTruck(truckRoute, instance.Ctruck);
    totalCost += truckCost;
    cout << "\tTotal Truck Cost: " << truckCost << "\n";

    for (int i = 0; i < drones.size(); ++i) {
        double droneCost = tinhCostDrone(drones[i].route, instance.Cdrone);
        totalCost += droneCost;
        cout << "Drone " << i << "\tTime: " << drones[i].total_time << "\tRoute: ";
        for (int node : drones[i].route) {
            cout << node << " ";
        }
        /*cout << "\nCluster Route: ";
        for (int node : drones[i].Customersroute) {
            cout << node << " ";
        }*/
        cout << "\tTotal Drone Cost: " << droneCost << endl;
    }

    cout << "\nTotal Cost: " << totalCost << endl;
}
