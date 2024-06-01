#include <iomanip>
#include <algorithm>
#include <sstream>

#include "FM_M16121093.h"

using namespace std;

void FM::set_show(set<int> show_set) {
    for (auto &unit : show_set) {
        log_file << unit << " ";
    }
    log_file << endl;
}

void FM::node_input(string node_file) {
    ifstream input_file("benchmark/" + node_file, ios::in);

    string input_temp3, input_temp2, input_temp1, input_temp;

    log_file << "---------- NODE ----------" << endl;

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
    log_file << "NumMovableNodes : " << NumMovableNodes << endl;

    // cell_array.resize(NumMovableNodes); // Movable cells

    int i = 0, area_temp = 0;
    string line;
    // istringstream istr;

    // for(int i = 0; i < NumMovableNodes; i++) {
    while(getline(input_file, line)) {
        if(!line.length()) continue;

        istringstream istr(line);
        istr >> input_temp >> input_temp1 >> input_temp2 >> input_temp3;

        area_temp = stoi(input_temp2) * stoi(input_temp1);

        if(area_temp && !input_temp3.length()) {

            cell_map.insert(pair<string, int>(input_temp, i));

            // cell_array[i].cell_name = input_temp;
            // cell_array[i].index = i;
            // cell_array[i].area = area_temp;

            cell_array.push_back({input_temp, i, area_temp});

            // W += cell_array[i].area;
            W += area_temp;
            // if(S_max < cell_array[i].area) S_max = cell_array[i].area;
            if(S_max < area_temp) S_max = area_temp;

            i++;
        }
    }

    input_file.close();

    log_file << "Numcell_array : " << cell_array.size() << endl;
    log_file << "W : " << W << endl;
    log_file << "S_max : " << S_max << endl;
    log_file << "----------" << endl;
}

void FM::net_input(string net_file) {
    ifstream input_file("benchmark/" + net_file, ios::in);

    string input_temp4, input_temp3, input_temp2, input_temp1, input_temp;

    log_file << "---------- NET ----------" << endl;

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

    int pin_num = 0, cell_index = 0, i = 0;
    int pin_num_temp = 0;

    map<string, int>::iterator iter;

    while(input_file >> input_temp) {
        input_file >> input_temp3 >> input_temp2 >> input_temp1;
        pin_num = stoi(input_temp2);

        net_array[i].net_name = input_temp1;
        net_array[i].index = i;

        net_map.insert(pair<string, int>(input_temp1, i));

        pin_num_temp = 0;

        for(int j = 0; j < pin_num; j++) {
            input_file >> input_temp4 >> input_temp3 >> input_temp2 >> input_temp1 >> input_temp;
            
            iter = cell_map.find(input_temp4);

            if(iter != cell_map.end()){
                cell_index = iter->second;

                // if(cell_index >= NumMovableNodes) continue; // terminal

                // 避免自連
                net_array[i].cell_index.insert(cell_index);
                if (cell_array[cell_index].net_index.insert(net_array[i].index).second) pin_num_temp += 1; // 插入成功
            }
            else continue;
        }
        
        if(P_max < pin_num_temp) P_max = pin_num_temp;

        i++;
    }

    input_file.close();

    log_file << "Numnet_array : " << net_array.size() << endl;
    log_file << "P_max : " << P_max << endl;
    log_file << "----------" << endl;
}

bool FM::balance_check(int cell_index) { // cell移動
    int G1_area_temp;

    if(cell_array[cell_index].G1or2 == 1) G1_area_temp = G1_area - cell_array[cell_index].area; // 搬去G2
    else G1_area_temp = G1_area + cell_array[cell_index].area; // 搬去G1

    if(r_goal*W - S_max < G1_area_temp && G1_area_temp < r_goal*W + S_max) return 1;
    else return 0;
}

