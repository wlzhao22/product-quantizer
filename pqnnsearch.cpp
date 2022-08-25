#include "pqnnsearch.h"

#include "abstractpquantizer.h"
#include "scriptparser.h"
#include "invtflloader.h"
#include "iodelegator.h"
#include "vstring.h"
#include "cleaner.h"
#include "pqmath.h"
#include "timer.h"

#include <iostream>
#include <cstring>
#include <cmath>

using namespace std;

const unsigned int PQnnSearch::W0    = 512; ///2^(m)
const float PQnnSearch::alpha        = 0.2f; ///power-law parameter
const unsigned int PQnnSearch::topk0 = 200;

PQnnSearch::PQnnSearch(const char *conf, const char *dt_opt)
{
    const char* taboptions[1] = {"pqctab"};

    unsigned int i = 0;
    mNum = mDim = prjNum = prjDim = 0;
    this->init(conf);
    cout<<"Search option .................... ";
    _isivfpq_ = true;
    __LV2__   = false;
    if(strcmp(dt_opt, "pqa") == 0)
    {
        cout<<"asymmetric PQ search with IVF support\n";
    }else if(strcmp(dt_opt, "pqs") == 0)
    {
        cout<<"symmetric PQ search with IVF support\n";
    }else{
        exit(0);
    }

    this->vqVocab = AbstractPQuantizer::load_VQVocab(paras["vocab"], this->ftDim, this->vqNum);
    this->pqVocab = AbstractPQuantizer::load_PQVocab(paras["vocab"], this->pqDim,
                    this->pqNum, this->seg0);

    this->pqseg_sz = this->pqNum;
    this->pqseg_sz = this->pqseg_sz*(this->pqseg_sz+1)/2;

    this->PQDstTab = NULL;
    this->refPQCodes = NULL;
    this->tmpVect  = new float[this->ftDim];

    this->PQDstTab = PQnnSearch::calPQDstTab(this->pqVocab, this->pqNum, this->pqDim, this->pqseg_sz, this->seg0);

    this->ADCTab = new float[this->pqNum*this->seg0];
    memset(this->ADCTab, 0, this->pqNum*this->seg0*sizeof(float));

    cout<<"VQ dim ........................... "<<this->ftDim<<endl;
    cout<<"VQ Size .......................... "<<this->vqNum<<endl;
    cout<<"PQ dim ........................... "<<this->pqDim<<endl;
    cout<<"PQ Size .......................... "<<this->pqNum<<endl;
    cout<<"No. of PQ dv0s ................... "<<this->seg0<<endl;

    if(paras.find("medv") != paras.end())
    {
        medians = IODelegator::loadTXTMatrix(paras["medv"], mNum, mDim);
        cout<<"Medians ......................... "<<mDim<<"x"<<mNum<<endl;
        __LV2__ = true;
    }
    else
    {
        medians = vqVocab;
        __LV2__ = false;
    }

    map<unsigned int, map<string, const char*>* >::iterator mit;
    map<string, const char*> *crntMap = NULL;

    this->pqcodes = new unsigned int[this->seg0];
    this->qryRsd  = new float[this->ftDim];

    strcpy(itmTabFn, paras["refer"]);

    itmMaps = ScriptParser::getItmMaps(itmTabFn);

    for(mit = itmMaps.begin(), i = 0; mit != itmMaps.end(); mit++)
    {
        crntMap = mit->second;
        ScriptParser::verifyParaMap(taboptions, 1, *crntMap);
        i++;
    }

    if(this->offsets == NULL)
    {
        this->offsets    = new unsigned int[int(itmMaps.size())+1];
        this->offsets[0] = 0;
    }

    cout<<"Loading data "<<i<<" bank (s) ......... "<<"succeed\n";
    cout<<"Distance measure ................. l2\n";

    if(this->paras.find("topk") != this->paras.end())
    {
        this->topk = atof(this->paras["topk"]);
    }
    else
    {
        this->topk = PQnnSearch::topk0;
    }
    cout<<"Consider ......................... ";
    cout<<"topk = "<<this->topk0<<endl;
    topk_heap = new TopkHeap(PQnnSearch::topk0, "lt");
    knn_heap  = new TopkHeap(PQnnSearch::W0, "lt");
    if(_isivfpq_)
    {
        cout<<"Consider ......................... W = "<<PQnnSearch::W0<<endl;
    }

    this->_rerank_ = false;
    cout<<"Re-ranking with source coding .... ";
    if(this->paras.find("rerank") != this->paras.end())
    {
        if(!strcmp(this->paras["rerank"], "yes"))
        {
            this->_rerank_ = true;
            cout<<"yes\n";
        }
        else
        {
            cout<<"no\n";
        }
    }
    else
    {
        cout<<"no\n";
    }

    this->invertTable = new Hdr_InvtPQCBlock[this->vqNum];

    for(unsigned int i = 0; i < this->vqNum; i++)
    {
        this->invertTable[i].Id   = i;
        this->invertTable[i].next = NULL;
    }

    unsigned long loaded = false;

    if(_isivfpq_)
    {
        loaded = InvtFLLoader::load_ivfpq_InvtF(this->invertTable, this->offsets, this->itmMaps, this->vqNum, this->seg0);
        ptNum = loaded;
    }
    else
    {
        refPQCodes = InvtFLLoader::load_pq_Codes(this->itmMaps, this->imgNum, this->ftDim);
        ptNum = this->imgNum;
    }

    cout<<"Referece size .................... "<<ptNum<<endl;

    if(this->ptNum == 0)
    {
        cerr<<"Error: no reference set has been loaded!\n";
        exit(0);
    }

    this->rankList = new list<IndexMeta*>;

    cout<<endl;
}

