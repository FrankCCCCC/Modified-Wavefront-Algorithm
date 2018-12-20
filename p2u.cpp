#include <iostream>
#include <fstream>
#include <queue>
#include <string>
#include <stack>
#include <deque>
//#include <vector>
#include <map>

//using namespace std;

class Coord {
public:
	Coord(int r, int c) : row(r), col(c) {};
	Coord() : row(0), col(0) {};

	Coord operator = (Coord c) {
		row = c.row;
		col = c.col;
		return *this;
	}

	bool operator == (const Coord &c)const {
		if (row == c.row && col == c.col) { return true; }
		else { return false; }
	}
	bool operator != (const Coord &c)const {
		if (row == c.row && col == c.col) { return false; }
		else { return true; }
	}
	bool operator > (const Coord &c)const { return row > c.row; }
	bool operator < (const Coord &c)const { return row < c.row; }

	Coord up() { Coord re(row - 1, col); return re; }
	Coord down() { Coord re(row + 1, col); return re; }
	Coord left() { Coord re(row, col - 1); return re; }
	Coord right() { Coord re(row, col + 1); return re; }

	Coord rightUp() { Coord re(row - 1, col + 1); return re; }
	Coord rightDown() { Coord re(row + 1, col + 1); return re; }
	Coord leftUp() { Coord re(row - 1, col - 1); return re; }
	Coord leftDown() { Coord re(row + 1, col - 1); return re; }

	int row;
	int col;
};
Coord getCoord(int row, int col) { Coord re{ row, col }; return re; }

std::ifstream is;
std::ofstream os;

const int wall = -1, uncover = -2, sleepTime = 0;
Coord R{ 1, 1 };
Coord nowC;
int row = 0, col = 0, b = 0, full = 0, totalStep = 0, maxDis = 0, maxFront = 0;
bool** mapIsVisited;
int** mapCount;
int** mapFront;
//Ouput control
int showStrCount = 0;
bool showStr = false;
bool showStep = false;
bool showCurrent = false;
bool onlyShowTotalStep = false;
int coorOfSSBC = 0;
int record = 0;

//Debug used variables
std::string logcat[500];
Coord bugNode(-1, -1);
Coord lastStep;
int totalStepPlusBattery;

int encode(int diago, int ortho) { return diago * 1000000 + ortho; }
int decodeDis(int code) { return (code / 1000000) * 2 + code % 1000000; }
int decodeFront(int code) { return (code / 1000000) * 4 + (code % 1000000) * 3; }
int disOfCoord(Coord c) { int code = mapCount[c.row][c.col]; return (code / 1000000) * 2 + code % 1000000; }
int frontOfCoord(Coord c) { int code = mapCount[c.row][c.col]; return (code / 1000000) + (code % 1000000); }
int MFOfCoord(Coord c) { int code = mapCount[c.row][c.col]; return (code / 1000000) * 4 + (code % 1000000) * 3; }
int disBetweenABN(Coord a, Coord b) { return (a.row - b.row) * (a.row - b.row) + (a.col - b.col) * (a.col - b.col); }
/*
void showStringByCoord(std::string str, Coord c) {
	//std::string s(str + " \tPos: " + std::to_string(c.row) + " " + std::to_string(c.col) + " Row:" + std::to_string(row) + " Col:" + std::to_string(col) + "\t\t\t\t" + "\n");
	//logcat[record % 500] = s;
	//logcat[(record + 1) % 500] = "----------------------------------------------------\n";
	record++;
	if (showStr) {
		coorOfSSBC++;
		short shortX = 2 * c.col;
		short shortY = c.row + (coorOfSSBC % 7000);
		HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
		COORD position = { shortX, shortY };
		SetConsoleCursorPosition(h, position);
		//std::cout << std::dec << s;
	}

}*/

