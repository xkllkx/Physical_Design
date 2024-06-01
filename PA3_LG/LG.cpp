#include<cmath>

#include "LG.h"

using namespace std;

bool debug = 0;
bool check_move = 0;

// 可調參數
int window_offset = 5;  //  it上下幾個row
double up_hill_ratio = 0;

// TOOL -------------------------------------------------------------------

void LG::set_show(set<int> show_set) {
    for (auto &unit : show_set) {
        log_file << unit << " ";
    }
    log_file << endl;
}

void LG::vector_show(vector<int> show_vector) {
    for (unsigned int i = 0; i < show_vector.size(); i++) {
        log_file << show_vector[i] << " ";
    }
    log_file << endl;
}

int LG::vector_find(vector<int> find_vector, int temp) {
    for (unsigned int i = 0; i < find_vector.size(); i++) {
        if (find_vector[i] == temp) return i;
    }
    return -1;
}

// INPUT -------------------------------------------------------------------

void LG::scl_input(string file_name) { // row資訊
    ifstream input_file(file_name + ".scl", ios::in);

    string input_temp5, input_temp4, input_temp3, input_temp2, input_temp1, input_temp;

    log_file << "============ SCL ==========" << endl;

    for(int i = 0; i < 4; i++) {
        getline(input_file, input_temp); // 空行
    }

    input_file >> input_temp2 >> input_temp1 >> input_temp; // NumRows
    NumRows = stoi(input_temp);
    log_file << "NumRows : " << NumRows << endl;

    getline(input_file, input_temp); // 空行
    getline(input_file, input_temp); // 空行 // 不知為什麼多

    row_array.resize(NumRows);
    int row_y = 0;

    for(int i = 0; i < NumRows; i++) {
        row_array[i].index = i;

        getline(input_file, input_temp); // CoreRow Horizontal
        // cout << input_temp << endl;

        input_file >> input_temp2 >> input_temp1 >> input_temp;
        // cout << input_temp << endl;
        row_y = stoi(input_temp);

        input_file >> input_temp2 >> input_temp1 >> input_temp;
        // cout << input_temp << endl;
        if(!Row_H) Row_H = stoi(input_temp);

        input_file >> input_temp2 >> input_temp1 >> input_temp; // Sitewidth
        input_file >> input_temp2 >> input_temp1 >> input_temp;
        // cout << input_temp << endl;
        // if(!Sitespacing) Sitespacing = stoi(input_temp);
        input_file >> input_temp2 >> input_temp1 >> input_temp; // Siteorient
        input_file >> input_temp2 >> input_temp1 >> input_temp; // Sitesymmetry

        input_file >> input_temp5 >> input_temp4 >> input_temp3 >> input_temp2 >> input_temp1 >> input_temp;
        // cout << input_temp3 << endl;
        // cout << input_temp << endl;
        row_array[i].lb = {stoi(input_temp3), row_y};
        // row_array[i].W = Sitespacing * stoi(input_temp);
        row_array[i].W = stoi(input_temp);

        getline(input_file, input_temp); // 空行 // 不知為什麼多

        getline(input_file, input_temp); // End
        // cout << input_temp << endl;

        // cout << "----------" << endl;
    }

    input_file.close();

    log_file << "Numrow_array : " << row_array.size() << endl;
    // log_file << "bound : (" << row_x_min << " , " << row_x_max << ")"<<  endl;
    log_file << "----------" << endl;

    if(debug) row_show(0);
}

void LG::node_input(string file_name) {
    ifstream input_file(file_name + ".nodes", ios::in);

    string input_temp2, input_temp1, input_temp;

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

    for(int i = 0; i < (int)node_array.size(); i++) {
        input_file >> input_temp >> input_temp1 >> input_temp2;

        node_array[i].node_name = input_temp;
        node_array[i].index = i;

        node_map.insert(pair<string, int>(input_temp, i));

        node_array[i].W = stoi(input_temp1);
        node_array[i].H = stoi(input_temp2);
        
        if(i >= NumMovableNodes) {
            input_file >> input_temp; // movetype

            if(input_temp == "terminal") {
                node_array[i].movetype = 1;
                NumTerminal ++;
            }
            else {
                node_array[i].movetype = 2; // terminal_NI
                NumTerminal_NI ++;
            }
        }
    }

    input_file.close();

    log_file << "Numnode_array : " << node_array.size() << endl;
    log_file << "NumMovableNodes : " << NumMovableNodes << endl;
    log_file << "NumTerminal : " << NumTerminal << endl;
    log_file << "NumTerminal_NI : " << NumTerminal_NI << endl << endl;
    log_file << "----------" << endl;

    // if(debug) node_show(765596); // node_map["A1/B1/C1/D1/E42/F4/G3/H2/o765596"]
    if(debug) node_show(node_map["A1/B1/C1/D1/E42/F4/G3/H2/o765596"]);
}

bool node_x_compare(node &a, node &b){ // 由小到大
    if(a.lb.x != b.lb.x) return a.lb.x < b.lb.x;
    else return a.lb.y < b.lb.y;
}

bool node_y_compare(node &a, node &b){ // 由小到大
    if(a.lb.y != b.lb.y) return a.lb.y < b.lb.y;
    else return a.lb.x < b.lb.x;
}