int PQnnSearch::init(const char *conf)
{
    const char* general_opt[3] = {"vocab", "refer", "dim"};

    cout<<"Initializing ..................... ";
    this->vqVocab = NULL;
    this->pqVocab = NULL;
    this->offsets = NULL;

    paras = ScriptParser::getConf(conf);
    ScriptParser::verifyParaMap(general_opt, 3, paras);

    cout<<"succeed\n";

    this->_norm_ = false;
    cout<<"Normalize input vector ........... ";
    if(paras.find("norm") != paras.end())
    {
        if(!strcmp(paras["norm"], "yes"))
        {
            this->_norm_ = true;
            cout<<"yes\n";
        }
        else
        {
            cout<<"no\n";
        }
    }
    else
    {
        cout<<"no\n";
    }
    cout << "Search results size .............. topk = ";
    if(paras.find("topk") != paras.end())
    {
        topx = atoi(paras["topk"]);
        topx = topx <= 0?8:topx;
        topx = topx > 512?512:topx;
    }
    else
    {
        topx = 8;
    }
    cout<< topx << "\n";

    if(paras.find("time") != paras.end())
    {
        strcpy(this->tmLogFn, paras["time"]);
        if(VString::validatePath(this->tmLogFn))
        {
            cout<<"Log on processing ................ on\n";
            this->_timeon_ = true;
        }
        else
        {
            cout<<"Log on processing ................ off\n";
            cout<<"Path of log file '"<<this->tmLogFn<<"' is invalid!\n";
            this->_timeon_ = false;
            strcpy(this->tmLogFn, "");
        }
    }
    else
    {
        cout<<"Log on processing ................ off\n";
        strcpy(this->tmLogFn, "");
        this->_timeon_ = false;
    }



    return 0;
}

float *PQnnSearch::calPQDstTab(const float *pqVocab, const unsigned int pvqNum0, const unsigned int pDim0,
                               const unsigned long segsz0, const unsigned int dv0)
{
    assert(pqVocab);

    unsigned int s = 0, i = 0, k = 0, loc = 0, loc0 = 0, j = 0, p = 0;
    const float *pi_vocab = NULL, *pj_vocab = NULL;
    float *PQdistTab0 = new float[segsz0*dv0];
    float  dst = 0, delta = 0;

    for(s = 0; s < dv0; s++)
    {
        loc = s*pvqNum0*pDim0;
        loc0 = s*segsz0;
        p = 0;
        for(i = 0; i < pvqNum0; i++)
        {
            pi_vocab = pqVocab + loc + i*pDim0;
            for(j = i; j < pvqNum0; j++)
            {
                pj_vocab = pqVocab + loc + j*pDim0;
                     dst = 0;
                for(k = 0, dst = 0; k < pDim0; k++)
                {
                    delta = pi_vocab[k] - pj_vocab[k];
                    dst += delta*delta;
                }
                PQdistTab0[loc0+p] = dst;
                p++;
            }
        }
    }
    return PQdistTab0;
}

float *PQnnSearch::calPQPrdctTab(const float *pqVocab, const unsigned int pvqNum0,
                                 const unsigned int pDim0, const unsigned long segsz0, const unsigned int dv0)
{
    assert(pqVocab);

    unsigned int s, i, k, loc = 0, loc0 = 0, j, p = 0;
    const float *pi_vocab = NULL, *pj_vocab = NULL;
    float *PQdistTab0 = new float[segsz0*dv0];
    float  dst = 0, delta = 0;

    for(s = 0; s < dv0; s++)
    {
        loc  = s*pvqNum0*pDim0;
        loc0 = s*segsz0;
        p = 0;
        cout<<s<<endl;
        for(i = 0; i < pvqNum0; i++)
        {
            pi_vocab = pqVocab + loc + i*pDim0;
            for(j = i; j < pvqNum0; j++)
            {
                pj_vocab = pqVocab + loc + j*pDim0;
                for(k = 0, dst = 0; k < pDim0; k++)
                {
                    delta  = pi_vocab[k]*pj_vocab[k];
                    dst += delta;
                }
                if(p > segsz0)
                {
                    cout<<i<<"\t"<<j<<"\t"<<p<<endl;
                    exit(0);
                }
                PQdistTab0[loc0+p] = dst;
                p++;
            }
        }
        exit(0);
    }
    return PQdistTab0;
}


void PQnnSearch::updateIVFDstQ2P(const float *qry, const unsigned int vqi, const unsigned int seg0,
                                 const unsigned int pn0, const unsigned int dim0)
{
    float *r = new float[dim0], *ppq_vocab = NULL, *ppq = NULL, dst = 0, delta = 0;
    unsigned int loc = vqi*dim0, p = 0, locs = 0, s = 0, i = 0;
    unsigned int ds  = dim0/seg0, di = 0;
    for(i = 0; i < dim0; i++)
    {
        r[i] = qry[i] - this->medians[loc+i];
    }

    for(s = 0; s < seg0; s++)
    {
        ppq_vocab = this->pqVocab + s*ds*pn0;
        locs = s*ds;
        for(i = 0; i < pn0; i++)
        {
            ppq = ppq_vocab + i*ds;
            dst = 0;
            for(di = 0; di < ds; di++)
            {
                delta = (r[locs+di] - ppq[di]);
                dst  += delta*delta;
            }
            this->ADCTab[p] = dst;
            p++;
        }
    }

    delete [] r;
    r = NULL;
    return ;
}

