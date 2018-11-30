#include <iostream>
#include <fstream>
#include <queue>
#include <string>
#include <stack>
#include <deque>
#include <vector>
#include <map>

#include <windows.h>

//using namespace std;

class Coord{
public:
  Coord(int r, int c): row(r), col(c){};
  Coord(): row(0), col(0){};

  Coord operator = (Coord c){
    row = c.row;
    col = c.col;
    return *this;
  }

  bool operator == (const Coord &c)const{
    if(row == c.row && col == c.col){ return true; }
    else{ return false; }
  }
  bool operator != (const Coord &c)const {
    if(row == c.row && col == c.col){ return false; }
    else{ return true; }
  }
  bool operator > (const Coord &c)const { return row > c.row; }
  bool operator < (const Coord &c)const { return row < c.row; }

  Coord up(){ Coord re(row-1, col); return re; }
  Coord down(){ Coord re(row+1, col); return re; }
  Coord left(){ Coord re(row, col-1); return re; }
  Coord right(){ Coord re(row, col+1); return re; }

  Coord rightUp(){ Coord re(row-1, col+1); return re; }
  Coord rightDown(){ Coord re(row+1, col+1); return re; }
  Coord leftUp(){ Coord re(row-1, col-1); return re; }
  Coord leftDown(){ Coord re(row+1, col-1); return re; }

  int row;
  int col;
};
Coord getCoord(int row, int col){Coord re{row, col}; return re;}

typedef struct CoordStatus{
  Coord c;
  bool up;
  bool down;
  bool left;
  bool right;
}CoordStatus;

CoordStatus getCoordStatus(Coord c, bool up, bool down, bool left, bool right){
  CoordStatus CS{c, up, down, left, right};
  return CS;
}

std::ifstream is("floor30.data");
std::ofstream os("final.path");

const int wall=-1, uncover=-2, sleepTime = 0, bugNodeArrayLeng = 10;
Coord R{1, 1};
Coord nowC;
int row=0, col=0, b=0, full=0, totalStep=0, maxDis=0, maxFront=0;
bool** mapIsVisited;
int** mapCount;
int** mapFront;
Coord bugNodeArray[bugNodeArrayLeng];
Coord bugNode(37, 9);
int showStrCount = 0;
bool showStr = true;

int coorOfSSBC = 0;
/*
int map[hei][wid]={{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                   {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                   {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                   {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                   {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                   {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                   {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                   {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                   {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};*/
/*
int map[hei][wid]={{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                   {1, 0, 0, 0, 1, 0, 0, 1, 0, 1},
                   {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                   {1, 0, 0, 0, 1, 0, 0, 1, 0, 1},
                   {1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
                   {1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
                   {1, 0, 0, 0, 1, 1, 1, 1, 0, 1},
                   {1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
                   {1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
                   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};*/

int getAbsoluteValue(int i){
  if(i<0){ return -i; }
  else{ return i; }
}
void showString(std::string str){
  if(showStr){std::cout << str << std::endl;}
}

void showStringByCoord(std::string str, Coord c){
  coorOfSSBC++;
  HANDLE h = GetStdHandle (STD_OUTPUT_HANDLE);
	COORD position = { 2*c.col, c.row+coorOfSSBC };
	SetConsoleCursorPosition (h, position);
  std::cout << std::dec << "A*: " << str << " \tPos: " << c.row << " " << c.col << " Row:" << row << " Col:" << col << "                            "<< "\n";
}

template<class t>
bool findInStack(std::stack<t> stck, t data){
  std::stack<t> tempStck;
  bool f=false;
  while(!stck.empty()){
    t top(stck.top());
    if(top == data){
      std::cout << "Found" << std::endl;
      f=true;
      system("pause");
      break;
    }
    else {
      tempStck.push(stck.top());
      stck.pop();
    }
  }
  while(!tempStck.empty()){
    t top(tempStck.top());
    stck.push(top);
    tempStck.pop();
  }

  return f;
}

int encode(int diago, int ortho){return diago*1000000 + ortho;}
int decodeDis(int code){return (code/1000000)*2 + code%1000000;}
int decodeFront(int code){return (code/1000000)*4 + (code%1000000)*3;}
int disOfCoord(Coord c){int code = mapCount[c.row][c.col]; return (code/1000000)*2 + code%1000000;}
int frontOfCoord(Coord c){int code = mapCount[c.row][c.col]; return (code/1000000) + (code%1000000);}
int MFOfCoord(Coord c){int code = mapCount[c.row][c.col]; return (code/1000000)*4 + (code%1000000)*3;}
int disBetweenABN(Coord a, Coord b){return (a.row - b.row) * (a.row - b.row) + (a.col - b.col) * (a.col - b.col);}

