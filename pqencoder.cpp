#include "pqencoder.h"

#include "scriptparser.h"
#include "iodelegator.h"
#include "vstring.h"
#include "pqmath.h"
#include "timer.h"

#include <dirent.h>
#include <fstream>
#include <cstring>
#include <cassert>
#include <cmath>
#include <map>

const int AbstractPQuantizer::verbose = 1;

PQEncoder::PQEncoder(const char *conf, const char *opt)
{
    cout<<"Qunatization Option ............. PQ\n";

    this->init(conf);
    this->vqVocab = NULL;
    this->medians = NULL;
    mNum = mDim = 0;
    _isivfpq_ = false;
    __LV2__   = false;

    if(!strcmp(opt, "ivfpq"))
    {
        if(paras.find("vocab") != paras.end())
        {
            cout<<"Loading Visual Vocabulary ....... ";
            this->vqVocab = AbstractPQuantizer::load_VQVocab(paras["vocab"], this->ftDim, this->vqNum);
            assert(this->vqVocab);
            cout<<this->vqNum<<"x"<<this->ftDim<<endl;
        }
        _isivfpq_ = true;
    }

    cout<<"Loading Product quantizer ....... ";
    this->pqVocab = AbstractPQuantizer::load_PQVocab(paras["vocab"], this->pqDim, this->pqNum,  this->dv0);
    assert(this->pqVocab);
    cout<<this->pqNum<<": "<<this->pqDim<<"x"<<this->dv0<<endl;

    if(this->pqDim == 0 || this->pqNum == 0)
    {
        cout<<"Initlizing vocabularies failed!\n";
        exit(0);
    }

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

    scale = -1;
    cout<<"Encoding scale .................. ";
    if(paras.find("scale") != paras.end())
    {
        scale = atoi(paras["scale"]);
        cout<<scale<<endl;
    }
    else
    {
        cout<<"all\n";
    }

    /********initialize for nnSearch ***************/
    cout<<"VQ dim .......................... "<<this->ftDim<<endl;
    cout<<"VQ Size ......................... "<<this->vqNum<<endl;
    cout<<"PQ dim .......................... "<<this->pqDim<<endl;
    cout<<"PQ Size ......................... "<<this->pqNum<<endl;

    this->init_nnSearch(this->paras);

    if(paras.find("time") != paras.end())
    {
        strcpy(this->tmLogFn, paras["time"]);
        if(VString::validatePath(this->tmLogFn))
        {
            cout<<"Log on processing ............... on\n";
            this->_timeon_ = true;
        }
        else
        {
            cout<<"Log on processing ............... off\n";
            cout<<"Path of log file '"<<this->tmLogFn<<"' is invalid!\n";
            this->_timeon_ = false;
            strcpy(this->tmLogFn, "");
        }
    }
    else
    {
        cout<<"Log on processing ............... off\n";
        strcpy(this->tmLogFn, "");
        this->_timeon_ = false;
    }
}

int PQEncoder::init(const char *conf)
{
    const char* options[1] = {"vocab"};
    unsigned int i = 0;

    cout<<"Initializing .................... ";
    this->vqVocab  = NULL;
    this->medians  = NULL;

    paras = ScriptParser::getConf(conf);
    cout<<"succeed\n";

    for(i = 0; i < 1; i++)
    {
        if(paras.find(options[i]) == paras.end())
        {
            cout<<"\nOption '"<<options[i]<<"' is missing!\n";
            exit(1);
        }
    }

    ///this->ftDim  = atoi(paras["dim"]);
    strcpy(vocabfn, paras["vocab"]);

    this->_norm_ = false;
    cout<<"Normalize input vector .......... ";
    if(paras.find("norm") != paras.end())
    {
        if(!strcmp(paras["norm"], "yes"))
        {
            this->_norm_ = true;
            cout<<"yes\n";
        }
        else
        {
            this->_norm_ = false;
            cout<<"no\n";
        }
    }
    else
    {
        cout<<"no\n";
    }

    return 0;
}

bool PQEncoder::fetchVectorFromTXT(ifstream *&inStrm, float *fvector, const int di)
{
    int k = 0;
    for(k = 0; k < di; k++)
    {
        (*inStrm)>>fvector[k];
    }
    return 1;
}

bool PQEncoder::fetchVectorFromBIN1(ifstream *&inStrm, float *fvector, const int dim)
{
    int di = 0, bfsize = 0;
    char *tmpVector = nullptr;
    inStrm->read((char*)&di, sizeof(int));
    assert(di == dim);
    bfsize = dim*sizeof(char);
    inStrm->read((char*)tmpVector, bfsize);
    for(int i= 0; i < dim; i++)
    {
        fvector[i] = tmpVector[i];
    }
    delete [] tmpVector;
    tmpVector = NULL;
    return 1;
}

