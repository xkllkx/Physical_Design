#include <sstream>

#include "CE.h"

using namespace std;

bool debug = 0;

// 可調參數
double edge_cost = 2.5;

// TOOL -------------------------------------------------------------------

void CE::set_show(set<int> show_set) {
    for (auto &unit : show_set) {
        log_file << unit << " ";
    }
    log_file << endl;
}

void CE::vector_show(vector<int> show_vector) {
    for (unsigned int i = 0; i < show_vector.size(); i++) {
        log_file << show_vector[i] << " ";
    }
    log_file << endl;
}

int CE::vector_find(vector<int> find_vector, int temp) {
    for (unsigned int i = 0; i < find_vector.size(); i++) {
        if (find_vector[i] == temp) return i;
    }
    return -1;
}

// INPUT -------------------------------------------------------------------

void CE::node_input(string file_name) {
    ifstream input_file(file_name + ".nodes", ios::in);

    string input_temp3, input_temp2, input_temp1, input_temp;

    log_file << "============ NODE ==========" << endl;

    for(int i = 0; i < 4; i++) {  // 空行
        getline(input_file, input_temp);
    }

    input_file >> input_temp2 >> input_temp1 >> input_temp; // NumNodes
    NumNodes = stoi(input_temp);
    log_file << "NumNodes : " << NumNodes << endl;

    input_file >> input_temp2 >> input_temp1 >> input_temp; // NumTerminals
    NumTerminals = stoi(input_temp);
    log_file << "NumTerminals : " << NumTerminals << endl;

    NumMovableNodes = NumNodes - NumTerminals;

    getline(input_file, input_temp); // 空行

    // node_array.resize(NumMovableNodes); // Movable cells
    node_array.resize(NumNodes);

    istringstream istr;
    string line;
    int i = 0;

    while(getline(input_file, line)) {
        if(!line.length()) continue;

        // istringstream istr(line);
        istr.str(line);
        istr >> input_temp >> input_temp1 >> input_temp2 >> input_temp3;

        node_array[i].node_name = input_temp;
        // node_array[i].index = i;

        node_map.insert(pair<string, int>(input_temp, i));

        node_array[i].W = stoi(input_temp1);
        node_array[i].H = stoi(input_temp2);

        if(input_temp3.length()) { // terminal
            if(input_temp3 == "terminal") NumTerminal ++;
            else NumTerminal_NI ++;
        }

        input_temp3.clear();
        istr.clear(); // 要加 預防出錯

        i++;
    }

    input_file.close();

    log_file << "Numnode_array : " << node_array.size() << endl;
    log_file << "NumMovableNodes : " << NumMovableNodes << endl;
    log_file << "NumTerminal : " << NumTerminal << endl;
    log_file << "NumTerminal_NI : " << NumTerminal_NI << endl;
    log_file << "----------" << endl;

    if(debug) node_show(node_map["o211446"]);
}

bool node_x_compare(node &a, node &b){ // 由小到大
    if(a.lb.x != b.lb.x) return a.lb.x < b.lb.x;
    else return a.lb.y < b.lb.y;
}

bool node_y_compare(node &a, node &b){ // 由小到大
    if(a.lb.y != b.lb.y) return a.lb.y < b.lb.y;
    else return a.lb.x < b.lb.x;
}

void CE::legal_input(string file_name) {
    ifstream input_file(file_name + ".pl", ios::in);

    string input_temp5, input_temp4, input_temp3, input_temp2, input_temp1, input_temp;

    log_file << "============ LEGAL ==========" << endl;

    for(int i = 0; i < 2; i++) {
        getline(input_file, input_temp); // 空行
    }

    istringstream istr;
    string line;
    int i = 0;

    while(getline(input_file, line)) {
        if(!line.length()) continue;

        // istringstream istr(line);
        istr.str(line);
        istr >> input_temp >> input_temp5 >> input_temp4 >> input_temp3 >> input_temp2 >> input_temp1;

        node_array[i].lb = {stoi(input_temp5), stoi(input_temp4)};

        node_array[i].center_x = (double)node_array[i].lb.x + (double)node_array[i].W / 2.;
        node_array[i].center_y = (double)node_array[i].lb.y + (double)node_array[i].H / 2.;

        istr.clear(); // 要加 預防出錯

        i++;
    }

    input_file.close();

    // sort(node_array.begin(), node_array.end(),node_x_compare); // 照x排
    sort(node_array.begin(), node_array.end(),node_y_compare); // 照y排 // 為了row initial

    // node 重新編號
    for(int i = 0; i < (int)node_array.size(); i++) {
        // node_array[i].index = i;
        node_map[node_array[i].node_name] = i;
    }

    log_file << "----------" << endl;

    if(debug) node_show(node_map["o211446"]);
}

void CE::net_input(string file_name) {
    ifstream input_file(file_name + ".nets", ios::in);

    string input_temp4, input_temp3, input_temp2, input_temp1, input_temp;

    log_file << "========== NET ==========" << endl;

    for(int i = 0; i < 4; i++) {
        getline(input_file, input_temp); // 空行
    }

    input_file >> input_temp2 >> input_temp1 >> input_temp; // NumNets
    NumNets = stoi(input_temp);
    log_file << "NumNets : " << NumNets << endl;

    input_file >> input_temp2 >> input_temp1 >> input_temp; // NumPins
    NumPins = stoi(input_temp);
    log_file << "NumPins : " << NumPins << endl;

    getline(input_file, input_temp); // 空行

    net_array.resize(NumNets);

    int pin_num = 0, node_index = 0, i = 0;
    map<string, int>::iterator iter;

    while(input_file >> input_temp) {
        input_file >> input_temp3 >> input_temp2 >> input_temp1;
        pin_num = stoi(input_temp2);

        net_array[i].net_name = input_temp1;
        // net_array[i].index = i;

        net_array[i].pin_x.resize(pin_num);
        net_array[i].pin_y.resize(pin_num);

        for(int j = 0; j < pin_num; j++) {
            input_file >> input_temp4 >> input_temp3 >> input_temp2 >> input_temp1 >> input_temp;
            
            iter = node_map.find(input_temp4);

            if(iter != node_map.end()){
                node_index = iter->second;

                // offset要理
                net_array[i].pin_x[j] = node_array[node_index].center_x + stod(input_temp1);
                net_array[i].pin_y[j] = node_array[node_index].center_y + stod(input_temp);
            }
            else continue;
        }

        i++;
    }

    input_file.close();

    log_file << "Numnet_array : " << net_array.size() << endl;
    log_file << "----------" << endl;

    if(debug) {
        net_show(0);
        node_show(node_map["o211446"]); // node_map["o211446"]
    }
}

