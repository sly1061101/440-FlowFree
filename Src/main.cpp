//
//  main.cpp
//  448-FlowFree
//
//  Created by Liuyi Shi on 10/29/17.
//  Copyright © 2017 Liuyi Shi. All rights reserved.
//

#include <stdio.h>
#include "puzzle.h"

int main(){
    CPuzzle testPuzzle("puzzle.txt");
    testPuzzle.initialize();
    testPuzzle.printResult();
    testPuzzle.test();
    testPuzzle.puzzleArcCheck();
    testPuzzle.test();
    testPuzzle.solve();
    testPuzzle.printResult();
    return 0;
}
