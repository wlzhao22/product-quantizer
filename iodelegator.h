#ifndef IODELEGATOR_H
#define IODELEGATOR_H

#include <unordered_map>
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include <map>

#include "invtitem.h"

using namespace std;

/*****************************************************
In charge of input operations

@author:     Wan-Lei Zhao
@updated:    Feb-2015
@institute:  XMU.CN

*****************************************************/

class IODelegator
{
private:
    static const unsigned int LONGCHAR  = 2000;
    static const unsigned int FNLEN     = 1024;
    static const unsigned int precision = 3;

public:
    IODelegator();

    static float *loadTXTMatrix(const char *fn,  unsigned int &num, unsigned int &dim);
    static float *loadFVECMatrix(const char *fn, unsigned int &num, unsigned int &dim);
    static void   loadMultiMap(const char *srcfn, vector<vector<unsigned int> > &supmap);

    static bool loadWghs(const char *weightfn, map<unsigned int,float> &i2wMap, const unsigned int offset);
    static bool loadWghs(const char *weightfn, map<unsigned int,float> &i2wMap, map<unsigned int, unsigned int>  &pmap,
                         map<unsigned int, unsigned int>  &_pmap, map<unsigned int, int>  &tmap, const unsigned int offset);

    static void load_k2imap(const char *srcfn,  map<string, unsigned int> &referTab);
    static void load_k2imap(const char *srcfn,  map<string, int> &referTab);
    static void load_i2kmap(const char *nmapfn, map<unsigned int, const char*> &i2kMap,
                            const unsigned int offset);

    static void load_map(const char *srcfn, map<string, unsigned char> &referTab);

    static vector<const char*> loadNameList(const char *kflstfn, const char *msg);
    static vector<const char*> loadStrings(const char *kflstfn);

    static void parse(const char *oneline, list<float> &vals, const char splitter);
    static void parse(const char *oneline, list<unsigned int> &vals,const char splitter);
    static void parseLine(vector<char> &oneline, vector<float> &vals, const char splitter);
    static void parseLine(vector<char> &oneline, vector<float> &vals, const char splitters[]);
    static void test();

    ~IODelegator();
};

#endif
