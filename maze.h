#include "colormod.h"
#include <iostream>
#include <stack>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;
using namespace Color;

Modifier bRed(Color::BG_RED);
Modifier bGreen(Color::BG_GREEN);
Modifier bBlue(Color::BG_BLUE);
Modifier bYellow(Color::BG_YELLOW);
Modifier bDef(Color::BG_DEFAULT);
Modifier red(Color::FG_RED);
Modifier green(Color::FG_GREEN);
Modifier blue(Color::FG_BLUE);
Modifier yellow(Color::FG_YELLOW);
Modifier def(Color::FG_DEFAULT);

struct rowCol {
  short Row;
  short Col;
};

class Maze {
  public:
    Maze();
    ~Maze();
    void setHeightWidth(short, short);
    void mazeInitialize();
    void countUnvisited();
    void eraseWall(short);
    void moveTo(short);
    void updateMaze();
    void setVisited();
    void mazeGenerate(bool animate, long seedNum);
    char **grid;
    rowCol currentCell;
    rowCol mazeSize;
    short numUnvisited;

    short *unvisitedCell;
    stack<rowCol> trail;
};

Maze::Maze() {
  // random seed
  // srand(time(NULL));
  currentCell = {-1, -1};
  mazeSize = {0, 0};
  numUnvisited = 0;
}

Maze::~Maze() {
  delete unvisitedCell;
  delete grid;
}

void Maze::setHeightWidth(short mRow, short mCol) {
  mazeSize.Row = (mRow * 2) + 1;
  mazeSize.Col = (mCol * 2) + 1;
}

void Maze::mazeInitialize() {
  // creating grid, walls and spaces
  grid = new char *[mazeSize.Row];
  for (short i = 0; i < mazeSize.Row; i++) {
    grid[i] = new char[mazeSize.Col];
    for (short j = 0; j < mazeSize.Col; j++) {
      grid[i][j] = '#';
      if (i % 2 != 0 && j % 2 != 0)
        grid[i][j] = ' ';
    }
  }
}

void Maze::countUnvisited() {
  numUnvisited = 0;
  stack<int> tempStack;
  if (currentCell.Row - 2 > 0) {
    // check top
    if (grid[currentCell.Row - 2][currentCell.Col] == ' ')
    {
      numUnvisited++;
      tempStack.push(0);
    }
  }
  if (currentCell.Row + 2 < mazeSize.Row - 1) {
    // check bottom
    if (grid[currentCell.Row + 2][currentCell.Col] == ' ') {
      numUnvisited++;
      tempStack.push(1);
    }
  }
  if (currentCell.Col - 2 > 0) {
    // check left
    if (grid[currentCell.Row][currentCell.Col - 2] == ' ') {
      numUnvisited++;
      tempStack.push(2);
    }
  }
  if (currentCell.Col + 2 < mazeSize.Col - 1) {
    // check right
    if (grid[currentCell.Row][currentCell.Col + 2] == ' ') {
      numUnvisited++;
      tempStack.push(3);
    }
  }
  unvisitedCell = new short[numUnvisited];
  for (short i = 0; i < numUnvisited; i++) {
    unvisitedCell[i] = tempStack.top();
    tempStack.pop();
  }
}

void Maze::eraseWall(short x) {
  if (x == 0)
    grid[currentCell.Row - 1][currentCell.Col] = ' ';
  else if (x == 1)
    grid[currentCell.Row + 1][currentCell.Col] = ' ';
  else if (x == 2)
    grid[currentCell.Row][currentCell.Col - 1] = ' ';
  else if (x == 3)
    grid[currentCell.Row][currentCell.Col + 1] = ' ';
}

void Maze::moveTo(short x) {
  if (x == 0 && (currentCell.Row - 2) > 0)
    currentCell.Row -= 2;
  else if (x == 1 && (currentCell.Row + 2) < mazeSize.Row - 1)
    currentCell.Row += 2;
  else if (x == 2 && (currentCell.Col - 2) > 0)
    currentCell.Col -= 2;
  else if (x == 3 && (currentCell.Col + 2) < mazeSize.Col - 1)
    currentCell.Col += 2;
}

void Maze::updateMaze() {
  cout << "\033[1;1H"; // move cursor to the top-left corner
  for (short i = 0; i < mazeSize.Row; i++) {
    for (short j = 0; j < mazeSize.Col; j++) {
      if (i == currentCell.Row && j == currentCell.Col) {
        cout << bYellow << "  " << bDef;
      } else {
        if (grid[i][j] == 'V')
          cout << "  ";
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

void Maze::setVisited() {
  grid[currentCell.Row][currentCell.Col] = 'V';
}

void Maze::mazeGenerate(bool animate, long seedNum) {
  srand(seedNum);
  mazeInitialize();
  currentCell.Row = (rand() % (mazeSize.Row / 2)) * 2 + 1; // setting random start for generator
  currentCell.Col = (rand() % (mazeSize.Col / 2)) * 2 + 1;
  trail.push(currentCell); // pushing start position to stack
  setVisited();            // setting cell as visited
  do {
    countUnvisited(); // count unvisited cells
    if (numUnvisited > 0) {
      short index = rand() % numUnvisited; // select randomly from unvisited
      eraseWall(unvisitedCell[index]);     // erase corresponding wall
      moveTo(unvisitedCell[index]);        // move to selected cell
      trail.push(currentCell);             // push to stack
      setVisited();                        // setting cell as visited
    } else {
      trail.pop(); // if no more unvisited cells pop stack
      // set current row and column to to top of the stack
      currentCell = trail.top();
    }
    if (animate == true) {
      usleep(3000);
      updateMaze();
    }
    cout << endl << "Status : " << green << "GENERATING..." << def << endl;
  } while (trail.size() > 1);
  // clearing visited cells
  for (short i = 0; i < mazeSize.Row; i++) {
    for (short j = 0; j < mazeSize.Col; j++) {
      if (grid[i][j] == 'V')
        grid[i][j] = ' ';
    }
  }
  updateMaze();
  cout << endl;
  currentCell = {-1, -1};
  trail.pop();
}
