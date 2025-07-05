#include "maze.h"
#include <string>

class Solver : public Maze {
  public:
    Solver(short y, short x, long seedCtrl);
    void setStartPoint(short, short);
    void setEndPoint(short, short);
    void updateMaze();
    void clearCells();
    void checkWalls();
    virtual void solveMaze() = 0;
    rowCol startCell;
    rowCol endCell;
    short numWalls;
    short *availableWalls;
};

class Solver_Backtracking : public Solver {
  public:
    Solver_Backtracking(short y, short x, long seedCtrl) : Solver(y, x, seedCtrl) {};
    virtual void solveMaze();
};

struct pathScore {
  short G;
  short H;
  short F;
};

class Solver_AStar : public Solver {
  public:
    Solver_AStar(short y, short x, long seedCtrl) : Solver(y, x, seedCtrl) {};
    void checkAdjacent(short);
    void checkAdjacent();
    pathScore calcFGH(short, short, short);
    virtual void solveMaze();
    short newCell;
    short numAvailable;
    short availableWall[4];
    pathScore **calculations;
};

// Main Function
int main(int argc, char *args[]) {
  char ch;
  short x, y;
  long seedCtrl;
  Maze M;
  do {
    seedCtrl = time(NULL);
    system("clear||cls");
    do {
      cout << "Input " << yellow << "WIDTH" << def << " of Maze [min:";
      cout << green << "2" << def << " max:" << red << "36" << def << "]: ";
      cin >> x;
    } while (x < 2 || x > 36);
    do {
      cout << "Input " << yellow << "HEIGHT" << def << " of Maze [min:";
      cout << green << "2" << def << " max:" << red << "17" << def << "]: ";

      cin >> y;
    } while (y < 2 || y > 17);
    M.setHeightWidth(y, x);
    M.mazeGenerate(true, seedCtrl);
    Solver *p;
    Solver_Backtracking A(y, x, seedCtrl);
    Solver_AStar B(y, x, seedCtrl);
    for (;;) {
      system("clear||cls");
      M.updateMaze();
      cout << green << "\nChoose Command:" << def << endl;
      cout << yellow << " A." << def << " SOLVE using Backtracking Algorithm." << endl;
      cout << yellow << " B." << def << " SOLVE using A*(A-Star) Algorithm." << endl;
      cout << yellow << " C." << def << " GENERATE another Maze." << endl;
      cout << red << " Q." << def << " Quit." << endl;
      do {
        cout << ">> ";
        cin >> ch;
        ch = tolower(ch);
      } while (ch != 'a' && ch != 'b' && ch != 'c' && ch != 'q');
      switch (ch) {
        case 'a':
          p = &A;
          break;
        case 'b':
          p = &B;
          break;
        case 'q':
          system("clear||cls");
          return 0;
        default:
          break;
      }
      if (ch == 'a' || ch == 'b') {
        system("clear||cls");
        p->updateMaze();
        cout << "\nSet " << green << "START" << def << " point [row][col]:" << green;
        cin >> y >> x;
        cout << def;
        p->setStartPoint(y, x);
        cout << "\nSet " << red << "END" << def << " point [row][col]: " << red;
        cin >> y >> x;
        cout << def;
        p->setEndPoint(y, x);
        cout << "\nPress " << green << "ENTER" << def << " to SOLVE...";
        cin.ignore().get();
        p->solveMaze();
        cout << "\n"
             << red << "DONE" << def << " solving maze!" << endl;
      } else {
        break;
      }
    }
  } while (ch == 'c');
}

// class Solver definitions
Solver::Solver(short y, short x, long seedCtrl) {
  startCell = {-1, -1};
  endCell = {-1, -1};
  setHeightWidth(y, x);
  mazeGenerate(false, seedCtrl);
}

void Solver::setStartPoint(short sRow, short sCol) {
  startCell.Row = (sRow * 2) + 1;
  startCell.Col = (sCol * 2) + 1;
  updateMaze();
}

void Solver::setEndPoint(short sRow, short sCol) {
  endCell.Row = (sRow * 2) + 1;
  endCell.Col = (sCol * 2) + 1;
  updateMaze();
}

