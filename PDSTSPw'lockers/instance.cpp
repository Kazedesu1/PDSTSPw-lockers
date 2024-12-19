#include "instance.h"
using namespace std;

// hiển thị thông tin các cụm
void Cluster::displayCluster() const {
    cout << "Cluster ID: " << clusterID << "\n";
    cout << "Weight: " << weight << "\n";
    cout << "Nodes: ";
    for (int node : nodes) {
        cout << node << " ";
    }
    cout << "\nDiscounts: ";
    for (int discount : discounts) {
        cout << discount << " ";
    }
    cout << "\n\n";
}

// đọc instance từ file
bool Instance::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filename << "\n";
        return false;
    }

    string line;

    while (getline(file, line)) {
        if (line.find("num_nodes") != string::npos) {
            sscanf_s(line.c_str(), "num_nodes, %d", &num_nodes);
        }
        else if (line.find("num_clusters") != string::npos) {
            sscanf_s(line.c_str(), "num_clusters, %d", &num_clusters);
        }
        else if (line.find("num_drone") != string::npos) {
            sscanf_s(line.c_str(), "num_drone, %d", &num_drones);
        }
        else if (line.find("num_lockers") != string::npos) {
            sscanf_s(line.c_str(), "num_lockers, %d", &num_lockers);
        }
        else if (line.find("v_truck(km/h)") != string::npos) {
            sscanf_s(line.c_str(), "v_truck(km/h), %lf", &v_truck);
        }
        else if (line.find("v_drone(km/h)") != string::npos) {
            sscanf_s(line.c_str(), "v_drone(km/h), %lf", &v_drone);
        }
        else if (line.find("max_fly_distance(km)") != string::npos) {
            sscanf_s(line.c_str(), "max_fly_distance(km), %lf", &max_fly_distance);
        }
        else if (line.find("max_working_time(hour)") != string::npos) {
            sscanf_s(line.c_str(), "max_working_time(hour), %lf", &max_working_time);
        }
        else if (line.find("capacity_drone(kg)") != string::npos) {
            sscanf_s(line.c_str(), "capacity_drone(kg), %lf", &capacity_drone);
        }
        else if (line.find("time_swap_battery(hour)") != string::npos) {
            sscanf_s(line.c_str(), "time_swap_battery(hour), %lf", &time_swap_battery);
        }
        else if (line.find("time_service(hour)") != string::npos) {
            sscanf_s(line.c_str(), "time_service(hour), %lf", &time_service);
        }
        else if (line.find("truck_cost_factor") != string::npos) {
            sscanf_s(line.c_str(), "truck_cost_factor, %lf", &truck_cost_factor);
        }
        else if (line.find("drone_cost_factor") != string::npos) {
            sscanf_s(line.c_str(), "drone_cost_factor, %lf", &drone_cost_factor);
        }
        else if (line.find("discount_cost_factor") != string::npos) {
            sscanf_s(line.c_str(), "discount_cost_factor, %lf", &discount_cost_factor);
        }
        else if (line.find("---------- nodes -------------") != string::npos) {
            break;
        }
    }

    // đọc nodes
    while (getline(file, line)) {
        if (line.find("---------- clusters -------------") != string::npos) {
            break;
        }
        if (line.empty() || line[0] == 'n') continue;

        Node node;
        sscanf_s(line.c_str(), "%d %d %d %lf", &node.nodeID, &node.capacity, &node.chargeStation, &node.euclidDist);
        nodes.push_back(node);
    }

    // đọc clusters
    while (std::getline(file, line)) {
        if (line.find("---------- truck distance matrix -------------") != std::string::npos) {
            break;
        }
        if (line.empty() || line[0] == '#' || line.find("w") != std::string::npos) continue;

        Cluster cluster;
        std::istringstream iss(line);
        iss >> cluster.clusterID >> cluster.weight;

        std::vector<int> nodes;
        std::vector<int> discounts;

        std::string temp;
        while (iss >> temp && temp != "0" && temp.find_first_not_of("0123456789") == std::string::npos) {
            nodes.push_back(std::stoi(temp));
        }

        if (temp == "0" || temp.find_first_not_of("0123456789") == std::string::npos) {
            discounts.push_back(std::stoi(temp));
            int discount;
            while (iss >> discount) {
                discounts.push_back(discount);
            }
        }

        cluster.nodes = nodes;
        cluster.discounts = discounts;
        clusters.push_back(cluster);
    }

    // đọc ma trận khoảng cách của truck
    truck_distance_matrix.resize(num_nodes+1, vector<double>(num_nodes+1, 0.0));

    getline(file, line);

    for (int i = 0; i <= num_nodes; ++i) {
        getline(file, line);
        istringstream iss(line);

        string row_label;
        iss >> row_label;

        for (int j = 0; j <= num_nodes; ++j) {
            iss >> truck_distance_matrix[i][j];
        }
    }
    file.close();
    //tính Ttruck (ma trận thời gian di chuyển từ node i đến node j của truck) và 
    //Ctruck (ma trận chi phí di chuyển từ node i đến node j của truck)
    Ttruck.resize(num_nodes + 1, vector<double>(num_nodes + 1, 0.0));
    Ctruck.resize(num_nodes + 1, vector<double>(num_nodes + 1, 0.0));


    for (int i = 0; i < truck_distance_matrix.size(); i++) {
        for (int j = 0; j < truck_distance_matrix.size(); j++) {
            Ttruck[i][j] = truck_distance_matrix[i][j] / v_truck;
            Ctruck[i][j] = truck_distance_matrix[i][j] * truck_cost_factor;
        }
    }
    //tính Tdrone và Cdrone
    Tdrone.resize(num_nodes + 1, 0.0);
    Cdrone.resize(num_nodes + 1, 0.0);

    for (int i = 0; i < num_nodes + 1; i++) {
        Tdrone[i] = nodes[i].euclidDist * 2 / v_drone;
        //tinh them thoi gian sac vao trong tdrone
        if (nodes[i].chargeStation == 1 && nodes[i].euclidDist > max_fly_distance/2) {
            Tdrone[i] += time_swap_battery;
        }
        Cdrone[i] = nodes[i].euclidDist * 2 * drone_cost_factor;

    }
    //phan loai C va Cf ( cac clusters ma drone co the cho hang duoc)
    for (int i = 0; i < num_clusters ; i++) {
        C.push_back(clusters[i].clusterID);
        if (clusters[i].weight <= capacity_drone)
            Cf.push_back(clusters[i].clusterID);
    }
    return true;
}