void LG::legal_input(string file_name) {
    ifstream input_file(file_name + ".legal", ios::in);

    string input_temp4, input_temp3, input_temp2, input_temp1, input_temp;

    log_file << "============ LEGAL ==========" << endl;

    for(int i = 0; i < 4; i++) {
        getline(input_file, input_temp); // 空行
    }

    int row_index = 0, in_row_num = 0;

    for(int i = 0; i < (int)node_array.size(); i++) {
        input_file >> input_temp >> input_temp4 >> input_temp3 >> input_temp2 >> input_temp1;

        node_array[i].lb = {stoi(input_temp4), stoi(input_temp3)};
        node_array[i].lb_temp = node_array[i].lb;

        // 先把 node 歸進 最近row
        if(node_array[i].movetype != 2) { // termianl_NI 不用歸進 row
            row_index = row_find(0, i);
            if(row_index == -1) {
                cout << node_array[i].node_name << " no row" << endl;
                log_file << node_array[i].node_name << " no row" << endl;
                return;
            }
            else {
                node_array[i].row_index = row_index;
                in_row_num ++;
            }
        }

        if(input_temp1 != "N") node_array[i].orient = 1;
    }

    input_file.close();

    // sort(node_array.begin(), node_array.end(),node_x_compare); // 照x排
    sort(node_array.begin(), node_array.end(),node_y_compare); // 照y排 // 為了row initial

    // node 重新編號
    for(int i = 0; i < (int)node_array.size(); i++) {
        node_array[i].index = i;
        node_map[node_array[i].node_name] = i;

        if(!node_array[i].movetype) move_node_order.push_back(i);
    }

    log_file << "Numinitial_movable_node : " << move_node_order.size() << endl;
    log_file << endl;
    log_file << "NumInRow : " << in_row_num << " (" << NumMovableNodes + NumTerminal << ")" << endl; // MovableNodes + Terminal
    log_file << "----------" << endl;

    // if(debug) node_show(765596); // node_map["A1/B1/C1/D1/E42/F4/G3/H2/o765596"]
    if(debug) node_show(node_map["A1/B1/C1/D1/E42/F4/G3/H2/o765596"]);
}

void LG::shape_input(string file_name) { // 非矩形node資訊
    ifstream input_file(file_name + ".shapes", ios::in);

    string input_temp4, input_temp3, input_temp2, input_temp1, input_temp;

    log_file << "============ SHAPE ==========" << endl;

    for(int i = 0; i < 4; i++) {
        getline(input_file, input_temp); // 空行
    }

    input_file >> input_temp2 >> input_temp1 >> input_temp; // NumNonRectangularNodes
    NumNonRectangularNodes = stoi(input_temp);
    log_file << "NumNonRectangularNodes : " << NumNonRectangularNodes << endl;

    getline(input_file, input_temp); // 空行

    int shape_num = 0, node_index = 0, non_rect_num = 0;
    map<string, int>::iterator iter;

    while(input_file >> input_temp) {
        input_file >> input_temp2 >> input_temp1;
        shape_num = stoi(input_temp1);

        iter = node_map.find(input_temp);

        if(iter != node_map.end()){
            node_index = iter->second;

            node_array[node_index].shape.resize(shape_num);

            for(int j = 0; j < shape_num; j++) {
                input_file >> input_temp4 >> input_temp3 >> input_temp2 >> input_temp1 >> input_temp;
                
                node_array[node_index].shape[j].host = node_index;
                node_array[node_index].shape[j].lb = {stoi(input_temp3), stoi(input_temp2)};

                node_array[node_index].shape[j].W = stoi(input_temp1);
                node_array[node_index].shape[j].H = stoi(input_temp);
            }
        }
        else continue;

        non_rect_num ++;
    }

    input_file.close();

    log_file << "NumNonRect : " << non_rect_num << endl;
    log_file << "----------" << endl;

    if(debug) {
        // int test = 765596; // node_map["A1/B1/C1/D1/E42/F4/G3/H2/o765596"]
        int test = node_map["A1/B1/C1/D1/E42/F4/G3/H2/o765596"];

        log_file << "node_name : " << node_array[test].node_name << endl;
        log_file << "index : " << node_array[test].index << endl;
        log_file << "row_index : " << node_array[test].row_index << endl;
        log_file << "lb : (" << node_array[test].lb.x << ", " << node_array[test].lb.y << ")" << endl;
        log_file << "W : " << node_array[test].W << "\tH : " << node_array[test].H << endl;
        log_file << "orient : " << node_array[test].orient << endl;
        log_file << "movetype : " << node_array[test].movetype << endl;
        log_file << "net_index_num : " << node_array[test].net_index.size() << endl;
        log_file << "shape_num : " << node_array[test].shape.size() << endl;
        if(node_array[test].shape.size()) {
            log_file << "shape_lb : (" << node_array[test].shape[0].lb.x << ", " << node_array[test].shape[0].lb.y << ")" << endl;
            log_file << "shape_W : " << node_array[test].shape[0].W << "\tshape_H : " << node_array[test].shape[0].H << endl;
        }
        log_file << "----------" << endl;
    }
}

void LG::net_input(string file_name) {
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

    int pin_num = 0, node_index = 0, i = 0, node_repeat = 0;
    map<string, int>::iterator iter;

    while(input_file >> input_temp) {
        input_file >> input_temp3 >> input_temp2 >> input_temp1;
        pin_num = stoi(input_temp2);

        net_array[i].net_name = input_temp1;
        net_array[i].index = i;

        for(int j = 0; j < pin_num; j++) {
            input_file >> input_temp4 >> input_temp3 >> input_temp2 >> input_temp1 >> input_temp;
            
            iter = node_map.find(input_temp4);

            if(iter != node_map.end()){
                node_index = iter->second;

                // offset要理
                // 檢查自連
                node_repeat = vector_find(net_array[i].node_index, node_index);
                if(node_repeat != -1) {
                    // 有自連 取offset小的
                    if(abs(net_array[i].node_x_offset[node_repeat]) + abs(net_array[i].node_y_offset[node_repeat]) > abs(stod(input_temp1)) + abs(stod(input_temp))) {
                        net_array[i].node_x_offset[node_repeat] = stod(input_temp1);
                        net_array[i].node_y_offset[node_repeat] = stod(input_temp);
                    }
                }
                else { // 沒重複 不自連
                    net_array[i].node_index.push_back(node_index);
                    net_array[i].node_x_offset.push_back(stod(input_temp1));
                    net_array[i].node_y_offset.push_back(stod(input_temp));
                }

                // 紀錄node 接了哪些net
                node_array[node_index].net_index.insert(i);
            }
            else continue;
        }

        i++;
    }

    input_file.close();

    log_file << "Numnet_array : " << net_array.size() << endl;
    log_file << "----------" << endl;

    if(debug) {
        // net_show(0); // int test = 6970; // net_map["n6970"] // 有自連
        node_show(node_map["o816034"]); // node_map["o816034"] // 檢查node_index node_weight
    }
}