bool PQEncoder::fetchVectorFromBIN4(ifstream *&inStrm, float *fvector, const int dim)
{
    int di = 0, bfsize = 0;
    inStrm->read((char*)&di, sizeof(int));
    assert(di == dim);
    bfsize = dim*sizeof(float);
    inStrm->read((char*)fvector, bfsize);
    return 1;
}

ifstream *PQEncoder::openTXTMat(const char *srcfn, unsigned  int &sz, unsigned int &dim)
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

ifstream *PQEncoder::openBINMat1(const char *srcfn, unsigned int &sz, unsigned int &dim)
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

ifstream *PQEncoder::openBINMat4(const char *srcfn, unsigned int &sz, unsigned int &dim)
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

bool PQEncoder::performPQEncodeMat(const char *itmFn)
{
    const char *options[2] = {"pqctab", "vectab"};
    char srcfn[512];

    map<unsigned int, map<string, const char*>* > itmMaps;
    map<string, const char*>* crntMap;
    ifstream *inStrm  = NULL;
    ofstream *pqcStrm = NULL;

    unsigned int vnum = 0, i, di = 0, sz = 0, k = 0, idx = 0, sc = 0;
    float *feat = new float[this->ftDim];
    float el2 = 0;
    unsigned int *pqcodes = new unsigned int[this->dv0 + 1];

    cout<<"Items to be encoded ............. ";
    itmMaps = ScriptParser::getItmMaps(itmFn);
    cout<<itmMaps.size()<<endl;

    for(idx = 0; idx < itmMaps.size(); idx++)
    {
        crntMap = itmMaps[idx];
        if(!ScriptParser::verifyParaMap(options, 2, *crntMap))
        {
            exit(0);
        }

        strcpy(srcfn, (*crntMap)["vectab"]);

        if(!VString::existFile(srcfn))
        {
            cout<<"Source file "<<srcfn<<" cannot open!\n";
            exit(0);
        }

        if(VString::endWith(srcfn, ".txt"))
        {
                openMatrixFile  =  & PQEncoder::openTXTMat;
                fetchVectFrmMat =  & PQEncoder::fetchVectorFromTXT;
        }else if(VString::endWith(srcfn, ".bvecs"))
        {
                openMatrixFile  =  & PQEncoder::openBINMat1;
                fetchVectFrmMat =  & PQEncoder::fetchVectorFromBIN1;
        }else if(VString::endWith(srcfn, ".fvecs"))
        {
                openMatrixFile  =  & PQEncoder::openBINMat4;
                fetchVectFrmMat =  & PQEncoder::fetchVectorFromBIN4;
        }else{
            cout << "Error: unrecognizable format!!\n";
            exit(0);
        }

        pqcStrm = new ofstream((*crntMap)["pqctab"], ios::out);
        if(!pqcStrm->is_open())
        {
            cout<<"File '"<<(*crntMap)["pqctab"]<<"' cannot open for writing!\n";
            pqcStrm->close();
            exit(0);
        }

        cout<<"Encode vectors into PQ .......... '"<<srcfn<<"'\n";
        inStrm = (this->*openMatrixFile)(srcfn, sz, di);
        
        cout<<"Sizes ........................... "<<sz<<"x"<<di<<endl;
        assert(di == this->ftDim);
        vnum = 0;
        sc = (scale == -1)?sz:scale;
        while(!inStrm->eof() && vnum < sc)
        {
            (this->*fetchVectFrmMat)(inStrm, feat, di);
            
            if(this->_norm_)
            {
                PQMath::l2_norm(feat, this->ftDim);
            }
            if(_isivfpq_)
            {
                el2 = this->quantzIVFPQ(feat, this->ftDim, this->dv0, pqcodes);
                (*pqcStrm)<<pqcodes[0]<<" ";
            }
            else
            {
                el2 = this->quantzPQ(feat, this->ftDim, this->dv0, pqcodes);
                (*pqcStrm)<<" ";
            }
            for(i = 0; i < this->dv0; i++)
            {
                (*pqcStrm)<<pqcodes[i+1]<<" ";
            }
            (*pqcStrm)<<endl;
            memset(feat, 0, sizeof(float)*this->ftDim);
            vnum++;
            printf("\r\r\t%6d", vnum);
        }
        inStrm->close();
        pqcStrm->close();
        cout<<endl;
    }///for(idx)

    cout<<endl;
    ScriptParser::clearItmMaps(itmMaps);

    delete [] feat;
    delete [] pqcodes;
    pqcodes = NULL;
    feat = NULL;

    return true;
}

