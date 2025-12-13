#include <iostream> 
#include <vector>
#include "Process.h"
#include "Scheduler.h"

using namespace std; 

vector<Process> getScenario1() {
    return {
        {"P1", 0, 8},
        {"P2", 0, 4},
        {"P3", 0, 1},
        {"P4", 0, 3}
    };
}

vector<Process> getScenario2() {
    return {
        {"P1", 0, 7},
        {"P2", 2, 4},
        {"P3", 4, 1},
        {"P4", 5, 4}
    };
}

vector<Process> getScenario3() {
    return {
        {"P1", 0, 20},
        {"P2", 0, 3},
        {"P3", 0, 3}
    };
}

int main(){
    cout << "==========================================\n";
    cout << "       CPU SCHEDULING SIMULATOR           \n";
    cout << "==========================================\n\n";

    cout << ">>> SCENARIO 1 <<<\n";
    testFCFS(getScenario1());
    testSJF(getScenario1());
    testRR(getScenario1(), 1);
    testRR(getScenario1(), 4);

    cout << "\n>>> SCENARIO 2 <<<\n";
    testFCFS(getScenario2());
    testSJF(getScenario2());
    testRR(getScenario2(), 1);
    testRR(getScenario2(), 4);

    cout << "\n>>> SCENARIO 3 <<<\n";
    testFCFS(getScenario3());
    testSJF(getScenario3());
    testRR(getScenario3(), 1);
    testRR(getScenario3(), 4);

    return 0;
}