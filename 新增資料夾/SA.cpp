#include <iomanip>
#include <random>
#include <algorithm>

#include <cmath>
#include "SA.h"

using namespace std;

// TOOL -------------------------------------------------------------------

void SA::vector_show(vector<int> show_vector) {
    for (unsigned int i = 0; i < show_vector.size(); i++) {
        log_file << show_vector[i] << " ";
    }
    log_file << endl;
}

void SA::vector_show(vector<bool> show_vector) {
    for (unsigned int i = 0; i < show_vector.size(); i++) {
        log_file << show_vector[i] << " ";
    }
    log_file << endl;
}

int SA::vector_find(vector<int> find_vector, int temp) {
    for (unsigned int i = 0; i < find_vector.size(); i++) {
        if (find_vector[i] == temp) return i;
    }
    return -1;
}

int SA::vector_rfind(vector<int> find_vector, int temp) {
    for (int i = find_vector.size(); i > -1; i--) {
        if (find_vector[i] == temp) return i;
    }
    return -1;
}

int SA::vector_find(vector<coor> find_vector, coor temp) {
    for (unsigned int i = 0; i < find_vector.size(); i++) {
        if (find_vector[i] == temp) return i;
    }
    return -1;
}

int SA::vector_find(vector<line> find_vector, line temp) {
    for (unsigned int i = 0; i < find_vector.size(); i++) {
        if (find_vector[i] == temp) return i;
    }
    return -1;
}

void SA::set_show(set<int> show_set) {
    for (auto &unit : show_set) {
        log_file << unit << " ";
    }
}

// SHOW -------------------------------------------------------------------

void SA::module_show(vector<module> &show_module){
    for(int i = 0; i < (int)show_module.size(); i++) {
        log_file << show_module[i].module_name << " ";
        log_file << i << " ";
        if(show_module[i].node != NULL) log_file << show_module[i].node << " ";
        else log_file << "NULL ";
        log_file << show_module[i].area << " ";

        log_file << show_module[i].W << " " << show_module[i].H << " ";
        log_file << show_module[i].rotate << " [";
        
        // cluster
        for (unsigned int j = 0; j < show_module[i].cluster.size(); j++) {
            log_file << show_module[i].cluster[j] << " ";
        }
        log_file << "] ";

        log_file << "(" << show_module[i].lb.x << " , " << show_module[i].lb.y << ") ";
        log_file << "(" << show_module[i].lb.x + show_module[i].W << " , " << show_module[i].lb.y << ") ";
        log_file << "(" << show_module[i].lb.x + show_module[i].W << " , " << show_module[i].lb.y + show_module[i].H << ") ";
        log_file << "(" << show_module[i].lb.x << " , " << show_module[i].lb.y + show_module[i].H << ") ";
        log_file << endl;
    }
    log_file << endl;
}

void SA::coor_show(vector<coor> &show_coor){
    for(int i = 0; i < (int)show_coor.size(); i++) {
        log_file << "(" << show_coor[i].x << " , " << show_coor[i].y << ") ";
    }
    log_file << endl;
}

void SA::contour_show(vector<line> &contour){
    for(int i = 0; i < (int)contour.size(); i++) {
        log_file << contour[i].module_index << " <(" << contour[i].start.x << " , " << contour[i].start.y << ") , (" << contour[i].end.x << " , " << contour[i].end.y << ")>" << endl;
    }
    log_file << endl;
}

void SA::Tree_show(TreeNode* temp){
    if(temp == NULL) return;

    log_file << temp->data << " | l | ";

    if(temp->left) log_file << temp->left->data <<  " | r | ";
    else log_file << "NULL | r | ";

    if(temp->right) log_file << temp->right->data << endl;
    else log_file << " NULL" << endl;

    Tree_show(temp->left);
    Tree_show(temp->right);
}

void SA::state_show(int state_index){
    log_file << "B_tree : ";
    vector_show(STATE[state_index].B_tree);
    module_show(STATE[state_index].MODULE);
    log_file << "BEST_W_H : " << "(" << STATE[state_index].W << " , " << STATE[state_index].H << ")" << endl;
    log_file << "BEST_COST : " << STATE[state_index].cost << endl;
}

// INPUT -------------------------------------------------------------------

bool module_area_compare(module &a, module &b){ // 由大到小
    return a.area > b.area;
}

bool module_H_compare(module &a, module &b){ // 由大到小
    if(a.H == b.H) return a.W > b.W;
    else return a.H > b.H;
}

bool module_W_compare(module &a, module &b){ // 由大到小
    if(a.W == b.W) return a.H > b.H;
    else return a.W > b.W;
}

bool module_WH_compare(module &a, module &b){ // 由大到小
    return a.W + a.H > b.W + b.H;
}

void SA::yal_input(string file_name) {
    ifstream input_file;
    input_file.open("benchmark/sample/" + file_name);

    string input_temp, input_temp1;

    log_file << "---------- YAL ----------" << endl;

    // MODULE -------------------------------------------------------------------

    while(1) {  // /* */
        getline(input_file, input_temp);
        if(input_temp.find("*/") != std::string::npos && 
        input_temp.find("/*") == std::string::npos) break;
    }

    string module_name;
    coor rb,rt,lb,lt;

    while(input_file >> input_temp >> module_name >> input_temp1) {
        module_name.pop_back();

        getline(input_file, input_temp); // TYPE
        if(input_temp.find("PARENT") != std::string::npos) break;
        
        input_file >> input_temp; // DIMENSIONS
        input_file >> rb.x >> rb.y;
        input_file >> rt.x >> rt.y;
        input_file >> lt.x >> lt.y;
        input_file >> lb.x >> input_temp;

        input_temp.pop_back();
        lb.y = stoi(input_temp);

        if(lb.x < 0) { // x是負的情況
            rb.x += abs(lb.x);
            rt.x += abs(lb.x);
            lt.x = 0;
            lb.x = 0;
        }

        if(lb.y < 0) { // y是負的情況
            rt.y += abs(lb.y);
            lt.y += abs(lb.y);
            rb.y = 0;
            lb.y = 0;
        }

        A += (rb.x - lb.x) * (rt.y - rb.y);

        if((rb.x - lb.x) < (rt.y - rb.y)) initial_MODULE.push_back({module_name, (rb.x - lb.x), (rt.y - rb.y), (rb.x - lb.x) * (rt.y - rb.y), lb, lb, 0});
        else initial_MODULE.push_back({module_name, (rt.y - rb.y), (rb.x - lb.x), (rb.x - lb.x) * (rt.y - rb.y), lb, lb, 1});

        while(1) {  // ENDMODULE
            getline(input_file, input_temp);
            if(input_temp.find("ENDMODULE") != std::string::npos) break;
        }
    }

    // for (auto &module : module_map) {
    //     log_file << module.first << " : " << module.second << endl;
    // }

    // A_star
    A_star = ceil((1 + whitespace / 100.) * A);

    // H_star W_star
    H_star = ceil(sqrt((1 + whitespace / 100.) * A * R_star));
    W_star = ceil(sqrt((1 + whitespace / 100.) * A / R_star));

    log_file << "A : " << A << endl;
    log_file << "A_star : " << A_star << endl;
    log_file << "W_star : " << W_star << endl;
    log_file << "H_star : " << H_star << endl;
    log_file << "NumMODULE : " << initial_MODULE.size() << endl;
    // log_file << "NumModulemap : " << module_map.size() << endl;

    // optimal_W_H
    optimal_W_H = ceil(sqrt(A) * 2);
    log_file << "optimal_W_H : " << optimal_W_H << endl;
    log_file << endl;

    // A_avg
    // A_avg = ceil((double) A / initial_MODULE.size());
    // log_file << "A_avg : " << A_avg << endl;
    // log_file << "----------" << endl;

    sort(initial_MODULE.begin(), initial_MODULE.end(), module_area_compare);

    for(int i = 0; i < (int)initial_MODULE.size(); i++) {
        module_map.insert(pair<string, int>(initial_MODULE[i].module_name, i));

        initial_MODULE[i].cluster = {i};
    }

    // module_show(initial_MODULE);

    MODULE_cluster();

    log_file << "NumsMODULE_cs : " << MODULE.size() << endl;
    log_file << "----------" << endl;

    // NETWORK -------------------------------------------------------------------

    while(1) {  
        getline(input_file, input_temp);
        if(input_temp.find("NETWORK") != std::string::npos) break;
    }

    string net_name;
    int net_module, module_index;
    bool end = 0;

    map<string, int>::iterator iter;
    
    while(1) {
        input_file >> input_temp >> module_name;

        if(input_temp.find("ENDNETWORK") != std::string::npos) break;

        iter = module_map.find(module_name);

        if(iter != module_map.end()) module_index = iter->second;
        else continue;

        end = 0;

        while(1) {
            input_file >> net_name;

            if(net_name.find(";") != std::string::npos) {
                net_name.pop_back();
                end = 1;
            }

            iter = net_map.find(net_name);

            if(iter != net_map.end()) {
                net_module = iter->second;
                net_array[net_module].module_index.insert(module_index);
            }
            else {
                net_map.insert(pair<string, int>(net_name, (int)net_array.size()));
                net_array.push_back({net_name, {module_index}});
            }

            if(end) break;
        }
    }

    input_file.close();

    // for(int i = 0; i < (int)net_array.size(); i++) {
    //     log_file << net_array[i].net_name << " : ";
    //     set_show(net_array[i].module_index);
    //     log_file << endl;
    // }

    log_file << "Num_NET : " << net_array.size() << endl;
}

