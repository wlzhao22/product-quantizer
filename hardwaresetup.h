#ifndef HARDWARESETUP_H
#define HARDWARESETUP_H

#include <iostream>

using namespace std;

void print(int i, int  j, int &k, int &l);

class HardWareSetup
{
    public:
        static void cpuID(unsigned i, unsigned regs[4]);
        static unsigned int getCPUNum();
        static unsigned int getCoreNum();
        static void test();
    public:
        HardWareSetup(){}
        virtual ~HardWareSetup(){}
};

#endif