void CE::route_input(string file_name) { // grid_initial grid_supply_update
    ifstream input_file(file_name + ".route", ios::in);

    string input_temp4, input_temp3, input_temp2, input_temp1, input_temp;

    log_file << "============ ROUTE ==========" << endl;

    for(int i = 0; i < 4; i++) {  // 空行
        getline(input_file, input_temp);
    }

    input_file >> input_temp4 >> input_temp3 >> input_temp2 >> input_temp1 >> input_temp; // Grid
    grid_x_num = stoi(input_temp2);
    log_file << "grid_x_num : " << grid_x_num << endl;

    grid_y_num = stoi(input_temp1);
    log_file << "grid_y_num : " << grid_y_num << endl;

    metal_num = stoi(input_temp);
    log_file << "metal_num : " << metal_num << endl;

    metal_wire_width.resize(metal_num);
    metal_wire_space.resize(metal_num);

    getline(input_file, input_temp); // 不知為啥
    getline(input_file, input_temp); // Capacity
    getline(input_file, input_temp); // Capacity

    istringstream istr;
    string line;

    getline(input_file, line);
    istr.str(line);
    istr >> input_temp4 >> input_temp3; // MinWireWidth : 

    for(int i = 0; i < metal_num; i++) {
        istr >> input_temp;

        metal_wire_width[i] = stoi(input_temp);
    }

    istr.clear(); // 要加 預防出錯

    getline(input_file, line);
    istr.str(line);
    istr >> input_temp4 >> input_temp3; // MinWireSpacing : 

    for(int i = 0; i < metal_num; i++) {
        istr >> input_temp;

        metal_wire_space[i] = stoi(input_temp);
    }

    istr.clear(); // 要加 預防出錯

    vector_show(metal_wire_width);
    vector_show(metal_wire_space);

    getline(input_file, input_temp); // ViaSpacing

    input_file >> input_temp3 >> input_temp2 >> input_temp1 >> input_temp;
    GridOrigin = {stoi(input_temp1), stoi(input_temp)};
    log_file << "GridOrigin : (" << GridOrigin.x << ", " << GridOrigin.y << ")" << endl;

    input_file >> input_temp3 >> input_temp2 >> input_temp1 >> input_temp;
    TileSize = {stoi(input_temp1), stoi(input_temp)};
    // TileSize = {stoi(input_temp), stoi(input_temp1)};
    log_file << "TileSize : (" << TileSize.x << ", " << TileSize.y << ")" << endl;

    for(int i = 0; i < 4; i++) {  // 空行
        getline(input_file, input_temp);
    }

    //  grid_initial ------------------------------------------------------------------
    grid_initial();
    // -------------------------------------------------------------------

    input_file >> input_temp2 >> input_temp1 >> input_temp; // NumBlockageNodes
    NumBlockageNodes = stoi(input_temp);

    getline(input_file, input_temp); // 空行
    getline(input_file, input_temp); // 不知為啥

    // grid_supply_update -------------------------------------------------------------------

    log_file << "============ GRID SUPPLY UPDATE ==========" << endl;

    map<string, int>::iterator iter;
    int node_index = 0, block_layer_num = 0;

    vector<int> block_layer;

    for(int i = 0; i < NumBlockageNodes; i++) {
        getline(input_file, line);
        if(!line.length()) continue;

        istr.str(line);
        istr >> input_temp >> input_temp1;

        // log_file << input_temp << endl;
        // log_file << input_temp1 << endl;

        block_layer_num = stoi(input_temp1);

        iter = node_map.find(input_temp);

        if(iter != node_map.end()){
            node_index = iter->second;

            node_array[node_index].block = 1;

            for(int j = 0; j < block_layer_num; j++) {
                istr >> input_temp;

                block_layer.push_back(stoi(input_temp)-1);
            }

            // vector_show(block_layer);

            block_edge_supply_update(node_index, block_layer);
            block_layer.clear();
        }
        else continue;

        istr.clear(); // 要加 預防出錯

        NumBlockageNode ++;
    }

    input_file.close();

    log_file << "NumBlockageNodes : " << NumBlockageNodes << endl;
    log_file << "NumBlockageNode : " << NumBlockageNode << endl;
    log_file << "----------" << endl;

    if(debug) grid_show(133, 125);
}