void FM::initial_state_generate_G1() {
    log_file << "---------- G1 INITIAL STATE ----------" << endl;

    int G1or2 = 1; // 1:G1 2:G2

    for(int i = 0; i < (int)cell_array.size(); i++) {
        if(G1or2 == 1 && G1_area + cell_array[i].area > r_goal*W + S_max) G1or2 = 2;

        if(G1or2 == 1) {
            G1_area += cell_array[i].area;
            initial_node_G1.insert(i);
        }
        else {
            G2_area += cell_array[i].area;
            initial_node_G2.insert(i);
        }

        cell_array[i].G1or2 = G1or2;
    }

    log_file << "Nodes : " << initial_node_G1.size() + initial_node_G2.size() << " (" << initial_node_G1.size() << " , " << initial_node_G2.size() << ") (" << (double)(initial_node_G1.size() + initial_node_G2.size())/NumPins*100 << "%)" << endl;
    log_file << "G1_area : " << G1_area << endl;
    log_file << "G2_area : " << G2_area << endl;
    
    if(r_goal*W - S_max < G1_area && G1_area < r_goal*W + S_max) log_file << "balance" << endl;
    else log_file << (int)(r_goal*W - S_max) << " " << (int)(r_goal*W + S_max) << " unbalance" << endl;

    r_cur = (double)G1_area / W;
    log_file << "r_cur : " << r_cur << endl;

    initial_G1_area = G1_area;
    initial_G2_area = G2_area;

    cut_size_initial();

    bucket_list_initial();
}

void FM::initial_state_generate_indicate() {
    log_file << "---------- INDICATE INITIAL STATE ----------" << endl;

    ifstream input_file("benchmark/initial_" + output_file + ".txt", ios::in);

    string input_temp;
    int index;

    getline(input_file, input_temp); // G1:

    while(input_file >> input_temp){ // G1
        if(input_temp == "G2:") break;

        index = stoi(input_temp);

        G1_area += cell_array[index].area;
        initial_node_G1.insert(index);

        cell_array[index].G1or2 = 1;
    }

    while(input_file >> input_temp){ // G2
        index = stoi(input_temp);

        G2_area += cell_array[index].area;
        initial_node_G2.insert(index);

        cell_array[index].G1or2 = 2;
    }

    log_file << "Nodes : " << initial_node_G1.size() + initial_node_G2.size() << " (" << initial_node_G1.size() << " , " << initial_node_G2.size() << ") (" << (double)(initial_node_G1.size() + initial_node_G2.size())/NumPins*100 << "%)" << endl;
    log_file << "G1_area : " << G1_area << endl;
    log_file << "G2_area : " << G2_area << endl;
    
    if(r_goal*W - S_max < G1_area && G1_area < r_goal*W + S_max) log_file << "balance" << endl;
    else log_file << (int)(r_goal*W - S_max) << " " << (int)(r_goal*W + S_max) << " unbalance" << endl;

    r_cur = (double)G1_area / W;
    log_file << "r_cur : " << r_cur << endl;

    initial_G1_area = G1_area;
    initial_G2_area = G2_area;

    cut_size_initial();

    bucket_list_initial();
}

void FM::cut_net_check(int state_index) {
    int mov_cell_index;

    int cell_num_G1 = 0, cell_num_G2 = 0;

    cut_net.clear();

    if(initial_cut_size > min_cut_size){
        // 回朔 initial_node
        for (auto &unit : initial_node_G1) {
            cell_array[unit].G1or2 = 1;
        }

        for (auto &unit : initial_node_G2) {
            cell_array[unit].G1or2 = 2;
        }

        // 回朔 state
        for(int i = 0; i <= state_index; i++) {
            mov_cell_index = state_list[i].mov_cell;

            // 不能用現在狀態推 要用initial_node
            if(cell_array[mov_cell_index].G1or2 == 2) {
                initial_node_G1.insert(mov_cell_index);
                initial_node_G2.erase(mov_cell_index);

                initial_G1_area += cell_array[mov_cell_index].area;
                initial_G2_area -= cell_array[mov_cell_index].area;

                cell_array[mov_cell_index].G1or2 = 1;
            }
            else{
                initial_node_G2.insert(mov_cell_index);
                initial_node_G1.erase(mov_cell_index);

                initial_G1_area -= cell_array[mov_cell_index].area;
                initial_G2_area += cell_array[mov_cell_index].area;

                cell_array[mov_cell_index].G1or2 = 2;
            }
        }

        initial_cut_size = min_cut_size;
    }
    else min_cut_size_index = -1;

    log_file << "Nodes : " << initial_node_G1.size() + initial_node_G2.size() << " (" << initial_node_G1.size() << " , " << initial_node_G2.size() << ") (" << (double)(initial_node_G1.size() + initial_node_G2.size())/NumPins*100 << "%)" << endl;
    log_file << "step : " << min_cut_size_index + 1 << " (" << min_cut_size << ")" << endl;

    for(int i = 0; i < (int)net_array.size(); i++) {
        if(net_array[i].cell_index.size() <= 1) continue; // 自連不考慮

        cell_num_G1 = 0;
        cell_num_G2 = 0;

        for (auto &unit : net_array[i].cell_index) {
            if(initial_node_G1.count(unit)) cell_num_G1 += 1;
            if(initial_node_G2.count(unit)) cell_num_G2 += 1;
        }

        if(cell_num_G1 && cell_num_G2) cut_net.insert(i);
    }
}