// SHOW -------------------------------------------------------------------

void LG::row_show(int show_row_index) {
    log_file << "row_index : " << row_array[show_row_index].index << endl;
    log_file << "lb : (" << row_array[show_row_index].lb.x << ", " << row_array[show_row_index].lb.y << ")" << endl;
    log_file << "W : " << row_array[show_row_index].W << "\tH : " << Row_H << endl;
    // log_file << "Sitespacing : " << Sitespacing << endl;
    
    log_file << "node_cluster_num : " << row_array[show_row_index].node_list.size() << endl;
    if(row_array[show_row_index].node_list.size()) {
        log_file << "----------" << endl;
        node_show(row_array[show_row_index].node_list.front().host);
    }
    
    log_file << "----------" << endl;
}

void LG::row_list_show(int show_row_index) {
    log_file << "============ ROW " << row_array[show_row_index].index << " ============" << endl;
    
    for (auto &tar_sub_node : row_array[show_row_index].node_list) {
        log_file << node_array[tar_sub_node.host].node_name << endl;

        if(node_array[tar_sub_node.host].shape.size()) log_file << "not rec" << endl;
        
        log_file << "lb : (" << node_array[tar_sub_node.host].lb.x << ", " << node_array[tar_sub_node.host].lb.y << ")" << endl;
        log_file << "sub_node_lb : (" << tar_sub_node.lb.x << ", " << tar_sub_node.lb.y << ")" << endl;
        log_file << "W : " << node_array[tar_sub_node.host].W << endl;
        log_file << "W : " << tar_sub_node.W << endl;
        log_file << "----------" << endl;
    }
}

void LG::net_show(int show_net_index) {
    log_file << "net_name : " << net_array[show_net_index].net_name << endl;
    log_file << "index : " << net_array[show_net_index].index << endl;
    log_file << "node_index_num : " << net_array[show_net_index].node_index.size() << endl;
    log_file << "node_x_offset_num : " << net_array[show_net_index].node_x_offset.size() << endl;
    log_file << "node_y_offset_num : " << net_array[show_net_index].node_y_offset.size() << endl;
    
    log_file << "HPWL : " << net_array[show_net_index].HPWL << endl;
    log_file << "x_boundary : (" << net_array[show_net_index].x_min << ", " << net_array[show_net_index].x_max << ")" << endl;
    log_file << "y_boundary : (" << net_array[show_net_index].y_min << ", " << net_array[show_net_index].y_max << ")" << endl << endl;

    log_file << "node_offset : " << endl;
    for(int i = 0; i < (int)net_array[show_net_index].node_index.size(); i++) {
        log_file << node_array[net_array[show_net_index].node_index[i]].node_name << " (" << net_array[show_net_index].node_x_offset[i] << ", " << net_array[show_net_index].node_y_offset[i] << ")" << endl;
    }
    log_file << "----------" << endl;
}

void LG::node_show(int show_node_index) {
    log_file << "node_name : " << node_array[show_node_index].node_name << endl;
    log_file << "index : " << node_array[show_node_index].index << endl;
    log_file << "row_index : " << node_array[show_node_index].row_index << endl;
    log_file << "lb : (" << node_array[show_node_index].lb.x << ", " << node_array[show_node_index].lb.y << ")" << endl;
    log_file << "W : " << node_array[show_node_index].W << "\tH : " << node_array[show_node_index].H << endl;
    log_file << "orient : " << node_array[show_node_index].orient << endl;
    log_file << "movetype : " << node_array[show_node_index].movetype << endl;
    log_file << "net_index_num : " << node_array[show_node_index].net_index.size() << endl;
    
    log_file << "shape_num : " << node_array[show_node_index].shape.size() << endl;
    log_file << "----------" << endl;
}


// HPWL_cal -------------------------------------------------------------------

void LG::total_HPWL_cal(bool initial, bool temp, bool recal) {
    // temp:1 用lb_temp // 用temp 一律不紀錄 // recal:1 強制重算
    log_file << "============ TOTAL HPWL_CAL ==========" << endl;
    cout << "============ TOTAL HPWL_CAL ==========" << endl;

    double x_min = (double)INT_MAX, x_max = 0., y_min = (double)INT_MAX, y_max = 0.;
    double node_x_temp = 0., node_y_temp = 0.;
    int j = 0;

    double HPWL_temp = 0.;

    if(initial) {
        temp = 0;
        recal = 1;
    }

    // cout << x_max << " " <<  x_min << " " << y_max << " " << y_min << endl;

    for(int i = 0; i < (int)net_array.size(); i++) {
        if(net_array[i].node_index.size() <= 1) continue; // net 沒有 node 或是 只有一個node 沒有HPWL

        if(initial || net_array[i].HPWL < 0. || recal) {
            x_min = (double)INT_MAX;
            x_max = 0.;
            y_min = (double)INT_MAX;
            y_max = 0.;

            j = 0;

            for (auto &unit : net_array[i].node_index) {
                if(temp) {
                    node_x_temp = node_array[unit].lb_temp.x + node_array[unit].W/2. + net_array[i].node_x_offset[j]; // 中心 + offset
                    node_y_temp = node_array[unit].lb_temp.y + node_array[unit].H/2. + net_array[i].node_y_offset[j]; // 中心 + offset
                }
                else {
                    node_x_temp = node_array[unit].lb.x + node_array[unit].W/2. + net_array[i].node_x_offset[j]; // 中心 + offset
                    node_y_temp = node_array[unit].lb.y + node_array[unit].H/2. + net_array[i].node_y_offset[j]; // 中心 + offset
                }

                if(x_min > node_x_temp) x_min = node_x_temp;
                if(x_max < node_x_temp) x_max = node_x_temp;
                if(y_min > node_y_temp) y_min = node_y_temp;
                if(y_max < node_y_temp) y_max = node_y_temp;

                j++;
            }

            HPWL_temp += (x_max - x_min) + (y_max - y_min);

            if(!temp) { // 用temp 一律不紀錄
                net_array[i].HPWL = (x_max - x_min) + (y_max - y_min);
            
                net_array[i].x_min = x_min;
                net_array[i].x_max = x_max;
                net_array[i].y_min = y_min;
                net_array[i].y_max = y_max;
            }
        }
        else HPWL_temp += net_array[i].HPWL;  // HPWL已完成計算
    }

    total_HPWL = round(HPWL_temp);
    if(initial) initial_HPWL = total_HPWL;
    
    // log_file << "HPWL_temp : " << setprecision(15) << HPWL_temp << endl;
    if(initial) log_file << "initial_HPWL : " << initial_HPWL << endl;
    log_file << "total_HPWL : " << total_HPWL << " (" << (double)total_HPWL/(double)initial_HPWL*100. << " %)" << endl;
    cout << "total_HPWL : " << total_HPWL << " (" << (double)total_HPWL/(double)initial_HPWL*100. << " %)" << endl;
    log_file << "----------" << endl;

    if(initial && debug) net_show(0);
}