void showMapInRange(Coord bugNode) {
	for (int i = bugNode.row - 25; i < bugNode.row + 25 && i < row; i++) {
		for (int j = bugNode.col - 25; j < bugNode.col + 25 && j < col; j++) {
			if (i >= 0) {
				if (j >= 0) {
					if (mapCount[i][j] == wall) { std::cout << "W" << " "; }
					else if (!mapIsVisited[i][j]) { std::cout << " " << " "; }
					else if (bugNode.row == i && bugNode.col == j) { std::cout << "@" << " "; }
					else if (mapIsVisited[i][j]) { std::cout << "#" << " "; }
				}
			}
		}
		std::cout << std::endl;
	}
	for (int i = 0; i < 500; i++) {
		std::cout << logcat[i];
	}
}

void showStepAndBattery(Coord s) {
	//Ouput File
	b--;
	totalStep++;
	os << s.row << " " << s.col << "\n";
/*
	//Check legal
	short shortSRow = s.row;
	short shortSCol = 2 * s.col;
	short shortRow = row;
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = { shortSCol, shortSRow };
	//s Not Wall
	if (mapCount[s.row][s.col] == wall) {
		showStringByCoord("Coord is WAll ERROR", getCoord(row, 0));
		system("pause");
	}

	if (showStep) {
		SetConsoleCursorPosition(h, position);
		std::cout << "@";
		Sleep(sleepTime);
		SetConsoleCursorPosition(h, position);
		std::cout << "#";
	}
	//Auto Detect BugNode
	std::string str("");
	bool stop = false;
	if (disBetweenABN(s, lastStep) == 1) {
		if (b >= 0) {
			//Fine
		}
		else {
			stop = true;
			str += " Battery and TotalSteps Error ";
		}
	}
	else {
		if (b >= 0) {
			stop = true;
			str += " NextStep Error S: " + std::to_string(s.row) + " " + std::to_string(s.col) + " LastStep: " + std::to_string(lastStep.row) + " " + std::to_string(lastStep.col);
		}
		else {
			stop = true;
			str += " Battery, Steps, and NextStep  Error ";
		}
	}
	//Show Something
	if (stop) {
		position = { 0, shortRow };
		SetConsoleCursorPosition(h, position);
		std::cout << std::dec << "Current: " << s.row << " " << s.col << " Row:" << row << " Col:" << col << " Battery:" << b << " TotalSteps:" << totalStep << str << " " << strAt << "\n";
		showMapInRange(s);
		system("pause");
	}
	else if (showCurrent) {
		position = { 0, shortRow };
		SetConsoleCursorPosition(h, position);
		std::cout << std::dec << "Current: " << s.row << " " << s.col << " Row:" << row << " Col:" << col << " Battery:" << b << " TotalSteps:" << totalStep << "\n";
	}
	else if (onlyShowTotalStep) {
		position = { 0, shortRow };
		SetConsoleCursorPosition(h, position);
		std::cout << std::dec << " TotalSteps:" << totalStep << "\n";
	}
	//BugNode control
	if (s == bugNode) {
		showMapInRange(s);
	}
	lastStep = s;*/
}
/*
void showPath(Coord s, std::string str) {
	short shortSRow = s.row;
	short shortSCol = 2 * s.col;
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = { shortSCol, shortSRow };
	SetConsoleCursorPosition(h, position);
	std::cout << str;
	Sleep(sleepTime);
}

void showMapEmpty(int** mapCount) {
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			Coord tc(i, j);
			if (mapCount[tc.row][tc.col] == -1) { std::cout << "W" << " "; }
			else if (mapCount[tc.row][tc.col] == -2) { std::cout << " " << " "; }
			else if (mapCount[tc.row][tc.col] == 0) { std::cout << "0" << " "; }

		}
		std::cout << std::endl;
	}
}

void showMapCount(int** mapCount) {
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			Coord tc(i, j);
			if (mapCount[tc.row][tc.col] == -1) { std::cout << "W" << " "; }
			else if (mapCount[tc.row][tc.col] == -2) { std::cout << " " << " "; }
			else if (mapCount[tc.row][tc.col] == 0) { std::cout << "0" << " "; }
			else {
				if (disOfCoord(tc) < 16) { std::cout << std::hex << disOfCoord(tc) << " "; }
				else { std::cout << std::hex << disOfCoord(tc); }
			}
		}
		std::cout << std::endl;
	}
}

void showMapCountFront(int** mapCount) {
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			Coord tc(i, j);
			if (mapCount[tc.row][tc.col] == -1) { std::cout << "W" << " "; }
			else if (mapCount[tc.row][tc.col] == -2) { std::cout << " " << " "; }
			else if (mapCount[tc.row][tc.col] == 0) { std::cout << "0" << " "; }
			else {
				if (frontOfCoord(tc) < 16) { std::cout << std::hex << frontOfCoord(tc) << " "; }
				else { std::cout << std::hex << frontOfCoord(tc); }
			}
		}
		std::cout << std::endl;
	}
}

void showMapCountData(int** mapCount) {
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			Coord tc(i, j);
			if (mapCount[tc.row][tc.col] == -1) { std::cout << "W" << " "; }
			else if (mapCount[tc.row][tc.col] == -2) { std::cout << " " << " "; }
			else if (mapCount[tc.row][tc.col] == 0) { std::cout << "0" << " "; }
			else {
				std::cout << std::hex << mapCount[tc.row][tc.col] / 1000000 << std::hex << mapCount[tc.row][tc.col] % 1000000;
			}
		}
		std::cout << std::endl;
	}
}
*/
bool isNeedCharge(Coord now) {
	if (b <= disOfCoord(now) + 2) {
		Coord t = now;
		std::stack<Coord> path;
		while (1) {
			int disOfT = disOfCoord(t);
			int disOfUp = disOfCoord(t.up());
			int disOfDown = disOfCoord(t.down());
			int disOfLeft = disOfCoord(t.left());
			int disOfRight = disOfCoord(t.right());
			if (disOfUp < disOfT && disOfUp >= 0) { path.push(t.up()); }
			else if (disOfDown < disOfT && disOfDown >= 0) { path.push(t.down()); }
			else if (disOfLeft < disOfT && disOfLeft >= 0) { path.push(t.left()); }
			else if (disOfRight < disOfT && disOfRight >= 0) { path.push(t.right()); }

			//b--;
			showStepAndBattery(path.top());
			//if (b < 0) { std::cout << path.top().row << "rrr" << path.top().col << "\n"; system("pause"); }
			t.row = path.top().row;
			t.col = path.top().col;
			if (t == R) break;
		}
		b = full;
		path.pop();
		//b--;
		while (!path.empty()) {
			t.row = path.top().row;
			t.col = path.top().col;
			//b--;
			showStepAndBattery(t);
			//if (b < 0) { std::cout << "eee"; system("pause"); }
			path.pop();
		}
		showStepAndBattery(now);
		return true;

	}
	else { return false; }
}

