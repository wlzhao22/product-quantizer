#ifndef INVTFLLOADER_H
#define INVTFLLOADER_H

#include "invtitem.h"

#include <map>

using namespace std;

/****************************************************
In charge of loading inverted file from the
disc. The inverted file structure is built on the fly.

***************************************************/
class InvtFLLoader
{

public:
    static const unsigned int BLK_SIZE = 500;
    static const unsigned int FNLEN    = 1024;

    /**for VLAD + PQ framework**/
    static unsigned long  load_ivfpq_InvtF(Hdr_InvtPQBlock *invtTable,  unsigned int *offsets, map<unsigned int, map<string, const char *>* > &para_map, const int vqNum, const unsigned int dv0);
    static unsigned long  load_ivfpq_InvtF(Hdr_InvtPQCBlock *invtTable, unsigned int *offsets, map<unsigned int, map<string, const char *>* > &para_map, const int vqNum, const unsigned int dv0);
    static unsigned long  load_ivfpq_InvtF(Hdr_InvtPQSBlock *invtTable, unsigned int *offsets, map<unsigned int, map<string, const char *>* > &para_map, const int vqNum, const unsigned int dv0,
                                          const unsigned int s0);
    static unsigned char *load_pq_Codes(map<unsigned int, map<string, const char *>* > &itmMaps, unsigned long &refImgSz, unsigned int &dim);

    static bool calIDFWgh(const char *srcItm, const char *idfFn, const char *wghtabFn);
    static void test();
};
#endif
