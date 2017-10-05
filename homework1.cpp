// homework1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <math.h>
#include <vector>
#include <set>
#include <stack>
#include <queue>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <ctime>


#define END_TEM 1e-2
#define DELTA 0.999
#define LOOPCOUNT 5

using namespace std;

class point {
public:
	int row;
	int col;
	point(int row, int col) {
		point::row = row;
		point::col = col;
	}
	point() {};
};

bool OK_spot(vector<point> grid, point cur, int style = 0);
bool OK_spot(point compared, point cur);
bool no_obscure_line(int style, point &cur);
void print(bool state, int width, int lizard_num, vector<point> grid);
bool no_obscure_global(int style, point &cur, int width, vector<point> grid);
int po_loc(point cur, int width);//count for the location code of a point
point eofgrid(vector<point> grid, int width);//last place that could put point

class board {
public:
	vector<point> grid;
	int result = 0;
	int width;
	///initialize
	board(int width, int lizard_num) {
		board::width = width;
		for (int i = 0; i < lizard_num; i++) {
			point cur(rand() % width, rand() % width);
			no_obscure_global(0, cur, width, grid);///find a place if conflict with others or obscures
			grid.push_back(cur);

		}
	}

	/*void random(int width, int lizard_num) {
		board::width = width;
		for (int i = 0; i < lizard_num; i++) {
			point cur(rand() % width, rand() % width);
			no_obscure_global(0, cur, width, grid);///find a place if conflict with others or obscures
			grid.push_back(cur);

		}
	}*/

	int fit_num() {

		set<int> conflictpoint;
		for (int i = 0; i < grid.size(); i++) {
			for (int j = i + 1; j < grid.size(); j++) {
				if (!OK_spot(grid[j], grid[i])) {
					conflictpoint.insert(grid[i].col + grid[i].row*width); conflictpoint.insert(grid[j].col + grid[j].row*width);
				}
			}
		}
		if (conflictpoint.size() <= 1) {
			int row = conflictpoint.size() / width;
			int col = conflictpoint.size() % width;
			int a = 0;
		}
		result = grid.size() - conflictpoint.size();
		return result;
	}
};

/**DFS data structure**/
vector<point> grid;
stack<point> lizard_position;

/**BFS data structure**/
queue<vector<point> > gridq;

vector<point> obscure;

bool BFS(int width, int lizard_num) {
	///initialize queue
	vector<point> curp;
	point cur(0, 0);
	no_obscure_global(0, cur,width,curp);
	while (cur.col<width) {
		vector<point> temp;
		temp.push_back(cur);
		gridq.push(temp);
		no_obscure_line(1, cur);
	}
	///loop
	while (!gridq.empty()) {
		curp = gridq.front();//get the first vector in queue,use it as the basement of next added queues
		cur = curp.back();
		no_obscure_global(1, cur, width, curp);//get the deepest point in the vector
		point end = eofgrid(curp, width);
		while (po_loc(cur,width) <= po_loc(end,width)) {//get to the last point
			if (OK_spot(curp, cur)) {
				vector<point> temp = curp;
				temp.push_back(cur);
				gridq.push(temp);
				if (temp.size()==lizard_num) {
					print(true, width, lizard_num, temp); return true;}
			}
			no_obscure_global(1, cur, width, curp);//get next feasible point
		}

		gridq.pop();
	}
	print(false, width, lizard_num, grid);
	return false;

};

board random_jump(board best, board & latest, int width) {
	//make a latest board
	latest.result = best.result;
	latest.grid.assign(best.grid.begin(), best.grid.end());
	//random change points
	for (int i = 0; i < LOOPCOUNT; i++) {
		int x = rand() % latest.grid.size();

		latest.grid[x].row = rand() % width;
		latest.grid[x].col = rand() % width;
		///erase itself or always has the same point
		vector<point> temp;
		temp.assign(latest.grid.begin(), latest.grid.end());
		temp.erase(temp.begin() + x);
		///find a feasible place
		no_obscure_global(0, latest.grid[x], width, temp);
	}
	return latest;
}


