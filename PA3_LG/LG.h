#ifndef _LG_H_
#define _LG_H_

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

// #include <sys/stat.h> // fileExists()

#include <time.h>

using namespace std;

class LG;

struct coor;
struct sub_node;

struct node;
struct net;

struct node_cluster;
struct row;

struct coor{ 
	int x = 0,y = 0;

	bool operator ==(const coor &b) const{
        return (x == b.x && y == b.y);
    }
};

struct sub_node{
	int host;

	coor lb;
	int W = 0, H = 0;
};

struct node{
	string node_name;
	int index;
	int row_index = -1;

	coor lb;
	coor lb_temp;
	
	int W = 0, H = 0;

	bool orient = 0; // 0:N 1:Y
	int movetype = 0; // .node : 0:default 1:terminal 2:terminal_NI(allow overlap)
	// .pl : 0:default 1:/FIXED = terminal 2:/FIXED_NI = terminal_NI
	
	set<int> net_index; // set不重複

	vector<sub_node> shape; // 非矩形方塊
};

struct net{
	string net_name;
	int index;

	// 加速HPWL運算
	double x_min = -1., x_max = -1.; // 在total_HPWL_cal做第一次記錄
	double y_min = -1., y_max = -1.;

	double HPWL = -1.;

	// 有自連 // 因為還要篩選offset 所以用vector而不是set
	vector<int> node_index;
	vector<double> node_x_offset;
	vector<double> node_y_offset;
};

struct row{
	int index;

	coor lb;
	int W = 0; // 每個row H相同 但W不一定相同

	list<sub_node> node_list; // node_index // 照x排
};

class LG{ // Abacus Legalization
private:
	int NumNodes = 0; // number of nodes
	int NumTerminals = 0; // number of terminals
	int NumMovableNodes = 0; // number of movable nodes
	int NumTerminal = 0; // number of terminals
	int NumTerminal_NI = 0; // number of terminal_NIs

	int NumNets = 0; // number of nets
	int NumPins = 0; // number of pins

	int NumNonRectangularNodes = 0; // number of NonRectangularNodes
	int NumRows = 0; // number of Rows

	int Row_H = 0;

	int initial_HPWL = 0, total_HPWL = 0;

	vector<node> node_array;
	map<string, int> node_map;
	vector<int> move_node_order;

	vector<net> net_array;

	vector<row> row_array; // 照y排

	ofstream log_file;

	clock_t start_time;

	bool time_out = 0;

	// TOOL -------------------------------------------------------------------
	void set_show(set<int> show_set);
	void vector_show(vector<int> show_vector);
	int vector_find(vector<int> find_vector, int temp);

	// bool fileExists(const std::string &filePath) {
	// 	struct stat buffer;
	// 	return (stat(filePath.c_str(), &buffer) == 0);
	// }

	// INPUT -------------------------------------------------------------------
	void scl_input(string file_name); // row資訊
	void node_input(string file_name);
	void shape_input(string file_name); // 非矩形node資訊
	void legal_input(string file_name);
	void net_input(string file_name);

	// SHOW -------------------------------------------------------------------
	void row_show(int show_row_index);
	void row_list_show(int show_row_index);
	void net_show(int show_net_index);
	void node_show(int show_node_index);

	// HPWL_cal -------------------------------------------------------------------
	void total_HPWL_cal(bool initial, bool temp, bool recal);
	double node_HPWL_cal(int target_node_index, bool recal); // 用 lb_temp 算
	
	// ALG -------------------------------------------------------------------
	void row_list_initial();

	bool check_overlap(); // 檢查整個布局

	int row_find(bool temp, int target_node_index);
	list<sub_node>::iterator row_index_find(int row_index, int lb_x);
	list<sub_node>::iterator row_index_binary_find(int row_index, int lb_x);

	void min_HPWL_position_cal(int target_node_index);
	
	void node_X_slide(int target_row_index, int target_node_index, int &place_x, int &row_index, double &min_HPWL);

	void LG_alg();

	// OUTPUT -------------------------------------------------------------------
	void file_output(string file_name); // .pl

public:
	// ./LG_M16121093 –bench /benchmark/superblue1 –output /out/superblue1.pl
	LG(string benchmark_folder, string output_file_name) {
		start_time = clock();

		// cout << benchmark_folder << endl;
		// cout << output_file_name << endl;

		benchmark_folder = benchmark_folder.substr(1); // 去掉/
		string benchmark_name = benchmark_folder.substr(benchmark_folder.rfind("/")+1);
		string benchmark_path = benchmark_folder + "/" + benchmark_name;
		
		output_file_name = output_file_name.substr(1); // 去掉/

		log_file.open(benchmark_path + ".log");
		cout << "========== " << benchmark_name << " ==========" << endl;
		
		scl_input(benchmark_path);

		node_input(benchmark_path);
		legal_input(benchmark_path);
		shape_input(benchmark_path);
		
		net_input(benchmark_path);
		log_file << "Run time : " << (double)(clock()-start_time)/CLOCKS_PER_SEC << " s" << endl;

		log_file << "=============== LG ===============" << endl;
		cout << "=============== LG ===============" << endl;

		row_list_initial();
		total_HPWL_cal(1, 0, 1);

		int i = 0;
		clock_t iter_start = 0, iter_time = 0;

		while(!time_out) { // iteration
			log_file << "=============== ITER " << i << " ===============" << endl;
			cout << "=============== ITER " << i << " ===============" << endl;

			iter_start = clock();

			LG_alg();

			iter_time = clock() - iter_start;
			log_file << "Iter time : " << (double)iter_time/CLOCKS_PER_SEC << " s" << endl;
			cout << "Iter time : " << (double)iter_time/CLOCKS_PER_SEC << " s" << endl;
		
			i ++;
		}

		total_HPWL_cal(0, 0, 1);
		
		file_output(output_file_name);

		log_file << "===============================" << endl;
		log_file << "Run time : " << (double)(clock()-start_time)/CLOCKS_PER_SEC << " s" << endl;
		cout << "===============================" << endl;
		cout << "Run time : " << (double)(clock()-start_time)/CLOCKS_PER_SEC << " s" << endl;
	};

	~LG() {};
};

#endif //_LG_H_