void Solver::updateMaze() {
  system("clear||cls");

  for (short i = 0; i < mazeSize.Row; i++) {
    for (short j = 0; j < mazeSize.Col; j++) {
      if (i == currentCell.Row && j == currentCell.Col) {
        cout << bYellow << "  " << bDef;
      }
      else if (i == startCell.Row && j == startCell.Col) {
        cout << bGreen << "  " << bDef;
      }
      else if (i == endCell.Row && j == endCell.Col) {
        cout << bRed << "  " << bDef;
      } else {
        if (grid[i][j] == 'V')
          cout << yellow << "* " << def;
        else if (grid[i][j] == ' ')
          cout << "  ";
        else
          cout << bBlue << "  " << bDef;
      }
    }
    if ((i - 1) % 2 == 0)
      cout << yellow << " " << (i - 1) / 2 << def;
    cout << endl;
  }
  for (short i = 0; i < (mazeSize.Col - 1) / 2; i++) {
    cout << yellow << "  " << i << def;
    if (i < 10)
      cout << " ";
  }
  cout << endl;
}

void Solver::clearCells() {
  // loop for clearing Visited Cells
  for (short i = 0; i < mazeSize.Row; i++) {
    for (short j = 0; j < mazeSize.Col; j++) {
      if (grid[i][j] == 'V')
        grid[i][j] = ' ';
    }
  }
  startCell = {-1, -1};
  endCell = {-1, -1};
  currentCell = {-1, -1};
}

void Solver::checkWalls() {
  numWalls = 0;
  stack<short> tempStack;
  // check top
  if (currentCell.Row - 2 > 0) {

    if (grid[currentCell.Row - 1][currentCell.Col] == ' ' && grid[currentCell.Row - 2][currentCell.Col] != 'V') {
      if (currentCell.Row - 2 == endCell.Row && currentCell.Col == endCell.Col) {
        numWalls = -1;
        availableWalls = new short[1];
        availableWalls[0] = 0;
        return;
      } else {
        numWalls++;
        tempStack.push(0);
      }
    }
  }
  // check bottom
  if (currentCell.Row + 2 < mazeSize.Row - 1) {
    if (grid[currentCell.Row + 1][currentCell.Col] == ' ' && grid[currentCell.Row + 2][currentCell.Col] != 'V') {
      if (currentCell.Row + 2 == endCell.Row && currentCell.Col == endCell.Col) {
        numWalls = -1;
        availableWalls = new short[1];
        availableWalls[0] = 1;
        return;
      } else {
        numWalls++;
        tempStack.push(1);
      }
    }
  }
  // check left
  if (currentCell.Col - 2 > 0) {
    if (grid[currentCell.Row][currentCell.Col - 1] == ' ' && grid[currentCell.Row][currentCell.Col - 2] != 'V') {
      if (currentCell.Row == endCell.Row && currentCell.Col - 2 == endCell.Col) {
        numWalls = -1;
        availableWalls = new short[1];
        availableWalls[0] = 2;
        return;
      } else {
        numWalls++;
        tempStack.push(2);
      }
    }
  }
  // check right
  if (currentCell.Col + 2 < mazeSize.Col - 1) {
    if (grid[currentCell.Row][currentCell.Col + 1] == ' ' && grid[currentCell.Row][currentCell.Col + 2] != 'V') {
      if (currentCell.Row == endCell.Row && currentCell.Col + 2 == endCell.Col) {
        numWalls = -1;
        availableWalls = new short[1];
        availableWalls[0] = 3;
        return;
      } else {
        numWalls++;
        tempStack.push(3);
      }
    }
  }
  availableWalls = new short[numWalls];
  for (short i = 0; i < numWalls; i++) {
    availableWalls[i] = tempStack.top();
    tempStack.pop();
  }
}

// class Solver_Backtracking definitions
void Solver_Backtracking::solveMaze() {
  currentCell = startCell; // setting current row and column
  updateMaze();
  trail.push(currentCell); // pushing start position to stack
  short step = 0;
  do {
    setVisited(); // setting cell as visited
    checkWalls(); // check available walls
    if (numWalls > 0) {
      short index = rand() % numWalls; // select randomly from available walls
      moveTo(availableWalls[index]);   // move to selected cell
      trail.push(currentCell);         // push to stack
    }
    // if no available walls
    else if (numWalls == 0) {
      trail.pop();
      currentCell = trail.top(); // set current row and column
    } else {
      moveTo(availableWalls[0]); // move to selected cell
      trail.push(currentCell);   // push to stack
    }
    usleep(100000);
    updateMaze();
    cout << endl
         << "Algorithm : " << yellow << "BACKTRACKING" << def;
    cout << endl
         << "Status : " << green << "SOLVING..." << def;
    cout << endl
         << "Steps : " << red << step << def << endl;
    delete availableWalls;
    step++;
  } while (currentCell.Row != endCell.Row || currentCell.Col != endCell.Col);
  currentCell = {-1, -1};
  updateMaze();
  cout << endl
       << "Algorithm : " << yellow << "BACKTRACKING" << def;
  cout << endl
       << "Status : " << red << "DONE!" << def;
  cout << endl
       << "Steps : " << green << step << def << endl;

  cout << "\nPress " << green << "ENTER" << def << " to CONTINUE...";
  while (!trail.empty()) {
    trail.pop();
  }
  cin.ignore();
  clearCells();
  updateMaze();
}

