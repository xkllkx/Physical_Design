#ifndef _FM_H_
#define _FM_H_

#include <iostream>
#include <vector>
#include <map>
#include <set> // set不重複
#include <string>
#include <fstream>

#include <sys/stat.h>

#include <time.h>

using namespace std;

class FM;
struct cell;
struct net;
struct state;

struct cell{
	string cell_name;
	int index;
	int area;

	bool movable = 1;
	int G1or2 = 0; // 0:未定義 1:G1 2:G2
	int gain = 0;

	set<int> net_index; // set不重複

	cell* next = NULL;
	cell* prev = NULL;
};

struct net{
	string net_name;
	int index;

	set<int> cell_index; // set不重複
};

struct state{ // 只需要記動過的cell
	int gain = 0;
	int G1_area = 0;
	int mov_cell = -1;
	int cut_size = 0;
};

class FM{ // 2-way Partition
private:
	int NumNodes = 0; // number of cells
	int NumTerminals = 0; // number of terminals
	int NumMovableNodes = 0; // number of movable cells
	int NumNets = 0; // number of nets
	int NumPins = 0; // number of pins

	double r_goal = 0.4, r_cur = 0.; // 不均勻度

	int W = 0; // cell總面積
	int S_max = 0; // 最大cell面積

	int initial_G1_area = 0, initial_G2_area = 0;
	int G1_area = 0, G2_area = 0;

	int P_max = 0; // 最大pin數量
	int gain_max_index = 0;

	int initial_cut_size;
	int min_cut_size;
	int min_cut_size_index = 0;

	vector<cell> cell_array;
	map<string, int> cell_map;
	vector<net> net_array;
	map<string, int> net_map;

	vector<state> state_list;
	
	set<int> cut_net; // set不重複
	set<int> initial_node_G1;
	set<int> initial_node_G2;

	cell** bucket_list;
	cell** bucket_list_end;

	string output_file;
	ofstream log_file;

	clock_t start_time;

	void set_show(set<int> show_set);

	void node_input(string file_name);
	void net_input(string file_name);

	void initial_state_generate_indicate();
	void initial_state_generate_G1();
	void cut_net_check(int state_index);
	void cut_size_initial();
	void bucket_list_initial();
	void bucket_list_cell_move(bool f_or_b, int mov_cell_index, int updated_gain_index);

	int node_gain_initial(bool overwrite, int cell_index);

	bool balance_check(int cell_index);
	int bucket_list_index_2_gain(int bucket_list_index);
	int gain_2_bucket_list_index(int gain);
	
	void node_gain_update(int mov_cell_index);

	int base_cell_find();

	void best_state_find();
	void file_output(string file_name);

	void FM_alg();

	bool fileExists(const std::string &filePath) {
		struct stat buffer;
		return (stat(filePath.c_str(), &buffer) == 0);
	}

public:
	// FM() :NumNodes(0), NumNets(0), NumPins(0) {};
	FM(string node_file, string net_file, string output_file_name) {
		start_time = clock();

		output_file = output_file_name.substr(0,output_file_name.find("."));
		
		log_file.open("benchmark/" + output_file + ".log");
		cout << "========== " << output_file << " ==========" << endl;

		node_input(node_file);
		log_file << "Run time: " << (double)(clock()-start_time)/CLOCKS_PER_SEC << " s" << endl;
		net_input(net_file);
		log_file << "Run time: " << (double)(clock()-start_time)/CLOCKS_PER_SEC << " s" << endl;


		if (fileExists("benchmark/initial_" + output_file + ".txt")) {
			cout << "---------- INDICATE ----------" << endl;
			initial_state_generate_indicate();
		}
		else {
			cout << "---------- G1 ----------" << endl;
			initial_state_generate_G1();
		}
		
		log_file << "Run time: " << (double)(clock()-start_time)/CLOCKS_PER_SEC << " s" << endl;

		FM_alg();
		log_file << "Run time: " << (double)(clock()-start_time)/CLOCKS_PER_SEC << " s" << endl;

		file_output(output_file_name);

		cout << "===============================" << endl;
		cout << "Run time: " << (double)(clock()-start_time)/CLOCKS_PER_SEC << " s" << endl;
	};

	~FM() {};
};

#endif //_FM_H_