double LG::node_HPWL_cal(int target_node_index, bool recal) { // 用 lb_temp 算
    // log_file << "============ NODE HPWL_CAL ==========" << endl;

    double x_min = (double)INT_MAX, x_max = 0., y_min = (double)INT_MAX, y_max = 0.;
    double node_x_temp = 0., node_y_temp = 0.;
    int j = 0;

    double HPWL_temp = 0.;
    bool recal_x = 0, recal_y = 0;

    for (auto &net_index : node_array[target_node_index].net_index) {
        if(net_array[net_index].node_index.size() <= 1) continue; // net 沒有 node 或是 只有一個node 沒有HPWL

        x_min = (double)INT_MAX;
        x_max = 0.;
        y_min = (double)INT_MAX;
        y_max = 0.;

        j = 0;

        recal_x = 0;
        recal_y = 0;

        // 判斷是否重新計算
        if(!recal) { // 不一定recal // 剛好在邊界才recal
            // x 重算
            // if(node_array[target_node_index].lb_temp.x > net_array[net_index].x_max || 
            // node_array[target_node_index].lb_temp.x < net_array[net_index].x_min) recal_x = 1;

            recal_x = 1;

            // y 重算
            if(abs(net_array[net_index].y_max - row_array[node_array[target_node_index].row_index].lb.y) > Row_H || 
            abs(row_array[node_array[target_node_index].row_index].lb.y - net_array[net_index].y_min) > Row_H ) recal_y = 1;
        }
        else {
            recal_x = 1;
            recal_y = 1;
        }
    
        // 計算HPWL
        if(recal_x || recal_y || net_array[net_index].HPWL < 0.) {
            for (auto &unit : net_array[net_index].node_index) {
                if(recal_x) {
                    node_x_temp = node_array[unit].lb_temp.x + node_array[unit].W/2. + net_array[net_index].node_x_offset[j]; // 中心 + offset
                    
                    if(x_min > node_x_temp) x_min = node_x_temp;
                    if(x_max < node_x_temp) x_max = node_x_temp;
                }

                if(recal_y) {
                    node_y_temp = node_array[unit].lb_temp.y + node_array[unit].H/2. + net_array[net_index].node_y_offset[j]; // 中心 + offset
                    
                    if(y_min > node_y_temp) y_min = node_y_temp;
                    if(y_max < node_y_temp) y_max = node_y_temp;
                }

                j++;
            }

            if(!recal_x) {
                x_min = net_array[net_index].x_min;
                x_max = net_array[net_index].x_max;
            }
            if(!recal_y) {
                y_min = net_array[net_index].y_min;
                y_max = net_array[net_index].y_max;
            }

            HPWL_temp += (x_max - x_min) + (y_max - y_min);

            if(recal_x || recal_y) {
                net_array[net_index].HPWL = (x_max - x_min) + (y_max - y_min);

                if(recal_x) {
                    net_array[net_index].x_max = x_max;
                    net_array[net_index].x_min = x_min;
                }
                if(recal_y) {
                    net_array[net_index].y_max = y_max;
                    net_array[net_index].y_min = y_min;
                }
            }
        }
        else HPWL_temp += net_array[net_index].HPWL;  // HPWL已完成計算
    }

    // log_file << "HPWL_temp : " << setprecision(15) << HPWL_temp << endl;
    return HPWL_temp;
}

// ALG -------------------------------------------------------------------

void LG::row_list_initial(){
    log_file << "============ ROW_LIST INITIAL ==========" << endl;

    list<sub_node>::iterator it;

    int row_counter = 0;
    int row_index_temp = 0;

    for(int i = 0; i < (int)node_array.size(); i++) {
        if(node_array[i].movetype == 2) continue; // termianl_NI 不用歸進 row

        if(!node_array[i].shape.size()) {
            for(int j = 0; j < (int)node_array[i].H / Row_H; j++) { // multi row height
                row_index_temp = node_array[i].row_index + j;

                it = row_index_binary_find(row_index_temp, node_array[i].lb.x + node_array[i].W);

                row_array[row_index_temp].node_list.insert(it, {i, {node_array[i].lb.x, row_array[row_index_temp].lb.y}, node_array[i].W, Row_H});

                // log_file << "----------" << endl;
            }
        }
        else { // 非矩形node
            for(int j = 0; j < (int)node_array[i].shape.size(); j++) {
                row_index_temp = node_array[i].row_index + j;
                
                it = row_index_binary_find(row_index_temp, node_array[i].shape[j].lb.x + node_array[i].shape[j].W);

                row_array[row_index_temp].node_list.insert(it, node_array[i].shape[j]);

                // log_file << "----------" << endl;
            }
        }

        row_counter ++;
    }

    log_file << "row_counter : " << row_counter << endl;
    log_file << "----------" << endl;

    if(debug) {
        row_show(0);
        check_overlap(); // 檢查整個布局
    }
}