void showMapInRange(Coord bugNode){
  for(int i=0; i<bugNode.row+10 && i<row; i++){
    for(int j=0; j<bugNode.col+10 && j<col; j++){

      if(mapCount[i][j] == wall){ std::cout << "W" << " "; }
      else if(!mapIsVisited[i][j]){ std::cout << " " << " "; }
      else if(bugNode.row == i && bugNode.col == j){ std::cout << "@" << " "; }
      else if(mapIsVisited[i][j]){ std::cout << "#" << " "; }
    }
    std::cout << std::endl;
  }
}

void showStepAndBattery(Coord s){
  HANDLE h = GetStdHandle (STD_OUTPUT_HANDLE);
	COORD position = { 2*s.col, s.row };
	SetConsoleCursorPosition (h, position);
  std::cout << "@";
  Sleep(sleepTime);
  SetConsoleCursorPosition (h, position);
  std::cout << "#";
  COORD position1 = { 0, row+1 };
	SetConsoleCursorPosition (h, position1);
  //std::cout << std::dec << "Current: " << s.row << " " << s.col << " Row:" << row << " Col:" << col << " Battery:" << b << " TotalSteps:" << totalStep <<"             " << "\n";
  //std::cout << s.row << " " << s.col << "\n";
  if(s == bugNode)showMapInRange(s);
  os << s.row << " " << s.col << "\n";
  totalStep++;
}

void showPath(Coord s, std::string str){
  HANDLE h = GetStdHandle (STD_OUTPUT_HANDLE);
	COORD position = { 2*s.col, s.row };
	SetConsoleCursorPosition (h, position);
  std::cout << str;
  Sleep(sleepTime);
}

void showMapEmpty(int** mapCount){
  for(int i=0; i<row; i++){
    for(int j=0; j<col; j++){
      Coord tc(i,j);
      if(mapCount[tc.row][tc.col] == -1){ std::cout << "W" << " "; }
      else if(mapCount[tc.row][tc.col] == -2){ std::cout << " " << " "; }
      else if(mapCount[tc.row][tc.col] == 0){ std::cout << "0" << " "; }

    }
    std::cout << std::endl;
  }
}

void showMapCount(int** mapCount){
  for(int i=0; i<row; i++){
    for(int j=0; j<col; j++){
      Coord tc(i,j);
      if(mapCount[tc.row][tc.col] == -1){ std::cout << "W" << " "; }
      else if(mapCount[tc.row][tc.col] == -2){ std::cout << " " << " "; }
      else if(mapCount[tc.row][tc.col] == 0){ std::cout << "0" << " "; }
      else {
        if(disOfCoord(tc) < 16){ std::cout << std::hex << disOfCoord(tc) << " "; }
        else { std::cout << std::hex << disOfCoord(tc); }
      }
    }
    std::cout << std::endl;
  }
}

void showMapCountFront(int** mapCount){
  for(int i=0; i<row; i++){
    for(int j=0; j<col; j++){
      Coord tc(i,j);
      if(mapCount[tc.row][tc.col] == -1){ std::cout << "W" << " "; }
      else if(mapCount[tc.row][tc.col] == -2){ std::cout << " " << " "; }
      else if(mapCount[tc.row][tc.col] == 0){ std::cout << "0" << " "; }
      else {
        if(frontOfCoord(tc) < 16){ std::cout << std::hex << frontOfCoord(tc) << " "; }
        else { std::cout << std::hex << frontOfCoord(tc); }
      }
    }
    std::cout << std::endl;
  }
}

void showMapCountData(int** mapCount){
  for(int i=0; i<row; i++){
    for(int j=0; j<col; j++){
      Coord tc(i,j);
      if(mapCount[tc.row][tc.col] == -1){ std::cout << "W" << " "; }
      else if(mapCount[tc.row][tc.col] == -2){ std::cout << " " << " "; }
      else if(mapCount[tc.row][tc.col] == 0){ std::cout << "0" << " "; }
      else {
        std::cout << std::hex << mapCount[tc.row][tc.col] / 1000000 << std::hex << mapCount[tc.row][tc.col] % 1000000;
      }
    }
    std::cout << std::endl;
  }
}