// Hiển thị instance
void Instance::displayData() const {
    cout << "Number of nodes: " << num_nodes << "\n";
    cout << "Number of clusters: " << num_clusters << "\n";
    cout << "Number of drones: " << num_drones << "\n";
    cout << "Number of lockers: " << num_lockers << "\n";
    cout << "Truck speed: " << v_truck << "\n";
    cout << "Drone speed: " << v_drone << "\n";
    cout << "Drone max fly distance: " << max_fly_distance << "\n";
    cout << "Max working time: " << max_working_time << "\n";
    cout << "Drone capacity: " << capacity_drone << "\n";
    cout << "Time to swap battery: " << time_swap_battery << "\n";
    cout << "Service time: " << time_service << "\n";
    cout << "Truck cost factor: " << truck_cost_factor << "\n";
    cout << "Drone cost factor: " << drone_cost_factor << "\n";
    cout << "Discount cost factor: " << discount_cost_factor << "\n";

    cout << "\nNodes Information:\n";
    for (const auto& node : nodes) {
        cout << "Node ID: " << node.nodeID
            << ", Capacity: " << node.capacity
            << ", Charge Station: " << (node.chargeStation ? "Yes" : "No")
            << ", Euclidean Distance: " << node.euclidDist << "\n";
    }

    cout << "\nClusters Information:\n";
    for (const auto& cluster : clusters) {
        cluster.displayCluster();
    }

    cout << "\nTruck Distance Matrix:\n";
    for (const auto& row : truck_distance_matrix) {
        for (double dist : row) {
            cout << dist << " ";
        }
        cout << "\n";
    }
    cout << "\nTtruck Matrix:\n";
    for (const auto& row : Ttruck) {
        for (double time : row) {
            cout << time << " ";
        }
        cout << "\n";
    }
    cout << "\nCtruck Matrix:\n";
    for (const auto& row : Ctruck) {
        for (double cost : row) {
            cout << cost << " ";
        }
        cout << "\n";
    }
    cout << "\nTdrone Matrix:\n";
    for (const auto& time : Tdrone) {
            cout << time << " ";
    }
    cout << "\nCdrone Matrix:\n";
    for (const auto& cost : Cdrone) {
        cout << cost << " ";
    }
    cout << "\nC:\n";
    for (const auto& value : C) {
        cout << value << " ";
    }
    cout << "\nCf:\n";
    for (const auto& value : Cf) {
        cout << value << " ";
    }
}

//vector<int> Instance::getDiscountsByCluster(int clusterID) const {
//    for (const auto& cluster : clusters) {
//        if (cluster.clusterID == clusterID) {
//            return cluster.discounts;
//        }
//    }
//    return {}; 
//}
