// HW2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <cmath>

using namespace std;

class Point {
public:
	int row;
	int col;
	Point():row(0),col(0) {}
	Point(int row, int col):row(row),col(col) {}
};


class State {
public:
	vector<vector<int> > box;
	int runtimes;
	int gamepoints;
	Point justelimated;
	State()	:runtimes(0),gamepoints(0) {Point();}
	void makePoint(Point point) {
		justelimated.row = point.row;
		justelimated.col = point.col;
	}
};

class twiceint {
public:
	int fruit_types;
	bool isvisited;
	twiceint(int fruit_types, bool isvisited):fruit_types(fruit_types),isvisited(isvisited) {}
};


int search_depth = 0;


/*definition of function*/
void ReadInput(int & width, int & fruit_types, float & run_time, State &state);//run_time should map to the depth of minimax tree
void WriteOutput(int col/*letter8*/, int row/*number*/, State);

int depth_of_minimax(float run_time, int width);//to know how deep our tree could be

void fall_down(State&);//find the succedent state of a just-elimated box(with * in it)

int EVAL(State state);//use square to calculate evaluation


int MAX_VALUE(State, int alpha, int beta, Point &justelimated);
int MIN_VALUE(State, int alpha, int beta, Point &justelimated);
bool CUTOFF_TEST(State);
vector<State> SUCCESSORS(State state);
int MAX(int first, int second,bool &changed);
int MIN(int first, int second);
vector<vector <twiceint> > initialize_twiceint(State state);
void go(vector<vector <twiceint>> tbox, Point point, int fruit_types, vector<Point>& temp);
State Elimated_state(vector<Point> temp, State state);


int depth_of_minimax(float run_time, int width) {
	if (width > 7 && run_time < 10) { return 1; }
	if (width < 9 && run_time>50) { return 3; }
	else if (width < 9 && run_time > 20) {return 2;}
	if (width > 20) { return 1; }
	return 2;//suppose we go down 3 depth
}

int MAX(int first, int second, bool &changed) {
	if (first < second) { changed = true; return second; }
	else return first;
}
int MIN(int first, int second) {
	return first < second ? first : second;
}

void ReadInput(int & width, int & fruit_types, float & run_time, State &state) { //read in parameters and initialize box
	char c;
	ifstream fin("input.txt");
	fin >> width >> fruit_types >> run_time;
	fin.get();
	state.box.resize(width);
	for (int i = 0; i < width; ++i)	state.box[i].resize(width);
	for (int i = 0; i < width; ++i) {//row
		for (int j = 0; j < width; j++) {//col
			c = fin.get();
			if (c >= 48 && c <= 57)
				state.box[i][j] = c - 48;
			else
				state.box[i][j] = 42;//represent *
		}
		fin.get();//jump to next line
	}
	fin.close();
}
void WriteOutput(int col/*letter*/, int row/*number*/, State state) {
	ofstream fout("output.txt");
	fout.put(col + 65);
	fout << row;
	fout.put('\n');//jump to next line
	for (int i = 0; i < state.box.size(); ++i) {//row
		for (int j = 0; j < state.box.size(); ++j) {//col
			if (state.box[i][j] >= 0 && state.box[i][j] <= 9)
				state.box[i][j] += 48;
			fout.put(state.box[i][j]);
		}
		fout.put('\n');//jump to next line
	}
}

vector<vector <twiceint> > initialize_twiceint(State state) {
	vector<vector <twiceint> > tbox;
	tbox.resize(state.box.size());
	for (int i = 0; i < state.box.size(); ++i) {
		for (int j = 0; j < state.box.size(); ++j) {
			if(state.box[i][j]!=42)
			tbox[i].push_back(twiceint(state.box[i][j], false));
			else
			tbox[i].push_back(twiceint(state.box[i][j], true));
		}
	}
	return tbox;
}

bool rightend(vector<vector <twiceint>> tbox, Point point, int fruit_types) {
	if (point.col + 2 > tbox.size()) return true;//next node out of range
	if ((tbox[point.row][point.col + 1].isvisited == true) || tbox[point.row][point.col + 1].fruit_types != fruit_types) return true; //next node out out range but can't use
	return false;
}

bool downend(vector<vector <twiceint>> tbox, Point point, int fruit_types) {
	if (point.row + 2 > tbox.size()) return true;//next node out of range
	if ((tbox[point.row + 1][point.col].isvisited == true) || tbox[point.row + 1][point.col].fruit_types != fruit_types) return true; //next node out out range but can't use
	return false;
}

bool leftend(vector<vector <twiceint>> tbox, Point point, int fruit_types) {
	if (point.col -1< 0) return true;//next node out of range
	if ((tbox[point.row][point.col - 1].isvisited == true) || tbox[point.row][point.col - 1].fruit_types != fruit_types) return true; //next node out out range but can't use
	return false;
}

bool upend(vector<vector <twiceint>> tbox, Point point, int fruit_types) {
	if (point.row - 1< 0) return true;//next node out of range
	if ((tbox[point.row - 1][point.col].isvisited == true) || tbox[point.row - 1][point.col].fruit_types != fruit_types) return true; //next node out out range but can't use
	return false;
}

