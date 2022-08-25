#ifndef PQENCODER_H
#define PQENCODER_H

#include <iostream>
#include <cstring>
#include <vector>
#include <map>

#include "abstractpquantizer.h"

using namespace std;

/*****************************************************
This class contains implementation about quantizing
 points (multi-dimensional) into product-quantizer for
 nearest neighbor search. Similar class 'PQuantizer' is
 desgined for similar image retrieval.

 I try to make it self-containing rather than inheriting
 from 'AbstractQuantizer' as much as possible.

******************************************************/

class PQEncoder: public AbstractPQuantizer
{
private:
    unsigned int w, dv0, pqDim, pqNum, mNum, mDim, nSeg;
    long int scale;
    const float *pqVocab;
    const float *medians;
    bool _isivfpq_, __LV2__;

protected:
    ifstream *(PQEncoder::*openMatrixFile)(const char *srcFn, unsigned int &sz, unsigned int &dim);
    bool      (PQEncoder::*fetchVectFrmMat)(ifstream *&inStrm, float *fvector, const int di);



public:


    PQEncoder(const char* conf, const char *opt);
    int  init(const char *conf);
    float quantzIVFPQ(const float *feat, const unsigned int d0,
                   const unsigned int dv, unsigned int *codes);

    float quantzPQ(const float *feat, const unsigned int d0,
                const unsigned int dv, unsigned int *codes);

    bool quantzPQ_l2(const float *residues, const unsigned int d0,
                    const unsigned int dv, unsigned int codes[]);

    ifstream *openBINMat1(const char *srcFn, unsigned int &sz, unsigned int &dim);
    ifstream *openBINMat4(const char *srcFn, unsigned int &sz, unsigned int &dim);
    ifstream *openTXTMat(const char *srcFn, unsigned int &sz, unsigned int &dim);

    bool fetchVectorFromTXT(ifstream *&inStrm, float *fvector, const int di);
    bool fetchVectorFromBIN1(ifstream *&inStrm, float *fvector, const int dim);
    bool fetchVectorFromBIN4(ifstream *&inStrm, float *fvector, const int dim);

    bool performPQEncodeFvect(const char *itmFn);
    bool performPQEncodeMat(const char *itmFn);
    bool performPQEncodeMat(const char *srcFn, const char *dstFn);
    bool performPQEncodeMat(const char *srcfn, const unsigned int seg0);

    static float *loadFeat(ifstream *inStrm, unsigned int &num, unsigned int &dim);

    static void test();

    virtual ~PQEncoder()
    {
        cout<<"\nRelease allocated memories ...... ";
        if(pqVocab != nullptr)
        {
            delete [] pqVocab;
            this->pqVocab = nullptr;
        }

        if(__LV2__== true && this->medians != nullptr)
        {
            delete [] this->medians;
            this->medians = nullptr;
        }

        cout<<"done\n";
    }
};

#endif