// class Solver_AStar definitions
void Solver_AStar::solveMaze() {
  // creating grid for calculations
  calculations = new pathScore *[(mazeSize.Row - 1) / 2];
  for (short i = 0; i < (mazeSize.Row - 1) / 2; i++) {
    calculations[i] = new pathScore[(mazeSize.Col - 1) / 2];
    for (short j = 0; j < (mazeSize.Col - 1) / 2; j++) {
      calculations[i][j] = {-1, -1, -1};
    }
  }
  currentCell = endCell;   // setting current to endCell
  trail.push(currentCell); // push current to stack
  short initialG = 0;
  do {
    // setting FGH of current cell
    calculations[(currentCell.Row - 1) / 2][(currentCell.Col - 1) / 2] = calcFGH(currentCell.Row, currentCell.Col, initialG);
    setVisited();            // set current to visited
    checkAdjacent(initialG); // checking adjacent cells
    // if no available cell
    if (numAvailable == 0) {
      initialG--;
      trail.pop();
      currentCell = trail.top();
    } else {
      moveTo(newCell);
      initialG++;
      trail.push(currentCell);
    }
  } while (currentCell.Row != startCell.Row || currentCell.Col != startCell.Col);
  // loop for clearing Visited Cells
  for (short i = 0; i < mazeSize.Row; i++) {
    for (short j = 0; j < mazeSize.Col; j++) {
      if (grid[i][j] == 'V')
        grid[i][j] = ' ';
    }
  }
  currentCell = startCell; // setting current cell
  updateMaze();
  short step = 0;
  do {
    setVisited(); // set current to visited
    checkAdjacent();
    moveTo(newCell);
    usleep(100000);
    updateMaze();
    cout << endl
         << "Algorithm : " << yellow << "A* (A-STAR)" << def;
    cout << endl
         << "Status : " << green << "SOLVING..." << def;
    cout << endl
         << "Steps : " << red << step << def << endl;
    step++;
  } while (currentCell.Row != endCell.Row || currentCell.Col != endCell.Col);
  currentCell = {-1, -1};
  updateMaze();
  cout << endl
       << "Algorithm : " << yellow << "A* [ A-STAR ]" << def;
  cout << endl
       << "Status : " << red << "DONE!" << def;
  cout << endl
       << "Steps : " << green << step << def << endl;
  cout << "\nPress " << green << "ENTER" << def << " to CONTINUE...";
  cin.ignore();
  clearCells();
  updateMaze();
}

void Solver_AStar::checkAdjacent(short tempG) {
  pathScore tempFGH[4];
  for (short i = 0; i < 4; i++) {
    availableWall[i] = -1;
    tempFGH[i] = {-1, -1, -1};
  }
  short i = 0;
  // check top
  if (grid[currentCell.Row - 1][currentCell.Col] == ' ' && grid[currentCell.Row - 2][currentCell.Col] != 'V') {
    tempFGH[i] = calcFGH(currentCell.Row - 2, currentCell.Col, tempG);
    availableWall[i] = 0;
    i++;
  }
  // check bottom
  if (grid[currentCell.Row + 1][currentCell.Col] == ' ' && grid[currentCell.Row + 2][currentCell.Col] != 'V') {
    tempFGH[i] = calcFGH(currentCell.Row + 2, currentCell.Col, tempG);
    availableWall[i] = 1;
    i++;
  }
  // check left
  if (grid[currentCell.Row][currentCell.Col - 1] == ' ' && grid[currentCell.Row][currentCell.Col - 2] != 'V') {
    tempFGH[i] = calcFGH(currentCell.Row, currentCell.Col - 2, tempG);
    availableWall[i] = 2;
    i++;
  }
  // check right
  if (grid[currentCell.Row][currentCell.Col + 1] == ' ' && grid[currentCell.Row][currentCell.Col + 2] != 'V') {
    tempFGH[i] = calcFGH(currentCell.Row, currentCell.Col + 2, tempG);
    availableWall[i] = 3;
    i++;
  }
  if (i != 0) {
    // sorting values from lowest to highest
    for (short j = 0; j < i - 1; j++) {
      for (short k = j + 1; k < i; k++) {
        if (tempFGH[j].F > tempFGH[k].F) {
          pathScore temp1;
          temp1 = tempFGH[j];
          tempFGH[j] = tempFGH[k];
          tempFGH[k] = temp1;
          short temp2;
          temp2 = availableWall[j];
          availableWall[j] = availableWall[k];
          availableWall[k] = temp2;
        }
      }
    }
    // check for multiple minimum values
    short count = 0;
    for (short j = 1; j < i; j++) {
      if (tempFGH[0].F == tempFGH[i].F)
        count++;
      else
        break;
    }
    numAvailable = 0;
    if (count == 0) {
      numAvailable = 1;
      newCell = availableWall[0];
    } else {
      numAvailable = count + 1;
      newCell = availableWall[rand() % numAvailable];
    }
  }
  else
    numAvailable = 0;
}