bool SA(int width, int lizard_num) {
	board latest(width, lizard_num);//new result
	board best(width, lizard_num);//best result
	double T = 10;//initial temperature
	double dE;
	srand(time(NULL));
	for (int i = 0; i < LOOPCOUNT; i++) {
		while (T>END_TEM) {
			for (int i = 0; i<20; i++) {
				random_jump(best, latest, width);
				best.fit_num();
				latest.fit_num();

				dE = latest.result - best.result;
				if (dE > 0) {//better result must get it
					best.result = latest.result;
					best.grid.assign(latest.grid.begin(), latest.grid.end());
				}
				else {//not better but in some probability get it
					double random = rand() % 100 / 100.0;
					if (dE != 0) {
						double temp1 = dE / T;
						double temp2 = exp(dE / T);
						int u = 0;
					}

					if (exp((dE / T)) > random) {
						best.result = latest.result;
						best.grid.assign(latest.grid.begin(), latest.grid.end());
					}
				}
				//cout << best.result << endl;
				if (best.result == lizard_num) { print(true, width, lizard_num, best.grid); return true; }
			}
			T *= DELTA;
		}
		///can't find the right answer
		if (i < LOOPCOUNT) continue;
		print(false, width, lizard_num, best.grid);
		return false;
	}
	
}
bool has_tree(int style, point grid, point cur) {//style 1:horizontal 2:vertical 3:left diagonal 4: right diagonal
	if (style == 1) {
		for (int i = 0; i<obscure.size(); i++) {
			if ((grid.row == obscure[i].row) &&   ((obscure[i].col>grid.col&&obscure[i].col<cur.col) || (obscure[i].col>cur.col&&obscure[i].col<grid.col))   ){return true;}
		}
	}
	else if (style == 2) {
		for (int i = 0; i<obscure.size(); i++) {
			if ((grid.col == obscure[i].col) &&   ((obscure[i].row>grid.row&&obscure[i].row<cur.row) || (obscure[i].row>cur.row&&obscure[i].row<grid.row))   ){return true;}
		}
	}
	else if (style == 3) {
		for (int i = 0; i<obscure.size(); i++) {
			if (((grid.col - grid.row) == (obscure[i].col - obscure[i].row)) && (   (obscure[i].row>grid.row&&obscure[i].row<cur.row) || (obscure[i].row>cur.row&&obscure[i].row<grid.row))   ){return true;}
		}
	}
	else if (style == 4) {
		for (int i = 0; i<obscure.size(); i++) {
			if (((grid.col + grid.row) == (obscure[i].col + obscure[i].row)) && (   (obscure[i].row>grid.row&&obscure[i].row<cur.row) || (obscure[i].row>cur.row&&obscure[i].row<grid.row))   ){return true;}
		}
	}
	else {
		exit(0);
	}
	return false;
}

bool OK_spot(vector<point> grid, point cur, int style) {/// style = 1 drop off point itself

	if (style == 1) {//cur in grid so drop it
		for (int x = 0; x < grid.size(); x++) {
			if (grid[x].col == cur.col && grid[x].row == cur.row) {
				grid.erase(grid.begin() + x);
				break;
			}
		}
	}
	/*for (int j = 0; j<obscure.size(); j++) {
		if (obscure[j].row == cur.row&&obscure[j].col == cur.col)
			return false;
	}*/
	for (int i = 0; i < grid.size(); i++) {
		if (grid[i].row == cur.row) {
			if (!has_tree(1, grid[i], cur)) { return false; }
		}
		if (grid[i].col == cur.col) {
			if (!has_tree(2, grid[i], cur)) { return false; }
		}
		if ((grid[i].col - grid[i].row) == (cur.col - cur.row)) {
			if (!has_tree(3, grid[i], cur)) { return false; }
		}
		if (grid[i].col + grid[i].row == cur.col + cur.row) {
			if (!has_tree(4, grid[i], cur)) { return false; }
		}
	}
	return true;
}

bool OK_spot(point compared, point cur) {
	if (compared.row == cur.row) {
		if (!has_tree(1, compared, cur)) { return false; }
	}
	if (compared.col == cur.col) {
		if (!has_tree(2, compared, cur)) { return false; }
	}
	if ((compared.col - compared.row) == (cur.col - cur.row)) {
		if (!has_tree(3, compared, cur)) { return false; }
	}
	if ((compared.col + compared.row) == (cur.col + cur.row)) {
		if (!has_tree(4, compared, cur)) { return false; }
	}
	return true;
}

void print(bool state, int width, int lizard_num, vector<point> grid) {

	ofstream fout("output.txt");
	if (state == true) {
		fout << "OK" << '\n';
		int po = int(fout.tellp());
		/**make a '0' matrix**/
		for (int i = 0; i < width; i++) {//row
			for (int j = 0; j < width; j++) {//col
				fout.put('0');
			}
			fout.put('\n');//jump to next line
		}
		/*in windows, when use \n, it atuomally add a \r, so every line has a \r\n in the end*/
		int pos = int(fout.tellp()) - width*(width + 1);/**beginning of matrix**/
		fout.seekp(pos, ios_base::beg);
		for (int k = 0; k<grid.size(); k++) {
			//     int temp = grid[k].row*(width + 1) + grid[k].col;
			fout.seekp(grid[k].row*(width + 1) + grid[k].col, ios_base::cur);

			fout.put('1');
			fout.seekp(pos, ios_base::beg);
		}
		for (int l = 0; l<obscure.size(); l++) {
			fout.seekp(obscure[l].row*(width + 1) + obscure[l].col, ios_base::cur);
			fout.put('2');
			fout.seekp(pos, ios_base::beg);
		}
	}
	else { fout << "FAIL"; }
	fout.close();
}

