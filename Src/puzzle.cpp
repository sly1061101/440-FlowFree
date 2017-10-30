#include "puzzle.h"

void CPuzzle::loadPuzzle()
{
	char c;
	vector<char> row;
	ifstream myfile(filePath);
	if (myfile.is_open())
	{
		while (myfile.get(c))
		{
			if (c != '\n')
			{
				// if char is a color never seen before
				if ((c != '_') && (find(colors.begin(), colors.end(), c) == colors.end()) )
				{
					colors.push_back(c);
				}
				row.push_back(c);
			}
			else
			{
				arr.push_back(row);
				row.clear();
			}
		}
		arr.push_back(row);//push in the last line
		row.clear();
		myfile.close();
	}
}

void CPuzzle::initialize()
{
	loadPuzzle();
	columnSize = arr[0].size();
	rowSize = arr.size();
	//unAssignedGrids = new MinHeap(rowSize*columnSize, getHeuristic);
	//unAssignedGrids->initialize();
	vector<GridInfo> row;

	for (int rowIdx = 0; rowIdx < rowSize; rowIdx++)
	{
		for (int columnIdx = 0; columnIdx < columnSize; columnIdx++)
		{
            GridInfo newGridInfo;
			if (arr[rowIdx][columnIdx] == '_')
			{
				newGridInfo.SetInfo(rowIdx, columnIdx, columnSize, colors, false, 'U');
				// only push non source grids to queue since source grids are not involved in the push/pop process of CSP
				//unAssignedGrids.push_back(newGridInfo.coord);
			}
			else
			{
				newGridInfo.SetInfo(rowIdx, columnIdx, columnSize, colors, true, arr[rowIdx][columnIdx]);
				numSourceGrids++;
			}
			row.push_back(newGridInfo);
			//unAssignedGrids.push(newGridInfo);
		}
		puzzle.push_back(row);
		row.clear();
	}
}

bool CPuzzle::allAssigned(){
    bool allAssigned = true;
    for(auto &i: puzzle){
        for(auto &j: i){
            if(j.color == 'U'){
                allAssigned = false;
                break;
            }
        }
        if(allAssigned == false)
            break;
    }
    return allAssigned;
}

void CPuzzle::assignValue(Coord grid, char val)
{
    //vector<Coord>::iterator it = find(unAssignedGrids.begin(),unAssignedGrids.end(),grid);
    numAssignment++;
    puzzle[grid.row][grid.column].color = val;
//    if (it != unAssignedGrids.end())
//    {
//        unAssignedGrids.erase(it);
//    }
//    pendingGrids.push_back(grid);
    //unAssignedGrids->extractMin();
    //unAssignedGrids.pop();
}

Coord CPuzzle::chooseGrid()
{
    Coord Id(0,0);

    // printUnAssignedGridHeu();

    for (int i = 0; i<rowSize; ++i){
        for(int j = 0; j<columnSize; ++j){
            if(puzzle[i][j].color == 'U'){
                if( (Id.row == 0) && (Id.column == 0) )
                    Id = puzzle[i][j].coord;
//                if ( puzzle[i][j].legalVal.size() < puzzle[Id.row][Id.column].legalVal.size() )
//                {
//                    Id = puzzle[i][j].coord;
//                }
            }
        }
    }

    //GridInfo* nextCoordToAssign = unAssignedGrids.top();
    return Id;
}

//void CPuzzle::printUnAssignedGridHeu()
//{
//    for (auto &pGrid : unAssignedGrids)
//    {
//        printf("%d ", pGrid->heuristic);
//    }
//    printf("\n");
//}

//void CPuzzle::undoAssign(Coord grid)
//{
//    Coord gridToUndo = pendingGrids.back(); //pop the last element from stack
//    //assert(gridToUndo->gridId == grid->gridId);
//    pendingGrids.pop_back();
//    puzzle[gridToUndo.row][gridToUndo.column].color = 'U';
//    unAssignedGrids.push_back(gridToUndo);
//    //unAssignedGrids.push(gridToUndo);
//}