void PQnnSearch::updateDstQ2P(const float *qry,  const unsigned int seg0,
                              const unsigned int pn0, const unsigned int dim0)
{
    float *ppq_vocab = NULL, *ppq = NULL, dst = 0, delta = 0;
    unsigned int p = 0, locs = 0, i = 0, di = 0;
    unsigned int ds  = dim0/seg0, s = 0;

    for(s = 0; s < seg0; s++)
    {
        ppq_vocab = this->pqVocab + s*ds*pn0;
        locs = s*ds;
        for(i = 0; i < pn0; i++)
        {
            ppq = ppq_vocab + i*ds;
            dst = 0;
            for(di = 0; di < ds; di++)
            {
                delta = (qry[locs+di] - ppq[di]);
                dst+= delta*delta;
            }
            this->ADCTab[p] = dst;
            p++;
        }
    }

    return ;
}

bool PQnnSearch::fetchVectorFromTXT(ifstream *&inStrm, float *fvector, const int di)
{
    int k = 0;
    for(k = 0; k < di; k++)
    {
        (*inStrm)>>fvector[k];
    }
    return 1;
}

bool PQnnSearch::fetchVectorFromBIN1(ifstream *&inStrm, float *fvector, const int dim)
{
    int di = 0, bfsize = 0;
    char *tmp = nullptr;
    inStrm->read((char*)&di, sizeof(int));
    assert(di == dim);
    bfsize = dim*sizeof(char);
    inStrm->read((char*)tmp, bfsize);
    for(int i= 0; i < dim; i++)
    {
        fvector[i] = tmp[i];
    }
    delete [] tmp;
    tmp = NULL;
    return 1;
}

bool PQnnSearch::fetchVectorFromBIN4(ifstream *&inStrm, float *fvector, const int dim)
{
    int di = 0, bfsize = 0;
    inStrm->read((char*)&di, sizeof(int));
    assert(di == dim);
    bfsize = dim*sizeof(float);
    inStrm->read((char*)fvector, bfsize);
    return 1;
}

ifstream *PQnnSearch::openTXTMat(const char *srcfn, unsigned  int &sz, unsigned int &dim)
{
        ifstream *inStrm = new ifstream(srcfn, ios::in);
        if(!inStrm->is_open())
        {
            cout << "File " << srcfn << " cannot open!!\n";
            exit(0);
        }

        (*inStrm)>>sz;
        (*inStrm)>>dim;

        return inStrm;
}

ifstream *PQnnSearch::openBINMat1(const char *srcfn, unsigned int &sz, unsigned int &dim)
{
        ifstream *inStrm = new ifstream(srcfn, ios::in|ios::binary);
        long int bg;
        int bfsize = 0;
        if(!inStrm->is_open())
        {
            cout << "File " << srcfn << " cannot open!!\n";
            exit(0);
        }
        bg = inStrm->tellg();
        inStrm->read((char*)&dim, sizeof(int));
        bfsize = dim*sizeof(char);
        inStrm->seekg(0, ios::end);
        sz = ((long int)inStrm->tellg() - bg)/(bfsize + sizeof(int));

        inStrm->close();
        inStrm = new ifstream(srcfn, ios::in|ios::binary);

        return inStrm;
}

ifstream *PQnnSearch::openBINMat4(const char *srcfn, unsigned int &sz, unsigned int &dim)
{
    ifstream *inStrm = new ifstream(srcfn, ios::in|ios::binary);
    long int bg;
    int bfsize = 0;

    if(!inStrm->is_open())
    {
        cout << "File " << srcfn << " cannot open!!\n";
        exit(0);
    }

    bg = inStrm->tellg();
    inStrm->read((char*)&dim, sizeof(int));
    bfsize = dim*sizeof(float);
    inStrm->seekg(0, ios::end);

    sz = ((long int)inStrm->tellg() - bg)/(bfsize + sizeof(int));

    inStrm->close();

    inStrm = new ifstream(srcfn, ios::in|ios::binary);

    return inStrm;
}