void CE::gr_input(string file_name) {
    // n0 0
    // (2400,2160,2)-(2400,2240,2)
    // (2688,2160,2)-(2688,2240,2)
    // (2368,2240,3)-(2688,2240,3)
    // (2688,2160,3)-(2848,2160,3)
    // (2400,2160,1)-(2400,2160,2)
    // (2400,2240,2)-(2400,2240,3)
    // (2368,2240,1)-(2368,2240,3)
    // (2688,2240,1)-(2688,2240,3)
    // (2688,2160,2)-(2688,2160,3)
    // (2848,2160,1)-(2848,2160,3)
    // !

    ifstream input_file(file_name + ".gr", ios::in);

    string input_temp2, input_temp1, input_temp;
    string coor_temp, coor_temp1;

    int x_temp = 0, x_temp1 = 0;
    int y_temp = 0, y_temp1 = 0;

    coor x_bound = {0, 0};
    coor y_bound = {0, 0};

    int x_min_index = 0;
    int x_max_index = 0;

    int y_min_index = 0;
    int y_max_index = 0;

    log_file << "========== GR ==========" << endl;

    while(input_file >> input_temp) {
        getline(input_file, input_temp); // 空行

        while(1) {
            getline(input_file, input_temp);
            if(input_temp == "!") break;

            coor_temp = input_temp.substr(1, input_temp.find(")"));
            coor_temp1 = input_temp.substr(input_temp.rfind("(")+1);

            input_temp1 = coor_temp.substr(0, coor_temp.find(","));
            coor_temp = coor_temp.substr(coor_temp.find(",")+1);
            input_temp = coor_temp.substr(0, coor_temp.find(","));

            x_temp = stoi(input_temp1);
            y_temp = stoi(input_temp);

            input_temp1 = coor_temp1.substr(0, coor_temp1.find(","));
            coor_temp1 = coor_temp1.substr(coor_temp1.find(",")+1);
            input_temp = coor_temp1.substr(0, coor_temp1.find(","));

            x_temp1 = stoi(input_temp1);
            y_temp1 = stoi(input_temp);

            if(debug) {
                log_file << "(" << x_temp << ", " << y_temp << ")" << endl;
                log_file << "(" << x_temp1 << ", " << y_temp1 << ")" << endl;
                log_file << "---" << endl;
            }

            if(x_temp == x_temp1 && y_temp == y_temp1) continue;

            x_bound = {min(x_temp, x_temp1), max(x_temp, x_temp1)};
            y_bound = {min(y_temp, y_temp1), max(y_temp, y_temp1)};

            x_min_index = floor((x_bound.x - GridOrigin.x) / (double)TileSize.x); // double 一定要加
            x_max_index = floor((x_bound.y - GridOrigin.x) / (double)TileSize.x);

            y_min_index = floor((y_bound.x - GridOrigin.y) / (double)TileSize.y);
            y_max_index = floor((y_bound.y - GridOrigin.y) / (double)TileSize.y);

            if(x_min_index == x_max_index) { // V
                for(int u_i = y_max_index; u_i > y_min_index; u_i--) {
                    // 0:上, 1:左, 2:下, 3:右

                    if(u_i != y_min_index) grid_array[u_i][x_min_index].edges[2]->overflow -= edge_cost;
                }
            }
            else { // H
                for(int u_j = x_min_index; u_j < x_max_index; u_j++) {
                    // 0:上, 1:左, 2:下, 3:右

                    if(u_j != x_max_index) grid_array[y_min_index][u_j].edges[3]->overflow -= edge_cost;
                }
            }
        }
    }

    input_file.close();
}

// SHOW -------------------------------------------------------------------

void CE::net_show(int show_net_index) {
    log_file << "net_name : " << net_array[show_net_index].net_name << endl;
    // log_file << "index : " << net_array[show_net_index].index << endl;

    log_file << "pin : " << endl;
    for(int i = 0; i < (int)net_array[show_net_index].pin_x.size(); i++) {
        log_file << " (" << net_array[show_net_index].pin_x[i] << ", " << net_array[show_net_index].pin_y[i] << ")" << endl;
    }
    log_file << "----------" << endl;
}

void CE::node_show(int show_node_index) {
    log_file << "node_name : " << node_array[show_node_index].node_name << endl;
    // log_file << "index : " << node_array[show_node_index].index << endl;
    log_file << "lb : (" << node_array[show_node_index].lb.x << ", " << node_array[show_node_index].lb.y << ")" << endl;
    log_file << "W : " << node_array[show_node_index].W << "\tH : " << node_array[show_node_index].H << endl;
    log_file << "cenetr : (" << node_array[show_node_index].center_x << ", " << node_array[show_node_index].center_y << ")" << endl;
    log_file << "block : " << node_array[show_node_index].block << endl;

    // log_file << "shape_num : " << node_array[show_node_index].shape.size() << endl;
    log_file << "----------" << endl;
}

void CE::grid_show(int show_grid_index_x, int show_grid_index_y) {
    log_file << "GRID " << show_grid_index_x << " " << show_grid_index_y << " : " << endl;
    log_file << "total_H_overflow : " << grid_array[show_grid_index_y][show_grid_index_x].total_H_overflow << endl;
    log_file << "total_V_overflow : " << grid_array[show_grid_index_y][show_grid_index_x].total_V_overflow << endl << endl;
    
    log_file << "up : " << grid_array[show_grid_index_y][show_grid_index_x].edges[0]->overflow << endl;
    log_file << "left : " << grid_array[show_grid_index_y][show_grid_index_x].edges[1]->overflow << endl;
    log_file << "down : " << grid_array[show_grid_index_y][show_grid_index_x].edges[2]->overflow << endl;
    log_file << "right : " << grid_array[show_grid_index_y][show_grid_index_x].edges[3]->overflow << endl;

    log_file << "----------" << endl;
}

// ALG -------------------------------------------------------------------