bool CPuzzle::puzzleViolationCheck()
{
    bool checkResult = true;
    Coord puzzleIterator(0, 0);
    for (int rowIdx = 0; rowIdx < rowSize; rowIdx++)
    {
        for (int columnIdx = 0; columnIdx < columnSize; columnIdx++)
        {
            puzzleIterator.row = rowIdx;
            puzzleIterator.column = columnIdx;
            checkResult = gridViolationCheck(puzzleIterator);
            if (checkResult == false)
            {
                break;
            }
        }
        if (checkResult == false)
        {
            break;
        }
    }

    return checkResult;
}

vector<Coord> CPuzzle::getAdjGrids(Coord position)
{
    vector<Coord> adjacentGrids;

    Coord coordinate = position.left();
    if ((coordinate.column >= 0) && (coordinate.column < columnSize) &&
        (coordinate.row >= 0) && (coordinate.row < rowSize))
    {
        adjacentGrids.push_back(coordinate);
    }

    coordinate = position.right();
    if ((coordinate.column >= 0) && (coordinate.column < columnSize) &&
        (coordinate.row >= 0) && (coordinate.row < rowSize))
    {
        adjacentGrids.push_back(coordinate);
    }

    coordinate = position.up();
    if ((coordinate.column >= 0) && (coordinate.column < columnSize) &&
        (coordinate.row >= 0) && (coordinate.row < rowSize))
    {
        adjacentGrids.push_back(coordinate);
    }

    coordinate = position.down();
    if ((coordinate.column >= 0) && (coordinate.column < columnSize) &&
        (coordinate.row >= 0) && (coordinate.row < rowSize))
    {
        adjacentGrids.push_back(coordinate);
    }
    return adjacentGrids;
}

