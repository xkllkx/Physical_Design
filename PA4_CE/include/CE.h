#ifndef _CE_H_
#define _CE_H_

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <map>
#include <set> // set不重複
#include <list>
#include <string>
#include <fstream>
#include <climits>
#include <cmath>

#include <sys/stat.h> // fileExists()

#include <time.h>

extern "C" {
	#include "flute.h"
}

using namespace std;

class CE;

struct coor;
// struct sub_node;

struct node;
struct net;

struct edge;
struct grid;

struct grid_unit;

struct coor{
	int x = 0,y = 0;

	bool operator ==(const coor &b) const{
        return (x == b.x && y == b.y);
    }
};

struct node{
	string node_name;

	coor lb;
	int W = 0, H = 0;

	double center_x = 0., center_y = 0.;

	bool block = 0;

	// vector<sub_node> shape; // 非矩形方塊
};

struct net{
	string net_name;

	// D_type 是 int
	vector<int> pin_x;
	vector<int> pin_y;
};

struct edge{ 
	// 設定supply -block -demand  <0則overflow
	double overflow = 0; // demand - supply

	grid* host_grid[2] = {nullptr, nullptr}; // (上, 下) or (左, 右)
};

struct grid{ // gcell
	double total_H_overflow = 0.;
	double total_V_overflow = 0.;

	edge* edges[4] = {nullptr, nullptr, nullptr, nullptr}; // 0:上, 1:左, 2:下, 3:右
};


struct grid_unit{ // 用來sort
	int x = 0, y = 0;

	double total_H_overflow = 0.;
	double total_V_overflow = 0.;
};

struct edge_unit{ // 用來sort
	edge* grid_ptr = nullptr;

	double overflow = 0;
};


class CE{ // Abacus Legalization
private:
	int NumNodes = 0; // number of nodes
	int NumTerminals = 0; // number of terminals
	int NumMovableNodes = 0; // number of movable nodes
	int NumTerminal = 0; // number of terminals
	int NumTerminal_NI = 0; // number of terminal_NIs

	int NumNets = 0; // number of nets
	int NumPins = 0; // number of pins

	int NumBlockageNodes = 0;
	int NumBlockageNode = 0;

	coor TileSize = {0, 0}; // 寬長 // x , y
	coor GridOrigin = {0, 0};

	int grid_x_num = 0;
	int grid_y_num = 0;
	int metal_num = 0;
	
	vector<int> metal_wire_width; // metal_layer
	vector<int> metal_wire_space; // metal_layer

	int grid_num = 0, overflow_grid_num = 0;
	int edge_num = 0, overflow_edge_num = 0;

	vector<grid_unit> grid_H_overflow; // 用來sort
	vector<grid_unit> grid_V_overflow;

	vector<edge_unit> edge_overflow;

	double max_edge_overflow = 0.;
	double max_grid_H_overflow = 0.;
	double max_grid_V_overflow = 0.;

	double ACE_0_5 = 0.;
	double ACE_1 = 0.;
	double ACE_2 = 0.;
	double ACE_5 = 0.;
	double TOF = 0.; // total_overflow

	vector<vector<grid>> grid_array;

	vector<node> node_array;
	map<string, int> node_map;

	vector<net> net_array;

	ofstream log_file;

	clock_t start_time;

	// TOOL -------------------------------------------------------------------
	void set_show(set<int> show_set);
	void vector_show(vector<int> show_vector);
	int vector_find(vector<int> find_vector, int temp);

	bool fileExists(const std::string &filePath) {
		struct stat buffer;
		return (stat(filePath.c_str(), &buffer) == 0);
	}

	// INPUT -------------------------------------------------------------------
	void node_input(string file_name);
	void legal_input(string file_name);
	void net_input(string file_name);

	void route_input(string file_name); // grid_initial grid_supply_update
	void gr_input(string file_name);

	// SHOW -------------------------------------------------------------------
	void net_show(int show_net_index);
	void node_show(int show_node_index);
	void grid_show(int show_grid_index_x, int show_grid_index_y);

	// ALG -------------------------------------------------------------------
	void grid_initial();
	void block_edge_supply_update(int node_index, vector<int> block_layer);
	void grid_demand_update();
	void grid_L_demand_update();

	void overflow_cal();
	void cost_cal(string benchmark_name);

	void CE_alg();

	// OUTPUT -------------------------------------------------------------------
	void file_output(string file_name); // .pl
	void m_file_output(bool H_or_V, string file_name); // .m
	void dpx_file_output(int H_or_V, string file_name); // .dpx
	void dpx_block_output(string file_name); // .dpx

public:
	// # ./CE_M16121093 –bench /input/adaptec1.aux –output /output/adaptec1.txt
	CE(string benchmark_aux, string output_file_name) {
		start_time = clock();

		string benchmark_name = benchmark_aux.substr(benchmark_aux.rfind("/")+1, benchmark_aux.rfind(".") - benchmark_aux.rfind("/") - 1);
		string benchmark_path = "benchmarks/" + benchmark_name + "/" + benchmark_name;

		output_file_name = output_file_name.substr(1); // 去掉/
		string output_file_path = output_file_name.substr(0, benchmark_aux.rfind("."));

		log_file.open(benchmark_path + ".log");
		cout << "========== " << benchmark_name << " ==========" << endl;

		node_input(benchmark_path);
		legal_input(benchmark_path);
		net_input(benchmark_path);
		log_file << "Run time : " << (double)(clock()-start_time)/CLOCKS_PER_SEC << " s" << endl;

		log_file << "=============== CE ===============" << endl;
		cout << "=============== CE ===============" << endl;

		route_input(benchmark_path);
		log_file << "Run time : " << (double)(clock()-start_time)/CLOCKS_PER_SEC << " s" << endl;

		// dpx_block_output(output_file_path + "_block");

		readLUT(); // read look up table

		if (fileExists(benchmark_path + ".gr")) gr_input(benchmark_path);
		else grid_L_demand_update();

		overflow_cal();
		cost_cal(benchmark_name);

		file_output(output_file_name);

		m_file_output(0, output_file_path + "_H");
		m_file_output(1, output_file_path + "_V");

		// dpx_file_output(0, output_file_path + "_H");
		// dpx_file_output(1, output_file_path + "_V");

		log_file << "===============================" << endl;
		log_file << "Run time : " << (double)(clock()-start_time)/CLOCKS_PER_SEC << " s" << endl;
		cout << "===============================" << endl;
		cout << "Run time : " << (double)(clock()-start_time)/CLOCKS_PER_SEC << " s" << endl;
	};

	~CE() {};
};

#endif //_CE_H_