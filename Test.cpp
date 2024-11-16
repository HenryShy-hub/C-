#include <iostream>
#include <cassert>
#include "Option.h"
#include "TrinomialPrice.h"

// Test 1: Lattice Structure
void testLattice() {
    Option opt(100.0, 100.0, 0.05, 0.30, 1, European, Call);
    TrinomialPrice p(&opt, 5);
    p.buildLattice(0);

    double epsilon = 1e-6;
    assert(p.lattice->root->up->up->up->down - p.lattice->root->down->up->up->up < epsilon);
    assert(p.lattice->root->up->down->down->horizontal->horizontal - p.lattice->root->down->down->down->up->up < epsilon);

    std::cout << "testLattice passed." << std::endl;
}

// Test 2: European Call
void testEuropeanCall5Step() {
    Option opt(100.0, 100.0, 0.05, 0.30, 1, European, Call);
    TrinomialPrice p(&opt, 5);
    p.buildLattice(0);
    p.backwardEval(p.lattice->root);

    assert(std::abs(p.lattice->root->data.optionPrice - 14.22) < 0.01);
    std::cout << "testEuropeanCall passed." << std::endl;
}

// Test 3: European Put
void testEuropeanPut5Step() {
    Option opt(100.0, 100.0, 0.05, 0.30, 1, European, Put);
    TrinomialPrice p(&opt, 5);
    p.buildLattice(0);
    p.backwardEval(p.lattice->root);

    assert(std::abs(p.lattice->root->data.optionPrice - 9.34) < 0.01);
    std::cout << "testEuropeanPut passed." << std::endl;
}

// Test 4: American Call
void testAmericanCall5Step() {
    Option opt(100.0, 100.0, 0.05, 0.30, 1, American, Call);
    TrinomialPrice p(&opt, 5);
    p.buildLattice(0);
    p.backwardEval(p.lattice->root);

    assert(std::abs(p.lattice->root->data.optionPrice - 14.22) < 0.01);
    std::cout << "testAmericanCall passed." << std::endl;
}

// Test 5: American Put
void testAmericanPut5Step() {
    Option opt(100.0, 120.0, 0.05, 0.30, 1, American, Put);
    TrinomialPrice p(&opt, 5);
    p.buildLattice(0);
    p.backwardEval(p.lattice->root);

    assert(std::abs(p.lattice->root->data.optionPrice - 22.82) < 0.01);
    std::cout << "testAmericanPut passed." << std::endl;
}

int main() {
    testLattice();
    testEuropeanCall5Step();
    testEuropeanPut5Step();
    testAmericanCall5Step();
    testAmericanPut5Step();

    system("pause");

    return 0;
}