int LG::row_find(bool temp, int target_node_index){ // temp = 1 用 temp
    int Row_y_min = row_array[0].lb.y, Row_y_max = row_array.back().lb.y;
    int row_index = 0;

    if(temp) {
        if(node_array[target_node_index].lb_temp.y >= Row_y_min && node_array[target_node_index].lb_temp.y <= Row_y_max) {
            if(node_array[target_node_index].lb_temp.y - Row_y_min) row_index = (node_array[target_node_index].lb_temp.y - Row_y_min) / Row_H;
            else row_index = 0; // 最底列

            if(node_array[target_node_index].lb_temp.x >= row_array[row_index].lb.x && node_array[target_node_index].lb_temp.x <= row_array[row_index].lb.x + row_array[row_index].W) {
                return row_index;
            }
            else {
                log_file << "no row" << endl;
                return -1;
            }
        }
        else {
            log_file << "no row" << endl;
            return -1;
        }
    }
    else {
        if(node_array[target_node_index].lb.y >= Row_y_min && node_array[target_node_index].lb.y <= Row_y_max) {
            if(node_array[target_node_index].lb.y - Row_y_min) row_index = (node_array[target_node_index].lb.y - Row_y_min) / Row_H;
            else row_index = 0; // 最底列

            if(node_array[target_node_index].lb.x >= row_array[row_index].lb.x && node_array[target_node_index].lb.x <= row_array[row_index].lb.x + row_array[row_index].W) {
                return row_index;
            }
            else {
                log_file << "no row" << endl;
                return -1;
            }
        }
        else {
            log_file << "no row" << endl;
            return -1;
        }
    }

    return row_index;
}

bool LG::check_overlap(){
    log_file << "============ NEW CHECK OVERLAP ==========" << endl;
    cout << "============ NEW CHECK OVERLAP ==========" << endl;

    list<sub_node>::iterator it;

    for(int i = 0; i < (int)row_array.size(); i++) {

        it = row_array[i].node_list.begin();

        while(next(it) != row_array[i].node_list.end()) {
            if((*it).lb.y != row_array[i].lb.y) {
                log_file << node_array[(*it).host].node_name;
                log_file << " (" << (*it).lb.x << ", " << (*it).lb.y << ") Y FALSE" << endl;
                cout << node_array[(*it).host].node_name;
                cout << " (" << (*it).lb.x << ", " << (*it).lb.y << ") Y FALSE" << endl;
                return 0;
            }

            // log_file << (*it).lb.x << " " << row_cursor_temp << endl;

            if((*next(it)).lb.x < (*it).lb.x + (*it).W) {
                log_file << "OVERLAP" << endl << endl;
                cout << "OVERLAP" << endl << endl;
                
                log_file << "node : " << node_array[(*it).host].node_name << endl;
                log_file << "index : " << (*it).host << endl;
                log_file << "lb : (" << (*it).lb.x << ", " << (*it).lb.y << ")" << endl;
                log_file << "W : " << (*it).W << "\tH : " << (*it).H << endl << endl;

                node_show((*it).host);

                log_file << "node : " << node_array[(*next(it)).host].node_name << endl;
                log_file << "index : " << (*next(it)).host << endl;
                log_file << "lb : (" << (*next(it)).lb.x << ", " << (*next(it)).lb.y << ")" << endl;
                log_file << "W : " << (*next(it)).W << "\tH : " << (*next(it)).H << endl << endl;
                
                node_show((*next(it)).host);

                return 0;
            }

            it = next(it);
        }
    }

    log_file << "NO OVERLAP" << endl;
    log_file << "----------" << endl;
    cout << "NO OVERLAP" << endl;
    cout << "----------" << endl;
    return 1;
}

list<sub_node>::iterator LG::row_index_find(int row_index, int lb_x){ // 回傳最近一個 比lb.x 大的 node
    list<sub_node>::iterator it = row_array[row_index].node_list.begin();

    while(it != row_array[row_index].node_list.end() && (*it).lb.x < lb_x) {
        it = next(it);
    }

    return it;
}

list<sub_node>::iterator LG::row_index_binary_find(int row_index, int lb_x){ // 回傳最近一個 比lb.x 大的 node
    // lower_bound >=       // upper_bound >
    list<sub_node>::iterator it = lower_bound(row_array[row_index].node_list.begin(), row_array[row_index].node_list.end(), lb_x, 
        [](const sub_node& tar_sub_node, int value) {
            return tar_sub_node.lb.x < value;
        }
    );

    return it;
}