void FM::cut_size_initial() {
    log_file << "---------- CUT_SIZE INITIAL ----------" << endl;

    int cell_num_G1 = 0, cell_num_G2 = 0;
    cut_net.clear();

    for(int i = 0; i < (int)net_array.size(); i++) {
        if(net_array[i].cell_index.size() <= 1) continue; // 自連不考慮

        cell_num_G1 = 0;
        cell_num_G2 = 0;

        for (auto &unit : net_array[i].cell_index) {
            if(cell_array[unit].G1or2 == 1) cell_num_G1 += 1;
            if(cell_array[unit].G1or2 == 2) cell_num_G2 += 1;
        }

        if(cell_num_G1 && cell_num_G2) cut_net.insert(i);
    }

    min_cut_size = cut_net.size();
    initial_cut_size = min_cut_size;
    min_cut_size_index = 0;
    log_file << "cut_size : " << cut_net.size() << " (" << (double)cut_net.size()/NumNets*100 << "%)" << endl;
}

int FM::node_gain_initial(bool overwrite, int cell_index) {
    int cell_num_F = 0, cell_num_T = 0;
    int gain_temp = 0;

    // cell_array[cell_index].gain = 0;

    // 找 critical net
    for (auto &net_unit : cell_array[cell_index].net_index) {
        cell_num_F = 0;
        cell_num_T = 0;

        // 分清楚 F T (包含based cell)
        for (auto &cell_unit : net_array[net_unit].cell_index) {
            if(cell_array[cell_index].G1or2 == cell_array[cell_unit].G1or2) cell_num_F += 1; // F
            else cell_num_T += 1; // T
        }
        

        if(cell_num_F == 1) gain_temp += 1;
        if(cell_num_T == 0) gain_temp -= 1;
    }

    if(overwrite) cell_array[cell_index].gain = gain_temp;

    return gain_temp;
}

int FM::bucket_list_index_2_gain(int bucket_list_index) {
    return P_max - bucket_list_index;
}

int FM::gain_2_bucket_list_index(int gain) {
    return P_max - gain;
}

void FM::bucket_list_initial() {
    log_file << "---------- BUCKET_LIST INITIAL ----------" << endl;

    bucket_list = new cell* [2*P_max];
    bucket_list_end = new cell* [2*P_max];
    for(int i = 0; i < 2*P_max; i++) {
        bucket_list[i] = NULL;
        bucket_list_end[i] = NULL;
    }

    int bucket_list_index = 0, max_gain = 0;

    for(int i = 0; i < (int)cell_array.size(); i++) {
        // gain initial
        node_gain_initial(1,i);
        if(max_gain < cell_array[i].gain) max_gain = cell_array[i].gain;

        bucket_list_index = gain_2_bucket_list_index(cell_array[i].gain);
        
        // 將node插入bucket_list
        if(!bucket_list[bucket_list_index]) {
            bucket_list[bucket_list_index] = &cell_array[i];
            cell_array[i].prev = bucket_list[bucket_list_index];
        }
        else {
            // 插 bucket_list末端
            bucket_list_end[bucket_list_index]->next = &cell_array[i];
            cell_array[i].prev = bucket_list_end[bucket_list_index];
        }

        bucket_list_end[bucket_list_index] = &cell_array[i]; // bucket_list_end 存 cell
    }

    gain_max_index = gain_2_bucket_list_index(max_gain);
    log_file << "max_gain : " << max_gain << " | " << gain_max_index << " | " << bucket_list[gain_max_index] << endl;
    log_file << "----------" << endl;
}

