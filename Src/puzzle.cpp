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
	GridInfo newGridInfo;

	for (int rowIdx = 0; rowIdx < rowSize; rowIdx++)
	{
		for (int columnIdx = 0; columnIdx < columnSize; columnIdx++)
		{
			if (arr[rowIdx][columnIdx] == '_')
			{
				newGridInfo.SetInfo(rowIdx, columnIdx, columnSize, colors, false, 'U');
				// only push non source grids to queue since source grids are not involved in the push/pop process of CSP
				unAssignedGrids.push_back(newGridInfo.coord);
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

void CPuzzle::assignValue(Coord grid, char val)
{
    vector<Coord>::iterator it = find(unAssignedGrids.begin(),unAssignedGrids.end(),grid);
    numAssignment++;
    puzzle[grid.row][grid.column].color = val;
    if (it != unAssignedGrids.end())
    {
        unAssignedGrids.erase(it);
    }
    pendingGrids.push_back(grid);
    //unAssignedGrids->extractMin();
    //unAssignedGrids.pop();
}

Coord CPuzzle::chooseGrid()
{
    Coord Id;

    // printUnAssignedGridHeu();

    if (!unAssignedGrids.empty())
    {
        Id = unAssignedGrids[0];

        for (auto &i : unAssignedGrids)
        {
            if ( puzzle[i.row][i.column].heuristic < puzzle[Id.row][Id.column].heuristic )
            {
                Id = i;
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

void CPuzzle::undoAssign(Coord grid)
{
    Coord gridToUndo = pendingGrids.back(); //pop the last element from stack
    //assert(gridToUndo->gridId == grid->gridId);
    pendingGrids.pop_back();
    puzzle[gridToUndo.row][gridToUndo.column].color = 'U';
    unAssignedGrids.push_back(gridToUndo);
    //unAssignedGrids.push(gridToUndo);
}

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
    
    //is source grid
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
    //not source grid
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

//void CPuzzle::test()
//{
//    //print all recorded arc relationship
//    for (auto &arcMap : arcToCheck)
//    {
//        printf("%d -> %d \n",arcMap.first->gridId, arcMap.second->gridId);
//    }
//    for (int rowIdx = 0; rowIdx < rowSize; rowIdx++)
//    {
//        for (int columnIdx = 0; columnIdx < columnSize; columnIdx++)
//        {
//            printf("%d ", puzzle[rowIdx][columnIdx]->legalVal.size());
//        }
//        printf("\n");
//    }
//    /*
//    // test min heap
//    GridInfo* temp = static_cast<GridInfo*>(unAssignedGrids->extractMin());
//    GridInfo* insertBack = NULL;
//    int insertEveryTwoItr = 0;
//    while (temp)
//    {
//        printf("%d\n", temp->heuristic);
//        unAssignedGrids->printAllValues();
//        if (insertEveryTwoItr == 1)
//        {
//            insertBack = temp;
//        }
//        if (insertEveryTwoItr <=2)
//        {
//            temp = static_cast<GridInfo*>(unAssignedGrids->extractMin());
//        }
//        else
//        {
//            unAssignedGrids->insert(static_cast<void*>(insertBack));
//            insertEveryTwoItr = 0;
//        }
//        insertEveryTwoItr++;
//    }
//    */
//    //for (int i = 0; i < (rowSize*columnSize); i++)
//    //{
//    //    GridInfo* dummy=NULL;
//    //    pendingGrids.push_back(dummy);
//    //}
//
//    //Coord grid(3, 4);
//    //printf("%d\n",puzzleViolationCheck());
//    
//    /*GridInfo* cause[8];
//    GridInfo* effected[8];
//    for (int i = 0; i < 8; i++)
//    {
//        cause[i] = new GridInfo;
//        effected[i] = new GridInfo;
//        pair<GridInfo*, GridInfo*> arcPair;
//        arcPair.first = cause[i];
//        arcPair.second = effected[i];
//        arcToCheck[arcPair] = true;
//    }
//    pair<GridInfo*, GridInfo*> arcPair;
//    arcPair.first = cause[7];
//    arcPair.second = effected[7];
//    printf("key cound:%d \n", arcToCheck.count(arcPair));
//    printf("kfirst pair:%u -> %u\n", arcToCheck.begin()->first, arcToCheck.begin()->second);
//    for (int i = 0; i < 8; i++)
//    {
//        delete cause[i];
//        delete effected[i];
//    }*/
//
//    /*puzzleArcGen();
//    printf("%d\n", arcToCheck.size());*/
//    
//
//}

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

bool CPuzzle::solve()
{
    Coord nextGrid = chooseGrid();
    bool isValid = true;

    //assert(nextGrid != NULL);
    //assert((pendingGrids.size()+unAssignedGrids.size()) == ((columnSize*rowSize) - numSourceGrids));

    if (allAssigned())
    {
        return true;
    }

    for ( auto &it: puzzle[nextGrid.row][nextGrid.column].legalVal )
    {
        assignValue(nextGrid, it);
        //printResult();
        isValid = puzzleViolationCheck();

        if (isValid)
        {
            isValid = forwardChecking(puzzle[nextGrid.row][nextGrid.column].coord);
        }

        if (!isValid)
        {
            undoAssign(nextGrid);
            restorAdjGridLegalVal(puzzle[nextGrid.row][nextGrid.column].coord);
            continue;
        }
        else
        {
            isValid = solve();
            if (isValid)
            {
                return true;
            }
            else
            {
                undoAssign(nextGrid);
                restorAdjGridLegalVal(puzzle[nextGrid.row][nextGrid.column].coord);
            }
        }
    }
    return false;
}

void CPuzzle::discardLegalVal(Coord myGrid, int discardGridID, char val)
{
    //remove value from legalValue
    vector<char>::iterator it = find(puzzle[myGrid.row][myGrid.column].legalVal.begin(), puzzle[myGrid.row][myGrid.column].legalVal.end(), val);
    // == myVector.end() means the element was not found
    if (it != puzzle[myGrid.row][myGrid.column].legalVal.end())
    {
        puzzle[myGrid.row][myGrid.column].legalVal.erase(it);
    }
    //save removed value to discarded value
    if (puzzle[myGrid.row][myGrid.column].discardedValue.count(discardGridID) == 0)
    {
        vector<char> newVector = {val};
        puzzle[myGrid.row][myGrid.column].discardedValue.insert(pair<int, vector<char>>(discardGridID, newVector));
        puzzle[myGrid.row][myGrid.column].discardedValue[discardGridID].push_back(val);
    }
    else
    {
        puzzle[myGrid.row][myGrid.column].discardedValue[discardGridID].push_back(val);
    }
    puzzle[myGrid.row][myGrid.column].heuristic = puzzle[myGrid.row][myGrid.column].legalVal.size();
}

bool CPuzzle::forwardChecking(Coord responsibleGrid)
{
    bool isValid = true;
    vector<char> legalValCopy;    // a copy of legalVector to iterate through. Because original vector will be shrinked during the iteration
    vector<Coord> adjGrids = getAdjGrids(responsibleGrid);
    for (auto &i: adjGrids)
    {
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
                if (!isValid)
                {
                    discardLegalVal(i, puzzle[responsibleGrid.row][responsibleGrid.column].gridId, val);
                }
            }
            if (puzzle[i.row][i.column].legalVal.empty())
            {
                restorAdjGridLegalVal(responsibleGrid);
                return false;
            }

        }
    }
    return true;
}

void CPuzzle::restorAdjGridLegalVal(Coord responsibleCoord)
{
    vector<Coord> adjGrids = getAdjGrids(responsibleCoord);
    for (auto &pGrid : adjGrids)
    {
        if (puzzle[pGrid.row][pGrid.column].color == 'U')
        {
            restoreGridLegalVal(pGrid, puzzle[responsibleCoord.row][responsibleCoord.column].gridId);
        }
    }
}

void CPuzzle::restoreGridLegalVal(Coord thisGrid, int responsibleGridID)
{
    //GridInfo* pGrid = puzzle[currentCoord.row][currentCoord.column];
    for (auto &value : puzzle[thisGrid.row][thisGrid.column].discardedValue[responsibleGridID])
    {
        puzzle[thisGrid.row][thisGrid.column].legalVal.push_back(value);
    }
    puzzle[thisGrid.row][thisGrid.column].discardedValue.erase(responsibleGridID);
    puzzle[thisGrid.row][thisGrid.column].heuristic = puzzle[thisGrid.row][thisGrid.column].legalVal.size();
}

//void CPuzzle::changedGridArcGen(Coord coordinate)
//{
//    GridInfo* pEffectedGrid = puzzle[coordinate.row][coordinate.column];
//    vector<GridInfo*> adjGrids = getAdjGrids(coordinate);
//    for (auto &causingGrid : adjGrids)
//    {
//        if (!causingGrid->isSource)
//        {
//            pair<GridInfo*, GridInfo*> arcPair;
//            arcPair.first = causingGrid;
//            arcPair.second = pEffectedGrid;
//            //if ( find(arcToCheck.begin(), arcToCheck.end(), arcPair)== arcToCheck.end())
//            //{
//                arcToCheck.push_back(arcPair);
//            //}
//        }
//    }
//}
//
//void CPuzzle::gridArcGen(Coord coordinate)
//{
//    GridInfo* causeGrid = puzzle[coordinate.row][coordinate.column];
//    vector<GridInfo*> adjGrids = getAdjGrids(coordinate);
//    for (auto &pEffectedGrid : adjGrids)
//    {
//        //if (!pEffectedGrid->isSource)
//        //{
//            pair<GridInfo*, GridInfo*> arcPair;
//            arcPair.first = causeGrid;
//            arcPair.second = pEffectedGrid;
//            //if (find(arcToCheck.begin(), arcToCheck.end(), arcPair) == arcToCheck.end())
//            //{
//                arcToCheck.push_back(arcPair);
//            //}
//        //}
//    }
//}
//
//void CPuzzle::puzzleArcGen()
//{
//    for (int rowIdx = 0; rowIdx < rowSize; rowIdx++)
//    {
//        for (int columnIdx = 0; columnIdx < columnSize; columnIdx++)
//        {
//            if (!puzzle[rowIdx][columnIdx]->isSource)
//            {
//                gridArcGen(puzzle[rowIdx][columnIdx]->coord);
//            }
//        }
//    }
//}
//
////eliminate bad legal value of all grids in puzzle
//void CPuzzle::puzzleArcCheck()
//{
//    bool isModified = false; //is legalVal of causing grid modified?
//    pair<GridInfo*, GridInfo*> arcPair;
//    puzzleArcGen();
//    while (!arcToCheck.empty())
//    {
//        //arcPair.first is the causing grid, arcPair.second is the effected grid
//        arcPair = arcToCheck.front();
//        arcToCheck.erase(arcToCheck.begin());
//        //for debugging==================
//        if (puzzle[0][0]->color == 'U')
//        {
//            int o = 1;
//        }
//        //===============================
//        // check consistancy of arc pair, and remove legalVal of causing grid if their is arc violation
//        isModified = gridArcCheck(arcPair);
//        if (isModified)
//        {
//            // causing grid has change in legal value, add arc of other grid to the causing grid
//            changedGridArcGen(arcPair.first->coord);
//        }
//    }
//}
//
//// check consistancy of arc pair, and remove legalVal of causing grid if their is arc violation
//bool CPuzzle::gridArcCheck(pair<GridInfo*, GridInfo*> arcPair)
//{
//    bool isModified = false;
//    bool isValid = false;
//    GridInfo* causingGrid = arcPair.first;
//    GridInfo* effectedGrid = arcPair.second;
//    vector<char> causeLegalValCopy = causingGrid->legalVal;
//    vector<char>::iterator itLegalValToDiscard;
//    for (auto &causeVal : causeLegalValCopy)
//    {
//        isValid = false;
//        causingGrid->color = causeVal;
//        for (auto &effVal : effectedGrid->legalVal)
//        {
//            if (!effectedGrid->isSource)
//            {
//                effectedGrid->color = effVal;
//            }
//            isValid = puzzleViolationCheck();
//            if (!effectedGrid->isSource)
//            {
//                effectedGrid->color = 'U';
//            }
//            if (isValid)
//            {
//                break;
//            }
//        }
//        causingGrid->color = 'U';
//        if (!isValid)
//        {
//            itLegalValToDiscard = find(causingGrid->legalVal.begin(), causingGrid->legalVal.end(), causeVal);
//            if (itLegalValToDiscard != causingGrid->legalVal.end())
//            {
//                causingGrid->legalVal.erase(itLegalValToDiscard);
//                causingGrid->heuristic = causingGrid->legalVal.size();
//            }
//
//            if (!isModified)
//            {
//                isModified = true;
//            }
//        }
//    }
//    return isModified;
//}

//int main_o()
//{
//    //char* fileNames[3] = { "1212.txt","1214.txt","1414.txt" };
//    //for (int i = 0; i < 3; i++)
//    //{
//        using namespace std;
//        clock_t begin = clock();
//
//        bool canSolve = true;
//        CPuzzle* testPuzzle = new CPuzzle("puzzle.txt");
//        testPuzzle->initialize();
//        testPuzzle->puzzleArcCheck();
//        //testPuzzle->puzzleArcGen();
//        //testPuzzle->test();
//        canSolve = testPuzzle->solve();
//        testPuzzle->printResult();
//        printf("numAssignment:%u \n", testPuzzle->numAssignment);
//        if (!canSolve)
//        {
//            printf("cannot solve problem\n \n");
//        }
//        testPuzzle->destroy();
//
//        clock_t end = clock();
//        double elapsed_secs = double(end - begin) / (CLOCKS_PER_SEC/1000);
//        cout << "time: " << elapsed_secs << endl;
//    //}
//
//    return 0;
//}