void LG::min_HPWL_position_cal(int target_node_index){ // 用 lb_temp 計算 node HPWL最小處
    // log_file << "============ MIN HPWL POSITION CAL ==========" << endl;

    int ori_row_index = node_array[target_node_index].row_index;
    double ori_HPWL = node_HPWL_cal(target_node_index, 1);

    if(check_move) log_file << node_array[target_node_index].node_name << endl;

    if(debug) {
        log_file << "ori_lb : (" << node_array[target_node_index].lb.x << ", " << node_array[target_node_index].lb.y << ")" << endl;
        log_file << "W : " << node_array[target_node_index].W << endl;
        log_file << "ori_row : " << ori_row_index << endl;
        log_file << "ori_HPWL : " << ori_HPWL << endl;
        log_file << "----------" << endl;
    }

    // 從ori_list中拔除node
    if(debug) log_file << "row_node_num : " << row_array[node_array[target_node_index].row_index].node_list.size() << " -> ";
    
    row_array[node_array[target_node_index].row_index].node_list.remove_if([target_node_index] (auto &tar_sub_node) {return tar_sub_node.host == target_node_index;});
    
    if(debug) {
        log_file << row_array[node_array[target_node_index].row_index].node_list.size() << endl;
        log_file << "----------" << endl;
    }


    int row_index = -1;
    int place_x = -1;
    double min_HPWL = ori_HPWL;

    // 找 node min_HPWL處
    for(int i = ori_row_index - window_offset; i <= ori_row_index + window_offset; i++) {
        if(i < 0 || i > (int)row_array.size()-1) continue;

        node_array[target_node_index].lb_temp.y = row_array[i].lb.y;

        node_X_slide(i, target_node_index, place_x, row_index, min_HPWL);
    }

    if(place_x == -1) {
        row_index = ori_row_index;
        node_array[target_node_index].row_index = ori_row_index; // 預防
        node_array[target_node_index].lb_temp = node_array[target_node_index].lb;

        if(check_move) log_file << "HPWL no improve" << endl;
    }
    else {
        // 放入新row
        node_array[target_node_index].row_index = row_index;

        // 更改node lb
        node_array[target_node_index].lb_temp = {place_x, row_array[row_index].lb.y};
        node_array[target_node_index].lb = node_array[target_node_index].lb_temp;

        min_HPWL = node_HPWL_cal(target_node_index, 1);

        if(check_move) {
            log_file << "final_lb : (" << node_array[target_node_index].lb.x << ", " << node_array[target_node_index].lb.y << ")" << endl;
            log_file << "final_row : " << row_index << endl;
            log_file << "final_HPWL : " << min_HPWL;
            log_file << " (" << (double)min_HPWL/(double)ori_HPWL*100. << " %)" << endl;
            log_file << "----------" << endl;
        }
    }

    // node insert
    // list<sub_node>::iterator it = row_index_find(row_index, node_array[target_node_index].lb.x + node_array[target_node_index].W);
    list<sub_node>::iterator it = row_index_binary_find(row_index, node_array[target_node_index].lb.x + node_array[target_node_index].W);

    // log_file << "lb_node : " << node_array[(*it).host].node_name << endl;
    // log_file << "lb : (" << (*it).lb.x << ", " << (*it).lb.y << ")" << endl;
    // log_file << "lb_W : " << (*it).W << endl << endl;

    // log_file << "prev_lb_node : " << node_array[(*prev(it)).host].node_name << endl;
    // log_file << "prev_lb : (" << (*prev(it)).lb.x << ", " << (*prev(it)).lb.y << ")" << endl;
    // log_file << "prev_lb_W : " << (*prev(it)).W << endl << endl;

    if(debug) log_file << "row_node_num : " << row_array[row_index].node_list.size() << " -> ";

    row_array[row_index].node_list.insert(it, {target_node_index, node_array[target_node_index].lb, node_array[target_node_index].W, node_array[target_node_index].H});
    
    if(debug) {
        log_file << row_array[row_index].node_list.size() << endl;
        log_file << "----------" << endl;
    }
}

bool sub_node_delete(sub_node delete_sub_node, int target_node_index) {
    return delete_sub_node.host == target_node_index;
}

