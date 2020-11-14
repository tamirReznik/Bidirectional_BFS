#include "Cell.h"

Cell::Cell(int r, int c, Cell* pc, bool fromStart)
{
	row = r;
	col = c;
	parent = pc;
	isStart = fromStart;
}


int Cell::GetRow()
{
	return row;
}

int Cell::GetCol()
{
	return col;
}

Cell* Cell::GetParent()
{
	return parent;
}

bool Cell::GetIsStart()
{
	return isStart;
}