void go(vector<vector <twiceint>> tbox, Point point, int fruit_types, vector<Point> &temp) {
	///box start from 0 size start from 1 so add 1 on the row and col
	

	if (!rightend(tbox, point, fruit_types)) {//can go right
		tbox[point.row][point.col + 1].isvisited = true;
		go(tbox, Point(point.row, point.col + 1), fruit_types, temp);
	}
	if (!downend(tbox, point, fruit_types)) {//can go down
		tbox[point.row + 1][point.col].isvisited = true;
		go(tbox, Point(point.row + 1, point.col), fruit_types, temp);
	}
	if (!leftend(tbox, point, fruit_types)) {//can go right
		tbox[point.row][point.col - 1].isvisited = true;
		go(tbox, Point(point.row, point.col - 1), fruit_types, temp);
	}
	if (!upend(tbox, point, fruit_types)) {//can go down
		tbox[point.row - 1][point.col].isvisited = true;
		go(tbox, Point(point.row - 1, point.col), fruit_types, temp);
	}
	temp.push_back(point);
}


int EVAL(State state) {
	return state.gamepoints;
}

bool CUTOFF_TEST(State state) {
	if (state.runtimes >= search_depth) return true;
	else {
		for (auto s : state.box) {//if all the fruits are elimated
			for (auto ss : s) {
				if (ss != 42) return false;
			}
		}
		return true;
	}
}

void fall_down(State& state) {
	for (int v = 0; v < state.box.size(); ++v) {
		for (int lastNonZeroFoundAt = state.box.size() - 1, cur = state.box.size()-1; cur >=0; --cur) {
			if (state.box[cur][v] != 42) {
				swap(state.box[lastNonZeroFoundAt--][v], state.box[cur][v]);
			}
		}
	}
}

State Elimated_state(vector<Point> temp, State state) {
	for (int i = 1; i < temp.size(); ++i) {//first node is fruit_type
		state.box[temp[i].row][temp[i].col] = 42;
	}
	fall_down(state);
	state.runtimes++;//each time succesor++
	state.gamepoints += pow(-1, state.runtimes + 1)*pow(temp.size()-1, 2);
	return state;
}

void ChangevisitState(vector<vector <twiceint> > &tbox, vector<Point> temp) {
	for (int i = 1; i < temp.size(); ++i) {
		tbox[temp[i].row][temp[i].col].isvisited = true;
	}
}

vector<State> SUCCESSORS(State state) {
	vector<State> Vstate;
	vector<vector <twiceint> > tbox = initialize_twiceint(state);
	for (int i = 0; i < tbox.size(); ++i) {
		for (int j = 0; j < tbox.size(); ++j) {
			if (!tbox[i][j].isvisited) {
				vector<Point> temp;
				tbox[i][j].isvisited = true;
				go(tbox, Point(i, j), tbox[i][j].fruit_types, temp);
				temp.insert(temp.begin(), Point(tbox[i][j].fruit_types, -1));/////first node contains the fruit_type with col == -1
				ChangevisitState(tbox, temp);///note visited point 
				state.makePoint(temp[1]); //note the elimated point that lead to this situation
				Vstate.push_back(Elimated_state(temp, state));
			}
		}
	}
	return Vstate;
}


int MAX_VALUE(State state, int alpha, int beta, Point &justelimated) {
	if (CUTOFF_TEST(state)) { justelimated.col = state.justelimated.col; justelimated.row = state.justelimated.row; return EVAL(state); }
	vector<State> Vstate = SUCCESSORS(state);
	for (auto s : Vstate) {
		bool changed = false;
		alpha = MAX(alpha, MIN_VALUE(s, alpha, beta, justelimated),changed);
		if (changed) { justelimated.col = s.justelimated.col; justelimated.row = s.justelimated.row;}
		if (alpha >= beta) { return beta; }
	}
	return alpha;
}

int MIN_VALUE(State state, int alpha, int beta, Point &justelimated) {
	if (CUTOFF_TEST(state)) return EVAL(state);
	vector<State> Vstate = SUCCESSORS(state);
	for (auto s : Vstate) {
		beta = MIN(beta, MAX_VALUE(s, alpha, beta, justelimated));
		if (beta <= alpha) return alpha;
	}
	return beta;
}



int main()
{
	int width, fruit_types;
	int alpha = INT_MIN, beta = INT_MAX;
	float run_time;
	Point justelimated;
	State state;
	ReadInput(width, fruit_types, run_time, state);
	search_depth = depth_of_minimax(run_time, width);
	int i = MAX_VALUE(state, alpha, beta, justelimated);
	/*build the final state*/
	vector<vector <twiceint> > tbox = initialize_twiceint(state);
	vector<Point> temp;
	tbox[justelimated.row][justelimated.col].isvisited = true;
	go(tbox, Point(justelimated.row, justelimated.col), tbox[justelimated.row][justelimated.col].fruit_types, temp);
	temp.insert(temp.begin(), Point(tbox[justelimated.row][justelimated.col].fruit_types, -1));/////first node contains the fruit_type with col == -1
	state = Elimated_state(temp, state);


	WriteOutput(justelimated.col/*letter*/, justelimated.row+1/*number*/, state);


	return 0;
}

