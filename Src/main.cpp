//
//  main.cpp
//  448-FlowFree
//
//  Created by Liuyi Shi on 10/29/17.
//  Copyright © 2017 Liuyi Shi. All rights reserved.
//

#include <stdio.h>
#include "puzzle.h"

int main()
{
    //char* fileNames[3] = { "1212.txt","1214.txt","1414.txt" };
    //for (int i = 0; i < 3; i++)
    //{
    using namespace std;
    clock_t begin = clock();
    
    bool canSolve = true;
    CPuzzle testPuzzle=("puzzle.txt");
    testPuzzle.initialize();
    testPuzzle.printResult();
    testPuzzle.test();
//    testPuzzle.puzzleArcCheck();
//    //testPuzzle->puzzleArcGen();
//    testPuzzle.test();
    canSolve = testPuzzle.solve(NULL, NULL);
    testPuzzle.printResult();
    printf("numAssignment:%u \n", testPuzzle.numAssignment);
    if (!canSolve)
    {
        printf("cannot solve problem\n \n");
    }
    
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / (CLOCKS_PER_SEC/1000);
    cout << "time: " << elapsed_secs << endl;
    //}
    
    return 0;
}



int main_t(){
    CPuzzle testPuzzle("puzzle.txt");
    testPuzzle.initialize();
    testPuzzle.printResult();
    
    testPuzzle.test();
    testPuzzle.puzzleArcCheck();
    testPuzzle.test();
    
    testPuzzle.solve(NULL, NULL);
    testPuzzle.printResult();
    return 0;
}