void PQnnSearch::performIVFSDCNNSearch(const char *srcfn, const char* dstfn)
{
    unsigned int vnum   = 0, fsize = this->ftDim*sizeof(float);
    unsigned int idxq   = 0, idxr, s, k, prm_vqi = 0, di = 0, idxc = 0, bufsz = 0;
    unsigned int tmpidx = 0, si = 0, sj = 0;
    InvtPQCBlock *blckIter = NULL, *crntBlck = NULL;
    InvtPQCUnit  *crntUnit = NULL;
    unsigned short *ptcodes = NULL;
    float *feat = NULL, dist  = 0;
    bufsz  = this->seg0*sizeof(unsigned int);
    unsigned int sz = 0;
    ifstream *inStrm = NULL;

    if(VString::endWith(srcfn, ".txt"))
    {
            openMatrixFile  =  & PQnnSearch::openTXTMat;
            fetchVectFrmMat =  & PQnnSearch::fetchVectorFromTXT;
    }else if(VString::endWith(srcfn, ".bvecs"))
    {
            openMatrixFile  =  & PQnnSearch::openBINMat1;
            fetchVectFrmMat =  & PQnnSearch::fetchVectorFromBIN1;
    }else if(VString::endWith(srcfn, ".fvecs"))
    {
            openMatrixFile  =  & PQnnSearch::openBINMat4;
            fetchVectFrmMat =  & PQnnSearch::fetchVectorFromBIN4;
    }else{
        cout << "Error: unrecognizable format!!\n";
        exit(0);
    }
    inStrm = (this->*openMatrixFile)(srcfn, sz, di);

    assert(di == this->ftDim);
    feat   = new float[this->ftDim];

    cout<<"Perform symmetric NN search ....... \n";
    ofstream *outStrm = new ofstream(dstfn, ios::out);
    if(!outStrm->is_open())
    {
        cout<<"File '"<<dstfn<<"' cannot open for writing!\n";
        exit(0);
    }
    this->save(outStrm, vnum+1, topx);

    while(!inStrm->eof() && vnum < sz)
    {
 
        (this->*fetchVectFrmMat)(inStrm, feat, di);

        memset(this->pqcodes, 0 , bufsz);
        idxc = 0;

        if(this->_norm_)
        {
            PQMath::l2_norm(feat, this->ftDim);
        }

        prm_vqi = this->hardVPQSDC(feat, this->ftDim, this->vqNum, this->seg0);
        Hdr_InvtPQCBlock &crnt_headerpt = this->invertTable[prm_vqi];
        blckIter = crnt_headerpt.next;
        while(blckIter != NULL)
        {
            crntBlck = blckIter;
            for(k = 0; k < crntBlck->uIdx; k++)
            {
                crntUnit = (*crntBlck)[k];
                dist = 0;
                ptcodes = (unsigned short*)&(crntUnit->pqc);

                for(s = 0; s < this->seg0; s++)
                {
                    idxq = this->pqcodes[s];
                    idxr = (*ptcodes);
                    if(idxq != idxr)
                    {
                        sj = idxq>idxr?idxq:idxr;
                        si = idxq<idxr?idxq:idxr;
                        ///calc. Euclidean distance
                        tmpidx = s*this->pqseg_sz + this->pqNum*si + sj - ((si+1)*si)/2;
                        dist  += this->PQDstTab[tmpidx];
                    }///if(idxq)
                    ptcodes++;
                }///for(s)
                topk_heap->insert(crntUnit->Id, dist);

            }///for(k)
            blckIter = crntBlck->next;
        }///while(blckIter)
        idxc += (this->seg0+1);

        /**
        ///re-ranking, recover the query
        if(this->_rerank_)
        {
            this->rerank(prm_vqi, this->seg0, this->pqDim, this->ftDim);
        }
        /**/

        this->save(outStrm, vnum+1, topx);
        vnum++;
        cout<<"\r\r\r\r\t"<<vnum;
        topk_heap->makeEmpty();
        knn_heap->makeEmpty();
        memset(feat, 0, fsize);
    }///while(inStrm)

    inStrm->close();
    outStrm->close();
    delete [] feat;
    cout<<endl;
    return ;
}

void PQnnSearch::performIVFADCNNSearch(const char *srcfn, const char* dstfn)
{
    unsigned int vnum = 0, fsize = this->ftDim*sizeof(float), m = 0;
    unsigned int idxr, s, k, vqi, di = 0, bufsz, i = 0, tmpidx = 0;
    InvtPQCBlock *blckIter  = NULL, *crntBlck = NULL;
    InvtPQCUnit  *crntUnit  = NULL;
    unsigned char *ptcodes = NULL;
    float *query = NULL, dist = 0;
    bufsz = this->seg0*sizeof(unsigned int);
    vector<IndexItem*>::iterator it;
    IndexItem *crntItm = NULL;
    ifstream *inStrm = NULL;
    unsigned int sz = 0;

    if(VString::endWith(srcfn, ".txt"))
    {
            openMatrixFile  =  & PQnnSearch::openTXTMat;
            fetchVectFrmMat =  & PQnnSearch::fetchVectorFromTXT;
    }else if(VString::endWith(srcfn, ".bvecs"))
    {
            openMatrixFile  =  & PQnnSearch::openBINMat1;
            fetchVectFrmMat =  & PQnnSearch::fetchVectorFromBIN1;
    }else if(VString::endWith(srcfn, ".fvecs"))
    {
            openMatrixFile  =  & PQnnSearch::openBINMat4;
            fetchVectFrmMat =  & PQnnSearch::fetchVectorFromBIN4;
    }else{
        cout << "Error: unrecognizable format!!\n";
        exit(0);
    }
    inStrm = (this->*openMatrixFile)(srcfn, sz, di);

    cout<<sz<<"\t"<<this->ftDim<<endl;
    assert(di == this->ftDim);
    query = new float[this->ftDim];

    cout<<"Perform asymmetric NN search ....... \n";
    ofstream *outStrm = new ofstream(dstfn, ios::out);
    if(!outStrm->is_open())
    {
        cout<<"File '"<<dstfn<<"' cannot open for writing!\n";
        exit(0);
    }
    this->save(outStrm, vnum+1, topx);

    while(!inStrm->eof() && vnum < sz)
    {

        (this->*fetchVectFrmMat)(inStrm,  query, di);

        if(this->_norm_)
        {
            PQMath::l2_norm(query, this->ftDim);
        }
        memset(this->pqcodes, 0, bufsz);
        this->nnSearchVQ_l2(query, this->ftDim, this->vqNum);

        for(it = knn_heap->begin(), i = 0; it != knn_heap->end(); it++, i++)
        {
             crntItm = *it;
                 vqi = crntItm->index;
            Hdr_InvtPQCBlock & crnt_headerpt = this->invertTable[vqi];
            blckIter = crnt_headerpt.next;
            this->updateIVFDstQ2P(query, vqi, this->seg0, this->pqNum, this->ftDim);

            while(blckIter != NULL)
            {
                crntBlck = blckIter;
                for(k = 0; k < crntBlck->uIdx; k++)
                {
                           dist = 0;
                       crntUnit = (*crntBlck)[k];
                        ptcodes = crntUnit->pqc;
                        for(s = 0; s < this->seg0; s++)
                        {
                              idxr = (*ptcodes);
                            tmpidx = s*this->pqNum + idxr;
                             dist += this->ADCTab[tmpidx];
                            ptcodes++;
                        }///for(s)
                    topk_heap->insert(crntUnit->Id, dist);
                }///for(k)
                blckIter = crntBlck->next;
            }///while(blckIter)
        }///for(it)

        ///re-ranking, recover the query,
        /**
        if(this->_rerank_)
            this->rerank(query, this->seg0, this->pqDim, this->ftDim);
        /**/

        this->save(outStrm, vnum+1, topx);
        vnum++;
        cout<<"\r\r\r\r\t"<<vnum;

        topk_heap->makeEmpty();
        knn_heap->makeEmpty();
        memset(query, 0, fsize);
    }///while(inStrm)

    inStrm->close();
    outStrm->close();
    delete [] query;
    query = NULL;
    cout<<endl;
    return ;
}