void countMap(int** mapCount, int row, int col){
  std::queue<Coord> set;
  set.push(R);
  int ortho = 0, diago = 0;
  mapCount[R.row][R.col] = 0;

  while(!set.empty()){
    int setSize = set.size();
    for(int i=0; i<setSize; i++){
      Coord t(1,1);
      Coord frontC(set.front());
      Coord orthoNode;

      ortho = mapCount[frontC.row][frontC.col] % 1000000;
      diago = mapCount[frontC.row][frontC.col] / 1000000;

      t = frontC.up();//Go UP
      if(t.row >= 0 && mapCount[t.row][t.col] == uncover){
        mapCount[t.row][t.col] = encode(diago, ortho+1);
        set.push(t);

        //Orthogonal
        orthoNode = t.left();
        if(orthoNode.row >= 0 && mapCount[orthoNode.row][orthoNode.col] == uncover){
          mapCount[orthoNode.row][orthoNode.col] = encode(diago+1, ortho);
          set.push(orthoNode);
        }
        orthoNode = t.right();
        if(orthoNode.row < col && mapCount[orthoNode.row][orthoNode.col] == uncover){
          mapCount[orthoNode.row][orthoNode.col] = encode(diago+1, ortho);
          set.push(orthoNode);
        }
      }

      t = frontC.down();//Go DOWN
      if(t.row <= row && mapCount[t.row][t.col] == uncover){
        mapCount[t.row][t.col] = encode(diago, ortho+1);
        set.push(t);

        //Orthogonal
        orthoNode = t.left();
        if(orthoNode.row >= 0 && mapCount[orthoNode.row][orthoNode.col] == uncover){
          mapCount[orthoNode.row][orthoNode.col] = encode(diago+1, ortho);
          set.push(orthoNode);
        }
        orthoNode = t.right();
        if(orthoNode.row < col && mapCount[orthoNode.row][orthoNode.col] == uncover){
          mapCount[orthoNode.row][orthoNode.col] = encode(diago+1, ortho);
          set.push(orthoNode);
        }
      }

      t = frontC.left();//Go LEFT
      if(t.col >= 0 && mapCount[t.row][t.col] == uncover){
        mapCount[t.row][t.col] = encode(diago, ortho+1);
        set.push(t);

        //Orthogonal
        orthoNode = t.up();
        if(orthoNode.row >= 0 && mapCount[orthoNode.row][orthoNode.col] == uncover){
          mapCount[orthoNode.row][orthoNode.col] = encode(diago+1, ortho);
          set.push(orthoNode);
        }
        orthoNode = t.down();
        if(orthoNode.row < row && mapCount[orthoNode.row][orthoNode.col] == uncover){
          mapCount[orthoNode.row][orthoNode.col] = encode(diago+1, ortho);
          set.push(orthoNode);
        }
      }

      t = frontC.right();//Go RIGHT
      if(t.col <= col && mapCount[t.row][t.col] == uncover){
        mapCount[t.row][t.col] = encode(diago, ortho+1);
        set.push(t);

        //Orthogonal
        orthoNode = t.up();
        if(orthoNode.row >= 0 && mapCount[orthoNode.row][orthoNode.col] == uncover){
          mapCount[orthoNode.row][orthoNode.col] = encode(diago+1, ortho);
          set.push(orthoNode);
        }
        orthoNode = t.down();
        if(orthoNode.row < row && mapCount[orthoNode.row][orthoNode.col] == uncover){
          mapCount[orthoNode.row][orthoNode.col] = encode(diago+1, ortho);
          set.push(orthoNode);
        }
      }
      set.pop();
    }
    maxDis = diago*2 + ortho;
    maxFront = diago + ortho;
  }
  //mapCount[R.row][R.col] = 0;
}

bool isNeedCharge(int** mapCount, Coord now){
  if(b <= disOfCoord(now)+1){
    Coord t = now;
    std::stack<Coord> path;
    while(1){
      int disOfT = disOfCoord(t);
      int disOfUp = disOfCoord(t.up());
      int disOfDown = disOfCoord(t.down());
      int disOfLeft = disOfCoord(t.left());
      int disOfRight = disOfCoord(t.right());
      if(disOfUp < disOfT && disOfUp >= 0){ path.push(t.up()); }
      else if(disOfDown < disOfT && disOfDown >= 0){ path.push(t.down()); }
      else if(disOfLeft < disOfT && disOfLeft >= 0){ path.push(t.left()); }
      else if(disOfRight < disOfT && disOfRight >= 0){ path.push(t.right()); }
      b--;
      showStepAndBattery(path.top());
      if(b<0){std::cout << path.top().row << "rrr" << path.top().col << "\n"; system("pause");}
      t.row = path.top().row;
      t.col = path.top().col;
      if(t == R) break;
    }
    b = full;
    path.pop();
    b--;
    while(!path.empty()){
      t.row = path.top().row;
      t.col = path.top().col;
        b--;
        showStepAndBattery(t);
        if(b<0){std::cout << "eee"; system("pause");}
        path.pop();
    }
    return true;
  }else{ return false; }
}