// MODULE cluster -------------------------------------------------------------------

void SA::MODULE_cluster(){
    vector<module> MODULE_temp = initial_MODULE;
    vector<module> MODULE_c;

    set<int> finish;
    int cluster_num = 0, cluster_total_num = 0, module_used_size = 0;

    int WH_temp = 0;

    while(module_used_size == 0 || (int)module_used_size > (int)MODULE_temp.size()){

        module_used_size = MODULE_temp.size();

        for(int i = 0; i < (int)MODULE_temp.size(); i++) {
            if(finish.count(i)) continue;

            cluster_num = 0;

            for(int j = i+1; j < (int)MODULE_temp.size(); j++) {
                if(finish.count(j)) continue;

                // W < H => 不考慮 MODULE[i].W == MODULE[j].H
                // i又比j大 => 不考慮MODULE[i].H == MODULE[j].W

                // 維持 W < H rotate一律設0

                // -1: W
                // -2: H
                // -3: Wi Hj
                // -4: Hi Wj
                // -5: 分隔符

                // 改成
                // -1: 放上面
                // -2: 放右邊

                if(MODULE_temp[i].W == MODULE_temp[j].W) {
                    if(H_star >= MODULE_temp[i].H + MODULE_temp[j].H) { // H_star > W_star // 以 W_star為放置標準
                        MODULE_c.push_back({"c" + to_string(cluster_total_num), MODULE_temp[i].W, MODULE_temp[i].H + MODULE_temp[j].H, MODULE_temp[i].W * (MODULE_temp[i].H + MODULE_temp[j].H), {0,0}, {0,0}, 0, MODULE_temp[i].cluster});
                        
                        MODULE_c.back().cluster.insert(MODULE_c.back().cluster.end(), -1);

                        MODULE_c.back().cluster.insert(MODULE_c.back().cluster.end(), MODULE_temp[j].cluster.begin(), MODULE_temp[j].cluster.end()); 
                        
                        MODULE_c.back().cluster.insert(MODULE_c.back().cluster.begin(), -5);
                        MODULE_c.back().cluster.insert(MODULE_c.back().cluster.end(), -5);

                        finish.insert(i);
                        finish.insert(j);

                        cluster_total_num += 1;
                        cluster_num += 1;
                        break;
                    }
                }
                else if(MODULE_temp[i].H == MODULE_temp[j].H){
                    if(H_star >= MODULE_temp[i].W + MODULE_temp[j].W) { // H_star > W_star // 以 W_star為放置標準
                        if(MODULE_temp[i].H > MODULE_temp[i].W + MODULE_temp[j].W) {
                            MODULE_c.push_back({"c" + to_string(MODULE_temp.size()), MODULE_temp[i].W + MODULE_temp[j].W, MODULE_temp[i].H, (MODULE_temp[i].W + MODULE_temp[j].W) * MODULE_temp[i].H, {0,0}, {0,0}, 0, MODULE_temp[i].cluster});
                        
                            MODULE_c.back().cluster.insert(MODULE_c.back().cluster.end(), -2);
                        }
                        else {
                            for(int k = 0; k < (int)MODULE_temp[i].cluster.size(); k++) {
                                if(MODULE_temp[i].cluster[k] >= 0) {
                                    initial_MODULE[MODULE_temp[i].cluster[k]].rotate = !initial_MODULE[MODULE_temp[i].cluster[k]].rotate;

                                    WH_temp = initial_MODULE[MODULE_temp[i].cluster[k]].W;
                                    initial_MODULE[MODULE_temp[i].cluster[k]].W = initial_MODULE[MODULE_temp[i].cluster[k]].H;
                                    initial_MODULE[MODULE_temp[i].cluster[k]].H = WH_temp;
                                }
                                else {
                                    if(MODULE_temp[i].cluster[k] > -5) {
                                        if(abs(MODULE_temp[i].cluster[k]) % 2 == 0) MODULE_temp[i].cluster[k] += 1; // -2 => -1
                                        else MODULE_temp[i].cluster[k] -= 1; // -1 => -2
                                    }
                                }
                            }

                            for(int k = 0; k < (int)MODULE_temp[j].cluster.size(); k++) {
                                if(MODULE_temp[j].cluster[k] >= 0) {
                                    initial_MODULE[MODULE_temp[j].cluster[k]].rotate = !initial_MODULE[MODULE_temp[j].cluster[k]].rotate;

                                    WH_temp = initial_MODULE[MODULE_temp[j].cluster[k]].W;
                                    initial_MODULE[MODULE_temp[j].cluster[k]].W = initial_MODULE[MODULE_temp[j].cluster[k]].H;
                                    initial_MODULE[MODULE_temp[j].cluster[k]].H = WH_temp;
                                }
                                else {
                                    if(MODULE_temp[j].cluster[k] > -5) {
                                        if(abs(MODULE_temp[j].cluster[k]) % 2 == 0) MODULE_temp[j].cluster[k] += 1; // -2 => -1
                                        else MODULE_temp[j].cluster[k] -= 1; // -1 => -2
                                    }
                                }
                            }

                            MODULE_c.push_back({"c" + to_string(cluster_total_num), MODULE_temp[i].H, MODULE_temp[i].W + MODULE_temp[j].W, MODULE_temp[i].H * (MODULE_temp[i].W + MODULE_temp[j].W), {0,0}, {0,0}, 0, MODULE_temp[i].cluster});
                        
                            MODULE_c.back().cluster.insert(MODULE_c.back().cluster.end(), -1);
                        }

                        MODULE_c.back().cluster.insert(MODULE_c.back().cluster.end(), MODULE_temp[j].cluster.begin(), MODULE_temp[j].cluster.end()); 

                        MODULE_c.back().cluster.insert(MODULE_c.back().cluster.begin(), -5);
                        MODULE_c.back().cluster.insert(MODULE_c.back().cluster.end(), -5);

                        finish.insert(i);
                        finish.insert(j);

                        cluster_total_num += 1;
                        cluster_num += 1;
                        break;
                    }
                }
                else if(MODULE_temp[i].W == MODULE_temp[j].H){ // area i > area j
                    if(H_star >= MODULE_temp[i].H + MODULE_temp[j].W) { // H_star > W_star // 以 W_star為放置標準
                        for(int k = 0; k < (int)MODULE_temp[j].cluster.size(); k++) {
                            if(MODULE_temp[j].cluster[k] >= 0) {
                                initial_MODULE[MODULE_temp[j].cluster[k]].rotate = !initial_MODULE[MODULE_temp[j].cluster[k]].rotate;

                                WH_temp = initial_MODULE[MODULE_temp[j].cluster[k]].W;
                                initial_MODULE[MODULE_temp[j].cluster[k]].W = initial_MODULE[MODULE_temp[j].cluster[k]].H;
                                initial_MODULE[MODULE_temp[j].cluster[k]].H = WH_temp;
                            }
                            else {
                                if(MODULE_temp[j].cluster[k] > -5) {
                                    if(abs(MODULE_temp[j].cluster[k]) % 2 == 0) MODULE_temp[j].cluster[k] += 1; // -2 => -1
                                    else MODULE_temp[j].cluster[k] -= 1; // -1 => -2
                                }
                            }
                        }

                        MODULE_c.push_back({"c" + to_string(cluster_total_num), MODULE_temp[i].W, MODULE_temp[i].H + MODULE_temp[j].W, MODULE_temp[i].W * (MODULE_temp[i].H + MODULE_temp[j].W), {0,0}, {0,0}, 0, MODULE_temp[i].cluster});
                        
                        MODULE_c.back().cluster.insert(MODULE_c.back().cluster.end(), -1);

                        MODULE_c.back().cluster.insert(MODULE_c.back().cluster.end(), MODULE_temp[j].cluster.begin(), MODULE_temp[j].cluster.end()); 

                        MODULE_c.back().cluster.insert(MODULE_c.back().cluster.begin(), -5);
                        MODULE_c.back().cluster.insert(MODULE_c.back().cluster.end(), -5);

                        finish.insert(i);
                        finish.insert(j);

                        cluster_total_num += 1;
                        cluster_num += 1;
                        break;
                    }
                }
                else if(MODULE_temp[j].W == MODULE_temp[i].H){ // area j > area i
                    if(H_star >= MODULE_temp[j].H + MODULE_temp[i].W) { // H_star > W_star // 以 W_star為放置標準
                        for(int k = 0; k < (int)MODULE_temp[i].cluster.size(); k++) {
                            if(MODULE_temp[i].cluster[k] >= 0) {
                                initial_MODULE[MODULE_temp[i].cluster[k]].rotate = !initial_MODULE[MODULE_temp[i].cluster[k]].rotate;

                                WH_temp = initial_MODULE[MODULE_temp[i].cluster[k]].W;
                                initial_MODULE[MODULE_temp[i].cluster[k]].W = initial_MODULE[MODULE_temp[i].cluster[k]].H;
                                initial_MODULE[MODULE_temp[i].cluster[k]].H = WH_temp;
                            }
                            else {
                                if(MODULE_temp[i].cluster[k] > -5) {
                                    if(abs(MODULE_temp[i].cluster[k]) % 2 == 0) MODULE_temp[i].cluster[k] += 1; // -2 => -1
                                    else MODULE_temp[i].cluster[k] -= 1; // -1 => -2
                                }
                            }
                        }

                        MODULE_c.push_back({"c" + to_string(cluster_total_num), MODULE_temp[j].W, MODULE_temp[j].H + MODULE_temp[i].W, MODULE_temp[j].W * (MODULE_temp[j].H + MODULE_temp[i].W), {0,0}, {0,0}, 0, MODULE_temp[i].cluster});
                        
                        MODULE_c.back().cluster.insert(MODULE_c.back().cluster.end(), -1);

                        MODULE_c.back().cluster.insert(MODULE_c.back().cluster.end(), MODULE_temp[j].cluster.begin(), MODULE_temp[j].cluster.end()); 

                        MODULE_c.back().cluster.insert(MODULE_c.back().cluster.begin(), -5);
                        MODULE_c.back().cluster.insert(MODULE_c.back().cluster.end(), -5);

                        finish.insert(i);
                        finish.insert(j);

                        cluster_total_num += 1;
                        cluster_num += 1;
                        break;
                    }
                }
                else cluster_num = cluster_num;
                // else continue;
            }

            if(cluster_num == 0) {
                finish.insert(i);

                MODULE_c.push_back(MODULE_temp[i]);
            }
        }

        MODULE_temp.clear();
        MODULE_temp = MODULE_c;
        MODULE_c.clear();
        finish.clear();

        module_show(MODULE_temp);
    }

    sort(MODULE_temp.begin(), MODULE_temp.end(),module_H_compare);
    // sort(MODULE_temp.begin(), MODULE_temp.end(),module_area_compare);
    // sort(MODULE_temp.begin(), MODULE_temp.end(),module_WH_compare);
    // sort(MODULE_temp.begin(), MODULE_temp.end(),module_W_compare);
    
    MODULE = MODULE_temp;

    module_show(MODULE);

    module_show(initial_MODULE);
}

