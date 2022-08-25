#ifndef ABSTRACTKMEANS_H
#define ABSTRACTKMEANS_H

#include "nnitem.h"

#include <vector>
#include <map>

struct CLSInfo
{
public:
    CLSInfo()
    {
        n = 0;
        E = 0.0;
    }
    unsigned int n;    ///number of members
    double E;          ///sum of intra-sim
};

struct CLSIndex
{
public:
    CLSIndex()
    {
        index = 0;
        val   = 0.0;
    }
    CLSIndex(const unsigned int idx, const float val0)
    {
        index = idx;
        val   = val0;
    }
    unsigned int index;  /// number of members
    float val;           /// sum of intra-sim

    static bool LtComp (const CLSIndex *a,const CLSIndex *b)
    {
        return (a->val < b->val);
    }
    static bool LgComp (const CLSIndex *a,const CLSIndex *b)
    {
        return (a->val < b->val);
    }
};

enum KMN_OPT {_kppkmn_ = 1};

class AbstractKMeans
{
protected:
    AbstractKMeans();

protected:
    static const unsigned int paraBound;
    unsigned int count, ndim, clnumb;
    unsigned int nThrd;
    unsigned char *visited;
    vector<NNItem*>  sorted_stack;
    static const float smallVal0;
    char   srcmatfn[1024];
    float   *data;
    CLSInfo *infoMap;
    int     *labels;
    bool    _INIT_,  _REFER_;
    KMN_OPT kmMtd;

public:
    unsigned int buildcluster(const char *srcfn, const char *dstfn,   const char *lg_first,
                              const char *crtrn, const char *dstfunc, const char *numStr);
    unsigned int buildcluster(float *mat, const int row, const int dim, const char *dstfn,
                              const char  *lg_first, const char *crtrn, const char *distfunc, const char *numStr);

    virtual bool  init(const char *srcfn) = 0;
    virtual bool  init(float *data, const int row, const int dim) = 0;
    virtual bool  config(const char *lg_first, const char *crtrn,
                         const char *dst_func, int verbose) = 0;
    virtual unsigned int clust(const unsigned int clust_num, const char *dstfn, const int verbose) = 0;
    unsigned int nvclust(const unsigned int clnumb, const char *dstfn, const int verbose);

    float *load_fvecs(const char *fvecfn, unsigned int &d, unsigned int &r);
    float *loadMatrix(const char *fn, unsigned int &row, unsigned int &col);
    float *getCluster(const unsigned int clabel0, unsigned int &row, unsigned int &dim);

    void   printvect(vector<NNItem*> &vects);
    void   printvect(vector<NNItem*> &vects, const char *dst_info_fn);
    virtual void saveCenters(const char *dstfn, bool append) = 0;
    virtual int  fetchCenters(float *centers) = 0;
    virtual bool refresh() = 0;
    unsigned int getndim(){ return this->ndim;}
    void   printClusters(const char *dstdir);
    void   printCluster(const char *dstfn);
    void   printCLInfo(const CLSInfo *info, const unsigned int n);
    void   printCLInfo(const CLSInfo *info, const unsigned int n, const char *dstfn);
    bool   save_clust(const char *dstfn);

    bool   clearVector(vector<NNItem*> &vects);
    static void normVects(float *vects, const unsigned int d0, const unsigned int n0, float *lens);
    static void clearInfoMap(map<unsigned int, CLSInfo*> &infoMap);
    virtual ~AbstractKMeans();

};



#endif