void Solver_AStar::checkAdjacent() {
  pathScore tempFGH[4];
  for (short i = 0; i < 4; i++) {
    availableWall[i] = -1;
    tempFGH[i] = {-1, -1, -1};
  }
  short i = 0;
  // check top
  if (grid[currentCell.Row - 1][currentCell.Col] == ' ' && grid[currentCell.Row - 2][currentCell.Col] != 'V') {
    if (calculations[(currentCell.Row - 3) / 2][(currentCell.Col - 1) / 2].F != -1) {
      tempFGH[i] = calculations[(currentCell.Row - 3) / 2][(currentCell.Col - 1) / 2];
      availableWall[i] = 0;
      i++;
    }
  }
  // check bottom
  if (grid[currentCell.Row + 1][currentCell.Col] == ' ' && grid[currentCell.Row + 2][currentCell.Col] != 'V') {
    if (calculations[(currentCell.Row + 1) / 2][(currentCell.Col - 1) / 2].F != -1) {
      tempFGH[i] = calculations[(currentCell.Row + 1) / 2][(currentCell.Col - 1) / 2];
      availableWall[i] = 1;
      i++;
    }
  }
  // check left
  if (grid[currentCell.Row][currentCell.Col - 1] == ' ' && grid[currentCell.Row][currentCell.Col - 2] != 'V') {
    if (calculations[(currentCell.Row - 1) / 2][(currentCell.Col - 3) / 2].F != -1) {
      tempFGH[i] = calculations[(currentCell.Row - 1) / 2][(currentCell.Col - 3) / 2];
      availableWall[i] = 2;
      i++;
    }
  }
  // check right
  if (grid[currentCell.Row][currentCell.Col + 1] == ' ' && grid[currentCell.Row][currentCell.Col + 2] != 'V') {
    if (calculations[(currentCell.Row - 1) / 2][(currentCell.Col + 1) / 2].F != -1) {
      tempFGH[i] = calculations[(currentCell.Row - 1) / 2][(currentCell.Col + 1) / 2];
      availableWall[i] = 3;
      i++;
    }
  }
  // sorting F value from lowest to highest
  for (short j = 0; j < i - 1; j++) {
    for (short k = j + 1; k < i; k++) {
      if (tempFGH[j].F > tempFGH[k].F) {
        pathScore temp1;
        temp1 = tempFGH[j];
        tempFGH[j] = tempFGH[k];
        tempFGH[k] = temp1;
        short temp2;
        temp2 = availableWall[j];
        availableWall[j] = availableWall[k];
        availableWall[k] = temp2;
      }
    }
  }
  // check for multiple minimum values
  short count = 0;
  for (short j = 1; j < i; j++) {
    if (tempFGH[0].F == tempFGH[j].F)
      count++;
    else
      break;
  }
  if (count != 0) {
    // sorting G value from lowest to highest
    for (short j = 0; j < count; j++) {
      for (short k = j + 1; k <= count; k++) {
        if (tempFGH[j].G > tempFGH[k].G) {
          pathScore temp1;
          temp1 = tempFGH[j];
          tempFGH[j] = tempFGH[k];
          tempFGH[k] = temp1;
          short temp2;
          temp2 = availableWall[j];
          availableWall[j] = availableWall[k];
          availableWall[k] = temp2;
        }
      }
    }
  }
  newCell = availableWall[0];
}

pathScore Solver_AStar::calcFGH(short tempR, short tempC, short tempG) {
  pathScore tempCalc;
  short tempH;
  if (startCell.Row > tempR)
    tempH = (startCell.Row - tempR);
  else
    tempH = (tempR - startCell.Row);
  if (startCell.Col > tempC)
    tempH += (startCell.Col - tempC);
  else
    tempH += (tempC - startCell.Col);
  tempH /= 2;
  tempCalc.G = tempG;
  tempCalc.H = tempH;
  tempCalc.F = tempG + tempH;
  return tempCalc;
}
