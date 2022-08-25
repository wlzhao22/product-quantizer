#include "hardwaresetup.h"

#include <iostream>
#include <string>
#include <vector>


using namespace std;

void HardWareSetup::cpuID(unsigned i, unsigned regs[4])
{

#ifdef _WINDOWS_
    __cpuid((int *)regs, (int)i);

#else
    asm volatile
    ("cpuid": "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
         : "a" (i), "c" (0));
#endif
}

unsigned int HardWareSetup::getCPUNum()
{
    unsigned regs[4] = {0};
    /// Get vendor
    char vendor[12];
    cpuID(0, regs);
    ((unsigned *)vendor)[0] = regs[1]; // EBX
    ((unsigned *)vendor)[1] = regs[3]; // EDX
    ((unsigned *)vendor)[2] = regs[2]; // ECX
    string cpuVendor = string(vendor, 12);

    /// Get CPU features
    cpuID(1, regs);
    unsigned cpuFeatures = regs[3]; // EDX

    /// Logical core count per CPU
    cpuID(1, regs);
    unsigned logical = (regs[1] >> 16) & 0xff; // EBX[23:16]
    ///cout <<" logical cpus: " << logical << endl;
    unsigned cores = logical;

    if(cpuVendor == "GenuineIntel")
    {
        cpuID(4, regs);
        cores = ((regs[0] >> 26) & 0x3f) + 1; // EAX[31:26] + 1

    }
    else if (cpuVendor == "AuthenticAMD")
    {
        cpuID(0x80000008, regs);
        cores = ((unsigned)(regs[2] & 0xff)) + 1; // ECX[7:0] + 1
    }

    ///cout <<" cpu cores: " << cores << endl;

    bool hyperThreads = cpuFeatures & (1 << 28) && cores < logical;

    //cout << " hyper-threads: " << (hyperThreads ? "true" : "false") << endl;

    return cores;
}


unsigned int HardWareSetup::getCoreNum()
{
    unsigned regs[4] = {0};
    /// Get vendor
    char vendor[12];
    cpuID(0, regs);
    ((unsigned *)vendor)[0] = regs[1]; /// EBX
    ((unsigned *)vendor)[1] = regs[3]; /// EDX
    ((unsigned *)vendor)[2] = regs[2]; /// ECX
    string cpuVendor = string(vendor, 12);

    /// Get CPU features
    cpuID(1, regs);
    unsigned cpuFeatures = regs[3]; /// EDX

    /// Logical core count per CPU
    cpuID(1, regs);
    unsigned logical = (regs[1] >> 16) & 0xff;    /// EBX[23:16]
    unsigned cores   = logical;

    if(cpuVendor == "GenuineIntel")
    {
        cpuID(4, regs);
        cores = ((regs[0] >> 26) & 0x3f) + 1;     /// EAX[31:26] + 1

    }
    else if (cpuVendor == "AuthenticAMD")
    {
        cpuID(0x80000008, regs);
        cores = ((unsigned)(regs[2] & 0xff)) + 1; /// ECX[7:0] + 1
    }

    bool hyperThreads = cpuFeatures & (1 << 28) && cores < logical;

    ///cout << " hyper-threads: " << (hyperThreads ? "true" : "false") << endl;

    return logical;
}

void print(int i, int j, int &k, int &l)
{
    cout<<i<<"\t"<<j<<"\t"<<k<<"\t"<<l<<endl;
    k = k+1;
    l = l+2;
}

void HardWareSetup::test()
{
    ///unsigned int ncpu = HardWareSetup::getCPUNum();
    ///cout<<ncpu<<endl;
}