void FM::bucket_list_cell_move(bool f_or_b, int mov_cell_index, int updated_gain_index) {
    int current_gain_index = gain_2_bucket_list_index(cell_array[mov_cell_index].gain); // gain 還沒改變

    // 拆除cell 三種情形：頭中尾
    // 頭
    if(&cell_array[mov_cell_index] == bucket_list[current_gain_index]) {
        bucket_list[current_gain_index] = cell_array[mov_cell_index].next;
        if(cell_array[mov_cell_index].next) cell_array[mov_cell_index].next->prev = bucket_list[current_gain_index];
        else bucket_list_end[current_gain_index] = cell_array[mov_cell_index].prev; // 拆末端要更新 bucket_list_end
    }
    else{ // 中尾
        cell_array[mov_cell_index].prev->next = cell_array[mov_cell_index].next;
        if(cell_array[mov_cell_index].next) cell_array[mov_cell_index].next->prev = cell_array[mov_cell_index].prev; // 尾
        else bucket_list_end[current_gain_index] = cell_array[mov_cell_index].prev; // 拆末端要更新 bucket_list_end
    }


    // 將node插入bucket_list (f_or_b:1(movable) 插最前端 f_or_b:0(movable) 插最末端)
    if(!bucket_list[updated_gain_index]) {
        bucket_list[updated_gain_index] = &cell_array[mov_cell_index];
        cell_array[mov_cell_index].prev = bucket_list[updated_gain_index];

        bucket_list_end[updated_gain_index] = &cell_array[mov_cell_index];
        cell_array[mov_cell_index].next = NULL;
    }
    else{
        if(!f_or_b){
            // 插 bucket_list末端
            bucket_list_end[updated_gain_index]->next = &cell_array[mov_cell_index];
            cell_array[mov_cell_index].prev = bucket_list_end[updated_gain_index];

            bucket_list_end[updated_gain_index] = &cell_array[mov_cell_index];
            cell_array[mov_cell_index].next = NULL;
        }
        else{
            cell_array[mov_cell_index].next = bucket_list[updated_gain_index];
            bucket_list[updated_gain_index]->prev = &cell_array[mov_cell_index];

            bucket_list[updated_gain_index] = &cell_array[mov_cell_index];
            cell_array[mov_cell_index].prev = bucket_list[updated_gain_index];
        }
    }
}