void PQnnSearch::performADCNNSearch(const char *srcfn, const char* dstfn)
{
    unsigned int vnum = 0, fsize = this->ftDim*sizeof(float);
    unsigned int idxr = 0, s = 0, k = 0, ri = 0, di = 0, bufsz, tmpidx = 0;
    unsigned char *ptcodes = NULL;
    float *query = NULL, dist = 0;
    bufsz = this->seg0*sizeof(unsigned int);
    unsigned int sz = 0;
    ifstream *inStrm = NULL;

    cout << "search start .... \n";

    if(VString::endWith(srcfn, ".txt"))
    {
            openMatrixFile  =  & PQnnSearch::openTXTMat;
            fetchVectFrmMat =  & PQnnSearch::fetchVectorFromTXT;
    }else if(VString::endWith(srcfn, ".bvecs"))
    {
            openMatrixFile  =  & PQnnSearch::openBINMat1;
            fetchVectFrmMat =  & PQnnSearch::fetchVectorFromBIN1;
    }else if(VString::endWith(srcfn, ".fvecs"))
    {
            openMatrixFile  =  & PQnnSearch::openBINMat4;
            fetchVectFrmMat =  & PQnnSearch::fetchVectorFromBIN4;
    }else{
        cout << "Error: unrecognizable format!!\n";
        exit(0);
    }
    inStrm = (this->*openMatrixFile)(srcfn, sz, di);

    assert(di == this->ftDim);
    query = new float[this->ftDim];

    cout<<"Perform asymmetric NN search ...... \n";
    ofstream *outStrm = new ofstream(dstfn, ios::out);
    if(!outStrm->is_open())
    {
        cout<<"File '"<<dstfn<<"' cannot open for writing!\n";
        exit(0);
    }
    this->save(outStrm, vnum+1, topx);

    while(!inStrm->eof() && vnum < sz)
    {

        (this->*fetchVectFrmMat)(inStrm,  query, di);

        if(this->_norm_)
        {
            PQMath::l2_norm(query, this->ftDim);
        }

        this->updateDstQ2P(query, this->seg0, this->pqNum, this->ftDim);

        memset(this->pqcodes, 0, bufsz);

        for(ri = 0; ri < this->imgNum; ri++)
        {
            dist = 0;
            k = ri*this->seg0;
            ptcodes = &(refPQCodes[k]);

            for(s = 0; s < this->seg0; s++)
            {
                idxr = (*ptcodes);
                tmpidx = s*this->pqNum + idxr;
                dist += this->ADCTab[tmpidx];
                ptcodes++;
            }///for(s)
            topk_heap->insert(ri, dist);
        }

        this->save(outStrm, vnum+1, topx);
        vnum++;
        cout<<"\r\r\r\r\t"<<vnum;

        topk_heap->makeEmpty();
        memset(query, 0, fsize);
    }///while(inStrm)

    outStrm->close();
    inStrm->close();
    delete [] query;
    query = NULL;
    cout<<endl;
    return ;
}

void PQnnSearch::performIVFADCQuery(const float *query, const unsigned int dim)
{
    unsigned int idxr = 0, s = 0, k = 0, vqi = 0, bufsz = 0, i = 0, tmpidx = 0;
    InvtPQCBlock *blckIter  = NULL, *crntBlck = NULL;
    InvtPQCUnit  *crntUnit  = NULL;
    unsigned char *ptcodes = NULL;
    float dist = 0;

    bufsz = this->seg0*sizeof(unsigned int);
    vector<IndexItem*>::iterator it;
    IndexItem *crntItm = NULL;
    IndexMeta *crntMeta = NULL;
    memset(this->pqcodes, 0, bufsz);

    this->nnSearchVQ_l2(query, this->ftDim, this->vqNum);
    for(it = knn_heap->begin(), i = 0; it != knn_heap->end(); it++, i++)
    {
        crntItm = *it;
        vqi = crntItm->index;
        Hdr_InvtPQCBlock & crnt_headerpt = this->invertTable[vqi];
        blckIter = crnt_headerpt.next;

        if(blckIter == NULL)
            continue;

        this->updateIVFDstQ2P(query, vqi, this->seg0, this->pqNum, this->ftDim);
        while(blckIter != NULL)
        {
            crntBlck = blckIter;

            for(k = 0; k < crntBlck->uIdx; k++)
            {
                dist = 0;
                crntUnit = (*crntBlck)[k];
                    ptcodes = crntUnit->pqc;
                    for(s = 0; s < this->seg0; s++)
                    {
                          idxr = *ptcodes;
                        tmpidx = s*this->pqNum + idxr;
                         dist += this->ADCTab[tmpidx];
                        ptcodes++;
                    }///for(s)

                crntMeta = new IndexMeta(0, dist);
                crntMeta->pp = crntUnit->Id;
                this->rankList->push_back(crntMeta);
                /**/
            }///for(k)
            blckIter = crntBlck->next;
        }///while(blckIter)
    }///for(it)

    return ;
}