// OUTPUT -------------------------------------------------------------------

void SA::txt_file_output(string file_name){
    // Benchmark : ami33.yal
    // ratio : 1
    // whitespace : 10 %
    // Runtime : 55.66 s
    // Wirelength : 12345678
    // W_staridth : 12345
    // H_stareight : 12345

    ofstream output_file("benchmark/" + file_name, ios::out);

    string yal_name = target_file_name.substr(0,target_file_name.find("_"));

    output_file << "Benchmark : " << yal_name << ".yal" << endl;

    output_file << "ratio : " << R_star << endl;
    output_file << "whitespace : " << whitespace << " %" << endl;

    run_time = (double)(clock()-start_time)/CLOCKS_PER_SEC;
    output_file << "Runtime : " << run_time << " s" << endl;

    output_file << "Wirelength : " << fixed << best_HPWL << endl;
    output_file << "W_staridth : " << x_max << endl;
    output_file << "H_stareight : " << y_max << endl;

    output_file.close();
}

void SA::out_file_output(string file_name){
    // <module name> <x> <y> <rotate: 0 or 1>
    // bk1 100 100 0

    ofstream output_file("benchmark/" + file_name, ios::out);

    for(int i = 0; i < (int)MODULE.size(); i++) {
        output_file << MODULE[i].module_name << " ";
        output_file << MODULE[i].lb.x << " ";
        output_file << MODULE[i].lb.y << " ";
        output_file << MODULE[i].rotate << endl;
    }
    
    output_file.close();
}

void SA::m_file_output(bool print_name, bool temp, string file_name){
    // axis equal;
    // hold on
    // grid on
    // block_x=[ 0 0 21 21 0];
    // block_y=[ 0 22 22 0 0];
    // fill(block_x,block_y,'c');
    // text(10,11,'n1');

    ofstream output_file("benchmark/" + file_name, ios::out);

    output_file << "axis equal;" << endl;
    output_file << "hold on;" << endl;
    output_file << "grid on;" << endl;

    // bound
    output_file << "cap_x=[ 0 0 " << W_star << " " << W_star << " 0 ];" << endl;
    output_file << "cap_y=[ 0 " << H_star << " " << H_star << " 0 0 ];" << endl;
    output_file << "fill(cap_x,cap_y,'w');" << endl;

    for(int i = 0; i < (int)MODULE.size(); i++) {
        if(temp){
            output_file << "block_x=[ ";
            output_file << MODULE[i].lb.x << " " << MODULE[i].lb.x + MODULE[i].W << " " << MODULE[i].lb.x + MODULE[i].W << " " << MODULE[i].lb.x << " " << MODULE[i].lb.x << " ];" << endl;

            output_file << "block_y=[ ";
            output_file << MODULE[i].lb.y << " " << MODULE[i].lb.y << " " << MODULE[i].lb.y + MODULE[i].H << " " << MODULE[i].lb.y + MODULE[i].H << " " << MODULE[i].lb.y << " ];" << endl;
                
            output_file << "fill(block_x,block_y,'c');" << endl;
            
            if(print_name) output_file << "text(" << MODULE[i].lb.x + MODULE[i].W/2 << "," << MODULE[i].lb.y + MODULE[i].H/2 << ",'" << MODULE[i].module_name << " ("  << i << ")');" << endl;
            else output_file << "text(" << MODULE[i].lb.x + MODULE[i].W/2 << "," << MODULE[i].lb.y + MODULE[i].H/2 << ",'"  << i << "');" << endl;
        }
        else{
            output_file << "block_x=[ ";
            output_file << MODULE[i].lb_temp.x << " " << MODULE[i].lb_temp.x + MODULE[i].W << " " << MODULE[i].lb_temp.x + MODULE[i].W << " " << MODULE[i].lb_temp.x << " " << MODULE[i].lb_temp.x << " ];" << endl;

            output_file << "block_y=[ ";
            output_file << MODULE[i].lb_temp.y << " " << MODULE[i].lb_temp.y << " " << MODULE[i].lb_temp.y + MODULE[i].H << " " << MODULE[i].lb_temp.y + MODULE[i].H << " " << MODULE[i].lb_temp.y << " ];" << endl;
                
            output_file << "fill(block_x,block_y,'c');" << endl;
            
            if(print_name) output_file << "text(" << MODULE[i].lb_temp.x + MODULE[i].W/2 << "," << MODULE[i].lb_temp.y + MODULE[i].H/2 << ",'" << MODULE[i].module_name << " ("  << i << ")');" << endl;
            else output_file << "text(" << MODULE[i].lb_temp.x + MODULE[i].W/2 << "," << MODULE[i].lb_temp.y + MODULE[i].H/2 << ",'"  << i << "');" << endl;
        }
    }

    output_file.close();
}