void CE::grid_initial(){
    vector<grid> grid_x_row(grid_x_num);
    grid_array.resize(grid_y_num, grid_x_row);

    log_file << "============ GRID INITIAL ==========" << endl;

    log_file << "GRID : " << grid_array[0].size() << " X " << grid_array.size() << endl << endl;

    double horizontal_supply_temp = 0., vertical_supply_temp = 0.;

    // 算 horizontal_supply_temp vertical_supply_temp
    for(int k = 0; k < metal_num; k++) { // metal
        horizontal_supply_temp += TileSize.x / (metal_wire_width[k] + metal_wire_space[k]);
        vertical_supply_temp += TileSize.y / (metal_wire_width[k] + metal_wire_space[k]);
    }

    for(int i = (int)grid_array.size()-1; i >= 0; i--) {
        for(int j = 0; j < (int)grid_array[i].size(); j++) {

            // 建立edge // 0:上, 1:左, 2:下, 3:右
            for(int l = 0; l < 4; l++) {
                if(grid_array[i][j].edges[l] == nullptr) {
                    if(l == 0) { // 上
                        if(i+1 <= (int)grid_array.size()-1 && grid_array[i+1][j].edges[2] != nullptr) {
                            grid_array[i][j].edges[l] = grid_array[i+1][j].edges[2];
                            grid_array[i+1][j].edges[2]->host_grid[1] = &grid_array[i][j];
                        }
                        else {
                            grid_array[i][j].edges[l] = new edge;
                            grid_array[i][j].edges[l]->host_grid[0] = &grid_array[i][j];
                        }
                    }
                    else if(l == 1) { // 左
                        if(j-1 >= 0 && grid_array[i][j-1].edges[3] != nullptr) {
                            grid_array[i][j].edges[l] = grid_array[i][j-1].edges[3];
                            grid_array[i][j-1].edges[3]->host_grid[1] = &grid_array[i][j];
                        }
                        else {
                            grid_array[i][j].edges[l] = new edge;
                            grid_array[i][j].edges[l]->host_grid[0] = &grid_array[i][j];
                        }
                    }
                    else if(l == 2) { // 下
                        if(i-1 >= 0 && grid_array[i-1][j].edges[0] != nullptr) {
                            grid_array[i][j].edges[l] = grid_array[i-1][j].edges[0];
                            grid_array[i-1][j].edges[0]->host_grid[1] = &grid_array[i][j];
                        }
                        else {
                            grid_array[i][j].edges[l] = new edge;
                            grid_array[i][j].edges[l]->host_grid[0] = &grid_array[i][j];
                        }
                    }
                    else { // 右
                        if(j+1 <= (int)grid_array[0].size()-1 && grid_array[i][j+1].edges[1] != nullptr) {
                            grid_array[i][j].edges[l] = grid_array[i][j+1].edges[1];
                            grid_array[i][j+1].edges[1]->host_grid[1] = &grid_array[i][j];
                        }
                        else {
                            grid_array[i][j].edges[l] = new edge;
                            grid_array[i][j].edges[l]->host_grid[0] = &grid_array[i][j];
                        }
                    }
                }

                // 更新supply // 0:上, 1:左, 2:下, 3:右
                if(l % 2) grid_array[i][j].edges[l]->overflow = vertical_supply_temp;
                else grid_array[i][j].edges[l]->overflow = horizontal_supply_temp;
            }

            // 顯示 grid
            // log_file << "grid " << i << " " << j << " : ";
            // for(int l = 0; l < 4; l++) {
            //     log_file << grid_array[i][j].edges[l] << " ";
            // }
            // log_file << endl;

            // if(debug) grid_show(j, i);
        }
    }
    
    log_file << "----------" << endl;
    if(debug) grid_show(0, 0);
}

void CE::block_edge_supply_update(int node_index, vector<int> block_layer){ // 找出被block的edge

    // 避免 W、H = 0
    if(!node_array[node_index].W || !node_array[node_index].H) return;

    coor x_bound = {node_array[node_index].lb.x, node_array[node_index].lb.x + node_array[node_index].W};
    coor y_bound = {node_array[node_index].lb.y, node_array[node_index].lb.y + node_array[node_index].H};

    int x_min_index = floor((x_bound.x - GridOrigin.x) / (double)TileSize.x); // double 一定要加
    int x_max_index = floor((x_bound.y - GridOrigin.x) / (double)TileSize.x);
    
    int y_min_index = floor((y_bound.x - GridOrigin.y) / (double)TileSize.y);
    int y_max_index = floor((y_bound.y - GridOrigin.y) / (double)TileSize.y);

    double horizontal_supply_temp, vertical_supply_temp;
    int x_cursor = 0, y_cursor = y_bound.y;

    int grid_x_temp = 0, grid_y_temp = 0;

    // 只考慮右、下的 edge 就好
    for(int i = y_max_index; i >= y_min_index; i--) {

        x_cursor = x_bound.x;

        for(int j = x_min_index; j <= x_max_index; j++) {

            if(j == x_max_index && i == y_min_index) break; // 最右下的 grid 不用考慮

            // 0:上, 1:左, 2:下, 3:右
            if(j != x_max_index) { // 要算右 edge
                grid_y_temp = y_cursor - max((GridOrigin.y + (TileSize.y * i)), y_bound.x); // 右 edge 的 y
                
                for (auto &layer : block_layer) { // metal
                    vertical_supply_temp = (grid_y_temp / (metal_wire_width[layer] + metal_wire_space[layer]));

                    grid_array[i][j].edges[3]->overflow -= vertical_supply_temp;
                }
            }

            if(i != y_min_index) { // 要算下 edge
                grid_x_temp = min(GridOrigin.x + (TileSize.x * (j+1)), x_bound.y) - x_cursor; // 下 edge 的 x
                
                for (auto &layer : block_layer) { // metal
                    horizontal_supply_temp = (grid_x_temp / (metal_wire_width[layer] + metal_wire_space[layer]));

                    grid_array[i][j].edges[2]->overflow -= horizontal_supply_temp;
                }
            }

            x_cursor = GridOrigin.x + (TileSize.x * (j+1));
        }

        y_cursor = GridOrigin.y + (TileSize.y * i);
    }

    if(debug) grid_show(x_min_index, y_min_index);
}

bool grid_H_overflow_compare(grid_unit &a, grid_unit &b){ // 由大到小
    return a.total_H_overflow > b.total_H_overflow;
}

bool grid_V_overflow_compare(grid_unit &a, grid_unit &b){ // 由大到小
    return a.total_V_overflow > b.total_V_overflow;
}

bool edge_overflow_compare(edge_unit &a, edge_unit &b){ // 由小到大 因為overflow是負的
    return a.overflow < b.overflow;
}