//Select Coord put into stack, if neighbor are visited, return;
void swipe(bool** mapIsVisited, int** mapCount, int row, int col, Coord st){
  //Use wavefront ALgorithm
  //Follow the wavefront and walk, put node into stack<Coord> stck, repeatly
  std::stack<Coord> stck;
  Coord current = st;
  Coord minNode(st);

  mapIsVisited[st.row][st.col] = true;
  showStepAndBattery(st);
  stck.push(st);

  showStepAndBattery(st);

  while(1){
    while(1){
      //find a smallest distance neighbor node that is uncover
      int minDis = row*col;
      Coord up(current.up());
      Coord down(current.down());
      Coord left(current.left());
      Coord right(current.right());
      int disUp = disOfCoord(up);
      int disDown = disOfCoord(down);
      int disLeft = disOfCoord(left);
      int disRight = disOfCoord(right);
      bool isFoundMinNide = false;


      if(current.row == 472 && current.col == 19){
        showString("Pause");
        system("pause");
      }
      showString("Current: " + std::to_string(current.row) + " " + std::to_string(current.col));

      if(disUp <= minDis && !mapIsVisited[up.row][up.col] && mapCount[up.row][up.col] != wall){ minNode = up; minDis = disUp; isFoundMinNide = true;}
      if(disDown <= minDis && !mapIsVisited[down.row][down.col] && mapCount[down.row][down.col] != wall){ minNode = down; minDis = disDown; isFoundMinNide = true;}
      if(disLeft <= minDis && !mapIsVisited[left.row][left.col] && mapCount[left.row][left.col] != wall){ minNode = left; minDis = disLeft; isFoundMinNide = true;}
      if(disRight <= minDis && !mapIsVisited[right.row][right.col] && mapCount[right.row][right.col] != wall){ minNode = right; minDis = disRight; isFoundMinNide = true;}
      if(!isFoundMinNide) break;
      showString("minNode: " + std::to_string(minNode.row) + " " + std::to_string(minNode.col) + "                     ");

      //Put minNode into deque, and DFS it along two direction, front and back
      std::deque<Coord> dq;
      Coord tempC(minNode);
      int currentFront = frontOfCoord(tempC);

      while(1){//add back
        mapIsVisited[tempC.row][tempC.col] = true;
        dq.push_back(tempC);
        showString("Back_tempC " + std::to_string(tempC.row) + " " + std::to_string(tempC.col) + " " + std::to_string(currentFront) + "                     ");

        up = tempC.up();
        down = tempC.down();
        left = tempC.left();
        right = tempC.right();
        if(frontOfCoord(up) == currentFront && !mapIsVisited[up.row][up.col]){ tempC = up; }
        else if(frontOfCoord(down) == currentFront && !mapIsVisited[down.row][down.col]){ tempC = down; }
        else if(frontOfCoord(left) == currentFront && !mapIsVisited[left.row][left.col]){ tempC = left; }
        else if(frontOfCoord(right) == currentFront && !mapIsVisited[right.row][right.col]){ tempC = right; }
        else break;
      }
      tempC = minNode;
      showString("pop_front " + std::to_string(dq.front().row) + " " + std::to_string(dq.front().col) + " " +  std::to_string(currentFront)  + "              ");
      dq.pop_front();//pop Front
      while(1){//add front
        mapIsVisited[tempC.row][tempC.col] = true;
        dq.push_front(tempC);
        showString("Front_tempC " + std::to_string(tempC.row) + " " + std::to_string(tempC.col) + " " +  std::to_string(currentFront) + "              ");
        up = tempC.up();
        down = tempC.down();
        left = tempC.left();
        right = tempC.right();
        if(frontOfCoord(up) == currentFront && !mapIsVisited[up.row][up.col]){ tempC = up; }
        else if(frontOfCoord(down) == currentFront && !mapIsVisited[down.row][down.col]){ tempC = down; }
        else if(frontOfCoord(left) == currentFront && !mapIsVisited[left.row][left.col]){ tempC = left; }
        else if(frontOfCoord(right) == currentFront && !mapIsVisited[right.row][right.col]){ tempC = right; }
        else break;
      }

      //pop deque and cout/os path
      Coord front = dq.front();
      Coord back = dq.back();
      bool isDfsWalk = false;
      int disToFront = (current.row - front.row) * (current.row - front.row) + (current.col - front.col) * (current.col - front.col);
      int disToBack = (current.row - back.row) * (current.row - back.row) + (current.col - back.col) * (current.col - back.col);

      if(disToFront >= disToBack){
        if(disOfCoord(back) - disOfCoord(current) == 1 || disOfCoord(back) - disOfCoord(current) == -1){//if minNode is neighbor, Directly Add Node
          isDfsWalk = false;
        }
        else {//DFS walk to dq.back
          Coord temp(current);
          int minDis;
          int a=0;
          while(temp != back){
            a++;
            minDis = 2000000000;
            Coord tempUp = temp.up();
            Coord tempDown = temp.down();
            Coord tempLeft = temp.left();
            Coord tempRight = temp.right();
            int disToUp = disBetweenABN(tempUp, back);
            int disToDown = disBetweenABN(tempDown, back);
            int disToLeft = disBetweenABN(tempLeft, back);
            int disToRight = disBetweenABN(tempRight, back);

            if(disToUp < minDis && mapCount[tempUp.row][tempUp.col] != wall){minDis = disToUp; temp=tempUp;}
            if(disToDown < minDis && mapCount[tempDown.row][tempDown.col] != wall){minDis = disToDown; temp=tempDown;}
            if(disToLeft < minDis && mapCount[tempLeft.row][tempLeft.col] != wall){minDis = disToLeft; temp=tempLeft;}
            if(disToRight < minDis && mapCount[tempRight.row][tempRight.col] != wall){minDis = disToRight; temp=tempRight;}
            stck.push(temp);

            showString("DFS walk to dq.back " + std::to_string(temp.row) + " " + std::to_string(temp.col) + " t " + std::to_string(a) + "From: " + std::to_string(current.row) + " " + std::to_string(current.col) + " to:" + std::to_string(back.row) + " " + std::to_string(back.col));
            /*
            std::cout + "Temp: " + temp.row + " " + temp.col + " minDis: " + minDis + std::endl;
            std::cout + "Temp.up: " + tempUp.row + " " + tempUp.col + " Dis: " + disToUp + std::endl;
            std::cout + "Temp.down: " + tempDown.row + " " + tempDown.col + " Dis: " + disToDown + std::endl;
            std::cout + "Temp.left: " + tempLeft.row + " " + tempLeft.col + " Dis: " + disToLeft + std::endl;
            std::cout + "Temp.down: " + tempDown.row + " " + tempDown.col + " Dis: " + disToRight + std::endl;*/

            //os << temp.row << " " << temp.col << "\n";
            b--;
            showStepAndBattery(temp);
          }
          isDfsWalk = true;
        }
        if(isDfsWalk){dq.pop_back();}
        while(!dq.empty()){//pop deque from deque.back
          Coord dqBack = dq.back();
          stck.push(dqBack);
          showString("dqBack" + std::to_string(dqBack.row) + " " + std::to_string(dqBack.col) + "                     ");
          //os << dqBack.row << " " << dqBack.col << "\n";
          b--;
          showStepAndBattery(dqBack);
          dq.pop_back();
        }
      }
      else if(disToFront < disToBack){
        if(disOfCoord(front) - disOfCoord(current) == 1 || disOfCoord(front) - disOfCoord(current) == -1){//if minNode is neighbor, Directly Add Node
          isDfsWalk = false;
        }
        else {//DFS walk to dq.front
          Coord temp(current);
          int minDis;

          while(temp != front){
            minDis = 2000000000;
            Coord tempUp = temp.up();
            Coord tempDown = temp.down();
            Coord tempLeft = temp.left();
            Coord tempRight = temp.right();
            int disToUp = disBetweenABN(tempUp, front);
            int disToDown = disBetweenABN(tempDown, front);
            int disToLeft = disBetweenABN(tempLeft, front);
            int disToRight = disBetweenABN(tempRight, front);

            if(disToUp < minDis && mapCount[tempUp.row][tempUp.col] != wall){minDis = disToUp; temp=tempUp;}
            if(disToDown < minDis && mapCount[tempDown.row][tempDown.col] != wall){minDis = disToDown; temp=tempDown;}
            if(disToLeft < minDis && mapCount[tempLeft.row][tempLeft.col] != wall){minDis = disToLeft; temp=tempLeft;}
            if(disToRight < minDis && mapCount[tempRight.row][tempRight.col] != wall){minDis = disToRight; temp=tempRight;}
            stck.push(temp);

            showString("DFS walk to dq.front " + std::to_string(temp.row) + " " + std::to_string(temp.col) + "From: " + std::to_string(current.row) + " " + std::to_string(current.col) + " to:" + std::to_string(front.row) + " " + std::to_string(front.col));
            /*
            std::cout + "Temp: " + temp.row + " " + temp.col + " minDis: " + minDis + std::endl;
            std::cout + "Temp.up: " + tempUp.row + " " + tempUp.col + " Dis: " + disToUp + std::endl;
            std::cout + "Temp.down: " + tempDown.row + " " + tempDown.col + " Dis: " + disToDown + std::endl;
            std::cout + "Temp.left: " + tempLeft.row + " " + tempLeft.col + " Dis: " + disToLeft + std::endl;
            std::cout + "Temp.Right: " + tempRight.row + " " + tempRight.col + " Dis: " + disToRight + std::endl;*/
            //os << temp.row << " " << temp.col << "\n";
            b--;
            showStepAndBattery(temp);
          }
          isDfsWalk = true;
        }
        if(isDfsWalk){dq.pop_front();}
        while(!dq.empty()){//pop deque from deque.front
          Coord dqFront = dq.front();
          stck.push(dqFront);
          showString("dqFront" + std::to_string(dqFront.row) + " " + std::to_string(dqFront.col));
          //os << dqFront.row << " " << dqFront.col << "\n";
          b--;
          showStepAndBattery(dqFront);
          dq.pop_front();
        }
      }
      current = stck.top();
    }
    if(stck.empty()) break;
    stck.pop();
    current = stck.top();
    if(!stck.empty()){}

    showString("stck.top() " + std::to_string(stck.top().row) + " " + std::to_string(stck.top().col));//LastEdit


  }
}
class Node{
public:
  Node(Coord c, Node *f, Coord start, Coord goal): coord(c), from(f), value( ((goal.row-c.row)*(goal.row-c.row)+(goal.col-c.col)*(goal.col-c.col) + (start.row-c.row)*(start.row-c.row)+(start.col-c.col)*(start.col-c.col)) ){};
  Node(Coord c, Coord goal): coord(c), from(NULL), value((goal.row-c.row)*(goal.row-c.row)+(goal.col-c.col)*(goal.col-c.col)){};