// B* TREE -------------------------------------------------------------------

bool line_x_compare(line &a, line &b) { // 用start x 排
    return a.start.x < b.start.x;
}

bool line_y_compare(line &a, line &b) { // 用start y 排
    return a.start.y < b.start.y;
}

bool coor_compare(coor &a, coor &b) { // 用start x 排
    if(a.x != b.x) return a.x < b.x;
    else return a.y < b.y;
}

bool SA::TETRIS_Insert(){
    int val;
    int contour_y_max = 0;
    bool place_success = 0;

    for(int i = 0; i < (int)B_tree.size(); i++) {
        val = B_tree[i];
        // log_file << val << endl;

        if(i == 0) { // root
            TREE.root->data = val;
            MODULE[val].node = TREE.root;

            MODULE[val].lb = {0,0};
            MODULE[val].lb_temp = {0,0};

            x_max = MODULE[val].W;
            y_max = MODULE[val].H;

            contour.clear();
            contour.push_back({{MODULE[val].lb.x, MODULE[val].lb.y + MODULE[val].H}, {MODULE[val].lb.x + MODULE[val].W, MODULE[val].lb.y + MODULE[val].H}, val});
        }
        else {
            place_success = 0;

            for(int j = 0; j < (int)contour.size(); j++) {
                if(contour[j].end.x + MODULE[val].W <= W_star && MODULE[contour[j].module_index].node->left == NULL) { // 放左
                    // 檢查在contour的哪個位置 // 找y_max
                    contour_y_max = 0;

                    for(int k = j+1; k < (int)contour.size(); k++) {
                        // 不能快速結束 因為不是照x排
                        // if(MODULE[val].lb.x + MODULE[val].W <= contour[k].start.x) break;

                        if((contour[j].end.x >= contour[k].start.x && contour[j].end.x < contour[k].end.x) || // 右半
                        (contour[j].end.x <= contour[k].start.x && contour[j].end.x + MODULE[val].W >= contour[k].end.x) || // 完全覆蓋
                        (contour[j].end.x + MODULE[val].W > contour[k].start.x && contour[j].end.x + MODULE[val].W <= contour[k].end.x)) { // 左半
                            if(contour_y_max < contour[k].start.y) contour_y_max = contour[k].start.y;
                        }

                        if(y_max < contour_y_max + MODULE[val].H) y_max = contour_y_max + MODULE[val].H;
                    }

                    if(contour_y_max > contour[j].start.y){
                        // log_file << contour[j].module_index << " WALL" << endl;
                        continue;
                    }

                    // log_file << contour[j].module_index << " LEFT" << endl;
                    
                    MODULE[val].lb.x = contour[j].end.x;
                    if(x_max < contour[j].end.x + MODULE[val].W) x_max = contour[j].end.x + MODULE[val].W;

                    MODULE[val].lb.y = MODULE[contour[j].module_index].lb.y;

                    MODULE[contour[j].module_index].node->left = new TreeNode(val);
                    MODULE[contour[j].module_index].node->left->parent = MODULE[contour[j].module_index].node;
                    MODULE[val].node = MODULE[contour[j].module_index].node->left;

                    // 會改變contour
                    line_insert({{MODULE[val].lb.x,MODULE[val].lb.y + MODULE[val].H},{MODULE[val].lb.x + MODULE[val].W,MODULE[val].lb.y + MODULE[val].H}, val});

                    MODULE[val].lb_temp = MODULE[val].lb;
                    rotate[val] = MODULE[val].rotate;

                    place_success = 1;
                    break;
                }
                else{
                    if(contour[j].start.x + MODULE[val].W <= W_star && MODULE[contour[j].module_index].node->right == NULL){ // 放上
                        // 檢查在contour的哪個位置 // 找y_max
                        contour_y_max = 0;

                        for(int k = j+1; k < (int)contour.size(); k++) {
                            // 不能快速結束 因為不是照x排
                            // if(MODULE[val].lb.x + MODULE[val].W <= contour[k].start.x) break;

                            if((contour[j].start.x >= contour[k].start.x && contour[j].start.x < contour[k].end.x) || // 右半
                            (contour[j].start.x <= contour[k].start.x && contour[j].start.x + MODULE[val].W >= contour[k].end.x) || // 完全覆蓋
                            (contour[j].start.x + MODULE[val].W > contour[k].start.x && contour[j].start.x + MODULE[val].W <= contour[k].end.x)) { // 左半
                                if(contour_y_max < contour[k].start.y) contour_y_max = contour[k].start.y;
                            }

                            if(y_max < contour_y_max + MODULE[val].H) y_max = contour_y_max + MODULE[val].H;
                        }

                        if(contour_y_max > contour[j].start.y) {
                            // log_file << contour[j].module_index << " WALL" << endl;
                            continue;
                        }

                        // log_file << contour[j].module_index << " UP" << endl;

                        MODULE[val].lb.x = contour[j].start.x;
                        if(x_max < MODULE[val].lb.x + MODULE[val].W) x_max = MODULE[val].lb.x + MODULE[val].W;


                        MODULE[val].lb.y = contour[j].start.y;

                        MODULE[contour[j].module_index].node->right = new TreeNode(val);
                        MODULE[contour[j].module_index].node->right->parent = MODULE[contour[j].module_index].node;
                        MODULE[val].node = MODULE[contour[j].module_index].node->right;

                        // 會改變contour
                        line_insert({{MODULE[val].lb.x,MODULE[val].lb.y + MODULE[val].H},{MODULE[val].lb.x + MODULE[val].W,MODULE[val].lb.y + MODULE[val].H}, val});

                        MODULE[val].lb_temp = MODULE[val].lb;
                        rotate[val] = MODULE[val].rotate;

                        place_success = 1;
                        break;
                    }
                }
            }

            if(!place_success){
                x_max = INT_MAX;
                y_max = INT_MAX;
                return 0;
            }
            
            sort(contour.begin(), contour.end(), line_y_compare);
            // contour_show(contour);

            // Tree_show(TREE.root);
            // log_file << endl;
        }
    }

    return 1;
}

