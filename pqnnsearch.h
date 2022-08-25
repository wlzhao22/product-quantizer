#ifndef PQNNSEARCH_H
#define PQNNSEARCH_H

#include "indexmeta.h"
#include "topkheap.h"
#include <iostream>
#include <fstream>
#include <list>
#include <map>

using namespace std;

enum Task_Opt {_nns_, _img_rtr_};

/****************************************************
Implementation of (multi-dimensional) NN search by
product-quantizer (PQ)

For symmetric search, both query and candidate points
are quantized into one VQ cell. For asymmetric search,
query has been quantized into 'nn' cells, which improves
the recall considerably.

For PQ NNsearch, residue of the input vector should not
be normalized.

Please refer to following paper for variables defined in
the code.

Product quantization for nearest neighbor search, Herve J., et al.

The naming about the variables and functions is inline with the original
paper as much as possible.

*****************************************************/

/**********nnSearch performance of PQ*****************
k'=8192, dataset=1M SIFT
site for data: http://corpus-texmex.irisa.fr/
Top     1       10      50      100     k-means
asymm   28.6    66.6    86.7    91.2    k-means++ (w=16)
asymm   22.0    40.0    42.3    42.3    k-means++ (w=1)
symm    16.7    35.4    41.6    42.3	k-means++ (w=1)

k'=1024, dataset=1M SIFT
asymm   28.3    68.5    88.6    92.8    k-means++ (w=16)
asymm   21.1    42.5    47.7    48.2    k-means++ (w=1)
symm    13.9    33.9    44.5    47.8	k-means++ (w=1)

======nnSearch performance of PQ on rotated 1M SIFT=====
k'=8192, dataset=1M rotated SIFT
Top     1       10      50      100     k-means
asymm	24.7	55.9	76.0	82.2	k-means++ (w=16)
asymm	20.4	38.2	42.0	42.1	k-means++ (w=1)
symm	14.4	32.3	40.8	42.1	k-means++ (w=1)
*******************************************************/

class PQnnSearch
{
    protected:
        static const unsigned int FNLEN = 1024;
        static const unsigned int W0, topk0;
        static const float alpha;

    private:
        bool _timeon_, _rerank_, _norm_;
        bool _isivfpq_, _sdc_, __LV2__;

        unsigned int ptNum, vqNum, ftDim, seg0, pqDim, pqNum;
        unsigned int mNum, mDim, prjNum, prjDim;
        unsigned long pqseg_sz;
        unsigned long imgNum;

        int topk, topx;
        float _thresh, *qryRsd;
        unsigned int *offsets, *pqcodes;
        unsigned char *refPQCodes;   ///where to keep the reference items for non-IVF case;

        map<unsigned int, map<string, const char *>* > itmMaps;
        map<string, const char *> paras;
        vector<const char*> qryImgLst;
        list<IndexMeta *> *rankList;

        char itmTabFn[FNLEN], tmLogFn[FNLEN], rotatePCA[FNLEN];

        Hdr_InvtPQCBlock *invertTable;
        TopkHeap *knn_heap, *topk_heap;
        float *vqVocab, *pqVocab, *PQDstTab, *ADCTab;
        float *prjMat, *medians, *tmpVect;

    protected:

        ifstream *(PQnnSearch::*openMatrixFile)(const char *srcFn,  unsigned int &sz, unsigned int &dim);
        bool      (PQnnSearch::*fetchVectFrmMat)(ifstream *&inStrm, float *fvector, const int di);

        ifstream *openBINMat1(const char *srcFn, unsigned int &sz, unsigned int &dim);
        ifstream *openBINMat4(const char *srcFn, unsigned int &sz, unsigned int &dim);
        ifstream *openTXTMat(const char *srcFn, unsigned int &sz,  unsigned int &dim);

        bool fetchVectorFromTXT(ifstream *&inStrm, float *fvector,  const int di);
        bool fetchVectorFromBIN1(ifstream *&inStrm, float *fvector, const int dim);
        bool fetchVectorFromBIN4(ifstream *&inStrm, float *fvector, const int dim);


    public:
        PQnnSearch(const char *conf, const char *dt_opt);
        int init(const char *conf);

        /**NNS*/
        void performADCNNSearch(const char *srcFn,    const char* dstfn);
        void performIVFADCNNSearch(const char *srcFn, const char* dstfn);
        void performIVFSDCNNSearch(const char *srcFn, const char* dstfn);

        void performIVFADCQuery(const float *query,  const unsigned int dim);
        void performADCQuery(const float *query,     const unsigned int dim);

        void batchRun(const char *srcItms, const char *dstfn);

        /**to support asymmetric query (IVFADC)**/
        void updateIVFDstQ2P(const float *qry, const unsigned int vqi,
                         const unsigned int seg0, const unsigned int pn0,
                         const unsigned int dim0);

        /**to support asymmetric query (ADC)**/
        void updateDstQ2P(const float *qry, const unsigned int seg0,
                          const unsigned int pn0, const unsigned int dim0);

        void rerank(const unsigned int qvqi, const unsigned int seg0,
                    const unsigned int pd0, const unsigned int dim0);
        void rerank(const float *query, const unsigned int seg0,
                    const unsigned int pd0, const unsigned int dim0);

        unsigned int hardVPQSDC(const float *feat, const unsigned int dim0,
                      const unsigned int vqnum0, const unsigned int seg0);

        bool quantzPQ_l2(const float *residues, const unsigned int dim0,
                         const unsigned int seg, unsigned int codes[]);
        bool quantzPQ_cos(const float *residues, const unsigned int dim0,
                         const unsigned int seg, unsigned int codes[]);

        void nnSearchVQ_l2(const float *query, const unsigned int d0,
                              const unsigned int vqnum0);
        void nnSearchVQ_cos(const float *query, const unsigned int d0,
                              const unsigned int vqnum0);

        bool rotateSIFT(float *feat, const unsigned int dim);

        void saveRanklst(list<IndexMeta *> *rankList, const char *qryImgNm, bool selfMatch, ofstream *outStrm);
        void save(ofstream *outStrm, vector<IndexMeta*> &itms, const unsigned int ipt);
        void save(const char *dstfn, const unsigned int i_pt, bool app, const int top0);
        void save(ofstream *outStrm, const unsigned int i_pt, const int top0);

        virtual ~PQnnSearch();

        static float *calPQPrdctTab(const float *prdctVocab, const unsigned int pvqNum,
                                    const unsigned int pvkDim, const unsigned long segsz0, const unsigned int segment);

        static float *calPQDstTab(const float *prdctVocab, const unsigned int pvqNum,
                                   const unsigned int pvkDim, const unsigned long segsz0, const unsigned int segment);

        static float *loadGMMTabs(const unsigned int Num0, const unsigned int D0,
                                map<unsigned int, map<string, const char*> *> &itmMaps0);
        static void test();
};

#endif