  //bool operator < (const Node &a) const { return value < a.value; }
  //bool operator > (const Node &a) const { return value > a.value; }

  Coord coord;
  Node* from;
  int value;
};

std::string ctos(Coord c) {
  std::string a(std::to_string(c.row));
  std::string b(std::to_string(c.col));
  return a+b;
}

bool Astar(Coord toB, Coord fromA){
  //A* path search
  if(mapCount[toB.row][toB.col] == wall || mapCount[fromA.row][fromA.col] == wall){ return false; }
  if(fromA.row<0 || fromA.row>row || fromA.col<0 || fromA.col>col || toB.row<0 || toB.row>row || toB.col<0 || toB.col>col){ return false; }
  auto cmp0 = [](Node* left, Node* right) { return left->value > right->value;};
  std::priority_queue<Node*, std::vector<Node*>, decltype(cmp0)> pq(cmp0);
  std::map<std::string, Node*> mappq;
  Node* now;
  Node *A = new Node(fromA, NULL, fromA, toB);
  pq.push(A);
  mappq.insert(std::pair<std::string, Node*>(ctos(fromA), A));
  showStringByCoord("Enter fromA " + std::to_string(fromA.row) + " " + std::to_string(fromA.col) + " ToB " + std::to_string(toB.row) +" "+ std::to_string(toB.col), getCoord(row, 0));
  showStringByCoord("Enter pq.top " + std::to_string(pq.top()->coord.row) + " " + std::to_string(pq.top()->coord.col), getCoord(row, 0));
  showStringByCoord("A " + ctos(fromA) + " ", getCoord(row, 0));

  while (!pq.empty()) {
    Node* pqTop = pq.top();
    pq.pop();
    Coord up(pqTop->coord.row-1, pqTop->coord.col);
    Coord down(pqTop->coord.row+1, pqTop->coord.col);
    Coord left(pqTop->coord.row, pqTop->coord.col-1);
    Coord right(pqTop->coord.row, pqTop->coord.col+1);
    std::string str("pqTop " + std::to_string(pqTop->coord.row) + " " + std::to_string(pqTop->coord.col));
    showStringByCoord("Loop " + str, getCoord(row, 0));

    showStringByCoord("LEFT row: " + std::to_string(left.row) + " col: " + std::to_string(left.col) + " !wall " + std::to_string(mapCount[left.row][left.col] != wall) + " !FoundInMap "+ std::to_string(!mappq.count(ctos(left))), getCoord(row,0));
    if(mapCount[left.row][left.col] != wall && !mappq.count(ctos(left))){
      Node* leftOfpq = new Node(left, pqTop, fromA, toB);
      mappq.insert(std::pair<std::string, Node*>(ctos(left), leftOfpq));
      showStringByCoord("LEFT Add: ", getCoord(row,0));
      showStepAndBattery(left);
      if(left != toB){
        pq.push(leftOfpq);
      }
      else{now = leftOfpq; break;}
    }
    showStringByCoord("RIGHT row: " + std::to_string(right.row) + " col: " + std::to_string(right.col) + " !wall " + std::to_string(mapCount[right.row][right.col] != wall) + " !FoundInMap " + std::to_string(!mappq.count(ctos(right))), getCoord(row,0));
    if(mapCount[right.row][right.col] != wall && !mappq.count(ctos(right))){
      Node* rightOfpq = new Node(right, pqTop, fromA, toB);
      mappq.insert(std::pair<std::string, Node*>(ctos(right), rightOfpq));
      showStringByCoord("RIGHT Add: ", getCoord(row,0));
      showStepAndBattery(right);
      if(right != toB){
        pq.push(rightOfpq);
      }
      else{now = rightOfpq; break;}
    }
    showStringByCoord("DOWN row: " + std::to_string(down.row) + " col: " + std::to_string(down.col) + " !wall " + std::to_string(mapCount[down.row][down.col] != wall) + " !FoundInMap " + std::to_string(!mappq.count(ctos(down))), getCoord(row,0));
    if(mapCount[down.row][down.col] != wall && !mappq.count(ctos(down))){
      Node* downOfpq = new Node(down, pqTop, fromA, toB);
      mappq.insert(std::pair<std::string, Node*>(ctos(down), downOfpq));
      showStringByCoord("DOWN Add: ", getCoord(row,0));
      showStepAndBattery(down);
      if(down != toB){
        pq.push(downOfpq);
      }
      else{now = downOfpq; break;}
    }
    showStringByCoord("UP row: " + std::to_string(up.row) + " col: " + std::to_string(up.col) + " !wall " + std::to_string(mapCount[up.row][up.col] != wall) + " !FoundInMap " + std::to_string(!mappq.count(ctos(up))), getCoord(row,0));
    if(mapCount[up.row][up.col] != wall && !mappq.count(ctos(up))){
      Node* upOfpq = new Node(up, pqTop, fromA, toB);
      mappq.insert(std::pair<std::string, Node*>(ctos(up), upOfpq));
      showStringByCoord("UP Add: ", getCoord(row,0));
      showStepAndBattery(up);
      if(up != toB){
        pq.push(upOfpq);
      }
      else{now = upOfpq; break;}
    }
    std::string str1;
    for(std::map<std::string, Node*>::iterator it=mappq.begin(); it!=mappq.end(); it++){
      str1 = "Coord in map: " + it->first + "\n";
      //showStringByCoord(str1,getCoord(row, 0));
    }
  }

  showPath(fromA, "G");
  showPath(toB, "S");
  Node* delN = now;
  now = now->from;
  delete(delN);
  while(now->coord != fromA){
    showStringByCoord("Output " + std::to_string(now->coord.row) + " " + std::to_string(now->coord.col), getCoord(row, 0));
    showStepAndBattery(now->coord);
    showPath(now->coord, "@");
    //os << now->coord.row << " " << now->coord.col << "\n";
    delN = now;
    now = now->from;
    delete(delN);
  }

  for(std::pair<std::string, Node*> d: mappq){
    delete(d.second);
    showStringByCoord("D: " + std::to_string(d.second->coord.row) + " " + std::to_string(d.second->coord.col), getCoord(row, 0));
  }
  if(pq.empty()){return false;}
  else{ return true; }

}
/*
void manage(bool** mapIsVisited, int** mapCount, int row, int col, Coord st){
  std::stack<Coord> stck;
  showStepAndBattery(st);
  mapIsVisited[st.row][st.col] = true;
  stck.push(st);
  //DFS
  while(!stck.empty()){
    if(!isNeedCharge(mapCount, stck.top())){


      Coord tempUp(stck.top().up());
      Coord tempDown(stck.top().down());
      Coord tempLeft(stck.top().left());
      Coord tempRight(stck.top().right());

      if(!mapIsVisited[tempUp.row][tempUp.col] && disOfCoord(tempUp) != wall){
        mapIsVisited[tempUp.row][tempUp.col] = true;
        stck.push(tempUp);
        showStepAndBattery(tempUp);
        b--;
      }
      else if(!mapIsVisited[tempDown.row][tempDown.col] && disOfCoord(tempDown) != wall){
        mapIsVisited[tempDown.row][tempDown.col] = true;
        stck.push(tempDown);
        showStepAndBattery(tempDown);
        b--;
      }
      else if(!mapIsVisited[tempLeft.row][tempLeft.col] && disOfCoord(tempLeft) != wall){
        mapIsVisited[tempLeft.row][tempLeft.col] = true;
        stck.push(tempLeft);
        showStepAndBattery(tempLeft);
        b--;
      }
      else if(!mapIsVisited[tempRight.row][tempRight.col] && disOfCoord(tempRight) != wall){
        mapIsVisited[tempRight.row][tempRight.col] = true;
        stck.push(tempRight);
        showStepAndBattery(tempRight);
        b--;
      }
      else {
        stck.pop();
        //showStepAndBattery(stck.top());
        //b--;

      }
    }
  }
  //Count Distance to Charger
  //
}*/