class Node {
public:
	Node(Coord c, Node *f, Coord start, Coord goal) : coord(c), from(f), value(((goal.row - c.row)*(goal.row - c.row) + (goal.col - c.col)*(goal.col - c.col) + (start.row - c.row)*(start.row - c.row) + (start.col - c.col)*(start.col - c.col))) {};
	Node(Coord c, Coord goal) : coord(c), from(NULL), value((goal.row - c.row)*(goal.row - c.row) + (goal.col - c.col)*(goal.col - c.col)) {};

	//bool operator < (const Node &a) const { return value < a.value; }
	//bool operator > (const Node &a) const { return value > a.value; }

	Coord coord;
	Node* from;
	int value;
};

int ctoi(Coord c) {
	return c.row * 10000 + c.col;
}

bool Astar(Coord toB, Coord fromA) {
	//A* path search
	if (mapCount[toB.row][toB.col] == wall || mapCount[fromA.row][fromA.col] == wall || fromA == toB) { return false; }
	if (fromA.row<0 || fromA.row>row || fromA.col<0 || fromA.col>col || toB.row<0 || toB.row>row || toB.col<0 || toB.col>col) { return false; }

	auto cmp0 = [](Node* left, Node* right) { return left->value > right->value; };
	std::priority_queue<Node*, std::vector<Node*>, decltype(cmp0)> pq(cmp0);
	std::map<int, Node*> mappq;
	Node* now = NULL;
	Node *A = new Node(fromA, NULL, fromA, toB);
	pq.push(A);
	mappq.insert(std::pair<int, Node*>(ctoi(fromA), A));
	//showStringByCoord("A*: Enter fromA " + std::to_string(toB.row) + " " + std::to_string(toB.col) + " ToB " + std::to_string(fromA.row) + " " + std::to_string(fromA.col), getCoord(row, 0));
	//showStringByCoord("A*: Enter pq.top " + std::to_string(pq.top()->coord.row) + " " + std::to_string(pq.top()->coord.col), getCoord(row, 0));
	//showStringByCoord("A*: A " + std::to_string(ctoi(fromA)) + " ", getCoord(row, 0));

	while (!pq.empty()) {
		Node* pqTop = pq.top();
		pq.pop();
		//if (pq.empty()) { showStringByCoord("A*: TOP EMPTY ", getCoord(row, 0)); }
		Coord up(pqTop->coord.row - 1, pqTop->coord.col);
		Coord down(pqTop->coord.row + 1, pqTop->coord.col);
		Coord left(pqTop->coord.row, pqTop->coord.col - 1);
		Coord right(pqTop->coord.row, pqTop->coord.col + 1);
		std::string str("A*: pqTop " + std::to_string(pqTop->coord.row) + " " + std::to_string(pqTop->coord.col));
		//showStringByCoord("A*: Loop " + str, getCoord(row, 0));

		//showStringByCoord("A*: LEFT row: " + std::to_string(left.row) + " col: " + std::to_string(left.col) + " !wall " + std::to_string(mapCount[left.row][left.col] != wall) + " !FoundInMap " + std::to_string(!mappq.count(ctoi(left))), getCoord(row, 0));
		if (mapCount[left.row][left.col] != wall && !mappq.count(ctoi(left))) {
			Node* leftOfpq = new Node(left, pqTop, fromA, toB);
			mappq.insert(std::pair<int, Node*>(ctoi(left), leftOfpq));
			//showPath(left, "#");
			//showStringByCoord("A*: LEFT Add map: ", getCoord(row, 0));

			if (left != toB) {
				pq.push(leftOfpq);
				//showStringByCoord("A*: LEFT Add pq: ", getCoord(row, 0));
				//showPath(left, "@");
			}
			else {
				now = leftOfpq;
				//showStringByCoord("A*: Found " + std::to_string(now->coord.row) + " " + std::to_string(now->coord.col), getCoord(row, 0));
				break;
			}
		}
		//showStringByCoord("A*: RIGHT row: " + std::to_string(right.row) + " col: " + std::to_string(right.col) + " !wall " + std::to_string(mapCount[right.row][right.col] != wall) + " !FoundInMap " + std::to_string(!mappq.count(ctoi(right))), getCoord(row, 0));
		if (mapCount[right.row][right.col] != wall && !mappq.count(ctoi(right))) {
			Node* rightOfpq = new Node(right, pqTop, fromA, toB);
			mappq.insert(std::pair<int, Node*>(ctoi(right), rightOfpq));
			//showPath(right, "#");
			//showStringByCoord("A*: RIGHT Add Map: ", getCoord(row, 0));

			if (right != toB) {
				pq.push(rightOfpq);
				//showStringByCoord("A*: RIGHT Add pq: ", getCoord(row, 0));
				//showPath(right, "@");
			}
			else {
				now = rightOfpq;
				//showStringByCoord("A*: Found " + std::to_string(now->coord.row) + " " + std::to_string(now->coord.col), getCoord(row, 0));
				break;
			}
		}

		//showStringByCoord("A*: DOWN row: " + std::to_string(down.row) + " col: " + std::to_string(down.col) + " !wall " + std::to_string(mapCount[down.row][down.col] != wall) + " !FoundInMap " + std::to_string(!mappq.count(ctoi(down))), getCoord(row, 0));
		if (mapCount[down.row][down.col] != wall && !mappq.count(ctoi(down))) {
			Node* downOfpq = new Node(down, pqTop, fromA, toB);
			mappq.insert(std::pair<int, Node*>(ctoi(down), downOfpq));
			//showPath(down, "#");
			//showStringByCoord("A*: DOWN Add map: ", getCoord(row, 0));

			if (down != toB) {
				pq.push(downOfpq);
				//showStringByCoord("A*: DOWN Add pq: ", getCoord(row, 0));
				//showPath(down, "@");
			}
			else {
				now = downOfpq;
				//showStringByCoord("A*: Found " + std::to_string(now->coord.row) + " " + std::to_string(now->coord.col), getCoord(row, 0));
				break;
			}
		}

		//showStringByCoord("A*: UP row: " + std::to_string(up.row) + " col: " + std::to_string(up.col) + " !wall " + std::to_string(mapCount[up.row][up.col] != wall) + " !FoundInMap " + std::to_string(!mappq.count(ctoi(up))), getCoord(row, 0));
		if (mapCount[up.row][up.col] != wall && !mappq.count(ctoi(up))) {
			Node* upOfpq = new Node(up, pqTop, fromA, toB);
			mappq.insert(std::pair<int, Node*>(ctoi(up), upOfpq));
			//showPath(up, "#");
			//showStringByCoord("A*: UP Add map: ", getCoord(row, 0));

			if (up != toB) {
				pq.push(upOfpq);
				//showStringByCoord("A*: UP Add pq: ", getCoord(row, 0));
				//showPath(up, "@");
			}
			else {
				now = upOfpq;
				//showStringByCoord("A*: Found " + std::to_string(now->coord.row) + " " + std::to_string(now->coord.col), getCoord(row, 0));
				break;
			}
		}/*
		std::string str1;
		for (std::map<int, Node*>::iterator it = mappq.begin(); it != mappq.end(); it++) {
			str1 = "A*: Coord in map: " + std::to_string(it->first) + "\n";
			//showStringByCoord(str1,getCoord(row, 0));
		}*/
	}
	//if (pq.empty()) { showStringByCoord("A*: TOP EMPTY^^^ ", getCoord(row, 0)); }

	if (now != NULL) {

		now = now->from;
		while (now->coord != fromA) {
			//showStringByCoord("A*: Output " + std::to_string(now->coord.row) + " " + std::to_string(now->coord.col), getCoord(row, 0));
			showStepAndBattery(now->coord);
			isNeedCharge(now->coord);

			now = now->from;
		}
		//showStringByCoord("A*: Output End", getCoord(row, 0));
		for (std::pair<int, Node*> d : mappq) {
			delete(d.second);
			//showStringByCoord("A*: D: " + std::to_string(d.second->coord.row) + " " + std::to_string(d.second->coord.col), getCoord(row, 0));
		}
		return true;
	}
	else {
		//showStringByCoord("A*: No Output FromA: " + std::to_string(toB.row) + " " + std::to_string(toB.col) + " toB: " + std::to_string(fromA.row) + " " + std::to_string(fromA.col), getCoord(row, 0));
		return false;
	}
}