void SA::B_tree_initial(){
    bool initial_flag = 0, decluster_flag = 0;

    double cost_temp;

    // 初始化 或 單個decluster 或 全部decluster
    if(!B_tree.size()) initial_flag = 1;
    if(B_tree.size() != MODULE.size()) decluster_flag = 1;

    if(initial_flag || decluster_flag){
        if(initial_flag) {
            log_file << "---------- B* TREE_INITIAL ----------" << endl;
            cout << "---------- INITIAL STATE ----------" << endl;
        }

        rotate.clear();
        rotate.resize(MODULE.size());

        B_tree.clear();
        B_tree.resize(MODULE.size());
        rotate.clear();
        rotate.resize(MODULE.size());

        for(int i = 0; i < (int)MODULE.size(); i++) {
            B_tree[i] = i;
        }
    }
    else {
        // log_file << "RESTORE" << endl;
        TREE.Tree_Delete(TREE.root);

        rotate.clear();
        rotate.resize(MODULE.size());
    }

    if(TETRIS_Insert()){
        // Tree_show(TREE.root);
        // log_file << endl;

        // module_show(MODULE);

        // m_file_output(0,1, "tetris.m");

        Pre_x_coor_update(1, TREE.root);
        Pre_contour_update(1, TREE.root);

        // Tree_show(TREE.root);
        // log_file << endl;

        // module_show(MODULE);

        if(initial_flag){
            best_cost = (double)INT_MAX;
            best_HPWL = (double)INT_MAX;
            best_state_index = 0;
            initial_HPWL = best_HPWL;

            cost_temp = cost_cal();
            STATE.push_back({B_tree, MODULE, x_max, y_max, cost_temp});
            log_file << "initial_HPWL : " << fixed << initial_HPWL << endl;
            log_file << "W_H : (" << x_max << " , " << y_max << ")" << endl;
            log_file << "COST : " << cost_temp << endl;

            // csv << cost_temp << "\n";
            log_file << "----------" << endl;

            m_file_output(0,1, "I_" + target_file_name + "_" + to_string(x_max) + "_" + to_string(y_max) + ".m");
            m_file_output(1,1, "IwN_" + target_file_name + "_" + to_string(x_max) + "_" + to_string(y_max) + ".m");
        }
    }
}

bool SA::boundry_check(){
    x_max = 0;
    y_max = 0;

    for(int i = 0; i < (int)MODULE.size(); i++) {
        if(x_max < MODULE[i].lb.x + MODULE[i].W) x_max = MODULE[i].lb.x + MODULE[i].W;
        if(y_max < MODULE[i].lb.y + MODULE[i].H) y_max = MODULE[i].lb.y + MODULE[i].H;
    }

    log_file << "x_max : " << x_max << endl;
    log_file << "y_max : " << y_max << endl;

    if(y_max > H_star || x_max > W_star) {
        log_file << "out of boundary" << endl; //  illegal
        return 0;
    }
    else return 1;
}

vector<int> SA::cluster_list_boundry_check(vector<int> cluster_list){
    vector<int> out_bound_cluster_list;

    for(int i = 0; i < (int)cluster_list.size(); i++) {
        if(MODULE[cluster_list[i]].lb.y + MODULE[cluster_list[i]].H > H_star || MODULE[cluster_list[i]].lb.x + MODULE[cluster_list[i]].W > W_star) {
            out_bound_cluster_list.push_back(cluster_list[i]); // out_bound
        }
    }

    log_file << "out_bound_cluster : ";
    vector_show(out_bound_cluster_list);

    return out_bound_cluster_list;
}

void SA::Pre_x_coor_update(bool overwrite, TreeNode* temp){
    if(temp == NULL) return;

    if(temp == TREE.root){
        // 必留
        if(overwrite) MODULE[temp->data].lb = {0,0};
        MODULE[temp->data].lb_temp = {0,0};

        x_max = MODULE[temp->data].W;
    }
    else{
        if(overwrite){
            if(temp->parent->left == temp) MODULE[temp->data].lb.x = MODULE[temp->parent->data].lb.x + MODULE[temp->parent->data].W;
            else MODULE[temp->data].lb.x = MODULE[temp->parent->data].lb.x;

            if(x_max < MODULE[temp->data].lb.x + MODULE[temp->data].W) x_max = MODULE[temp->data].lb.x + MODULE[temp->data].W;
        }
        else{
            if(temp->parent->left == temp) MODULE[temp->data].lb_temp.x = MODULE[temp->parent->data].lb_temp.x + MODULE[temp->parent->data].W;
            else MODULE[temp->data].lb_temp.x = MODULE[temp->parent->data].lb_temp.x;

            if(x_max < MODULE[temp->data].lb_temp.x + MODULE[temp->data].W) x_max = MODULE[temp->data].lb_temp.x + MODULE[temp->data].W;
        }
    }

    // PreOrder
    Pre_x_coor_update(overwrite, temp->left);
    Pre_x_coor_update(overwrite, temp->right);
}

void SA::line_insert(line insert_line){
    // 先看重疊哪些線段 再來更新 不能用點看
    vector<coor> contour_index;
    vector<coor> line_compare_temp(4);

    for(int i = 0; i < (int)contour.size(); i++) {
        line_compare_temp.clear();
        // insert_line:0  contour:1
        line_compare_temp = {{insert_line.start.x,0},{insert_line.end.x,0},{contour[i].start.x,1},{contour[i].end.x,1}};

        sort(line_compare_temp.begin(), line_compare_temp.end(), coor_compare);
        // coor_show(line_compare_temp);

        if(line_compare_temp[0].y != line_compare_temp[1].y) {
            // 左半:0  右半:1  全:2
            if(line_compare_temp[1].x != line_compare_temp[2].x){ // 等於則剛好切齊
                if(line_compare_temp[1].y == 1 && line_compare_temp[2].y == 0) contour_index.push_back({i,0}); // 左半
                else if(line_compare_temp[1].y == 0 && line_compare_temp[2].y == 1) contour_index.push_back({i,1}); // 右半
                else { // 全
                    if(line_compare_temp[0].y == 0 && line_compare_temp[1].y == 1) contour_index.push_back({i,3}); // 0110
                    else contour_index.push_back({i,2});
                }
            }
        }
    }

    // coor_show(contour_index);
    // 一定照順序 由小到大

    int contour_index_num = contour_index.size(), cursor = 0;
    int delete_num = 0, insert_num = 0;

    // line更新
    if(contour_index_num){
        for(int i = 0; i < contour_index_num; i++) {
            cursor = contour_index[i].x - delete_num + insert_num;

            if(contour_index[i].y == 3){ // 全 // 0110
                contour.erase(contour.begin() + cursor); 
                delete_num += 1;
            }
            else if(contour_index[i].y == 2){ // 全 // 1001
                contour[cursor].end.x = insert_line.start.x; // 左邊一定切

                if(insert_line.end.x < contour[cursor].end.x){
                    contour.insert(contour.begin() + cursor, {{insert_line.start.x, insert_line.start.y}, {contour[cursor].end.x, insert_line.start.y}, contour[cursor].module_index});
                    insert_num += 1;
                }
            }
            else{
                if(contour_index[i].y) contour[cursor].end.x = insert_line.start.x; // 右半:1
                else contour[cursor].start.x = insert_line.end.x;  // 左半:0

                if(contour[cursor].start.x == contour[cursor].end.x) {
                    contour.erase(contour.begin() + cursor); // 全
                    delete_num += 1;
                }
            }

            if(vector_find(contour, insert_line) == -1){
                contour.insert(contour.begin() + cursor, insert_line);
                insert_num += 1;
            }
        }
    }
    else {
        contour.push_back(insert_line);
        sort(contour.begin(), contour.end(), line_x_compare);
    }

    // contour_show(contour);
    // log_file << endl;
}