bool PQEncoder::performPQEncodeMat(const char *srcfn, const char *dstFn)
{

    if(!VString::existFile(srcfn))
    {
        cout<<"Source file "<<srcfn<<"cannot open!\n";
        exit(0);
    }

    ofstream *pqcStrm = new ofstream(dstFn, ios::out);
    if(!pqcStrm->is_open())
    {
        cout<<"File '"<<dstFn<<"' cannot open for writing!\n";
        pqcStrm->close();
        exit(0);
    }

    unsigned int vnum = 0, i = 0, di = 0, sz = 0, k = 0;
    float *feat = new float[this->ftDim];
    unsigned int *pqcodes = new unsigned int[this->dv0+1];

    cout<<"Encode vectors into PQ .......... '"<<srcfn<<"'\n";
    ifstream *inStrm = new ifstream(srcfn, ios::in);
    (*inStrm)>>sz;
    (*inStrm)>>di;
    assert(di == this->ftDim);
    scale = (scale == -1)?sz:scale;
    float el2 = 0;
    while(!inStrm->eof() && vnum < scale)
    {
        for(k = 0; k < di; k++)
        {
            (*inStrm)>>feat[k];
        }

        if(this->_norm_)
        {
            PQMath::l2_norm(feat, this->ftDim);
        }
        if(_isivfpq_)
        {
            el2 = this->quantzIVFPQ(feat, this->ftDim, this->dv0, pqcodes);
            (*pqcStrm)<<pqcodes[0]<<" "<<el2<<" ";
        }
        else
        {
            this->quantzPQ(feat, this->ftDim, this->dv0, pqcodes);
            (*pqcStrm)<<el2<<" ";
        }
        for(i = 0; i < this->dv0; i++)
        {
            (*pqcStrm)<<pqcodes[i+1]<<" ";
        }
        (*pqcStrm)<<endl;
        memset(feat, 0, sizeof(float)*this->ftDim);
        vnum++;
        printf("\r\r\t%6d", vnum);
    }
    inStrm->close();
    pqcStrm->close();
    cout<<endl;

    delete [] feat;
    delete [] pqcodes;
    pqcodes = NULL;
    feat = NULL;

    return true;
}

float PQEncoder::quantzIVFPQ(const float *feat, const unsigned int d0,
                            const unsigned int dv, unsigned int *codes)
{
    unsigned int nnIdx = 0, j = 0, loc = 0;
    assert(this->medians);
    assert(codes);
    assert(feat);

    float *residue = new float[d0];
    float *tmpVect = new float[d0];
    float el2 = 0;

    nnIdx = (*this.*nnFunc)(feat, d0);
    loc = nnIdx*d0;
    codes[0] = nnIdx;
    memcpy(tmpVect, feat, sizeof(float)*d0);

    for(j = 0; j < d0; j++)
    {
        residue[j] = tmpVect[j] - this->medians[loc+j];
        el2 += residue[j]*residue[j];
    }

    el2 = sqrt(el2);
    this->quantzPQ_l2(residue, d0, dv, codes+1);

    delete [] residue;
    delete [] tmpVect;
    residue = tmpVect = NULL;

    return el2;
}

float PQEncoder::quantzPQ(const float *feat, const unsigned int d0,
                         const unsigned int seg, unsigned int *codes)
{
    assert(codes);
    assert(feat);

    codes[0] = 0;
    float el2 = 0;
    for(unsigned int j = 0; j < d0; j++)
    {
        el2 += feat[j]*feat[j];
    }
    el2 = sqrt(el2);
    this->quantzPQ_l2(feat, d0, seg, codes+1);
    return el2;
}

bool PQEncoder::quantzPQ_l2(const float *vect, const unsigned int dim0,
                            const unsigned int dv, unsigned int codes[])
{
    assert(this->pqVocab);
    assert(codes);
    assert(vect);

    unsigned int j = 0, nnIdx = 0, s = 0, k = 0;
    unsigned int loc1 = 0, loc2 = 0, locv = 0;
    double  dst = 0, mdst = 0, w1 = 0;
    for(s = 0; s < dv; s++)
    {
        nnIdx = 0;
        loc1 = this->pqDim*s;
        locv = this->pqDim*this->pqNum*s;
        mdst = RAND_MAX;
        for(j = 0; j < this->pqNum; j++)
        {
            dst = 0;
            loc2 = locv + this->pqDim*j;
            for(k = 0; k < this->pqDim; k++)
            {
                w1  = vect[loc1 + k] - this->pqVocab[loc2 + k];
                dst += w1*w1;
            }
            if(dst < mdst)
            {
                nnIdx = j;
                mdst  = dst;
            }
        }
        codes[s] = nnIdx;
    }///for(s)

    return true;
}

float *PQEncoder::loadFeat(ifstream *inStrm, unsigned int &num, unsigned int &dim)
{
    float *feat = new float[dim];
    for(unsigned int i = 0; i < dim; i++)
    {
        (*inStrm)>>feat[i];
    }
    return feat;
}

void PQEncoder::test()
{
    const char *itmtab = "/home/wlzhao/src/prj/sotu/etc/itm_pq_sift_base.txt";
    const char *conf   = "/home/wlzhao/src/prj/sotu/etc/pq-ec.conf";

    Timer *tm = new Timer();

    PQEncoder *mypq = new PQEncoder(conf, "ivfpq");
    tm->start();
    mypq->performPQEncodeMat(itmtab);
    tm->end(true);
}