bool no_obscure_line(int style, point &cur) {/*1 means need move 0 means don't need*/
	if (style == 1) { cur.col++; }
	bool ifchanged = false;
	for (int i = 0; i<obscure.size(); i++) {
		if (obscure[i].col == cur.col && obscure[i].row == cur.row) {
			cur.col++;
			ifchanged = true;
		}
	}
	return ifchanged;
}

/*sub fuction of on_obscure_global*/

bool hasob(point cur) {
	for (int i = 0; i<obscure.size(); i++) {
		if (obscure[i].col == cur.col && obscure[i].row == cur.row) return true;
	}
	return false;
}
bool hasgr(point cur, vector<point> grid) {
	for (int j = 0; j<grid.size(); j++) {
		if (grid[j].col == cur.col && grid[j].row == cur.row) return true;
	}
	return false;
}


point eofgrid(vector<point> grid,int width) {
	point cur;
	for (int i = width - 1; i >= 0; i--) {
		for (int j = width - 1; j >= 0; j--) {
			cur.row = i; cur.col = j;
			if (  !(hasob(cur) || hasgr(cur, grid))  ) { return cur; }
		}
	}
	return point(-1, -1);
}

bool no_obscure_global(int style, point &cur, int width, vector<point> grid) {/*1 means need move 0 means don't need ,  possible to move out of the grid*/ 
	if (style == 1) { 
		cur.row += (cur.col + 1) / width;
		cur.col = ++cur.col%width;
	}
	bool ifchanged = false;
	while (hasob(cur) || hasgr(cur, grid)) {
		cur.row += (cur.col+1) / width;
		cur.col = ++cur.col%width;
		ifchanged = true;
	}
	return ifchanged;
}


int po_loc(point cur, int width) {
	return cur.row*width + cur.col;
}

bool DFS(int width, int lizard_num) {
	///initialize
	point cur(0, 0);///current pointing place
	no_obscure_global(0, cur, width, grid);///find a feasible place
	if (cur.col < width&&cur.row < width) {//first one must feasible
		lizard_position.push(cur);
		grid.push_back(cur);
		if ((--lizard_num) <= 0) {
			print(true, width, lizard_num, grid); return true;
		}
	}
	no_obscure_global(0, cur, width, grid);
	lizard_position.push(cur);//second one
	grid.push_back(cur);

	///loop
	while (!lizard_position.empty() ) {//find a feasible point then add into the grid
		
		vector<point> temp;///drop off the newly added point
		temp.assign(grid.begin(), grid.end());
		for (int x = 0; x < temp.size(); x++) {
			if (temp[x].col == cur.col && temp[x].row == cur.row) {
				temp.erase(temp.begin() + x);
			}
		}
		point end = eofgrid(temp,width);

		if (po_loc(cur,width)<=po_loc(end,width)) {//still in grid
			if (OK_spot(grid, cur, 1)) {
				if ((--lizard_num) <= 0) { 
					print(true, width, lizard_num, grid); return true; 
				}
			}
			else{///not get to the end
				lizard_position.pop();
				grid.pop_back();
			}
			no_obscure_global(1, cur, width, grid);///move to next place
			lizard_position.push(cur);
			grid.push_back(cur);
		}
		else{///come to the end but still have lizard
			lizard_position.pop();//need go to up node
			grid.pop_back();
			if(lizard_position.empty()){
				print(false, width, lizard_num, grid);
				return false;
			}
			cur = lizard_position.top();
			lizard_position.pop();
			grid.pop_back();
			no_obscure_global(1, cur, width, grid);
			lizard_position.push(cur);
			grid.push_back(cur);
			lizard_num++;
		}
	}
	print(false, width, lizard_num, grid);
	return false;
}





int main()
{
	/*read in input.txt*/
	int width, lizard_num;
	string str;//use which algorithm

	ifstream fin("input.txt");
	fin >> str >> width >> lizard_num;
	fin.get();

	for (int i = 0; i < width; i++) {//row
		for (int j = 0; j < width ; j++) {//col, take care of \n
			if (fin.get() == '2') {
				obscure.push_back(point(i, j));//store tree location
			}
		}
		fin.get();//jump to next line
	}

	/*choose algorithm*/
	if (str.compare("DFS") == 0) {
		DFS(width, lizard_num);
	}
	else if (str.compare("BFS") == 0) {
		BFS(width, lizard_num);
	}
	else if (str.compare("SA") == 0) {
		SA(width, lizard_num);
	}
	else {
		cout << "input error" << endl;
	}


	return 0;
}