void SA::Pre_contour_update(bool overwrite, TreeNode* temp){
    if(temp == NULL) return;

    if(temp == TREE.root){
        contour.clear();

        if(overwrite) contour.push_back({{MODULE[temp->data].lb.x, MODULE[temp->data].lb.y + MODULE[temp->data].H}, {MODULE[temp->data].lb.x + MODULE[temp->data].W, MODULE[temp->data].lb.y + MODULE[temp->data].H}, temp->data});
        else contour.push_back({{MODULE[temp->data].lb_temp.x, MODULE[temp->data].lb_temp.y + MODULE[temp->data].H}, {MODULE[temp->data].lb_temp.x + MODULE[temp->data].W, MODULE[temp->data].lb_temp.y + MODULE[temp->data].H}, temp->data});
        
        y_max = MODULE[temp->data].H;
    }
    else{
        int contour_y_max = 0;

        // 檢查在contour的哪個位置 // 找y_max
        for(int i = 0; i < (int)contour.size(); i++) {
            if(overwrite){
                if(MODULE[temp->data].lb.x + MODULE[temp->data].W <= contour[i].start.x) break;

                if((MODULE[temp->data].lb.x >= contour[i].start.x && MODULE[temp->data].lb.x < contour[i].end.x) || // 右半
                (MODULE[temp->data].lb.x <= contour[i].start.x && MODULE[temp->data].lb.x + MODULE[temp->data].W >= contour[i].end.x) || // 完全覆蓋
                (MODULE[temp->data].lb.x + MODULE[temp->data].W > contour[i].start.x && MODULE[temp->data].lb.x + MODULE[temp->data].W <= contour[i].end.x)) { // 左半
                    if(contour_y_max < contour[i].start.y) contour_y_max = contour[i].start.y;
                }
            }
            else{
                if(MODULE[temp->data].lb_temp.x + MODULE[temp->data].W <= contour[i].start.x) break;

                if((MODULE[temp->data].lb_temp.x >= contour[i].start.x && MODULE[temp->data].lb_temp.x < contour[i].end.x) ||  // 右半
                (MODULE[temp->data].lb_temp.x <= contour[i].start.x && MODULE[temp->data].lb_temp.x + MODULE[temp->data].W >= contour[i].end.x) || // 完全覆蓋
                (MODULE[temp->data].lb_temp.x + MODULE[temp->data].W > contour[i].start.x && MODULE[temp->data].lb_temp.x + MODULE[temp->data].W <= contour[i].end.x)) { // 左半
                    if(contour_y_max < contour[i].start.y) contour_y_max = contour[i].start.y;
                }
            }

            if(y_max < contour_y_max + MODULE[temp->data].H) y_max = contour_y_max + MODULE[temp->data].H;
        }

        if(overwrite) {
            MODULE[temp->data].lb.y = contour_y_max;
            line_insert({{MODULE[temp->data].lb.x,MODULE[temp->data].lb.y + MODULE[temp->data].H},{MODULE[temp->data].lb.x + MODULE[temp->data].W,MODULE[temp->data].lb.y + MODULE[temp->data].H}, temp->data});
        }
        else {
            MODULE[temp->data].lb_temp.y = contour_y_max;
            line_insert({{MODULE[temp->data].lb_temp.x,MODULE[temp->data].lb_temp.y + MODULE[temp->data].H},{MODULE[temp->data].lb_temp.x + MODULE[temp->data].W,MODULE[temp->data].lb_temp.y + MODULE[temp->data].H}, temp->data});
        }

        // contour_show(contour);
        // log_file << endl;
    }
    
    Pre_contour_update(overwrite, temp->left);
    Pre_contour_update(overwrite, temp->right);
}

// OPERATION -------------------------------------------------------------------

void SA::module_rotate(int module_index){
    int temp = MODULE[module_index].W;
    MODULE[module_index].W = MODULE[module_index].H;
    MODULE[module_index].H = temp;

    B_tree_initial();
}

void SA::module_swap(int module_index_1, int module_index_2){ // 改插入順序而已 不是改node
    int index_1,index_2;
    index_1 = vector_find(B_tree, module_index_1);
    index_2 = vector_find(B_tree, module_index_2);

    int temp = B_tree[index_1];
    B_tree[index_1] = B_tree[index_2];
    B_tree[index_2] = temp;

    B_tree_initial();
}

void SA::module_insert(int module_index_1, int module_index_2){ // // 改插入順序而已 module_index_1排module_index_2後面
    int index_1,index_2;
    index_1 = vector_find(B_tree, module_index_1);
    index_2 = vector_find(B_tree, module_index_2);

    B_tree.erase(B_tree.begin() + index_1);
    B_tree.insert(B_tree.begin() + index_2, module_index_1);

    B_tree_initial();
}

// SA -------------------------------------------------------------------

void SA::random_number(vector<int> module_index_list, int &module_index_1){
    int temp = rand() % module_index_list.size();

    module_index_1 = module_index_list[temp];

    // log_file << module_index_1 << endl;
}

void SA::swap_random_number(int &module_index_1, int &module_index_2){
    int temp = rand() % B_tree.size();
    int temp1 = rand() % B_tree.size();

    while(temp1 == temp){
        temp1 = rand() % B_tree.size();
    }

    module_index_1 = B_tree[temp];
    module_index_2 = B_tree[temp1];

    // log_file << module_index_1 << " " << module_index_2 << endl;
}