void PQnnSearch::performADCQuery(const float *query, const unsigned int dim)
{
    unsigned int idxr = 0, s = 0, k = 0, ri = 0, tmpidx = 0;
    unsigned char *ptcodes = NULL;
    IndexMeta *crntMeta = NULL;
    float dist = 0;

    this->updateDstQ2P(query, this->seg0, this->pqNum, this->ftDim);

    for(ri = 0; ri < this->imgNum; ri++)
    {
        dist = 0;
        k = ri*this->seg0;
        ptcodes = &(refPQCodes[k]);

        for(s = 0; s < this->seg0; s++)
        {
            idxr = (*ptcodes);
            tmpidx = s*this->pqNum + idxr;
            dist += this->ADCTab[tmpidx];
            ptcodes++;
        }///for(s)
        crntMeta = new IndexMeta(0, dist);
        crntMeta->pp = ri + 1;
        this->rankList->push_back(crntMeta);
    }
    return ;
}

unsigned int PQnnSearch::hardVPQSDC(const float *query, const unsigned int dim0,
                                    const unsigned int vqnum0, const unsigned int seg0)
{
    vector<IndexItem*>::iterator it;
    IndexItem *crntItm = NULL;
    assert(this->pqVocab);
    assert(this->vqVocab);
    assert(query);

    this->nnSearchVQ_l2(query, dim0, vqnum0);
    unsigned int i = 0, loc = 0, vqi = 0;
    memset(this->pqcodes, 0, sizeof(unsigned int)*(this->seg0));

    for(it = knn_heap->begin(); it != knn_heap->end(); it++)
    {
        crntItm  = *it;
        vqi       = crntItm->index;
        loc       = crntItm->index*dim0;

        for(i = 0; i < dim0; i++)
        {
            this->qryRsd[i] = query[i] - this->vqVocab[loc+i];
        }
        this->quantzPQ_l2(this->qryRsd, dim0, this->seg0, this->pqcodes);
        break;
    }

    return vqi;
}

void PQnnSearch::nnSearchVQ_l2(const float *query, const unsigned int d0, const unsigned int vqnum0)
{
    float dist = 0;
    knn_heap->makeEmpty();
    for(unsigned int i = 0; i < vqnum0; i++)
    {
        dist = PQMath::l2(query,  0, this->vqVocab, i, d0);
        knn_heap->insert(i, dist, 0, 0, 0, 0);
    }

    knn_heap->sort();

    return ;
}

void PQnnSearch::nnSearchVQ_cos(const float *query, const unsigned int d0,
                                const unsigned int vqnum0)
{
    unsigned int i = 0;
    float dist = 0;
    knn_heap->makeEmpty();
    for(i = 0; i < vqnum0; i++)
    {
        dist = PQMath::cos(query, 0, this->vqVocab, i, d0)+1;
        knn_heap->insert(i, 2-dist, 0, 0, 0, 0);
    }

    return ;
}

bool PQnnSearch::quantzPQ_l2(const float *residues, const unsigned int dim0,
                             const unsigned int seg, unsigned int codes[])
{
    assert(this->pqVocab);
    assert(residues);
    assert(codes);

    unsigned int j = 0, nnIdx = 0, s = 0, k = 0;
    unsigned int loc1 = 0, loc2 = 0, locv = 0;
    double dst = 0, mdst = 0, w1 = 0;
    double *dsts = new double[seg];
    for(s = 0; s < seg; s++)
    {
        mdst  = RAND_MAX;
        nnIdx = -1;
        loc1  = s*this->pqDim;
        locv  = this->pqDim*this->pqNum*s;

        for(j = 0; j < this->pqNum; j++)
        {
            loc2  = locv + this->pqDim*j;
            dst   = 0;
            for(k = 0; k < this->pqDim; k++)
            {
                w1   = residues[loc1+k] - this->pqVocab[loc2+k];
                dst += w1*w1;
            }
            dst = sqrt(dst);
            if(dst < mdst)
            {
                nnIdx = j;
                mdst  = dst;
            }
        }
        dsts[s]  = mdst;
        codes[s] = nnIdx;
    }///for(s)

    delete [] dsts;

    return true;
}