void countMap(int** mapCount, int row, int col) {
	std::queue<Coord> set;
	set.push(R);
	int ortho = 0, diago = 0;
	mapCount[R.row][R.col] = 0;

	while (!set.empty()) {
		int setSize = set.size();
		for (int i = 0; i < setSize; i++) {
			Coord t(1, 1);
			Coord frontC(set.front());
			Coord orthoNode;

			ortho = mapCount[frontC.row][frontC.col] % 1000000;
			diago = mapCount[frontC.row][frontC.col] / 1000000;

			t = frontC.up();//Go UP
			if (t.row >= 0 && mapCount[t.row][t.col] == uncover) {
				mapCount[t.row][t.col] = encode(diago, ortho + 1);
				set.push(t);

				//Orthogonal
				orthoNode = t.left();
				if (orthoNode.row >= 0 && mapCount[orthoNode.row][orthoNode.col] == uncover) {
					mapCount[orthoNode.row][orthoNode.col] = encode(diago + 1, ortho);
					set.push(orthoNode);
				}
				orthoNode = t.right();
				if (orthoNode.row < col && mapCount[orthoNode.row][orthoNode.col] == uncover) {
					mapCount[orthoNode.row][orthoNode.col] = encode(diago + 1, ortho);
					set.push(orthoNode);
				}
			}

			t = frontC.down();//Go DOWN
			if (t.row <= row && mapCount[t.row][t.col] == uncover) {
				mapCount[t.row][t.col] = encode(diago, ortho + 1);
				set.push(t);

				//Orthogonal
				orthoNode = t.left();
				if (orthoNode.row >= 0 && mapCount[orthoNode.row][orthoNode.col] == uncover) {
					mapCount[orthoNode.row][orthoNode.col] = encode(diago + 1, ortho);
					set.push(orthoNode);
				}
				orthoNode = t.right();
				if (orthoNode.row < col && mapCount[orthoNode.row][orthoNode.col] == uncover) {
					mapCount[orthoNode.row][orthoNode.col] = encode(diago + 1, ortho);
					set.push(orthoNode);
				}
			}

			t = frontC.left();//Go LEFT
			if (t.col >= 0 && mapCount[t.row][t.col] == uncover) {
				mapCount[t.row][t.col] = encode(diago, ortho + 1);
				set.push(t);

				//Orthogonal
				orthoNode = t.up();
				if (orthoNode.row >= 0 && mapCount[orthoNode.row][orthoNode.col] == uncover) {
					mapCount[orthoNode.row][orthoNode.col] = encode(diago + 1, ortho);
					set.push(orthoNode);
				}
				orthoNode = t.down();
				if (orthoNode.row < row && mapCount[orthoNode.row][orthoNode.col] == uncover) {
					mapCount[orthoNode.row][orthoNode.col] = encode(diago + 1, ortho);
					set.push(orthoNode);
				}
			}

			t = frontC.right();//Go RIGHT
			if (t.col <= col && mapCount[t.row][t.col] == uncover) {
				mapCount[t.row][t.col] = encode(diago, ortho + 1);
				set.push(t);

				//Orthogonal
				orthoNode = t.up();
				if (orthoNode.row >= 0 && mapCount[orthoNode.row][orthoNode.col] == uncover) {
					mapCount[orthoNode.row][orthoNode.col] = encode(diago + 1, ortho);
					set.push(orthoNode);
				}
				orthoNode = t.down();
				if (orthoNode.row < row && mapCount[orthoNode.row][orthoNode.col] == uncover) {
					mapCount[orthoNode.row][orthoNode.col] = encode(diago + 1, ortho);
					set.push(orthoNode);
				}
			}
			set.pop();
		}
		maxDis = diago * 2 + ortho;
		maxFront = diago + ortho;
	}
	//mapCount[R.row][R.col] = 0;
}