void SA::all_MODULE_decluster(){
    int WH_temp = 0;

    B_tree_initial();

    log_file << "---------- MODULE DECLUSTER ----------" << endl;
    cout << "---------- MODULE DECLUSTER ----------" << endl;

    if(MODULE.size() == initial_MODULE.size()) return;

    module_show(MODULE);

    // cluster module有轉
    for(int i = 0; i < (int)B_tree.size(); i++) {
        if(MODULE[B_tree[i]].rotate == 1 && MODULE[B_tree[i]].cluster.size() > 1) {
            for(int j = 0; j < (int)MODULE[B_tree[i]].cluster.size(); j++) {
                // 轉過 其他要跟著轉
                if(MODULE[B_tree[i]].cluster[j] >= 0) {
                    initial_MODULE[MODULE[B_tree[i]].cluster[j]].rotate = !initial_MODULE[MODULE[B_tree[i]].cluster[j]].rotate;

                    WH_temp = initial_MODULE[MODULE[B_tree[i]].cluster[j]].W;
                    initial_MODULE[MODULE[B_tree[i]].cluster[j]].W = initial_MODULE[MODULE[B_tree[i]].cluster[j]].H;
                    initial_MODULE[MODULE[B_tree[i]].cluster[j]].H = WH_temp;
                }
                else {
                    if(MODULE[B_tree[i]].cluster[j] > -5) {
                        if(abs(MODULE[B_tree[i]].cluster[j]) % 2 == 0) MODULE[B_tree[i]].cluster[j] += 1; // -2 => -1
                        else MODULE[B_tree[i]].cluster[j] -= 1; // -1 => -2
                    }
                }
            }
        }
    }

    module_show(MODULE);

    Tree_show(TREE.root);
    log_file << endl;

    module_show(initial_MODULE);

    TreeNode* left_temp;
    TreeNode* right_temp;
    TreeNode* temp;

    TreeNode* left_most;

    for(int i = 0; i < (int)MODULE.size(); i++) {
        for(int j = 0; j < (int)MODULE[i].cluster.size(); j++) {
            if(MODULE[i].cluster[j] >= 0){
                // 修改欲插入node
                initial_MODULE[MODULE[i].cluster[j]].node = MODULE[i].node;
                MODULE[i].node->data = MODULE[i].cluster[j];

                break;
            }
        }

        // 拆掉欲插入node的分支
        if(MODULE[i].cluster.size() > 1) {
            left_temp = MODULE[i].node->left;
            right_temp = MODULE[i].node->right;

            MODULE[i].node->left = NULL;
            MODULE[i].node->right = NULL;

            left_most = NULL;

            // vector_show(MODULE[i].cluster);

            while(vector_find(MODULE[i].cluster, -5) != -1){
                for(int j = 0; j < (int)MODULE[i].cluster.size()-4; j++) {
                    if(MODULE[i].cluster[j] == -5 &&
                    MODULE[i].cluster[j+1] >= 0 &&
                    MODULE[i].cluster[j+2] < 0 &&
                    MODULE[i].cluster[j+3] >= 0 &&
                    MODULE[i].cluster[j+4] == -5){

                        if(initial_MODULE[MODULE[i].cluster[j+1]].node == NULL) initial_MODULE[MODULE[i].cluster[j+1]].node = new TreeNode(MODULE[i].cluster[j+1]);
                        
                        temp = initial_MODULE[MODULE[i].cluster[j+1]].node;
                        if(MODULE[i].cluster[j+2] == -1) { // right分支一定接root(right_most)
                            while(temp->right != NULL){
                                temp = temp->right;
                            }
                            
                            if(initial_MODULE[MODULE[i].cluster[j+3]].node == NULL) {
                                temp->right = new TreeNode(MODULE[i].cluster[j+3]);
                                temp->right->parent = temp;
                                initial_MODULE[MODULE[i].cluster[j+3]].node = temp->right;
                            }
                            else {
                                temp->right = initial_MODULE[MODULE[i].cluster[j+3]].node;
                                temp->right->parent = temp;
                            }

                            MODULE[i].cluster[j] = MODULE[i].cluster[j+1];
                        }
                        else { // left分支一定接left_most
                            while(temp->left != NULL){
                                temp = temp->left;
                            }

                            if(initial_MODULE[MODULE[i].cluster[j+3]].node == NULL) {
                                temp->left = new TreeNode(MODULE[i].cluster[j+3]);
                                temp->left->parent = temp;
                                initial_MODULE[MODULE[i].cluster[j+3]].node = temp->left;
                            }
                            else {
                                temp->left = initial_MODULE[MODULE[i].cluster[j+3]].node;
                                temp->left->parent = temp;
                            }

                            left_most = temp->left;

                            MODULE[i].cluster[j] = MODULE[i].cluster[j+1];
                        }

                        Tree_show(initial_MODULE[MODULE[i].cluster[j+1]].node);
                        log_file << endl;

                        MODULE[i].cluster.erase(MODULE[i].cluster.begin() + j+1, MODULE[i].cluster.begin() + j+5);
                    }
                }

                // vector_show(MODULE[i].cluster);
            }

            // 將分支接回
            if(left_temp != NULL) { // left分支一定接left_most
                if(left_most != NULL) temp = left_most;
                else temp = MODULE[i].node;

                while(temp->left != NULL){
                    temp = temp->left;
                }

                temp->left = left_temp;
                left_temp->parent = temp;
            }

            if(right_temp != NULL) { // right分支一定接root(right_most)
                temp = MODULE[i].node; 
                while(temp->right != NULL){
                    temp = temp->right;
                }

                temp->right = right_temp;
                right_temp->parent = temp;
            }
        }
        else{ // 更新單個轉動
            if(initial_MODULE[MODULE[i].cluster[0]].rotate != MODULE[i].rotate){
                WH_temp = initial_MODULE[MODULE[i].cluster[0]].W;
                initial_MODULE[MODULE[i].cluster[0]].W = initial_MODULE[MODULE[i].cluster[0]].H;
                initial_MODULE[MODULE[i].cluster[0]].H = WH_temp;
            }
        }
    }

    Tree_show(TREE.root);
    log_file << endl;

    MODULE = initial_MODULE;

    module_show(MODULE);
    // log_file << endl;

    Pre_x_coor_update(1, TREE.root);
    Pre_contour_update(1, TREE.root);

    module_show(MODULE);

    log_file << "W_H : (" << x_max << " , " << y_max << ")" << endl;
}

bool SA::single_MODULE_decluster(){

    vector<int> cluster_list;
    vector<int> out_bound_cluster_list;

    int WH_temp;
    int module_decluster = 0;

    for(int i = 0; i < (int)MODULE.size(); i++) {
        if(MODULE[i].cluster.size() > 1) cluster_list.push_back(i);
    }

    if(!cluster_list.size()) return 1;

    // 凸出去的拆掉
    // out_bound_cluster_list = cluster_list_boundry_check(cluster_list);
    // if(out_bound_cluster_list.size()) random_number(out_bound_cluster_list, module_decluster);
    // else random_number(cluster_list, module_decluster);

    random_number(cluster_list, module_decluster);

    log_file << endl;
    cout << "========== decluster " << module_decluster << " (" << MODULE[module_decluster].module_name << ") ==========" << endl; 
    log_file << "========== decluster " << module_decluster << " (" << MODULE[module_decluster].module_name << ") ==========" << endl; 

    // module_show(MODULE);

    for(int i = 0; i < (int)MODULE[module_decluster].cluster.size(); i++) {    
        if(MODULE[module_decluster].cluster[i] >= 0) {
            // 轉過 其他要跟著轉
            if(MODULE[module_decluster].rotate == 1) {
                initial_MODULE[MODULE[module_decluster].cluster[i]].rotate = !initial_MODULE[MODULE[module_decluster].cluster[i]].rotate;

                // 這樣會轉兩次
                // WH_temp = initial_MODULE[MODULE[module_decluster].cluster[i]].W;
                // initial_MODULE[MODULE[module_decluster].cluster[i]].W = initial_MODULE[MODULE[module_decluster].cluster[i]].H;
                // initial_MODULE[MODULE[module_decluster].cluster[i]].H = WH_temp;

                // decluster 拆掉後 H > W
                if(initial_MODULE[MODULE[module_decluster].cluster[i]].W > initial_MODULE[MODULE[module_decluster].cluster[i]].H) {
                    initial_MODULE[MODULE[module_decluster].cluster[i]].rotate = !initial_MODULE[MODULE[module_decluster].cluster[i]].rotate;
                    
                    WH_temp = initial_MODULE[MODULE[module_decluster].cluster[i]].W;
                    initial_MODULE[MODULE[module_decluster].cluster[i]].W = initial_MODULE[MODULE[module_decluster].cluster[i]].H;
                    initial_MODULE[MODULE[module_decluster].cluster[i]].H = WH_temp;
                }
            }

            // decluster 拆掉後 H > W
            // if(initial_MODULE[MODULE[module_decluster].cluster[i]].W > initial_MODULE[MODULE[module_decluster].cluster[i]].H) {
            //     initial_MODULE[MODULE[module_decluster].cluster[i]].rotate = !initial_MODULE[MODULE[module_decluster].cluster[i]].rotate;
                
            //     WH_temp = initial_MODULE[MODULE[module_decluster].cluster[i]].W;
            //     initial_MODULE[MODULE[module_decluster].cluster[i]].W = initial_MODULE[MODULE[module_decluster].cluster[i]].H;
            //     initial_MODULE[MODULE[module_decluster].cluster[i]].H = WH_temp;
            // }

            MODULE.push_back(initial_MODULE[MODULE[module_decluster].cluster[i]]);
        }
    }

    MODULE.erase(MODULE.begin() + module_decluster);

    // module_show(initial_MODULE);

    module_show(MODULE);

    B_tree_initial();

    return 0;
}

double SA::cost_cal() {
    // double alpha = 0.01, beta = 1, gama = 100;
    double alpha = 0.01, gama = 100;

    return (alpha * x_max * y_max) + (gama * pow((R_star - (y_max/x_max)), 2));
}

double SA::HPWL_cal() { // 取的是MODULE
    double x_min = (double)INT_MAX, x_max = 0., y_min = (double)INT_MAX, y_max = 0.;
    double node_x_temp = 0., node_y_temp = 0.;
    double HPWL_temp = 0.;

    for(int i = 0; i < (int)net_array.size(); i++) {

        if(net_array[i].module_index.size() == 1) continue; // 只有一個pin的net不用算

        x_min = (double)INT_MAX;
        x_max = 0;
        y_min = (double)INT_MAX;
        y_max = 0;

        for (auto &unit : net_array[i].module_index) {
            node_x_temp = MODULE[unit].lb.x + MODULE[unit].W/2.;
            node_y_temp = MODULE[unit].lb.y + MODULE[unit].H/2.;

            if(x_min > node_x_temp) x_min = node_x_temp;
            if(x_max < node_x_temp) x_max = node_x_temp;
            if(y_min > node_y_temp) y_min = node_y_temp;
            if(y_max < node_y_temp) y_max = node_y_temp;
        }

        HPWL_temp += (x_max - x_min) + (y_max - y_min);
    }

    return HPWL_temp;
}