void LG::node_X_slide(int target_row_index, int target_node_index, int &place_x, int &row_index, double &min_HPWL) { // 這個node在這個row能放的位置
    int row_x_min = row_array[target_row_index].lb.x;
    int row_x_max = row_array[target_row_index].lb.x + row_array[target_row_index].W;

    int node_W = node_array[target_node_index].W;

    int bound_x_min = -1, bound_x_max = -1;
    bool HPWL_improve = 0, list_end = 0;

    double HPWL_temp = -1;
    double HPWL_start = -1 ,HPWL_mid = -1, HPWL_end = -1;

    double left_min_HPWL = min_HPWL, right_min_HPWL = min_HPWL;
    double left_place_x = place_x, right_place_x = place_x;

    // list<sub_node>::iterator it = row_index_find(target_row_index, node_array[target_node_index].lb_temp.x + node_W);
    list<sub_node>::iterator it = row_index_binary_find(target_row_index, node_array[target_node_index].lb_temp.x + node_W);
    list<sub_node>::iterator left_it = it, right_it = it;

    // log_file << "lb_node : " << node_array[*it].index << endl;
    // log_file << "lb : (" << node_array[*it].lb.x << ", " << node_array[*it].lb.y << ")" << endl;
    // log_file << "lb_temp : (" << node_array[*it].lb_temp.x << ", " << node_array[*it].lb_temp.y << ")" << endl;
    // log_file << "lb_W : " << node_array[*it].W << endl << endl;

    // 往右移
    while(1) {
        // 框出滑動邊界
        if(right_it == prev(row_array[target_row_index].node_list.end())) {
            bound_x_max = row_x_max;
            bound_x_min = (*right_it).lb.x + (*right_it).W;

            list_end = 1;
        }
        else if(right_it == row_array[target_row_index].node_list.begin()) {
            bound_x_max = (*right_it).lb.x;
            bound_x_min = row_x_min;
        }
        else {
            bound_x_max = (*next(right_it)).lb.x;
            bound_x_min = (*right_it).lb.x + (*right_it).W;
        }

        // log_file << "bound_x : (" << bound_x_min << ", " << bound_x_max << ")" << endl;

        if(bound_x_max - bound_x_min < node_W) { // 放不進去
            if(!list_end) {
                right_it = next(right_it);
                node_array[target_node_index].lb_temp.x = (*right_it).lb.x + (*right_it).W;
            }
            else break;
        }
        else {
            // HPWL_start
            node_array[target_node_index].lb_temp.x = bound_x_max - node_W;
            HPWL_start = node_HPWL_cal(target_node_index, 1);

            // HPWL_mid
            node_array[target_node_index].lb_temp.x = bound_x_min + (bound_x_max - node_W - bound_x_min) / 2;
            HPWL_mid = node_HPWL_cal(target_node_index, 1);

            // HPWL_end
            node_array[target_node_index].lb_temp.x = bound_x_min;
            HPWL_end = node_HPWL_cal(target_node_index, 1);

            // 判斷最小
            if(HPWL_mid > HPWL_start && HPWL_mid > HPWL_end) { // 上凸
                if(HPWL_start < HPWL_end) { // 取start
                    node_array[target_node_index].lb_temp.x = bound_x_max - node_W;

                    if(HPWL_start < right_min_HPWL && 
                        node_array[target_node_index].lb_temp.x + node_W <= bound_x_max && node_array[target_node_index].lb_temp.x >= bound_x_min) {  // 可以放
                        
                        right_min_HPWL = HPWL_start;
                        right_place_x = bound_x_max - node_W;

                        HPWL_improve = 1;
                    }

                    if(HPWL_start > right_min_HPWL * (1. + up_hill_ratio)) list_end = 1;
                }
                else { // 取end
                    node_array[target_node_index].lb_temp.x = bound_x_min;

                    if(HPWL_end < right_min_HPWL && 
                        node_array[target_node_index].lb_temp.x + node_W <= bound_x_max && node_array[target_node_index].lb_temp.x >= bound_x_min) {  // 可以放
                        
                        right_min_HPWL = HPWL_end;
                        right_place_x = bound_x_min;

                        HPWL_improve = 1;
                    }

                    if(HPWL_end > right_min_HPWL * (1. + up_hill_ratio)) list_end = 1;
                }
            }
            else if(HPWL_end < HPWL_start && HPWL_end < HPWL_mid) { // HPWL_end 最小
                if(HPWL_end < right_min_HPWL && 
                    node_array[target_node_index].lb_temp.x + node_W <= bound_x_max && node_array[target_node_index].lb_temp.x >= bound_x_min) {  // 可以放
                    
                    right_min_HPWL = HPWL_end;
                    right_place_x = bound_x_min;

                    HPWL_improve = 1;
                }

                if(HPWL_end > right_min_HPWL * (1. + up_hill_ratio)) list_end = 1;
            }
            else if(HPWL_start < HPWL_end && HPWL_start < HPWL_mid) { // HPWL_start 最小
                if(HPWL_start < right_min_HPWL && 
                    node_array[target_node_index].lb_temp.x + node_W <= bound_x_max && node_array[target_node_index].lb_temp.x >= bound_x_min) {  // 可以放
                    
                    right_min_HPWL = HPWL_start;
                    right_place_x = bound_x_max - node_W;

                    HPWL_improve = 1;
                }

                if(HPWL_start > right_min_HPWL * (1. + up_hill_ratio)) list_end = 1;
            }
            else {  // HPWL_mid 最小
                // 凹點
                node_array[target_node_index].lb_temp.x = bound_x_min + (bound_x_max - node_W - bound_x_min) / 2;

                if(HPWL_mid < right_min_HPWL && 
                    node_array[target_node_index].lb_temp.x + node_W <= bound_x_max && node_array[target_node_index].lb_temp.x >= bound_x_min) {  // 可以放
                    
                    right_min_HPWL = HPWL_mid;
                    right_place_x = node_array[target_node_index].lb_temp.x;

                    HPWL_improve = 1;
                }

                if(HPWL_mid > right_min_HPWL * (1. + up_hill_ratio)) list_end = 1;
                else {
                    while(node_array[target_node_index].lb_temp.x + node_W <= bound_x_max) {
                        HPWL_temp = node_HPWL_cal(target_node_index, 0);
                        // log_file << "right_HPWL_temp : " << HPWL_temp << "(" << node_array[target_node_index].lb_temp.x << ")" << endl;

                        if(node_array[target_node_index].lb_temp.x + node_W <= bound_x_max && node_array[target_node_index].lb_temp.x >= bound_x_min) {  // 可以放
                            if(HPWL_temp < right_min_HPWL) {  // 可以放
                                right_min_HPWL = HPWL_temp;
                                right_place_x = node_array[target_node_index].lb_temp.x;

                                HPWL_improve = 1;
                            }
                            else {
                                if(HPWL_temp > right_min_HPWL * (1. + up_hill_ratio)) list_end = 1;
                                break;
                            }
                        }

                        node_array[target_node_index].lb_temp.x += node_W;
                    }
                }
            }

            right_it = next(right_it);
        }

        if(list_end) break;
    }

    // 往左移
    node_array[target_node_index].lb_temp.x = node_array[target_node_index].lb.x;
    list_end = 0;

    while(1) {
        // 框出滑動邊界
        if(left_it == row_array[target_row_index].node_list.end()) {
            bound_x_max = row_x_max;
            bound_x_min = (*prev(left_it)).lb.x + (*prev(left_it)).W;
        }
        else if(left_it == row_array[target_row_index].node_list.begin()) {
            bound_x_max = (*left_it).lb.x;
            bound_x_min = row_x_min;

            list_end = 1;
        }
        else {
            bound_x_max = (*left_it).lb.x;
            bound_x_min = (*prev(left_it)).lb.x + (*prev(left_it)).W;
        }

        // log_file << "bound_x : (" << bound_x_min << ", " << bound_x_max << ")" << endl;

        if(bound_x_max - bound_x_min < node_W) {  // 放不進去
            if(!list_end) {
                left_it = prev(left_it);
                node_array[target_node_index].lb_temp.x = (*left_it).lb.x;
            }
            else break;
        }
        else {
            // HPWL_start
            node_array[target_node_index].lb_temp.x = bound_x_max - node_W;
            HPWL_start = node_HPWL_cal(target_node_index, 1);

            // HPWL_mid
            node_array[target_node_index].lb_temp.x = bound_x_min + (bound_x_max - node_W - bound_x_min) / 2;
            HPWL_mid = node_HPWL_cal(target_node_index, 1);

            // HPWL_end
            node_array[target_node_index].lb_temp.x = bound_x_min;
            HPWL_end = node_HPWL_cal(target_node_index, 1);

            // 判斷最小
            if(HPWL_mid > HPWL_start && HPWL_mid > HPWL_end) { // 上凸
                if(HPWL_start < HPWL_end) { // 取start
                    node_array[target_node_index].lb_temp.x = bound_x_max - node_W;

                    if(HPWL_start < left_min_HPWL && 
                        node_array[target_node_index].lb_temp.x + node_W <= bound_x_max && node_array[target_node_index].lb_temp.x >= bound_x_min) {  // 可以放
                        
                        left_min_HPWL = HPWL_start;
                        left_place_x = bound_x_max - node_W;

                        HPWL_improve = 1;
                    }

                    if(HPWL_start > left_min_HPWL * (1. + up_hill_ratio)) list_end = 1;
                }
                else { // 取end
                    node_array[target_node_index].lb_temp.x = bound_x_min;

                    if(HPWL_end < left_min_HPWL && 
                        node_array[target_node_index].lb_temp.x + node_W <= bound_x_max && node_array[target_node_index].lb_temp.x >= bound_x_min) {  // 可以放
                        
                        left_min_HPWL = HPWL_end;
                        left_place_x = bound_x_min;

                        HPWL_improve = 1;
                    }

                    if(HPWL_end > left_min_HPWL * (1. + up_hill_ratio)) list_end = 1;
                }
            }
            else if(HPWL_end < HPWL_start && HPWL_end < HPWL_mid) { // HPWL_end 最小
                if(HPWL_end < left_min_HPWL && 
                    node_array[target_node_index].lb_temp.x + node_W <= bound_x_max && node_array[target_node_index].lb_temp.x >= bound_x_min) {  // 可以放
                    
                    left_min_HPWL = HPWL_end;
                    left_place_x = bound_x_min;

                    HPWL_improve = 1;
                }

                if(HPWL_end > left_min_HPWL * (1. + up_hill_ratio)) list_end = 1;
            }
            else if(HPWL_start < HPWL_end && HPWL_start < HPWL_mid) { // HPWL_start 最小
                if(HPWL_start < left_min_HPWL && 
                    node_array[target_node_index].lb_temp.x + node_W <= bound_x_max && node_array[target_node_index].lb_temp.x >= bound_x_min) {  // 可以放
                    
                    left_min_HPWL = HPWL_start;
                    left_place_x = bound_x_max - node_W;

                    HPWL_improve = 1;
                }

                if(HPWL_start > left_min_HPWL * (1. + up_hill_ratio)) list_end = 1;
            }
            else {  // HPWL_mid 最小
                // 凹點
                node_array[target_node_index].lb_temp.x = bound_x_min + (bound_x_max - node_W - bound_x_min) / 2;

                if(HPWL_mid < left_min_HPWL && 
                    node_array[target_node_index].lb_temp.x + node_W <= bound_x_max && node_array[target_node_index].lb_temp.x >= bound_x_min) {  // 可以放
                    
                    left_min_HPWL = HPWL_mid;
                    left_place_x = node_array[target_node_index].lb_temp.x;

                    HPWL_improve = 1;
                }

                if(HPWL_mid > left_min_HPWL * (1. + up_hill_ratio)) list_end = 1;
                else {
                    while(node_array[target_node_index].lb_temp.x > bound_x_min) {
                        HPWL_temp = node_HPWL_cal(target_node_index, 0);
                        // log_file << "left_HPWL_temp : " << HPWL_temp << "(" << node_array[target_node_index].lb_temp.x << ")" << endl;

                        if(node_array[target_node_index].lb_temp.x + node_W <= bound_x_max && node_array[target_node_index].lb_temp.x >= bound_x_min) {  // 可以放
                            if(HPWL_temp < left_min_HPWL) {  // 可以放
                                left_min_HPWL = HPWL_temp;
                                left_place_x = node_array[target_node_index].lb_temp.x;

                                HPWL_improve = 1;
                            }
                            else {
                                if(HPWL_temp > left_min_HPWL * (1. + up_hill_ratio)) list_end = 1;
                                break;
                            }
                        }

                        node_array[target_node_index].lb_temp.x -= node_W;
                    }
                }
            }

            left_it = prev(left_it);
        }

        if(list_end) break;
    }

    if(HPWL_improve) {
        row_index = target_row_index;

        if(left_min_HPWL < right_min_HPWL) {
            place_x = left_place_x;
            min_HPWL = left_min_HPWL;
        }
        else {
            place_x = right_place_x;
            min_HPWL = right_min_HPWL;
        }

        if(debug) {
            log_file << "=============== X SLIDE - ROW " << target_row_index << " ===============" << endl;
            log_file << "place_x : " << place_x << endl;
            log_file << "row_index : " << row_index << " (" << row_array[row_index].lb.y <<  ")" << endl;
            log_file << "min_HPWL : " << min_HPWL << endl;
            log_file << "----------" << endl;
        }
    }
}