int main(int argc, char* argv[]){
    is >> row;
    is >> col;
    is >> b;
    full = b;
    os << "\n\n";

    //int (*array)[5] = new int[10][5];
    mapIsVisited = new bool* [row];
    mapCount = new int* [row];
    mapFront = new int* [row];
    char in;
    for(int i = 0; i<row; i++){
      mapIsVisited[i] = new bool [col];
      mapCount[i] = new int [col];
      mapFront[i] = new int [col];
      for(int j=0; j<col; j++){
        is >> in;
        if(in == 'R'){ R.row=i; R.col=j; mapCount[i][j] = 0; mapFront[i][j] = 0; mapIsVisited[i][j] = false;}
        else if(in == '1'){ mapCount[i][j] = wall; mapFront[i][j] = wall; mapIsVisited[i][j] = true;}
        else if(in == '0'){ mapCount[i][j] = uncover; mapFront[i][j] = uncover; mapIsVisited[i][j] = false;}
      }
    }
    showMapEmpty(mapCount);
    countMap(mapCount, row, col);
    Coord from(26,20);
    Coord to(12,3);
    //showStepAndBattery(from);
    //showStepAndBattery(to);

    Astar(from, to);
    showStringByCoord("", getCoord(row, 0));


    //showMapCount(mapCount);
    //showMapCountFront(mapCount);
    //showMapCountData(mapCount);


    //swipe(mapIsVisited, mapCount, row, col, R);
    os.seekp(0, std::ios::beg);
    os << totalStep;
    //std::cout << totalStep;
    is.close();
    os.close();

    return 0;
}