double SA::initial_HPWL_cal() { // 取的是initial_MODULE
    double x_min = (double)INT_MAX, x_max = 0., y_min = (double)INT_MAX, y_max = 0.;
    double node_x_temp = 0., node_y_temp = 0.;
    double HPWL_temp = 0.;

    for(int i = 0; i < (int)net_array.size(); i++) {

        if(net_array[i].module_index.size() == 1) continue; // 只有一個pin的net不用算

        x_min = (double)INT_MAX;
        x_max = 0;
        y_min = (double)INT_MAX;
        y_max = 0;

        for (auto &unit : net_array[i].module_index) {
            node_x_temp = initial_MODULE[unit].lb.x + initial_MODULE[unit].W/2.;
            node_y_temp = initial_MODULE[unit].lb.y + initial_MODULE[unit].H/2.;

            if(x_min > node_x_temp) x_min = node_x_temp;
            if(x_max < node_x_temp) x_max = node_x_temp;
            if(y_min > node_y_temp) y_min = node_y_temp;
            if(y_max < node_y_temp) y_max = node_y_temp;
        }

        HPWL_temp += (x_max - x_min) + (y_max - y_min);
    }

    return HPWL_temp;
}

void SA::SA_alg(){
    // double T0 = MODULE.size()/log(MODULE.size());
    // log_file << "T0 : " << T0 << endl;
    // log_file << "----------" << endl;

    // double r = 0.85;
    // double T = T0, T_stop = 0.5 * T; // epsilon 反3
    // int k = 5; // 5-10

    int reject_num = 0;
    // int uphill = 0;
    int counter = 1; // MT

    int movement;
    int module_index_1 = -1, module_index_2 = -1;

    // double fault_accept;
    double cost_temp;
    double cost_dif;

    int WH_temp;

    vector<int> B_tree_temp = B_tree;

    // int decluster_counter = pow(initial_MODULE.size(), 3) * 3; // initial_MODULE.size() ^ 3 * 3
    int decluster_counter = 100000;

    // -----------------------------------------------------------------------

    while(1){
        // decluster_counter = pow(MODULE.size(), 5); // initial_MODULE.size() ^ 5
        
        if(counter % decluster_counter == 0) {

            if(single_MODULE_decluster()) {
                log_file << endl;
                cout << "========== No cluster ==========" << endl;
                log_file << "========== No cluster ==========" << endl;

                // STATE SHOW
                state_show(best_state_index);

                // RESTORE
                B_tree = STATE[best_state_index].B_tree;
                MODULE = STATE[best_state_index].MODULE;

                // B_tree_initial();

                m_file_output(0,1, target_file_name + "_no_cluster_" + to_string(STATE[best_state_index].W) + "_" + to_string(STATE[best_state_index].H) + ".m");

                // Decluster
                all_MODULE_decluster();

                best_HPWL = HPWL_cal();
                log_file << "best_HPWL : " << fixed << best_HPWL << endl;

                m_file_output(0,1, target_file_name + "_" + to_string(x_max) + "_" + to_string(y_max) + ".m");
                m_file_output(1,1, "wN_" + target_file_name + "_" + to_string(x_max) + "_" + to_string(y_max) + ".m");
                
                out_file_output(target_file_name + ".out");
                break;
            }
        }
    

        // if(counter % (k * MODULE.size()) == 0) T = r * T; // kn

        // if(T < T_stop) {
        //     log_file << endl;
        //     cout << "========== temper low ==========" << endl;
        //     log_file << "========== temper low ==========" << endl;
        //     break;
        // }

        // if(uphill == 3){
        //     log_file << endl;
        //     cout << "========== too many uphill ==========" << endl;
        //     log_file << "========== too many uphill ==========" << endl;
        //     break;
        // }

        if(y_max <= H_star && x_max <= W_star){
            log_file << endl;
            cout << "========== In boundary ==========" << endl;
            log_file << "========== In boundary ==========" << endl;

            // Decluster
            all_MODULE_decluster();

            best_HPWL = HPWL_cal();
            cout << "best_HPWL : " << fixed << best_HPWL << endl;
            log_file << "best_HPWL : " << fixed << best_HPWL << endl;

            m_file_output(0,1, target_file_name + "_" + to_string(x_max) + "_" + to_string(y_max) + ".m");
            m_file_output(1,1, "wN_" + target_file_name + "_" + to_string(x_max) + "_" + to_string(y_max) + ".m");

            txt_file_output(target_file_name + ".txt");
            out_file_output(target_file_name + ".out");
            break;
        }

        // -----------------------------------------------------------------------

        // log_file << "----- step " << counter << " -----" << endl;

        // movement = rand() % 2; // ROTATE、SWAP
        // movement = rand() % 3; // 1:2 ROTATE、SWAP
        // movement = rand() % 3; // 1:1:1 ROTATE、INSERT、SWAP
        movement = rand() % 5; // 1:2:2 ROTATE、INSERT、SWAP

        // -----------------------------------------------------------------------

        if(movement == 0){ // ROTATE
            random_number(B_tree, module_index_1);

            module_rotate(module_index_1);
        }
        else if(movement < 3){ // INSERT
            swap_random_number(module_index_1, module_index_2);

            B_tree_temp = B_tree; // 備份

            module_insert(module_index_1, module_index_2);
        }
        else { // SWAP
            swap_random_number(module_index_1, module_index_2);

            B_tree_temp = B_tree; // 備份
            
            module_swap(module_index_1, module_index_2);
        }

        // 接受解 -----------------------------------------------------------------------

        // fault_accept = (initial_HPWL - optimal_W_H) * rand() / (RAND_MAX + 1.0) + optimal_W_H;
        // fault_accept = (double) rand() / (RAND_MAX + 1.0);

        cost_temp = cost_cal();
        cost_dif = best_cost - cost_temp;
        // log_file << "cost_dif : " << cost_dif << endl;

        if(cost_dif > 0){  // 先不要uphill
        // if(cost_dif > 0 || fault_accept < exp(-1 * cost_dif / T)){ // Success

            log_file << "----- step " << counter << " -----" << endl;

            best_cost = cost_temp;
            best_state_index = STATE.size();

            if(movement == 0){
                log_file << "ROTATE " << module_index_1;

                MODULE[module_index_1].rotate = !MODULE[module_index_1].rotate;
                rotate[module_index_1] = !rotate[module_index_1];
            }
            else if(movement < 3){ // INSERT
                log_file << "INSERT " << module_index_1 << " " << module_index_2;

                B_tree_temp = B_tree; // 覆蓋
            }
            else{
                log_file << "SWAP " << module_index_1 << " " << module_index_2;

                B_tree_temp = B_tree; // 覆蓋
            }

            log_file << " Success" << endl;

            log_file << "B_tree : ";
            vector_show(B_tree);
            log_file << "rotate : ";
            vector_show(rotate);

            // Tree_show(TREE.root);
            // log_file << endl;

            log_file << "W_H : (" << x_max << " , " << y_max << ")" << endl;
            log_file << "COST : " << cost_temp << endl;

            STATE.push_back({B_tree, MODULE, x_max, y_max, cost_temp});

            // csv << cost_temp << "\n";

            // m_file_output(0,1,"test" + to_string(counter) + ".m");
        }
        else { // Fault
            // log_file << " Fault" << endl;

            reject_num += 1;

            if(movement == 0){
                WH_temp = MODULE[module_index_1].W;
                MODULE[module_index_1].W = MODULE[module_index_1].H;
                MODULE[module_index_1].H = WH_temp;
            }
            else {
                B_tree = B_tree_temp; // 還原
                // vector_show(B_tree);
            }

            B_tree_initial();
        }

        counter += 1;

        // log_file << "----------" << endl;
    }
}

// -----------------------------------------------------------------------