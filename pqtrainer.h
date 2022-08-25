#ifndef PQTRAINER_H
#define PQTRAINER_H

#include "kppmeans.h"

#include <iostream>
#include <map>
#include <set>

using namespace std;

/********************************************************************
This class is an implementation about paper:
"Product Quantization for Nearest Neighbor Search"

by Herve Jegou, Matthijs Douze and Cordelia Schmid, TPAMI'11

The adopted clustering methods are either K-Means++ or RB-KMeans
The trained structure supports IVFADC and IVFSDC search, which is
implemented in class "PQRetriever" (see pqretriever.h and pqretriever.cpp)

Step 1. Train coase level vocabulary (save to the destine loc.)
Step 2. Train M refined level vocabularies (save to the destine loc.)
*********************************************************************/

class PQTrainer
{
private:
    static const unsigned int SZ_RMAT0;
    static const unsigned int SZ_CMAT0;
    static const unsigned int FT_DIM;
    static const unsigned int step0;
    static const unsigned int FNLEN = 1024;
    bool _sqrton_;

public:
    PQTrainer(const char *conf);
    bool init(const char *conf);

    bool buildPQuantizer(const char *rmatfn, const unsigned int rnum0,
                                 const unsigned int dv, const char *dstfn);

    bool buildIVFPQuantizer(const char *rmatfn0,  const unsigned int rnum0,
                                 const unsigned int dv, const char *dstfn);

    static unsigned int buildVQuantizer(const char *srcMat, const char *cnum0, const char *dstfn);
    static bool   validate(const float *vect1, const unsigned int dim);
    static bool   norm(float *vect1, const unsigned int dim);
    static float *load_fvecs(const char *fvecfn, unsigned int &d, unsigned int &r);
    static float *load_fvecs(const char *fvecfn, unsigned int &d,
                             const unsigned int r0, unsigned int &r);
    static float *load_fvecs(const char *fvecfn, unsigned int &d,
                             const unsigned int r0, const unsigned int step, unsigned int &r);
    static void   saveMat(const float *mat, const unsigned int r,
                          const unsigned int d, const char *dstfn);

    unsigned int  nnSearchl2(const float *query,  const unsigned int dim);
    unsigned int (PQTrainer::*nn_func)(const float *query, const unsigned int dim);

    static void clean2DMat(vector<vector<unsigned int> > &mat);
    static void test();

/**for parallel nn Search**/
   unsigned char *visited;
   unsigned int   nThrd;

protected:
    float *cmatrix, *rmatrix, *pmatrix;
    const float *medians, *prjMat;
    unsigned int vqNum, vqDim, supNum, mNum, mDim;
    unsigned int prjNum, prjDim;
    const float *vqVocab, *supVocab;
    unsigned int cRow, rRow, rCol;
    unsigned int csize, cdim;
    bool _norm_, __LV2__;

    char rmatfn[1024];
    AbstractKMeans *kmeans;
    float *tmpvect, *residues;

public:
    virtual ~PQTrainer()
    {
        if(this->tmpvect != NULL)
        {
            delete [] this->tmpvect;
            this->tmpvect = NULL;
        }

        if(this->kmeans != NULL)
        {
            delete this->kmeans;
            this->kmeans = NULL;
        }

        if(this->cmatrix != NULL)
        {
            delete [] this->cmatrix;
            this->cmatrix = NULL;
        }

        if(this->rmatrix != NULL)
        {
            delete [] this->rmatrix;
            this->rmatrix = NULL;
        }

        if(this->pmatrix != NULL)
        {
            delete [] this->pmatrix;
            this->pmatrix = NULL;
        }


        if(this->vqVocab != NULL)
        {
            delete [] this->vqVocab;
            this->vqVocab = NULL;
        }


        if(this->visited != NULL)
        {
            delete [] this->visited;
            this->visited = NULL;
        }

        if(this->__LV2__== true && this->medians != NULL)
        {
            delete [] this->medians;
            this->medians = NULL;
        }


        if(this->prjMat != NULL)
        {
            delete [] prjMat;
            prjMat = NULL;
        }

    }

};

#endif