void CE::grid_demand_update() {
    int pin_num = 0;

    Branch* s_node = nullptr;
    Branch* e_node = nullptr;

    coor x_bound = {0, 0};
    coor y_bound = {0, 0};

    int x_min_index = 0;
    int x_max_index = 0;

    int y_min_index = 0;
    int y_max_index = 0;

    log_file << "============ GRID DEMAND UPDATE ==========" << endl;

    for(int i = 0; i < (int)net_array.size(); i++) {
        // log_file << net_array[i].net_name << endl;

        pin_num = net_array[i].pin_x.size();
        // log_file << pin_num << endl;

        // 避免單 node 的 net
        if(pin_num <= 1) continue;

        int *pin_x = new int[pin_num];
        int *pin_y = new int[pin_num];

        for(int j = 0; j < pin_num; j++) {
            pin_x[j] = net_array[i].pin_x[j];
            pin_y[j] = net_array[i].pin_y[j];
        }
        
        Tree flutetree = flute(pin_num, pin_x, pin_y, ACCURACY);
        s_node = flutetree.branch;
        e_node = nullptr;

        for(int j = 0; j < 2*flutetree.deg-2; j++) {
            e_node = flutetree.branch + s_node->n;
            
            // 避免node自連
            if(s_node == e_node) continue;

            // log_file << s_node->x << " " << s_node->y << " " << s_node->n << endl;

            x_bound = {min(s_node->x, e_node->x), max(s_node->x, e_node->x)};
            y_bound = {min(s_node->y, e_node->y), max(s_node->y, e_node->y)};

            x_min_index = floor((x_bound.x - GridOrigin.x) / (double)TileSize.x); // double 一定要加
            x_max_index = floor((x_bound.y - GridOrigin.x) / (double)TileSize.x);

            y_min_index = floor((y_bound.x - GridOrigin.y) / (double)TileSize.y);
            y_max_index = floor((y_bound.y - GridOrigin.y) / (double)TileSize.y);

            // log_file << "x_bound : " << x_bound.x << " " << x_bound.y << endl;
            // log_file << "y_bound : " << y_bound.x << " " << y_bound.y  << endl;

            // 只考慮右、下的 edge 就好
            for(int u_i = y_max_index; u_i >= y_min_index; u_i--) {
                for(int u_j = x_min_index; u_j <= x_max_index; u_j++) {

                    if(u_j == x_max_index && u_i == y_min_index) break; // 最右下的 grid 不用考慮

                    // 0:上, 1:左, 2:下, 3:右

                    if(u_j != x_max_index) { // 要算右 edge
                        // RUDY
                        grid_array[u_i][u_j].edges[3]->overflow -= ((y_bound.y - y_bound.x) * (x_bound.y - x_bound.x)) / (double)((y_bound.y - y_bound.x) + (x_bound.y - x_bound.x)) / edge_cost;
                    }

                    if(u_i != y_min_index) { // 要算下 edge
                        // RUDY
                        grid_array[u_i][u_j].edges[2]->overflow -= ((y_bound.y - y_bound.x) * (x_bound.y - x_bound.x)) / (double)((y_bound.y - y_bound.x) + (x_bound.y - x_bound.x)) / edge_cost;
                    }
                }
            }

            s_node ++;
        }

        // log_file << "----------" << endl;

        delete [] pin_x;
        delete [] pin_y;
    }
}

void CE::grid_L_demand_update() {
    int pin_num = 0;

    Branch* s_node = nullptr;
    Branch* e_node = nullptr;

    coor x_bound = {0, 0};
    coor y_bound = {0, 0};

    int x_min_index = 0;
    int x_max_index = 0;

    int y_min_index = 0;
    int y_max_index = 0;

    log_file << "============ GRID DEMAND UPDATE ==========" << endl;

    for(int i = 0; i < (int)net_array.size(); i++) {
        // log_file << net_array[i].net_name << endl;

        pin_num = net_array[i].pin_x.size();
        // log_file << pin_num << endl;

        // 避免單 node 的 net
        if(pin_num <= 1) continue;

        int *pin_x = new int[pin_num];
        int *pin_y = new int[pin_num];

        for(int j = 0; j < pin_num; j++) {
            pin_x[j] = net_array[i].pin_x[j];
            pin_y[j] = net_array[i].pin_y[j];
        }
        
        Tree flutetree = flute(pin_num, pin_x, pin_y, ACCURACY);
        s_node = flutetree.branch;
        e_node = nullptr;

        for(int j = 0; j < 2*flutetree.deg-2; j++) {
            e_node = flutetree.branch + s_node->n;
            
            // 避免node自連
            if(s_node == e_node) continue;

            // log_file << s_node->x << " " << s_node->y << " " << s_node->n << endl;

            x_bound = {min(s_node->x, e_node->x), max(s_node->x, e_node->x)};
            y_bound = {min(s_node->y, e_node->y), max(s_node->y, e_node->y)};

            x_min_index = floor((x_bound.x - GridOrigin.x) / (double)TileSize.x); // double 一定要加
            x_max_index = floor((x_bound.y - GridOrigin.x) / (double)TileSize.x);

            y_min_index = floor((y_bound.x - GridOrigin.y) / (double)TileSize.y);
            y_max_index = floor((y_bound.y - GridOrigin.y) / (double)TileSize.y);

            // log_file << "x_bound : " << x_bound.x << " " << x_bound.y << endl;
            // log_file << "y_bound : " << y_bound.x << " " << y_bound.y  << endl;

            if(x_min_index == x_max_index && y_min_index != y_max_index) { // V
                for(int u_i = y_max_index; u_i > y_min_index; u_i--) {
                    // 0:上, 1:左, 2:下, 3:右

                    if(u_i != y_min_index) grid_array[u_i][x_min_index].edges[2]->overflow -= edge_cost;
                }
            }
            else if(x_min_index != x_max_index && y_min_index == y_max_index) { // H
                for(int u_j = x_min_index; u_j < x_max_index; u_j++) {
                    // 0:上, 1:左, 2:下, 3:右

                    if(u_j != x_max_index) grid_array[y_min_index][u_j].edges[3]->overflow -= edge_cost;
                }
            }
            else { // L-shape

                // 上L
                for(int u_j = x_min_index; u_j < x_max_index; u_j++) {
                    // 0:上, 1:左, 2:下, 3:右

                    if(u_j != x_max_index) grid_array[y_max_index][u_j].edges[3]->overflow -= edge_cost / 2.;
                }

                for(int u_i = y_max_index; u_i > y_min_index; u_i--) {
                    // 0:上, 1:左, 2:下, 3:右

                    if(u_i != y_min_index) grid_array[u_i][x_min_index].edges[2]->overflow -= edge_cost / 2.;
                }

                // 下L
                for(int u_j = x_min_index; u_j < x_max_index; u_j++) {
                    // 0:上, 1:左, 2:下, 3:右

                    if(u_j != x_max_index) grid_array[y_min_index][u_j].edges[3]->overflow -= edge_cost / 2.;
                }

                for(int u_i = y_max_index; u_i > y_min_index; u_i--) {
                    // 0:上, 1:左, 2:下, 3:右

                    if(u_i != y_min_index) grid_array[u_i][x_max_index].edges[2]->overflow -= edge_cost / 2.;
                }
            }
            
            s_node ++;
        }

        // log_file << "----------" << endl;

        delete [] pin_x;
        delete [] pin_y;
    }
}

