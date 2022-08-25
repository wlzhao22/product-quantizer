#ifndef CLEANER_H
#define CLEANER_H

#include "nnitem.h"
#include "invtitem.h"
#include "indexmeta.h"

#include <map>
#include <list>
#include <vector>

/**
In charge of memory recycling, acts as
a rubbish collector

@author:    Wanlei Zhao
@email:     stonescx@gmail.com
@date:      30-Jul-2012
@Institute: INRIA
**/

using namespace std;

class Cleaner
{

private:
    static void eraseInvtList(InvtPQCBlock *header);
    static void eraseInvtList(InvtPQOBlock *header);
    static void eraseInvtList(InvtPQSBlock *header);

public:

    static void clearRankList(list<IndexMeta*>    *rankList);
    static void clearRankList(vector<IndexMeta*>  *rankList);
    static void clearRetriList(list<RetriItem*>   &nnItems);
    static void clearRetriList(vector<RetriItem*> &nnItems);
    static void clearNNList(list<NNItem*> &nnItems);
    static void clearNNList(list<MiniNN*> &nnItems);

    static void clearMatrix(vector<vector<unsigned int> > &matrix);

    static void clearInvtList(InvtBlock    *header);

    static void clear_k2iMap(map<string, unsigned int>      &refTab);
    static void clear_k2iMap(map<string, unsigned char>     &refTab);
    static void clear_i2kMap(map<unsigned int, const char*> &i2kMap);

    static void releaseInvtList(Hdr_InvtPQCBlock *invertLst, const unsigned int vqNum);
    static void releaseInvtList(Hdr_InvtPQOBlock *invertLst, const unsigned int vqNum);
    static void releaseInvtList(Hdr_InvtPQSBlock *invertLst, const unsigned int vqNum);

    static void releaseInvtList(map<unsigned int, vector<unsigned int>* > &invtLst);

    static void freeItemMaps(map<unsigned int, map<string, const char*> *> &itmMaps);

    static void freeParaMap(map<string, const char*> &paras);
    static void freeStrVect(vector<const char*> &strVect);

};

#endif