/*
void bFindpath(int** map, int** mapCopy,Coord start, Coord end){
  Coord t = start;
  while(t != end){
    cout << t.row << " " << t.col << endl;
    t.findClosestToEnd(end);
  }
}



void expand(int** mapCopy, Coord s){
  Coord rightDown{0, 0};
  Coord leftUp{0, 0};
  for(int i=1; mapCopy[s.row][s.col+i] == 0; i++){ rightDown.col = s.col+i; }
  for(int i=1; mapCopy[s.row+i][s.col] == 0; i++){ rightDown.row = s.row+i; }
  for(int i=1; mapCopy[s.row][s.col-i] == 0; i++){ leftUp.col = s.col-i; }
  for(int i=1; mapCopy[s.row-i][s.col] == 0; i++){ leftUp.row = s.row-i; }

  Mas* mas = new Mas;
  mas->rightDown = rightDown;
  mas->leftUp = leftUp;

  //Put into Stack
  for(int i=leftUp.row; i<=rightDown.row; i++){
    for(int j=leftUp.col; j<=rightDown.col; j++){
      mapCopy[i][j] = 1;
    }
  }
}
void checkBorder(Matrix m){

}

void swipe(int** map, Matrix m, Coord end){
  int w = m.rightDown.col - m.leftUp.col + 1;
  int h = m.rightDown.row - m.leftUp.row + 1;
  bool t1BRowChange;
  bool t2BColChange;
  Coord t1;
  Coord t2;
  Coord opposite;
  if(m.leftUp.row == end.row){ opposite.row = m.rightDown.row; }
  else{ opposite.row = m.leftUp.row; }
  if(m.leftUp.col == end.col){ opposite.col = m.rightDown.col; }
  else{ opposite.col = m.leftUp.col; }

  if(w%2){ t1BRowChange=true; }
  else{ t1BRowChange=false; }
  if(h%2){ t2BColChange=true; }
  else{ t2BColChange=false; }

  if(t1BRowChange || t2BColChange){ t1=t2=opposite; }
  if(!t1BRowChange){ t1.row=end.row; t1.col=opposite.col; }
  if(!t2BColChange){ t2.col=end.col; t2.row=opposite.row; }

  if(nowC.dis(t1) > nowC.dis(t2)){
    //Go to t2
  }
  else{
    //Go to t1
    if(t1 == m.leftUp){
      for(int col=0; col<w; col++){
        if(col%2){
          for(int row=0; row<h; row++){ cout << t1.row+row << " " << t1.col+col << endl; }
        }else{
          for(int row=h-1; row>=0; row--){ cout << t1.row+row << " " << t1.col+col << endl; }
        }
      }
    }else if(t1.row == m.leftUp.row && t1.col == m.rightDown.col){
      for(int col=w-1; col>=0; col--){
        if(col%2){
          for(int row=0; row<h; row++){ cout << t1.row+row << " " << t1.col+col << endl; }
        }else{
          for(int row=h-1; row>=0; row--){ cout << t1.row+row << " " << t1.col+col << endl; }
        }
      }
    }else if(t1.row == m.rightDown.row && t1.col == m.leftUp.col){
      for(int col=0; col<w; col++){
        if(col%2){
          for(int row=h-1; row>=0; row--){ cout << t1.row+row << " " << t1.col+col << endl; }
        }else{
          for(int row=0; row<h; row++){ cout << t1.row+row << " " << t1.col+col << endl; }
        }
      }
    }else{
      for(int col=w-1; col>=0; col--){
        if(col%2){
          for(int row=h-1; row>=0; row--){ cout << t1.row+row << " " << t1.col+col << endl; }
        }else{
          for(int row=0; row<h; row++){ cout << t1.row+row << " " << t1.col+col << endl; }
        }
      }
    }
  }
  //t1 go row

}

void manage(int** map, int** mapCount, Coord** mapStep, int row, int col){//Queue Divide Map
  nowC = R;
  expand(map, nowC);//擴展矩陣，塞進STACK
  checkBorder(s.top());//檢查矩陣邊界，將start設到邊界擴展點
  Coord t = s.top().start;//邊界擴展點
  expand(map, s.top().start);//擴展
  swipe(closestP(s.top(), t));//找出次矩陣中最接近邊界擴展點的角
  while(!s.empty()){
    expand(map, nowC);
    checkBorder(s.top());
    Coord t = s.top().start;
    expand(map, s.top().start);
    swipe(closestP(s.top(), t));
  }
}
*/