bool CPuzzle::gridViolationCheck(Coord position)
{
    //     1
    //     ^
    //0<   >2
    //     v
    //     3
    // x->column
    // y->row
    vector<Coord> adjacentGrids;
    int numMatch = 0;
    int numOpenGrid = 0;
    
    // unassigned grid cannot be invalid
    if (puzzle[position.row][position.column].color == 'U')
    {
        return true;
    }

    adjacentGrids = getAdjGrids(position);

    for (auto &i: adjacentGrids)
    {
        if ( puzzle[i.row][i.column].color == puzzle[position.row][position.column].color )
        {
            numMatch++;
        }
        else if ( puzzle[i.row][i.column].color == 'U' )
        {
            numOpenGrid++;
        }
    }
    
    //not source grid
    if ( !puzzle[position.row][position.column].isSource )
    {
        // no matching, only good when more than 2 unassigned grids are available
        if (numMatch == 0)
        {
            if (numOpenGrid >= 2)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else if (numMatch == 1)
        {
            if (numOpenGrid >= 1)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            if (numMatch == 2)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    //is source grid
    else
    {
        if (numMatch == 1)
        {
            return true;
        }
        else if (numMatch == 0)
        {
            if (numOpenGrid >= 1)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

}

void CPuzzle::test()
{
    //print all recorded arc relationship
    for (auto &arcMap : arcToCheck)
    {
        printf("%d -> %d \n",puzzle[arcMap.first.row][arcMap.first.column].gridId, puzzle[arcMap.second.row][arcMap.second.column].gridId);
    }
    for (int rowIdx = 0; rowIdx < rowSize; rowIdx++)
    {
        for (int columnIdx = 0; columnIdx < columnSize; columnIdx++)
        {
            printf("%d ", puzzle[rowIdx][columnIdx].legalVal.size());
        }
        printf("\n");
    }
    printf("\n");
    /*
    // test min heap
    GridInfo* temp = static_cast<GridInfo*>(unAssignedGrids->extractMin());
    GridInfo* insertBack = NULL;
    int insertEveryTwoItr = 0;
    while (temp)
    {
        printf("%d\n", temp->heuristic);
        unAssignedGrids->printAllValues();
        if (insertEveryTwoItr == 1)
        {
            insertBack = temp;
        }
        if (insertEveryTwoItr <=2)
        {
            temp = static_cast<GridInfo*>(unAssignedGrids->extractMin());
        }
        else
        {
            unAssignedGrids->insert(static_cast<void*>(insertBack));
            insertEveryTwoItr = 0;
        }
        insertEveryTwoItr++;
    }
    */
    //for (int i = 0; i < (rowSize*columnSize); i++)
    //{
    //    GridInfo* dummy=NULL;
    //    pendingGrids.push_back(dummy);
    //}

    //Coord grid(3, 4);
    //printf("%d\n",puzzleViolationCheck());
    
    /*GridInfo* cause[8];
    GridInfo* effected[8];
    for (int i = 0; i < 8; i++)
    {
        cause[i] = new GridInfo;
        effected[i] = new GridInfo;
        pair<GridInfo*, GridInfo*> arcPair;
        arcPair.first = cause[i];
        arcPair.second = effected[i];
        arcToCheck[arcPair] = true;
    }
    pair<GridInfo*, GridInfo*> arcPair;
    arcPair.first = cause[7];
    arcPair.second = effected[7];
    printf("key cound:%d \n", arcToCheck.count(arcPair));
    printf("kfirst pair:%u -> %u\n", arcToCheck.begin()->first, arcToCheck.begin()->second);
    for (int i = 0; i < 8; i++)
    {
        delete cause[i];
        delete effected[i];
    }*/

    /*puzzleArcGen();
    printf("%d\n", arcToCheck.size());*/
    

}

void CPuzzle::printResult()
{
    for (int rowIdx = 0; rowIdx < rowSize; rowIdx++)
    {
        for (int columnIdx = 0; columnIdx < columnSize; columnIdx++)
        {
            printf("%c ", puzzle[rowIdx][columnIdx].color);
        }
        printf("\n");
    }
    printf("\n");
}

bool CPuzzle::solve(CPuzzle *p, unsigned int *numAssignment)
{
    if ( allAssigned() ){
        if(p != NULL)
            (*p).SetPuzzle( (*this).puzzle );
        (*numAssignment) += this->numAssignment;
        return true;
    }
    
    Coord nextGrid = chooseGrid();
    bool isValid = true;
    
    for ( auto &it: puzzle[nextGrid.row][nextGrid.column].legalVal )
    {
        CPuzzle p_next = (*this);
        
        p_next.assignValue(nextGrid, it);

        isValid = p_next.puzzleViolationCheck();

        if (isValid)
        {
            isValid = p_next.forwardChecking(puzzle[nextGrid.row][nextGrid.column].coord);
        }
        
        if (isValid)
        {
            isValid = p_next.puzzleArcCheck();
            printf("%s\n",isValid==true?"True":"False");
        }

        if (!isValid)
        {
            //undoAssign(nextGrid);
            //restorAdjGridLegalVal(puzzle[nextGrid.row][nextGrid.column].coord);
            continue;
        }
        else
        {
            p_next.printResult();
            
            if(p == NULL)
                isValid = p_next.solve(this, &(this->numAssignment) );
            else
                isValid = p_next.solve(this, numAssignment);
            
            if (isValid)
            {
                if(p != NULL){
                    (*p).SetPuzzle( (*this).puzzle );
                    (*numAssignment) += this->numAssignment;
                }
                return true;
            }
        }
    }
    return false;
}

//void CPuzzle::discardLegalVal(Coord myGrid, int discardGridID, char val)
//{
//    //remove value from legalValue
//    vector<char>::iterator it = find(puzzle[myGrid.row][myGrid.column].legalVal.begin(), puzzle[myGrid.row][myGrid.column].legalVal.end(), val);
//    // == myVector.end() means the element was not found
//    if (it != puzzle[myGrid.row][myGrid.column].legalVal.end())
//    {
//        puzzle[myGrid.row][myGrid.column].legalVal.erase(it);
//    }
//    //save removed value to discarded value
//    if (puzzle[myGrid.row][myGrid.column].discardedValue.count(discardGridID) == 0)
//    {
//        vector<char> newVector = {val};
//        puzzle[myGrid.row][myGrid.column].discardedValue.insert(pair<int, vector<char>>(discardGridID, newVector));
//        puzzle[myGrid.row][myGrid.column].discardedValue[discardGridID].push_back(val);
//    }
//    else
//    {
//        puzzle[myGrid.row][myGrid.column].discardedValue[discardGridID].push_back(val);
//    }
//    puzzle[myGrid.row][myGrid.column].heuristic = puzzle[myGrid.row][myGrid.column].legalVal.size();
//}

bool CPuzzle::forwardChecking(Coord responsibleGrid)
{
    vector<char> legalValCopy;    // a copy of legalVector to iterate through. Because original vector will be shrinked during the iteration
    vector<Coord> adjGrids = getAdjGrids(responsibleGrid);
    for (auto &i: adjGrids)
    {
        bool isValid = true;
        if(puzzle[i.row][i.column].color == 'U')
        {
            
            //for debugging====================================
            //if (pGrid->legalVal.size() < 5)
            //{
            //    int y = 1;
            //}
            //=================================================
            legalValCopy = puzzle[i.row][i.column].legalVal;
            for(auto &val:legalValCopy)
            {
                puzzle[i.row][i.column].color = val;
                isValid = puzzleViolationCheck();
                puzzle[i.row][i.column].color = 'U';
                if (isValid)
                {
                    break;
                    //discardLegalVal(i, puzzle[responsibleGrid.row][responsibleGrid.column].gridId, val);
                }
            }
//            if (puzzle[i.row][i.column].legalVal.empty())
//            {
//                restorAdjGridLegalVal(responsibleGrid);
//                return false;
//            }

        }
        if(!isValid)
            return false;
    }
    return true;
}

//void CPuzzle::restorAdjGridLegalVal(Coord responsibleCoord)
//{
//    vector<Coord> adjGrids = getAdjGrids(responsibleCoord);
//    for (auto &pGrid : adjGrids)
//    {
//        if (puzzle[pGrid.row][pGrid.column].color == 'U')
//        {
//            restoreGridLegalVal(pGrid, puzzle[responsibleCoord.row][responsibleCoord.column].gridId);
//        }
//    }
//}

//void CPuzzle::restoreGridLegalVal(Coord thisGrid, int responsibleGridID)
//{
//    //GridInfo* pGrid = puzzle[currentCoord.row][currentCoord.column];
//    for (auto &value : puzzle[thisGrid.row][thisGrid.column].discardedValue[responsibleGridID])
//    {
//        puzzle[thisGrid.row][thisGrid.column].legalVal.push_back(value);
//    }
//    puzzle[thisGrid.row][thisGrid.column].discardedValue.erase(responsibleGridID);
//    puzzle[thisGrid.row][thisGrid.column].heuristic = puzzle[thisGrid.row][thisGrid.column].legalVal.size();
//}

void CPuzzle::changedGridArcGen(Coord coordinate)
{
    Coord pEffectedGrid = coordinate;
    vector<Coord> adjGrids = getAdjGrids(coordinate);
    for (auto &causingGrid : adjGrids)
    {
        if ( puzzle[causingGrid.row][causingGrid.column].color == 'U' )
        {
            pair<Coord, Coord> arcPair;
            arcPair.first = causingGrid;
            arcPair.second = pEffectedGrid;
//            bool exist = false;
//            for(auto &i:arcToCheck)
//
            arcToCheck.push_back(arcPair);
        }
    }
}

void CPuzzle::gridArcGen(Coord coordinate)
{
    Coord causeGrid = coordinate;
    vector<Coord> adjGrids = getAdjGrids(coordinate);
    for (auto &pEffectedGrid : adjGrids)
    {
        //if (!pEffectedGrid->isSource)
        //{
            pair<Coord, Coord> arcPair;
            arcPair.first = causeGrid;
            arcPair.second = pEffectedGrid;
            //if (find(arcToCheck.begin(), arcToCheck.end(), arcPair) == arcToCheck.end())
            //{
                arcToCheck.push_back(arcPair);
            //}
        //}
    }
}

void CPuzzle::puzzleArcGen()
{
    for (int rowIdx = 0; rowIdx < rowSize; rowIdx++)
    {
        for (int columnIdx = 0; columnIdx < columnSize; columnIdx++)
        {
            if ( puzzle[rowIdx][columnIdx].color == 'U' )
            {
                gridArcGen(puzzle[rowIdx][columnIdx].coord);
            }
        }
    }
}

//eliminate bad legal value of all grids in puzzle
bool CPuzzle::puzzleArcCheck()
{
    pair<Coord, Coord> arcPair;
    puzzleArcGen();
    while (!arcToCheck.empty())
    {
        bool isModified = false; //is legalVal of causing grid modified?
        //arcPair.first is the causing grid, arcPair.second is the effected grid
        arcPair = arcToCheck.front();
        arcToCheck.erase(arcToCheck.begin());
        // check consistancy of arc pair, and remove legalVal of causing grid if their is arc violation
        isModified = gridArcCheck(arcPair);
        if (isModified)
        {
            if( puzzle[arcPair.first.row][arcPair.first.column].legalVal.size() == 0 )
                return false;
            // causing grid has change in legal value, add arc of other grid to the causing grid
            changedGridArcGen(arcPair.first);
        }
    }
    return true;
}

// check consistancy of arc pair, and remove legalVal of causing grid if their is arc violation
bool CPuzzle::gridArcCheck(pair<Coord, Coord> arcPair)
{
    bool isModified = false;
    bool isValid = false;
    Coord causingGrid = arcPair.first;
    Coord effectedGrid = arcPair.second;
    vector<char> causeLegalValCopy = puzzle[causingGrid.row][causingGrid.column].legalVal;
    vector<char>::iterator itLegalValToDiscard;
    for (auto &causeVal : causeLegalValCopy)
    {
        isValid = false;
        puzzle[causingGrid.row][causingGrid.column].color = causeVal;
        for (auto &effVal : puzzle[effectedGrid.row][effectedGrid.column].legalVal)
        {
            bool IsUnassignedGrid = ( puzzle[effectedGrid.row][effectedGrid.column].color == 'U' );
            
            if ( IsUnassignedGrid )
            {
                puzzle[effectedGrid.row][effectedGrid.column].color = effVal;
            }
            isValid = puzzleViolationCheck();
            if ( IsUnassignedGrid )
            {
                puzzle[effectedGrid.row][effectedGrid.column].color = 'U';
            }
            if (isValid)
            {
                break;
            }
        }
        puzzle[causingGrid.row][causingGrid.column].color = 'U';
        if (!isValid)
        {
            itLegalValToDiscard = find(puzzle[causingGrid.row][causingGrid.column].legalVal.begin(), puzzle[causingGrid.row][causingGrid.column].legalVal.end(), causeVal);
            if (itLegalValToDiscard != puzzle[causingGrid.row][causingGrid.column].legalVal.end())
            {
                puzzle[causingGrid.row][causingGrid.column].legalVal.erase(itLegalValToDiscard);
                puzzle[causingGrid.row][causingGrid.column].heuristic = puzzle[causingGrid.row][causingGrid.column].legalVal.size();
            }

            isModified = true;
        }
    }
    return isModified;
}