bool PQnnSearch::quantzPQ_cos(const float *residues, const unsigned int dim0,
                              const unsigned int seg, unsigned int codes[])
{
    assert(this->pqVocab);
    assert(residues);
    assert(codes);

    unsigned int j = 0, nnIdx = 0, s = 0, k = 0;
    double sim = 0, mSim = 0, w1 = 0, w2 = 0, w = 0;
    unsigned int loc1 = 0, loc2 = 0, loc_v = 0;
    double *dsts = new double[seg];
    for(s = 0; s < seg; s++)
    {
        mSim   = -3;
        nnIdx  = 0;
        loc1   = s*this->pqDim;
        loc_v  = this->pqDim*this->pqNum*s;

        for(j = 0; j < this->pqNum; j++)
        {
            loc2 = loc_v + this->pqDim*j;
            sim  = 0;
            w1   = w2 = w = 0;
            for(k = 0; k < this->pqDim; k++)
            {
                w1 += residues[loc1+k]*residues[loc1+k];
                w2 += this->pqVocab[loc2+k]*this->pqVocab[loc2+k];
                w  += residues[loc1+k]*this->pqVocab[loc2+k];
            }
            w1 = sqrt(w1);
            w2 = sqrt(w2);
            if(w1 == 0 && w2 != 0)
            {
                sim = w2/2;
            }
            else if(w1 != 0 && w2 == 0)
            {
                sim = w1/2;
            }
            else
            {
                sim = w/(w1*w2);
            }
            if(sim > mSim)
            {
                nnIdx = j;
                mSim  = sim;
            }
        }
        dsts[s]  = mSim;
        codes[s] = nnIdx;
    }///for(s)

    delete [] dsts;

    return true;
}


void PQnnSearch::rerank(const unsigned int qvqi, const unsigned int seg0,
                        const unsigned int pd0,  const unsigned int dim0)
{
    vector<IndexItem*>::iterator it;
    IndexItem *crntItm = NULL;
    unsigned int vloc = 0, qloc = 0, i = 0, ploc = 0, s = 0, d = 0;
    float *data  = new float[dim0], dist = 0;
    float *query = new float[dim0];
    unsigned char *ptcodes = NULL;
    assert(this->pqVocab);
    assert(this->vqVocab);

    qloc = dim0*qvqi;
    memcpy(query, this->vqVocab+qloc, sizeof(float)*dim0);
    for(s = i = 0; s < seg0; s++)
    {
        ploc  = s*this->pqNum*pd0 + this->pqcodes[s]*pd0;
        for(d = 0; d < pd0; d++,i++)
        {
            query[i] = query[i] + this->pqVocab[ploc+d];
        }
    }

    for(it = topk_heap->begin(); it != topk_heap->end(); it++)
    {
        crntItm  = *it;
        vloc     = crntItm->vkid*dim0;
        ptcodes  = (unsigned char*)&(crntItm->pcode);
        memcpy(data, this->vqVocab+vloc, sizeof(float)*dim0);
        for(s = i = 0; s < seg0; s++)
        {
            ploc  = s*this->pqNum*pd0 + ptcodes[s]*pd0;
            for(d = 0; d < pd0; d++, i++)
            {
                data[i] = data[i] + this->pqVocab[ploc+d];
            }
        }
        crntItm->val = 0;
        for(i = 0; i < dim0; i++)
        {
            dist          = query[i] - data[i];
            crntItm->val += dist*dist;
        }
    }
    topk_heap->sort();

    delete [] data;
    delete [] query;

    return ;
}

void PQnnSearch::rerank(const float *query, const unsigned int seg0,
                        const unsigned int pd0, const unsigned int dim0)
{
    vector<IndexItem*>::iterator it;
    IndexItem *crntItm = NULL;
    unsigned int vloc = 0, i = 0, ploc, s, d;
    float *data = new float[dim0], dist = 0;
    unsigned char *ptcodes = NULL;
    assert(this->pqVocab);
    assert(this->vqVocab);
    assert(query);

    for(it = topk_heap->begin(); it != topk_heap->end(); it++)
    {
        crntItm = *it;
        vloc    = crntItm->vkid*dim0;
        ptcodes = (unsigned char*)&(crntItm->pcode);
        memcpy(data, this->vqVocab+vloc, sizeof(float)*dim0);
        i = 0;
        for(s = 0; s < seg0; s++)
        {
            ploc = s*this->pqNum*pd0 + ptcodes[s]*pd0;
            for(d = 0; d < pd0; d++, i++)
            {
                data[i] = data[i] + this->pqVocab[ploc+d];
            }
        }
        crntItm->dc  = crntItm->val;
        crntItm->val = 0;
        for(i = 0; i < dim0; i++)
        {
            dist          = query[i] - data[i];
            crntItm->val += dist*dist;
        }
        crntItm->val = sqrt(crntItm->val);
    }
    topk_heap->sort();

    delete [] data;

    return ;
}

void PQnnSearch::save(const char *dstfn, const unsigned int ipt, bool app, const int top0)
{
    vector<IndexItem*>::iterator it;
    IndexItem* crntItm = NULL;
    ofstream *outStrm  = NULL;
    if(app)
    {
        outStrm = new ofstream(dstfn, ios::app);
    }
    else
    {
        outStrm = new ofstream(dstfn, ios::out);
    }

    if(!outStrm->is_open())
    {
        return ;
    }
    int i = 0;
    topk_heap->sort();
    for(it = topk_heap->begin(); it != topk_heap->end() && i < top0; it++, i++)
    {
        crntItm = *it;
        (*outStrm)<<ipt-1<<"\t"<<crntItm->index-1<<"\t"<<crntItm->val<<endl;
    }
    topk_heap->makeEmpty();
    outStrm->close();
    return ;
}