void FM::node_gain_update(int mov_cell_index) {
    int cell_num_F = 0, cell_num_T = 0;
    int bucket_list_index = gain_2_bucket_list_index(cell_array[mov_cell_index].gain);

    // lock base_cell (要移的那個cell)
    cell_array[mov_cell_index].movable = 0;
    
    // 更新area
    if(cell_array[mov_cell_index].G1or2 == 1) {
        G1_area -= cell_array[mov_cell_index].area;
        G2_area += cell_array[mov_cell_index].area;
    }
    else {
        G1_area += cell_array[mov_cell_index].area;
        G2_area -= cell_array[mov_cell_index].area;
    }

    // before move 看T
    for (auto &net_unit : cell_array[mov_cell_index].net_index) {
        cell_num_F = 0;
        cell_num_T = 0;

        // 分清楚 F T (包含based cell)
        for (auto &cell_unit : net_array[net_unit].cell_index) {
            if(cell_array[mov_cell_index].G1or2 == cell_array[cell_unit].G1or2) cell_num_F += 1; // F
            else cell_num_T += 1; // T
        }

        for (auto &cell_unit : net_array[net_unit].cell_index) {
            if(!cell_array[cell_unit].movable) continue; // 不能移動

            if(cell_num_T == 0) {
                bucket_list_cell_move(1,cell_unit, gain_2_bucket_list_index(cell_array[cell_unit].gain + 1));
                cell_array[cell_unit].gain += 1;
                if(cell_array[cell_unit].gain > bucket_list_index_2_gain(gain_max_index)) gain_max_index -= 1; // bucket_list往上
            }
            if(cell_num_T == 1) {
                bucket_list_cell_move(1,cell_unit, gain_2_bucket_list_index(cell_array[cell_unit].gain - 1));
                cell_array[cell_unit].gain -= 1;
            }
        }
    }

    // after move 看F
    if(cell_array[mov_cell_index].G1or2 == 1) cell_array[mov_cell_index].G1or2 = 2;
    else cell_array[mov_cell_index].G1or2 = 1;

    bucket_list_cell_move(0,mov_cell_index, bucket_list_index);

    for (auto &net_unit : cell_array[mov_cell_index].net_index) {
        cell_num_F = 0;
        cell_num_T = 0;

        // 分清楚 F T (包含based cell) // F T 一開始就決定
        for (auto &cell_unit : net_array[net_unit].cell_index) {
            if(cell_array[mov_cell_index].G1or2 != cell_array[cell_unit].G1or2) cell_num_F += 1; // F
            else cell_num_T += 1; // T
        }

        // 更新cut_net
        if(!cell_num_F || !cell_num_T) cut_net.erase(net_unit);
        else cut_net.insert(net_unit);

        for (auto &cell_unit : net_array[net_unit].cell_index) {
            if(!cell_array[cell_unit].movable) continue; // 不能移動

            if(cell_num_F == 0) {
                bucket_list_cell_move(1,cell_unit, gain_2_bucket_list_index(cell_array[cell_unit].gain - 1));
                cell_array[cell_unit].gain -= 1;
            }
            if(cell_num_F == 1) {
                bucket_list_cell_move(1,cell_unit, gain_2_bucket_list_index(cell_array[cell_unit].gain + 1));
                cell_array[cell_unit].gain += 1;
                if(cell_array[cell_unit].gain > bucket_list_index_2_gain(gain_max_index)) gain_max_index -= 1; // bucket_list往上
            }
        }
    }

    state_list.push_back({cell_array[mov_cell_index].gain, G1_area, mov_cell_index, (int)cut_net.size()});
}

int FM::base_cell_find() {
    int bucket_list_index = gain_max_index;
    int cell_index = -1;
    cell* cell_temp = bucket_list[bucket_list_index];

    while(bucket_list_index < 2*P_max && bucket_list_index >= 0) {
        if(cell_temp) {
            if(!cell_temp->movable) {
                if(cell_index != -1) return cell_index;
                bucket_list_index ++; // 往下找
                cell_temp = bucket_list[bucket_list_index];
                continue;
            }
            else{
                if(balance_check(cell_temp->index)) {
                    if(cell_index == -1) cell_index = cell_temp->index;
                    else {
                        if(cell_array[cell_index].area > cell_temp->area) cell_index = cell_temp->index;
                    }
                }

                if(cell_temp->next) cell_temp = cell_temp->next;
                else {
                    if(cell_index != -1) return cell_index;
                    bucket_list_index ++; // 往下找
                    cell_temp = bucket_list[bucket_list_index];
                }
            }
        }
        else {
            if(cell_index != -1) return cell_index;
            bucket_list_index ++; // 往下找
            cell_temp = bucket_list[bucket_list_index];
        }
    }

    return cell_index;
}

void FM::best_state_find() {
    log_file << "---------- FINAL PATITION ----------" << endl;

    cut_net_check(min_cut_size_index);

    min_cut_size_index = 0;
    state_list.clear();
    
    log_file << "G1 : " << initial_node_G1.size() << endl;
    log_file << "G2 : " << initial_node_G2.size() << endl;
    log_file << "Cut_set : " << cut_net.size() << " (" << (double)cut_net.size()/NumNets*100 << "%)" << endl;
    
    r_cur = (double)initial_G1_area / W;
    log_file << "r_cur : " << r_cur << endl;
}

