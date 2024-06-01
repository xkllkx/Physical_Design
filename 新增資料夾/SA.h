#ifndef _SA_H_
#define _SA_H_

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <fstream>

#include <climits>

#include <time.h>
#include "TreeNode.h"

using namespace std;

class SA;
struct coor;
struct line;
struct module;

struct coor{ 
	int x = 0,y = 0;

	bool operator ==(const coor &b) const{
        return (x == b.x && y == b.y);
    }
};

struct line{ 
	coor start,end;
	int module_index;

	bool operator ==(const line &b) const{
        return (start == b.start && end == b.end && module_index == b.module_index);
    }
};

struct module{
	string module_name;
	int W = 0,H = 0;
	int area = 0;
	coor lb;
	coor lb_temp; // 暫存 因為移動不一定成立
	bool rotate = 0; // 有轉:1

	vector<int> cluster;

	TreeNode* node = NULL;
};

struct state{
	vector<int> B_tree;
	// vector<bool> rotate;
	vector<module> MODULE;
	int W, H;
	double cost = 0;
};

struct net{
	string net_name;
	set<int> module_index; // set不重複
};

class SA{ // FloorPlanning
private:
	int R_star; // aspect_ratio(R*) = H* / W* = 1,2,3
	int whitespace; // r = 15, 10 // ratio %

	int W_star = 0; // W*
	int H_star = 0.; // H*

	int x_max = 0;
	int y_max = 0;

	int A = 0; // module_area_sum
	int A_star = 0;
	// int A_avg = 0;
	double optimal_W_H = 0;
	double initial_HPWL = 0;

	clock_t start_time;
	double run_time = 0;

	vector<module> initial_MODULE;
	vector<module> MODULE;
	map<string, int> module_map;
	vector<net> net_array;
	map<string, int> net_map;

	vector<state> STATE;
	double best_cost = (double)INT_MAX;
	double best_HPWL = (double)INT_MAX;
	int best_state_index;
	
	BST TREE; // B* tree
	vector<int> B_tree;
	vector<bool> rotate; // 用來顯示
	vector<line> contour;

	ofstream log_file;
	ofstream operation_file;
	// ofstream csv;

	string target_file_name;

	// TOOL -------------------------------------------------------------------
	void vector_show(vector<int> show_vector);
	void vector_show(vector<bool> show_vector);
	void Tree_show(TreeNode* temp);
	void set_show(set<int> show_set);

	int vector_find(vector<int> find_vector, int temp);
	int vector_find(vector<coor> find_vector, coor temp);
	int vector_find(vector<line> find_vector, line temp);

	int vector_rfind(vector<int> find_vector, int temp);
	
	// SHOW -------------------------------------------------------------------
	void module_show(vector<module> &show_module);
	void coor_show(vector<coor> &show_coor);
	void contour_show(vector<line> &contour);
	void state_show(int state_index);

	// INPUT -------------------------------------------------------------------
	void yal_input(string file_name);

	// MODULE cluster ----------------------------------------------------------
	void MODULE_cluster();

	// OUTPUT -------------------------------------------------------------------
	void txt_file_output(string file_name);
	void out_file_output(string file_name);
	void m_file_output(bool print_name, bool temp, string file_name);

	// B* TREE -------------------------------------------------------------------
	void B_tree_initial();

	// 更新 x_coor
	void Pre_x_coor_update(bool overwrite, TreeNode* temp);
	// 更新 y_coor
	void Pre_contour_update(bool overwrite, TreeNode* temp);

	void line_insert(line insert_line);

	bool boundry_check();
	vector<int> cluster_list_boundry_check(vector<int> cluster_list);

	bool TETRIS_Insert();

	// OPERATION -------------------------------------------------------------------
	void module_rotate(int module_index);
	void module_swap(int module_index_1, int module_index_2);
	void module_insert(int module_index_1, int module_index_2);

	// SA -------------------------------------------------------------------
	void random_number(vector<int> module_index_list, int &module_index_1);
	void swap_random_number(int &module_index_1, int &module_index_2);
	
	void all_MODULE_decluster();
	bool single_MODULE_decluster();

	double cost_cal();
	double HPWL_cal();
	double initial_HPWL_cal();

	void SA_alg();

public:
	// ./SA_M16121093 ami33.yal 1 10 ami33_1_10.txt ami33_1_10.out ami33_1_10.m
	SA(string yal_file_name, string R_star_str, string whitespace_str, string txt_file_name, string out_file_name, string m_file_name) {
		start_time = clock();

		srand(time(NULL));

		target_file_name = txt_file_name.substr(0,txt_file_name.find("."));

		log_file.open("benchmark/" + target_file_name + ".log");
		// csv.open("benchmark/" + target_file_name + ".csv");
		cout << "========== " << target_file_name << " ==========" << endl;

		R_star = stoi(R_star_str);
		whitespace = stoi(whitespace_str);
		log_file << "R_star : " << R_star << endl;
		log_file << "whitespace_ratio : " << whitespace << " %" << endl;

		yal_input(yal_file_name);
		log_file << "Run time: " << (double)(clock()-start_time)/CLOCKS_PER_SEC << " s" << endl;

		// B* TREE -------------------------------------------------------------------
		B_tree_initial();
		log_file << "Run time: " << (double)(clock()-start_time)/CLOCKS_PER_SEC << " s" << endl;

		// OPERATION -------------------------------------------------------------------
		log_file << "---------- OPERATION ----------" << endl;
		SA_alg();

		log_file.close();
        operation_file.close();
        // csv.close();

		run_time = (double)(clock()-start_time)/CLOCKS_PER_SEC;
		cout << "===============================" << endl;
		cout << "Run time: " << run_time << " s" << endl;
	};

	~SA() {};
};

#endif //_SA_H_