#ifndef ABSTRACTPQUANTIZER_H
#define ABSTRACTPQUANTIZER_H

#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "nnitem.h"

#ifndef FT_SFX0
#define FT_SFX0 ".pkeys"
#endif

#ifndef FT_SFX1
#define FT_SFX1 ".siftgeo"
#endif

using namespace std;

enum NNSOpt  {nns_brt, nns_lv, nns_no};
enum PQ_WGH  {pq_wgh_bin = 0, pq_wgh_hard, pq_wgh_aggr};
enum DST_OPT {__cos__ = 0, __l2__};


class AbstractPQuantizer
{
public:
    AbstractPQuantizer();
    virtual ~AbstractPQuantizer();

protected:
    static const int verbose;
    static const unsigned int FNLEN = 1024;
    static const unsigned int SNN   = 20;
    static const unsigned int fDim0 = 128; //feature dim by default;
    const float *vqVocab;
    unsigned int ftDim, vqNum;
    unsigned char *visited;
    unsigned int nThrd;
    unsigned int numA;

protected:
    /*****initialize for Hierarchical (two-layer) NN search*****/
    bool  _timeon_, _norm_, _sqrton_;
    unsigned int supVQNum;
    NNSOpt  _nns_opt;
    PQ_WGH  wgh_mthd;
    DST_OPT dst_msr;

    vector<vector<unsigned int> > supv_2_vocab;

    map<unsigned int, map<string, const char *>* > itmMaps;
    map<string, const char *> paras;
    char vocabfn[FNLEN], tmLogFn[FNLEN];

protected:


    void (AbstractPQuantizer::*nnVQFunc)(const float *query, const unsigned int ptNum,
                    const unsigned int dim, map<unsigned int, unsigned int> &bowVect);

    unsigned int (AbstractPQuantizer::*nnFunc)(const float *query, const unsigned int dim);


public:
    unsigned int nnSearchl2(const float *query,  const unsigned int dim);
            void nnSearch_SegL2(const float *query,   const unsigned int dim,
                                const unsigned int nSeg, unsigned int *idx);

    static float *load_VQVocab(const char *vocfn, unsigned int &vdim1,
                               unsigned int &vsize1);

    static float *load_PQVocab(const char *vocfn, unsigned int &vdim2,
                               unsigned int &vsize2, unsigned int &segment);
public:

    int  init(const char *conf);
    bool init_nnSearch(map<string, const char*> &paras);

};


#endif