void CE::overflow_cal() {
    double overflow_temp = 0.;

    // 找 total_H_overflow、total_V_overflow
    for(int i = 0; i < (int)grid_array.size(); i++) {
        for(int j = 0; j < (int)grid_array[i].size(); j++) {
            for(int l = 0; l < 4; l++) {

                // 判斷有沒有overflow
                if(grid_array[i][j].edges[l]->overflow < 0) {

                    overflow_temp = fabs(grid_array[i][j].edges[l]->overflow);

                    // TOF // 每個邊會被計算兩次 除了最外圍
                    if((i == 0 && l == 0) || (i == (int)grid_array.size()-1 && l == 2) || (j == 0 && l == 1) || (j == (int)grid_array[0].size()-1 && l == 3)) { // 外圍
                        TOF += overflow_temp * 2;
                    }
                    else TOF += overflow_temp;
                    
                    // 0:上, 1:左, 2:下, 3:右

                    // V 上下 H 左右
                    if(l % 2) grid_array[i][j].total_H_overflow += overflow_temp / edge_cost; // 左右 edge // 正規化
                    else grid_array[i][j].total_V_overflow += overflow_temp / edge_cost; // 上下 edge // 正規化
                }
            }
        }
    }

    TOF = TOF / 2. / edge_cost; // 正規化

    // -------------------------------------------------------------------

    // 排序 grid_H_overflow、grid_V_overflow、edge_overflow
    grid_H_overflow.resize(grid_x_num * grid_y_num);
    grid_V_overflow.resize(grid_x_num * grid_y_num);

    // E=m×(n+1)+n×(m+1)
    edge_overflow.resize(grid_x_num * (grid_y_num+1) + grid_y_num * (grid_x_num+1));

    for(int i = 0; i < (int)grid_array.size(); i++) {
        for(int j = 0; j < (int)grid_array[i].size(); j++) {
            grid_H_overflow[grid_num] = {j, i, grid_array[i][j].total_H_overflow, grid_array[i][j].total_V_overflow};
            grid_V_overflow[grid_num] = {j, i, grid_array[i][j].total_H_overflow, grid_array[i][j].total_V_overflow};
            
            // overflow_grid_num
            if(grid_array[i][j].total_H_overflow > 0 || grid_array[i][j].total_V_overflow > 0) overflow_grid_num ++;

            // overflow_edge_num
            for(int l = 0; l < 4; l++) {
                if(grid_array[i][j].edges[l]->overflow < 0) overflow_edge_num ++;
            }

            if(j == (int)grid_array[i].size()-1) { // 左
                edge_overflow[edge_num] = {grid_array[i][j].edges[1], grid_array[i][j].edges[1]->overflow};
                edge_num ++;
            }
            if(i == 0) { // 上
                edge_overflow[edge_num] = {grid_array[i][j].edges[0], grid_array[i][j].edges[0]->overflow};
                edge_num ++;
            }

            edge_overflow[edge_num] = {grid_array[i][j].edges[2], grid_array[i][j].edges[2]->overflow};
            edge_num ++;
            edge_overflow[edge_num] = {grid_array[i][j].edges[3], grid_array[i][j].edges[3]->overflow};
            edge_num ++;

            grid_num ++;
        }
    }

    sort(grid_H_overflow.begin(), grid_H_overflow.end(), grid_H_overflow_compare);
    sort(grid_V_overflow.begin(), grid_V_overflow.end(), grid_V_overflow_compare);
    sort(edge_overflow.begin(), edge_overflow.end(), edge_overflow_compare);
    
    log_file << "max_grid_H_overflow : " << endl;
    grid_show(grid_H_overflow[0].x, grid_H_overflow[0].y);
    grid_show(grid_H_overflow[1].x, grid_H_overflow[1].y);

    log_file << "max_grid_V_overflow : " << endl;
    grid_show(grid_V_overflow[0].x, grid_V_overflow[0].y);
    grid_show(grid_V_overflow[1].x, grid_V_overflow[1].y);

    max_edge_overflow = fabs(edge_overflow[0].overflow);
    log_file << "max_edge_overflow : " << max_edge_overflow << endl;
    max_grid_H_overflow = grid_H_overflow[0].total_H_overflow;
    log_file << "max_grid_H_overflow : " << grid_H_overflow[0].total_H_overflow << endl;
    max_grid_V_overflow = grid_V_overflow[0].total_V_overflow;
    log_file << "max_grid_V_overflow : " << grid_V_overflow[0].total_V_overflow << endl;

    log_file << "grid_num : " << grid_num << endl;
    log_file << "overflow_grid_num : " << overflow_grid_num << endl;
    log_file << "edge_num : " << edge_num << endl;
    log_file << "overfloe_edge_num : " << overflow_edge_num << endl << endl;

    // edge_num = overflow_edge_num;
    // grid_num = overflow_grid_num;
    
    // -------------------------------------------------------------------

    // 計算ACE
    int ACE_0_5_counter = 0;
    int ACE_1_counter = 0;
    int ACE_2_counter = 0;
    int ACE_5_counter = 0;

    for(int i = 0; i < (int)(edge_num * 0.05); i++) {
        overflow_temp = fabs(edge_overflow[i].overflow);

        if(i <= edge_num * 0.005) {
            ACE_0_5 += overflow_temp;
            ACE_0_5_counter ++;
        }

        if(i <= edge_num * 0.01) {
            ACE_1 += overflow_temp;
            ACE_1_counter ++;
        }

        if(i <= edge_num * 0.02) {
            ACE_2 += overflow_temp;
            ACE_2_counter ++;
        }

        ACE_5 += overflow_temp;
        ACE_5_counter ++;
    }

    // 取ACE平均
    ACE_0_5 = ACE_0_5 / ACE_0_5_counter / edge_cost; // 正規化
    ACE_1 = ACE_1 / ACE_1_counter / edge_cost; // 正規化
    ACE_2 = ACE_2 / ACE_2_counter / edge_cost; // 正規化
    ACE_5 = ACE_5 / ACE_5_counter / edge_cost; // 正規化

    log_file << "TOF : " << TOF << endl;
    log_file << "ACE_0_5 : " << ACE_0_5 << endl;
    log_file << "ACE_1 : " << ACE_1 << endl;
    log_file << "ACE_2 : " << ACE_2 << endl;
    log_file << "ACE_5 : " << ACE_5 << endl << endl;
}