void FM::file_output(string file_name) {
    int set_counter = 0;

    ofstream output_file("benchmark/" + file_name, ios::out);

    output_file << "Cut_size: " << cut_net.size() << endl;
    output_file << "G1_size: " << initial_G1_area << endl;
    output_file << "G2_size: " << initial_G2_area << endl;
    output_file << "Time: " << (double)(clock()-start_time)/CLOCKS_PER_SEC << " s" << endl;

    output_file << "G1:" << endl;
    set_counter = 0;
    for (auto &unit : initial_node_G1) {
        output_file << cell_array[unit].cell_name;
        if(set_counter < (int)initial_node_G1.size()-1) output_file << " ";

        set_counter += 1;
    }
    output_file << ";" << endl;
    
    output_file << "G2:" << endl;
    set_counter = 0;
    for (auto &unit : initial_node_G2) {
        output_file << cell_array[unit].cell_name;
        if(set_counter < (int)initial_node_G2.size()-1) output_file << " ";

        set_counter += 1;
    }
    output_file << ";" << endl;

    output_file << "Cut_set:" << endl;
    set_counter = 0;
    for (auto &unit : cut_net) {
        output_file << net_array[unit].net_name;
        if(set_counter < (int)cut_net.size()-1) output_file << " ";

        set_counter += 1;
    }
    output_file << ";" << endl;
    output_file.close();
}

void FM::FM_alg() {
    int base_cell_index = -1;
    int Gk = 0;

    double break_ratio = 0.3;

    int max_gain = 0, gain_temp = 0;
    int used_min_cut = -1;
    int repeat_num = 0, total_repeat_num = 0;

    log_file << "-------------- FM --------------" << endl;

    while(1){
        while(1) {
            // 選base_cell (要移的那個cell)
            base_cell_index = base_cell_find();
            // log_file << base_cell_index << endl;

            node_gain_update(base_cell_index);

            Gk += state_list.back().gain;

            // ----------------------------------------------------------------------

            // 更新 min_cut_size
            if((int)cut_net.size() < min_cut_size) {
                min_cut_size = cut_net.size();
                min_cut_size_index = state_list.size()-1;
            }
            else{
                // release node
                if((int)cut_net.size() > min_cut_size + (initial_cut_size - min_cut_size) * break_ratio){

                    log_file << "Run time: " << (double)(clock()-start_time)/CLOCKS_PER_SEC << " s" << endl;
                    
                    cout << "min : " << used_min_cut << " | " << min_cut_size << endl;
                    if(used_min_cut == -1) used_min_cut = min_cut_size;
                    else {
                        if(used_min_cut > min_cut_size) {
                            used_min_cut = min_cut_size;
                            repeat_num = 0;
                        }
                        else if(used_min_cut == min_cut_size && repeat_num == 0) repeat_num += 1;
                        else break;
                    }

                    total_repeat_num += 1;

                    // file_output(output_file + "_temp.out");

                    cout << "----- RELEASE NODE -----" << endl;
                    log_file << "----- RELEASE NODE -----" << endl;

                    max_gain = 0, gain_temp = 0;

                    for(int i = 0; i < (int)cell_array.size(); i++) {
                        cell_array[i].movable = 1;

                        // gain initial
                        gain_temp = node_gain_initial(0,i);
                        if(max_gain < gain_temp) max_gain = gain_temp;

                        // move bucket_list
                        if (cell_array[i].gain != gain_temp){
                            bucket_list_cell_move(1, i, gain_2_bucket_list_index(gain_temp));
                            cell_array[i].gain = gain_temp;
                        }
                    }

                    gain_max_index = gain_2_bucket_list_index(max_gain);
                }
            }
        }

        best_state_find();

        cout << "----- REPEAT " << total_repeat_num << "-----" << endl;
        log_file << "----- REPEAT " << total_repeat_num << "-----" << endl;

        if(total_repeat_num >= 2) break;

        total_repeat_num = 0;
        repeat_num = 0;

        used_min_cut = -1;
        Gk = 0;
    }
}