void LG::LG_alg() {
    // log_file << "============ NODE POSITION CHANGE ==========" << endl;
    // cout << "============ NODE POSITION CHANGE ==========" << endl;

    for(int i = 0; i < (int)move_node_order.size(); i++) {
        if(check_move) {
            log_file << "============ MOVE " << i;
            log_file << " (" << move_node_order.size() - i << ") ==========" << endl;
        }

        if((double)(clock()- start_time)/CLOCKS_PER_SEC >= 298) {
            cout << "TIME OUT" << endl;
            time_out = 1;
            break;
        }
        
        // cout << "============ MOVE " << i;
        // cout << " (" << move_node_order.size() - i << ") ==========" << endl;

        min_HPWL_position_cal(move_node_order[i]);

        // if(!((move_node_order.size() - i) % 10000)) cout << "========== move_node_order_num : " << move_node_order.size() - i << " ==========" << endl;
    }

    if(debug) check_overlap(); // 檢查整個布局
}

// OUTPUT -------------------------------------------------------------------

void LG::file_output(string file_name) { // .pl
    ofstream output_file(file_name, ios::out);

    output_file << "# Time: " << (double)(clock()-start_time)/CLOCKS_PER_SEC << " s" << endl;
    output_file << "# initial_HPWL : " << initial_HPWL << endl;
    output_file << "# total_HPWL : " << total_HPWL << " (" << (double)total_HPWL/(double)initial_HPWL*100. << " %)" << endl << endl;
    
    output_file << "# node_name" << "\t" << "ll_Xcoord" << "\t" << "ll_Ycoord" << "\t" << "orientation movetype" << endl << endl;

    for(int i = 0; i < (int)node_array.size(); i++) {
        output_file << node_array[i].node_name << "\t";
        output_file << node_array[i].lb.x << "\t";
        output_file << node_array[i].lb.y << "\t";
        output_file << ":" << "\t";

        if(node_array[i].orient) output_file << "Y" << "\t";  // 0:N 1:Y
        else output_file << "N" << "\t";

        // .pl : 0:default 1:/FIXED = terminal 2:/FIXED_NI = terminal_NI
        if(node_array[i].movetype == 0) output_file << endl;
        else if(node_array[i].movetype == 1) output_file << "/FIXED" << endl;
        else output_file << "/FIXED_NI" << endl;
    }

    output_file.close();
}