//Select Coord put into stack, if neighbor are visited, return;
void swipe(bool** mapIsVisited, int** mapCount, int row, int col, Coord st) {
	//Use wavefront ALgorithm
	//Follow the wavefront and walk, put node into stack<Coord> stck, repeatly
	std::stack<Coord> stck;
	Coord current = st;
	Coord lastCurrent(current);
	Coord minNode(st);
	bool isHop = false;

	mapIsVisited[st.row][st.col] = true;
	stck.push(st);

	while (1) {
		while (1) {
			//find a smallest distance neighbor node that is uncover
			int minDis = row * col;
			Coord up(current.up());
			Coord down(current.down());
			Coord left(current.left());
			Coord right(current.right());
			int disUp = disOfCoord(up);
			int disDown = disOfCoord(down);
			int disLeft = disOfCoord(left);
			int disRight = disOfCoord(right);
			bool isFoundMinNide = false;

			//showStringByCoord("SWIPE Current: " + std::to_string(current.row) + " " + std::to_string(current.col), getCoord(row, 0));

			if (disUp <= minDis && !mapIsVisited[up.row][up.col] && mapCount[up.row][up.col] != wall) { minNode = up; minDis = disUp; isFoundMinNide = true; }
			if (disDown <= minDis && !mapIsVisited[down.row][down.col] && mapCount[down.row][down.col] != wall) { minNode = down; minDis = disDown; isFoundMinNide = true; }
			if (disLeft <= minDis && !mapIsVisited[left.row][left.col] && mapCount[left.row][left.col] != wall) { minNode = left; minDis = disLeft; isFoundMinNide = true; }
			if (disRight <= minDis && !mapIsVisited[right.row][right.col] && mapCount[right.row][right.col] != wall) { minNode = right; minDis = disRight; isFoundMinNide = true; }
			if (!isFoundMinNide) { isHop = true; break; }
			else {
				if (isHop) {
					isHop = false;
					//showStringByCoord("SWIPE Hop From lastCurrent: " + std::to_string(lastCurrent.row) + " " + std::to_string(lastCurrent.col) + " to current: " + std::to_string(current.row) + " " + std::to_string(current.col) + " By minNode: " + std::to_string(minNode.row) + " " + std::to_string(minNode.col), getCoord(row, 0));
					Astar(lastCurrent, current);
					showStepAndBattery(current);
					isNeedCharge(current);
				}
			}

			//showStringByCoord("SWIPE minNode: " + std::to_string(minNode.row) + " " + std::to_string(minNode.col), getCoord(row, 0));

			//Put minNode into deque, and DFS it along two direction, front and back
			std::deque<Coord> dq;
			Coord tempC(minNode);
			int currentFront = frontOfCoord(tempC);

			while (1) {//add back
				mapIsVisited[tempC.row][tempC.col] = true;
				dq.push_back(tempC);
				//showStringByCoord("SWIPE Back_tempC " + std::to_string(tempC.row) + " " + std::to_string(tempC.col) + " " + std::to_string(currentFront), getCoord(row, 0));

				up = tempC.up();
				down = tempC.down();
				left = tempC.left();
				right = tempC.right();
				if (frontOfCoord(up) == currentFront && !mapIsVisited[up.row][up.col]) { tempC = up; }
				else if (frontOfCoord(down) == currentFront && !mapIsVisited[down.row][down.col]) { tempC = down; }
				else if (frontOfCoord(left) == currentFront && !mapIsVisited[left.row][left.col]) { tempC = left; }
				else if (frontOfCoord(right) == currentFront && !mapIsVisited[right.row][right.col]) { tempC = right; }
				else break;
			}
			tempC = minNode;
			//showStringByCoord("SWIPE pop_front " + std::to_string(dq.front().row) + " " + std::to_string(dq.front().col) + " " + std::to_string(currentFront), getCoord(row, 0));
			dq.pop_front();//pop Front
			while (1) {//add front
				mapIsVisited[tempC.row][tempC.col] = true;
				dq.push_front(tempC);
				//showStringByCoord("SWIPE Front_tempC " + std::to_string(tempC.row) + " " + std::to_string(tempC.col) + " " + std::to_string(currentFront), getCoord(row, 0));
				up = tempC.up();
				down = tempC.down();
				left = tempC.left();
				right = tempC.right();
				if (frontOfCoord(up) == currentFront && !mapIsVisited[up.row][up.col]) { tempC = up; }
				else if (frontOfCoord(down) == currentFront && !mapIsVisited[down.row][down.col]) { tempC = down; }
				else if (frontOfCoord(left) == currentFront && !mapIsVisited[left.row][left.col]) { tempC = left; }
				else if (frontOfCoord(right) == currentFront && !mapIsVisited[right.row][right.col]) { tempC = right; }
				else break;
			}

			//pop deque and cout/os path
			Coord front = dq.front();
			Coord back = dq.back();

			int disToFront = (current.row - front.row) * (current.row - front.row) + (current.col - front.col) * (current.col - front.col);
			int disToBack = (current.row - back.row) * (current.row - back.row) + (current.col - back.col) * (current.col - back.col);

			if (disToFront >= disToBack) {
				if (disBetweenABN(current, back) == 1) {//if minNode is neighbor, Directly Add Node

				}
				else {//A* walk to dq.back
					//showStringByCoord("SWIPE A* replace DFS current: " + std::to_string(current.row) + " " + std::to_string(current.col) + " back: " + std::to_string(back.row) + " " + std::to_string(back.col), getCoord(row, 0));
					//isNeedCharge(current, " GoCharge from  Swipe_Astar_Go_Back");
					Astar(current, back);
					//showStringByCoord("SWIPE A* replace DFS", getCoord(row, 0));
				}

				while (!dq.empty()) {//pop deque from deque.back
					Coord dqBack = dq.back();
					stck.push(dqBack);
					//showStringByCoord("SWIPE dqBack" + std::to_string(dqBack.row) + " " + std::to_string(dqBack.col), getCoord(row, 0));

					showStepAndBattery(dqBack);
					isNeedCharge(dqBack);
					dq.pop_back();
				}
			}
			else if (disToFront < disToBack) {
				//if(disOfCoord(front) - disOfCoord(current) == 1 || disOfCoord(front) - disOfCoord(current) == -1){//if minNode is neighbor, Directly Add Node
				if (disBetweenABN(current, front) == 1) {

				}
				else {//A* walk to dq.front
					//showStringByCoord("SWIPE A* replace DFS current: " + std::to_string(current.row) + " " + std::to_string(current.col) + " front: " + std::to_string(front.row) + " " + std::to_string(front.col), getCoord(row, 0));
					//isNeedCharge(current, " GoCharge from  Swipe_Astar_Go_dqFront ");
					Astar(current, front);
					//showStringByCoord("SWIPE A* replace DFS", getCoord(row, 0));
				}

				while (!dq.empty()) {//pop deque from deque.front
					Coord dqFront = dq.front();
					stck.push(dqFront);
					//showStringByCoord("SWIPE dqFront " + std::to_string(dqFront.row) + " " + std::to_string(dqFront.col), getCoord(row, 0));
					showStepAndBattery(dqFront);
					isNeedCharge(dqFront);
					dq.pop_front();
				}
			}
			current = stck.top();
			lastCurrent = current;
		}
		if (stck.size() < 2) break;
		else {
			stck.pop();
			current = stck.top();
			//showStringByCoord("SWIPE stck.top() " + std::to_string(stck.top().row) + " " + std::to_string(stck.top().col), getCoord(row, 0));//LastEdit
		}
	}
}

