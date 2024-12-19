#ifndef INSTANCE_H
#define INSTANCE_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

struct Node {
    int nodeID;
    int capacity;
    bool chargeStation;
    double euclidDist;
};

struct Cluster {
    int clusterID;                  
    double weight;                  
    std::vector<int> nodes;         
    std::vector<int> discounts;     

    void displayCluster() const;    
};

class Instance {
public:
    int num_nodes;
    int num_clusters;
    int num_drones;
    int num_lockers;
    double v_truck;
    double v_drone;
    double max_fly_distance;
    double max_working_time;
    double capacity_drone;
    double time_swap_battery;
    double time_service;
    double truck_cost_factor;
    double drone_cost_factor;
    double discount_cost_factor;

    std::vector<int> C;
    std::vector<int> Cf;
    std::vector<Node> nodes;
    std::vector<Cluster> clusters;
    std::vector<std::vector<double>> truck_distance_matrix;
    std::vector<std::vector<double>> Ttruck;
    std::vector<std::vector<double>> Ctruck;
    std::vector<double> Tdrone;
    std::vector<double> Cdrone;
    bool loadFromFile(const std::string& filename = "2.txt");
    void displayData() const;
    std::vector<int> getDiscountsByCluster(int clusterID) const;
};

#endif // INSTANCE_H