void CE::cost_cal(string benchmark_name) {
    // NCTU-GR
    // 			 0.5%    1%      2%      5%      TOF
    // adaptec1  129.52  124.46  120.92  112.54  136684
    // adaptec2  138.08  127.92  117.84  107.14  97514
    // adaptec3  100.76  100.38  100.19  97.87   3630 
    // adaptec4  102.62  101.31  100.65  93.71   15420

    double ACE_0_5_standard = 0.;
    double ACE_1_standard = 0.;
    double ACE_2_standard = 0.;
    double ACE_5_standard = 0.;
    double TOF_standard = 0.;

    double cost = 0.;

    if(benchmark_name == "adaptec1") {
        ACE_0_5_standard = 129.52;
        ACE_1_standard = 124.46;
        ACE_2_standard = 120.92;
        ACE_5_standard = 112.54;
        TOF_standard = 136684;
    }
    else if(benchmark_name == "adaptec2") {
        ACE_0_5_standard = 138.08;
        ACE_1_standard = 127.92;
        ACE_2_standard = 117.84;
        ACE_5_standard = 107.14;
        TOF_standard = 97514;
    }
    else if(benchmark_name == "adaptec3") {
        ACE_0_5_standard = 100.76;
        ACE_1_standard = 100.38;
        ACE_2_standard = 100.19;
        ACE_5_standard = 97.87;
        TOF_standard = 3630;
    }
    else { // adaptec4
        ACE_0_5_standard = 102.62;
        ACE_1_standard = 101.31;
        ACE_2_standard = 100.65;
        ACE_5_standard = 93.71;
        TOF_standard = 15420;
    }

    cost = (5*fabs(ACE_0_5_standard - ACE_0_5) + 
        4*fabs(ACE_1_standard - ACE_1) + 
        3*fabs(ACE_2_standard - ACE_2) + 
        2*fabs(ACE_5_standard - ACE_5) + 
        fabs(TOF_standard - TOF))/15.;

    log_file << "COST : " << cost << endl;
    cout << "COST : " << cost << endl;
}

// OUTPUT -------------------------------------------------------------------

void CE::file_output(string file_name) { // .txt
    ofstream output_file(file_name, ios::out);

    output_file << "runtime : " << (double)(clock()-start_time)/CLOCKS_PER_SEC << " s" << endl;
    output_file << "total overflow : " << TOF << endl;

    output_file << "ACE\t" << "0.50%\t" << "1.00%\t" << "2.00%\t" << "5.00%" << endl;
    output_file << "\t" << ACE_0_5 << "\t" << ACE_1 << "\t" << ACE_2 << "\t" << ACE_5 << endl;
    
    output_file.close();
}

void CE::m_file_output(bool H_or_V, string file_name){ // 前10% overflow grid
    // H_or_V   0:H 1:V

    // axis equal;
    // hold on
    // grid on
    // block_x=[ 0 0 21 21 0];
    // block_y=[ 0 22 22 0 0];
    // fill(block_x,block_y,'c');
    // text(10,11,'n1');

    ofstream output_file(file_name + ".m", ios::out);

    output_file << "axis equal;" << endl;
    output_file << "hold on;" << endl;
    output_file << "grid on;" << endl;

    // bound
    output_file << "cap_x=[ " << GridOrigin.x << " " << GridOrigin.x << " " << GridOrigin.x + (grid_x_num * TileSize.x) << " " << GridOrigin.x + (grid_x_num * TileSize.x) << " " << GridOrigin.x << " ];" << endl;
    output_file << "cap_y=[ " << GridOrigin.y << " " << GridOrigin.y + (grid_y_num * TileSize.y) << " " << GridOrigin.y + (grid_y_num * TileSize.y) << " " << GridOrigin.y << " " << GridOrigin.y << " ];" << endl;
    output_file << "fill(cap_x,cap_y,'b');" << endl;

    // cross 四等份
    output_file << "plot([" << GridOrigin.x + (grid_x_num * TileSize.x) / 2 << " " <<  GridOrigin.x + (grid_x_num * TileSize.x) / 2 << "]";
    output_file << ", [" << GridOrigin.y << " " <<  GridOrigin.y + (grid_y_num * TileSize.y) << "]";
    output_file << ", '--', 'Color', [0 0 0])" << endl;

    output_file << "plot([" << GridOrigin.x << " " <<  GridOrigin.x + (grid_x_num * TileSize.x) << "]";
    output_file << ", [" << GridOrigin.y + (grid_y_num * TileSize.y) / 2 << " " <<  GridOrigin.y + (grid_y_num * TileSize.y) / 2 << "]";
    output_file << ", '--', 'Color', [0 0 0])" << endl;

    coor grid_lb_temp;

    // 前10% overflow grid
    if(H_or_V == 0) { // H
        for(int i = 0; i < (int)(overflow_grid_num * 0.1); i++) { // 數量
        // for(int i = 0; i < (int)overflow_grid_num; i++) { // 數量
        // for(int i = 0; i < (int)(grid_num * 0.1); i++) { // 數量

            grid_lb_temp = {GridOrigin.x + (TileSize.x * grid_H_overflow[i].x), GridOrigin.y + (TileSize.y * grid_H_overflow[i].y)};

            output_file << "block_x=[ ";
            output_file << grid_lb_temp.x << " " << grid_lb_temp.x + TileSize.x << " " << grid_lb_temp.x + TileSize.x << " " <<  grid_lb_temp.x << " " << grid_lb_temp.x << " ];" << endl;

            output_file << "block_y=[ ";
            output_file << grid_lb_temp.y << " " << grid_lb_temp.y << " " << grid_lb_temp.y + TileSize.y << " " << grid_lb_temp.y + TileSize.y << " " << grid_lb_temp.y << " ];" << endl;
            
            output_file << "fill(block_x,block_y,'m','edgecolor','none');" << endl;
        }
    }
    else { // V
        for(int i = 0; i < (int)(overflow_grid_num * 0.1); i++) { // 數量
        // for(int i = 0; i < (int)overflow_grid_num; i++) { // 數量
        // for(int i = 0; i < (int)(grid_num * 0.1); i++) { // 數量
            // if(grid_H_overflow[i].total_V_overflow <= 0) break;

            grid_lb_temp = {GridOrigin.x + (TileSize.x * grid_V_overflow[i].x), GridOrigin.y + (TileSize.y * grid_V_overflow[i].y)};

            output_file << "block_x=[ ";
            output_file << grid_lb_temp.x << " " << grid_lb_temp.x + TileSize.x << " " << grid_lb_temp.x + TileSize.x << " " <<  grid_lb_temp.x << " " << grid_lb_temp.x << " ];" << endl;

            output_file << "block_y=[ ";
            output_file << grid_lb_temp.y << " " << grid_lb_temp.y << " " << grid_lb_temp.y + TileSize.y << " " << grid_lb_temp.y + TileSize.y << " " << grid_lb_temp.y << " ];" << endl;
            
            output_file << "fill(block_x,block_y,'m','edgecolor','none');" << endl;
        }
    }

    output_file.close();
}