int main(int argc, char* argv[]) {
	std::string p(argv[1]);
	std::string path = "./" + p + "/floor.data";
	is.open(path);
	path = "";
	path = "./" + p + "/final.path";
	os.open(path);

	is >> row;
	is >> col;
	is >> b;
	full = b;
	os << "               \n";
	//os << "\n";

	//int (*array)[5] = new int[10][5];
	mapIsVisited = new bool*[row];
	mapCount = new int*[row];
	mapFront = new int*[row];
	char in;
	for (int i = 0; i < row; i++) {
		mapIsVisited[i] = new bool[col];
		mapCount[i] = new int[col];
		mapFront[i] = new int[col];
		for (int j = 0; j < col; j++) {
			is >> in;
			if (in == 'R') { R.row = i; R.col = j; mapCount[i][j] = 0; mapFront[i][j] = 0; mapIsVisited[i][j] = false; }
			else if (in == '1') { mapCount[i][j] = wall; mapFront[i][j] = wall; mapIsVisited[i][j] = true; }
			else if (in == '0') { mapCount[i][j] = uncover; mapFront[i][j] = uncover; mapIsVisited[i][j] = false; }
		}
	}
	totalStepPlusBattery = totalStep + b;
	lastStep = R;

	//showMapEmpty(mapCount);
	countMap(mapCount, row, col);
	//Coord G = getCoord(23, 6);
	//Coord S = getCoord(1, 17);
	//showPath(S, "S");
	//showPath(G, "G");

	swipe(mapIsVisited, mapCount, row, col, R);
	//showStringByCoord("", getCoord(row, 0));

	std::string strM = std::to_string(totalStep);
	//for(unsigned int i=0; i<10-strM.size(); i++){strM+'\n';}
	os.seekp(0, std::ios::beg);
	os.write(strM.c_str(), strM.size());
	is.close();
	os.close();
	return 0;
}
