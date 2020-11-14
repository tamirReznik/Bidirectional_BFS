#include "glut.h"
#include <time.h>
#include <vector>
#include "Cell.h"
#include <iostream>

using namespace std;

const int MSZ = 100;

const int SPACE = 0;
const int WALL = 1;
const int START = 2;
const int TARGET = 3;
const int BLACK = 4;
const int START_GRAY = 5;
const int PATH = 6;
const int TARGET_GRAY = 7;

int maze[MSZ][MSZ] = { 0 }; // 0 is SPACE
bool bfs_is_on = false;

vector <Cell*> startGrays;
vector <Cell*> targetGrays;


//void InitMaze();
void InitMaze()
{
	int i, j;

	// set margins
	for (i = 0; i < MSZ; i++)
	{
		maze[0][i] = WALL;
		maze[MSZ - 1][i] = WALL;
		maze[i][0] = WALL;
		maze[i][MSZ - 1] = WALL;
	}

	for (i = 1; i < MSZ - 1; i++)
		for (j = 1; j < MSZ - 1; j++)
		{
			if (i % 2 == 1)  // this is mostly SPACE
			{
				if (rand() % 100 < 10) // 10% WALLs
					maze[i][j] = WALL;
				else
					maze[i][j] = SPACE;
			}
			else // this is mostly WALL
			{
				if (rand() % 100 < 40) // 40% SPACEs
					maze[i][j] = SPACE;
				else
					maze[i][j] = WALL;
			}
		}
	int targetRow = rand() % MSZ, targetCol = rand() % MSZ;
	maze[MSZ / 2][MSZ / 2] = START;
	maze[targetRow][targetCol] = TARGET;

	Cell* pcStart = new Cell(MSZ / 2, MSZ / 2, nullptr, true);
	startGrays.push_back(pcStart); // add first start cell to grays
	Cell* pcTarget = new Cell(targetRow, targetCol, nullptr, false);
	targetGrays.push_back(pcTarget); // add first target cell to grays

}
void init()
{
	glClearColor(0.8, 0.7, 0.5, 0);// color of window background
	glOrtho(-1, 1, -1, 1, 1, -1);

	srand(time(0));

	InitMaze();
}



void DrawMaze()
{
	int i, j;
	double sx, sy; // cell size
	double x, y;

	sx = 2.0 / MSZ;
	sy = 2.0 / MSZ;

	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			switch (maze[i][j])
			{
			case SPACE:
				glColor3d(1, 1, 1);   // white
				break;
			case WALL:
				glColor3d(0.7, 0.4, 0);   // dark-orange
				break;
			case START:
				glColor3d(0, 1, 1);   // cyan
				break;
			case TARGET:
				glColor3d(1, 0, 0);   // red
				break;
			case TARGET_GRAY:
			case START_GRAY:
				glColor3d(1, 1, 0);   // yellow
				break;
			case BLACK:
				glColor3d(0.8, 1, 0.8);   // green
				break;
			case PATH:
				glColor3d(0.8, 0.4, 1);   // magenta
				break;
			}
			// draw square maze[i][j]
			x = 2 * (j / (double)MSZ) - 1;
			y = 2 * (i / (double)MSZ) - 1;

			glBegin(GL_POLYGON);  // fill up
			glVertex2d(x, y);
			glVertex2d(x, y + sy);
			glVertex2d(x + sx, y + sy);
			glVertex2d(x + sx, y);
			glEnd();
			glColor3d(0, 0, 0);

		}
}

void RestorePath(Cell* current)
{
	while (current->GetParent() != nullptr)
	{
		maze[current->GetRow()][current->GetCol()] = PATH;
		current = current->GetParent();
	}
}

void CheckNeighbor(Cell* current, int row, int col, int lookForGrayType)
{
	Cell* next;

	vector <Cell*>* srcGrays, * destGrays;
	int srcGrayType;

	if (lookForGrayType == START_GRAY)
	{
		srcGrayType = TARGET_GRAY;
		destGrays = &startGrays;
		srcGrays = &targetGrays;
	}
	else
	{
		srcGrayType = START_GRAY;
		destGrays = &targetGrays;
		srcGrays = &startGrays;

	}

	if (maze[row][col] == lookForGrayType)
	{
		bfs_is_on = false;
		RestorePath(current);
		for (int i = 0; i < (int)(*destGrays).size(); i++)
			if ((*destGrays).at(i)->GetRow() == row && (*destGrays).at(i)->GetCol() == col)
				RestorePath((*destGrays).at(i));

	}
	if (maze[row][col] == SPACE) // paint it gray
	{
		maze[row][col] = srcGrayType;
		next = new Cell(row, col, current, current->GetIsStart());
		(*srcGrays).push_back(next);
	}

}


void BFSIteration()
{
	Cell* currentFromStart, * currentFromTarget;
	int start_curr_row, start_curr_col, target_curr_row, target_curr_col;

	if (startGrays.empty() || targetGrays.empty()) // no more grays....no solution
	{
		cout << "no more grays....no solution\n";
		bfs_is_on = false;
	}
	else
	{

		// take the first element, paint it black and add to grays its white neighbors
		currentFromStart = startGrays.front();
		currentFromTarget = targetGrays.front();


		// remove the front element
		startGrays.erase(startGrays.begin());
		targetGrays.erase(targetGrays.begin());

		start_curr_row = currentFromStart->GetRow();
		start_curr_col = currentFromStart->GetCol();
		target_curr_row = currentFromTarget->GetRow();
		target_curr_col = currentFromTarget->GetCol();

		// paint it black
		if (maze[start_curr_row][start_curr_col] != START)
			maze[start_curr_row][start_curr_col] = BLACK;

		if (maze[target_curr_row][target_curr_col] != TARGET)
			maze[target_curr_row][target_curr_col] = BLACK;

		// look for white neighbors
		// up

		CheckNeighbor(currentFromStart, start_curr_row + 1, start_curr_col, TARGET_GRAY);
		if (bfs_is_on)		// down
			CheckNeighbor(currentFromStart, start_curr_row - 1, start_curr_col, TARGET_GRAY);
		if (bfs_is_on)		// left
			CheckNeighbor(currentFromStart, start_curr_row, start_curr_col - 1, TARGET_GRAY);
		if (bfs_is_on)		// right
			CheckNeighbor(currentFromStart, start_curr_row, start_curr_col + 1, TARGET_GRAY);
		if (bfs_is_on)		// down
			CheckNeighbor(currentFromTarget, target_curr_row + 1, target_curr_col, START_GRAY);
		if (bfs_is_on)		// left
			CheckNeighbor(currentFromTarget, target_curr_row, target_curr_col - 1, START_GRAY);
		if (bfs_is_on)		// right
			CheckNeighbor(currentFromTarget, target_curr_row, target_curr_col + 1, START_GRAY);
		if (bfs_is_on)		// down
			CheckNeighbor(currentFromTarget, target_curr_row - 1, target_curr_col, START_GRAY);
	}

}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	DrawMaze();

	glutSwapBuffers(); // show all
}

void idle()
{

	if (bfs_is_on)
		BFSIteration();

	glutPostRedisplay(); // indirect call to display
}

void menu(int choice)
{
	switch (choice)
	{
	case 1: // BFS
		bfs_is_on = true;
		break;
	case 2: // DFS
		break;
	}
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Path Algorithms");

	init();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	// menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Bidirectional Search", 1);
	glutAddMenuEntry("DFS", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();
}