void CE::dpx_file_output(int H_or_V, string file_name){
    // H_or_V   0:H 1:V 2:H+V

    // COLOR blue
    // SR 18404 -16434 20323 -16443
    // SL  18404 -16434 20323 -16443
    // SL  18404 -16443 20323 -16434
    // COLOR black
    // SR  0 -0 22482 -16479

    ofstream output_file(file_name + ".dpx", ios::out);

    // y 要加負號

    // bound
    output_file << "COLOR blue" << endl;
    output_file << "SR " << GridOrigin.x << " -" << GridOrigin.y << " "
     << GridOrigin.x + (grid_x_num * TileSize.x) << " -" << GridOrigin.y + (grid_y_num * TileSize.y) << endl;

    // cross 四等份
    int line_width = 3;
    output_file << "COLOR black" << endl;
    output_file << "SR " << GridOrigin.x + (grid_x_num * TileSize.x) / 2 << " -" << GridOrigin.y << " "
     << GridOrigin.x + (grid_x_num * TileSize.x) / 2 + line_width << " -" << GridOrigin.y + (grid_y_num * TileSize.y) << endl;
    output_file << "COLOR black" << endl;
    output_file << "SR " << GridOrigin.x << " -" << GridOrigin.y + (grid_y_num * TileSize.y) / 2 << " "
     << GridOrigin.x + (grid_x_num * TileSize.x) << " -" << GridOrigin.y + (grid_y_num * TileSize.y) / 2 + line_width << endl;

    coor grid_lb_temp;
    double grid_color = 0.;

    // grid
    for(int i = 0; i < (int)grid_array.size(); i++) {
        for(int j = 0; j < (int)grid_array[i].size(); j++) {
            grid_lb_temp = {GridOrigin.x + (TileSize.x * j), GridOrigin.y + (TileSize.y * i)};

            if(H_or_V == 0) grid_color = grid_array[i][j].total_H_overflow / max_grid_H_overflow;
            else if (H_or_V == 1) grid_color = grid_array[i][j].total_V_overflow / max_grid_V_overflow;
            else grid_color = (grid_array[i][j].total_H_overflow + grid_array[i][j].total_V_overflow) / (max_grid_H_overflow + max_grid_V_overflow);

            // 不吃色碼
            if(1 >= grid_color && grid_color > 0.9) output_file << "COLOR black" << endl;
            else if(0.9 >= grid_color && grid_color > 0.8) output_file << "COLOR pink" << endl;
            else if(0.8 >= grid_color && grid_color > 0.7) output_file << "COLOR red" << endl;
            else if(0.7 >= grid_color && grid_color > 0.6) output_file << "COLOR orange" << endl;
            else if(0.6 >= grid_color && grid_color > 0.5) output_file << "COLOR yellow" << endl;
            else if(0.4 >= grid_color && grid_color > 0.2) output_file << "COLOR green" << endl;
            else output_file << "COLOR gray" << endl;
            
            output_file << "SR " << grid_lb_temp.x << " -" << grid_lb_temp.y << " "
            << grid_lb_temp.x + TileSize.x << " -" << grid_lb_temp.y + TileSize.y << endl;
        }
    }

    output_file.close();
}

void CE::dpx_block_output(string file_name){
    // COLOR blue
    // SR 18404 -16434 20323 -16443
    // SL  18404 -16434 20323 -16443
    // SL  18404 -16443 20323 -16434
    // COLOR black
    // SR  0 -0 22482 -16479

    ofstream output_file(file_name + ".dpx", ios::out);

    // y 要加負號

    // bound
    output_file << "COLOR blue" << endl;
    output_file << "SR " << GridOrigin.x << " -" << GridOrigin.y << " "
     << GridOrigin.x + (grid_x_num * TileSize.x) << " -" << GridOrigin.y + (grid_y_num * TileSize.y) << endl;

    // cross 四等份
    int line_width = 3;
    output_file << "COLOR black" << endl;
    output_file << "SR " << GridOrigin.x + (grid_x_num * TileSize.x) / 2 << " -" << GridOrigin.y << " "
     << GridOrigin.x + (grid_x_num * TileSize.x) / 2 + line_width << " -" << GridOrigin.y + (grid_y_num * TileSize.y) << endl;
    output_file << "COLOR black" << endl;
    output_file << "SR " << GridOrigin.x << " -" << GridOrigin.y + (grid_y_num * TileSize.y) / 2 << " "
     << GridOrigin.x + (grid_x_num * TileSize.x) << " -" << GridOrigin.y + (grid_y_num * TileSize.y) / 2 + line_width << endl;

    // grid
    for(int i = 0; i < (int)node_array.size(); i++) {
        if(node_array[i].block) { // block node

            output_file << "COLOR black" << endl;
            
            output_file << "SR " << node_array[i].lb.x << " -" << node_array[i].lb.y << " "
            << node_array[i].lb.x + node_array[i].W << " -" << node_array[i].lb.y + node_array[i].H << endl;
        }
    }

    output_file.close();
}