void PQnnSearch::save(ofstream *outStrm, const unsigned int ipt, const int top0)
{
    vector<IndexItem*>::iterator it;
    IndexItem* crntItm = NULL;
    if(!outStrm->is_open())
    {
        return ;
    }
    int i = 0;
    topk_heap->sort();
    for(it = topk_heap->begin(); it != topk_heap->end() && i < top0; it++, i++)
    {
        crntItm = *it;
        (*outStrm)<<ipt-1<<"\t"<<crntItm->index-1<<"\t"<<crntItm->val<<endl;
    }
    topk_heap->makeEmpty();
    return ;
}

void PQnnSearch::save(ofstream *outStrm, vector<IndexMeta*> &itms, const unsigned int ipt)
{
    vector<IndexMeta*>::iterator it;
    IndexMeta* crntItm = NULL;
    if(!outStrm->is_open())
    {
        return ;
    }
    ///stable_sort(itms.begin(), itms.end(), IndexMeta::LLcomparer);
    unsigned int top = 0;

    for(it = itms.begin(); it != itms.end(); it++, top++)
    {
        crntItm = *it;
        if(crntItm->Id < crntItm->pp)
            (*outStrm)<<crntItm->Id<<"\t"<<crntItm->pp<<"\t"<<crntItm->wgh<<endl;
    }
    return ;
}

float *PQnnSearch::loadGMMTabs(const unsigned int Num0, const unsigned int D0,
                               map<unsigned int, map<string, const char*> *> &itmMaps0)
{
    map<unsigned int, map<string, const char*> *>::iterator mit;
    map<string, const char*> *crntMap = NULL;
    float *gmmVects = NULL;
    unsigned int num = 0;

    for(mit = itmMaps0.begin(); mit != itmMaps0.end(); mit++)
    {
        crntMap = mit->second;
        if(crntMap->find("gmmtab") != crntMap->end())
        {
            if(VString::existFile((*crntMap)["gmmtab"]))
            {
                num++;
            }
        }
    }///for(mit)

    if(num < itmMaps0.size())
    {
        cout<<"not fulfilled!\n";
        return NULL;
    }

    gmmVects = new float[Num0*D0];
    memset(gmmVects, 0, sizeof(float)*Num0*D0);
    unsigned int tmpNum = 0, tmpDim = 0, i = 0, j = 0, loc = 0;
    float tmpVal = 0;
    for(mit = itmMaps0.begin(); mit != itmMaps0.end(); mit++)
    {
        crntMap = mit->second;
        ifstream *inStrm = new ifstream((*crntMap)["gmmtab"], ios::in);
        (*inStrm)>>tmpNum;
        (*inStrm)>>tmpDim;
        assert(tmpDim == D0);
        assert(tmpNum == Num0);
        for(i = 0; i < tmpNum; i++)
        {
            loc = i*tmpDim;
            for(j = 0; j < tmpDim; j++)
            {
                (*inStrm)>>tmpVal;
                gmmVects[loc+j] += tmpVal;
            }
        }///for(i)
        inStrm->close();
    }///for(mit)


    for(i = 0; i < tmpNum; i++)
    {
        loc = i*D0;
        PQMath::powerLaw(gmmVects+loc, alpha, D0);
    }///for(i)

    cout<<num<<endl;
    return gmmVects;
}

PQnnSearch::~PQnnSearch()
{
    if(offsets != NULL)
    {
        delete [] offsets;
        offsets = nullptr;
    }

    if(this->vqVocab != NULL)
    {
        delete [] this->vqVocab;
        this->vqVocab = NULL;
    }

    if(pqVocab != NULL)
    {
        delete [] this->pqVocab;
        this->pqVocab = NULL;
    }

    if(__LV2__&& this->medians != NULL)
    {
        delete [] this->medians;
        this->medians = nullptr;
    }

    if(this->prjMat != NULL)
    {
        delete [] this->prjMat;
        this->prjMat = nullptr;
    }

    if(this->PQDstTab != NULL)
    {
        delete [] this->PQDstTab;
        this->PQDstTab = NULL;
    }

    if(this->ADCTab != NULL)
    {
        delete [] this->ADCTab;
        this->ADCTab = NULL;
    }

    cout<<"Release Inverted List ....... ";
    Cleaner::releaseInvtList(invertTable, this->vqNum);
    cout<<"done\n";

    if(knn_heap != NULL)
    {
        this->knn_heap->makeEmpty();
        delete this->knn_heap;
        this->knn_heap = NULL;
    }

    if(this->topk_heap != NULL)
    {
        this->topk_heap->makeEmpty();
        delete this->topk_heap;
        this->topk_heap = NULL;
    }

    if(this->pqcodes != NULL)
    {
        delete [] this->pqcodes;
        this->pqcodes = NULL;
    }

    if(this->refPQCodes != NULL)
    {
        delete [] this->refPQCodes;
        this->refPQCodes = NULL;
    }

    if(this->qryRsd != NULL)
    {
        delete [] this->qryRsd;
        this->qryRsd = NULL;
    }

    if(this->tmpVect != NULL)
    {
        delete [] this->tmpVect;
        this->tmpVect = NULL;
    }

    ScriptParser::clearParaMap(this->paras);

}

void PQnnSearch::test()
{
    const char *conf    = "/home/wlzhao/c/src/pq/etc/pq-nc.conf";
    const char *query   = "/home/wlzhao/c/src/pq/etc/sift_qry.txt";
    const char *result1 = "/home/wlzhao/c/src/pq/etc/sift_rslt.txt";

    PQnnSearch *myret = new PQnnSearch(conf, "ivfpq");
    Timer *mytm = new Timer();
    myret->performIVFADCNNSearch(query, result1);
    ///myret->performIVFSDCNNSearch(query2, result2);

    mytm->end(true);

    return;
}
