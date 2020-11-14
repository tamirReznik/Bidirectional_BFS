#pragma once
class Cell
{
private:
	int row, col;
	Cell* parent;
	bool isStart;
public:
	Cell(int r, int c, Cell* pc, bool fromStart);

	int GetRow();
	int GetCol();
	Cell* GetParent();
	bool GetIsStart();
};

