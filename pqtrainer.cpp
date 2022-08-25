#include "pqtrainer.h"


#include "hardwaresetup.h"
#include "scriptparser.h"
#include "iodelegator.h"
#include "vstring.h"
#include "pqmath.h"
#include "nnitem.h"

#include <dirent.h>
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cmath>


const unsigned int PQTrainer::SZ_RMAT0 = 65536;
const unsigned int PQTrainer::FT_DIM   = 128;
const unsigned int PQTrainer::SZ_CMAT0 = 160000;
const unsigned int PQTrainer::step0    = 7;


PQTrainer::PQTrainer(const char *conf)
{
    this->kmeans     = new KppMeans();
    this->nThrd      = 0;
    this->visited    = NULL;
    this->medians    = NULL;
    this->prjMat     = NULL;
    this->vqVocab    = NULL;
    this->supVocab   = NULL;
    this->__LV2__    = false;
    this->init(conf);

}

bool PQTrainer::init(const char *conf)
{
    assert(conf);

    cmatrix = rmatrix = pmatrix = NULL;
    tmpvect = NULL;

    ///const char *options[1] = {"vocab"};
    map<string, const char*> paras;
    paras = ScriptParser::getConf(conf);
    ///ScriptParser::verifyParaMap(options, 1, paras);

    vqNum = vqDim = supNum = 0;

    if(paras.find("vocab") != paras.end())
    {
        vqVocab = IODelegator::loadTXTMatrix(paras["vocab"], vqNum, vqDim);
        cout<<"Vocabulary size .................. "<<vqNum<<"x"<<vqDim<<endl;
        assert(vqVocab);
    }

    if(paras.find("medv") != paras.end())
    {
        medians = IODelegator::loadTXTMatrix(paras["medv"], mNum, mDim);
        this->__LV2__ = true;
        cout<<"Medians .......................... "<<mNum<<"x"<<mDim<<endl;
    }else{
        if(vqVocab != NULL){
            medians = vqVocab;
        }
        else
        {
            cout << "loading vocabulary failed!\n";
            medians = NULL;
            exit(0);
        }
    }

    cout<<"Distance measure ................. ";
    nn_func = &PQTrainer::nnSearchl2;
    cout<<"l2\n";

    this->_sqrton_ = false;
    this->_norm_   = false;
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
            this->_norm_ = false;
            cout<<"no\n";
        }
    }else{
        cout<<"no\n";
    }

    tmpvect = new float[this->vqDim];
      csize = this->vqNum;
       cdim = this->vqDim;
    rmatrix = NULL;
    return true;
}

float *PQTrainer::load_fvecs(const char *fvecfn, unsigned int &d, unsigned int &r)
{
    float *mat = NULL, *vect = NULL, *ppmat = NULL;
    unsigned int di = 0;
    unsigned long bfsz = 0,fsize = 0, bg = 0;
       r = 0; d = 0;

    ifstream *inStrm = new ifstream(fvecfn, ios::in|ios::binary);
    if(!inStrm->is_open())
    {
         cout<<"File '"<<fvecfn<<"' cannot open for read!\n";
        return NULL;
    }

    inStrm = new ifstream(fvecfn, ios::in|ios::binary);
        bg = inStrm->tellg();
    inStrm->read((char*)&di,  sizeof(unsigned int));
         d = di;
      bfsz = d*sizeof(float);
    inStrm->seekg(0, ios::end);
     fsize = (long)inStrm->tellg() - bg;
    inStrm->close();
         r = fsize/(sizeof(unsigned int) + bfsz);

       mat = new float[r*d];
      vect = new float[d];
     ppmat = mat;
    inStrm = new ifstream(fvecfn, ios::in|ios::binary);
    while(!inStrm->eof())
    {
        inStrm->read((char*)&di,  sizeof(int));
        inStrm->read((char*)vect, bfsz);
        if(PQTrainer::validate(vect, d))
        {
            memcpy(ppmat, vect, bfsz);
            ppmat = ppmat + d;
        }
        memset(vect, 0, bfsz);
    }

    delete [] vect;
    vect = NULL;
    inStrm->close();
    return mat;
}

float *PQTrainer::load_fvecs(const char *fvecfn, unsigned int &d,
                             const unsigned int r0, unsigned int &r)
{
    float *mat = NULL, *vect = NULL, *ppmat = NULL;
    unsigned long bfsz = 0, fsize = 0, bg = 0;
    unsigned int cnt = 0, di = 0;

    ifstream *inStrm = new ifstream(fvecfn, ios::in|ios::binary);
    if(!inStrm->is_open())
    {
        cout<<"File '"<<fvecfn<<"' cannot open for read!\n";
        return NULL;
    }
         r = 0; d = 0;
        bg = inStrm->tellg();
    inStrm->read((char*)&di,  sizeof(unsigned int));
         d = di;
      bfsz = d*sizeof(float);
    inStrm->seekg(0, ios::end);
     fsize = (long)inStrm->tellg() - bg;
    inStrm->close();

         r = fsize/(sizeof(unsigned int) + bfsz);
         r = r > r0?r0:r;

       mat = new float[r*d];
      vect = new float[d];
     ppmat = mat;
       cnt = 0;

    inStrm = new ifstream(fvecfn, ios::in|ios::binary);
    while(!inStrm->eof())
    {
        inStrm->read((char*)&di, sizeof(int));

        assert(di == d);

        bfsz = d*sizeof(float);
        inStrm->read((char*)vect, bfsz);
        if(PQTrainer::validate(vect, d))
        {
            memcpy(ppmat, vect, bfsz);
            ppmat = ppmat + d;
            cnt++;
            if(cnt == r)
                break;
        }
        memset(vect, 0, bfsz);
    }

    delete [] vect;
    vect = NULL;
    inStrm->close();
    return mat;
}

float *PQTrainer::load_fvecs(const char *fvecfn, unsigned int &d, const unsigned int r0, const unsigned int step, unsigned int &r)
{
    float *mat = NULL, *vect = NULL, *ppmat = NULL;
    unsigned int cnt = 0, line = 0, di = 0;
    unsigned long bfsz = 0, bg = 0;

    ifstream *inStrm = new ifstream(fvecfn, ios::in|ios::binary);
    if(!inStrm->is_open())
    {
        cout<<"File '"<<fvecfn<<"' cannot open for read!\n";
        return NULL;
    }
         r = 0; d = 0;
        bg = inStrm->tellg();
    inStrm->read((char*)&di,  sizeof(unsigned int));
         d = di;
      bfsz = d*sizeof(float);
    inStrm->seekg(0, ios::end);
     r = ((long)inStrm->tellg() - bg)/(bfsz + sizeof(unsigned int));
    inStrm->close();

      r   = r>r0?r0:r;
    mat   = new float[r*d];
     vect = new float[d];
    ppmat = mat;

    inStrm = new ifstream(fvecfn, ios::in|ios::binary);
    while(!inStrm->eof())
    {
        inStrm->read((char*)&di, sizeof(int));
        assert(di == d);

        bfsz = d*sizeof(float);
        inStrm->read((char*)vect, bfsz);

        if(!PQTrainer::validate(vect, d))
        {
            memset(vect, 0, bfsz);
            continue;
        }

        line++;
        if(line%step == 0)
        {
            memcpy(ppmat, vect, bfsz);
            ppmat = ppmat + d;
            cnt++;
            if(cnt == r0)
                break;
        }
    }

    delete [] vect;
    inStrm->close();
    return mat;
}

unsigned int PQTrainer::buildVQuantizer(const char *cmatfn0, const char *cnum0, const char *dstfn)
{
    unsigned int clnum = 0, cRow = 0, Dim = 0;
    cout<<"Generate vector quantizer ........ "<<cnum0<<endl;

    KppMeans *kmeans = new KppMeans();
    cout<<"Loading matrix ................... ";
    float *cmatrix = nullptr;
    if(VString::endWith(cmatfn0, ".txt"))
    {
        cmatrix = IODelegator::loadTXTMatrix(cmatfn0, cRow, Dim);
    }else if(VString::endWith(cmatfn0, ".fvecs"))
    {
        cmatrix = IODelegator::loadFVECMatrix(cmatfn0, cRow, Dim);
    }else{
        cout << "File format is not supported!\n";
        cout << "Only '.fvecs' and '.txt' matrix format are supported!\n";
    }
    cout<<cRow<<"x"<<Dim<<endl;

    cout<<"Calculating vocabulary ........... ";
    clnum = kmeans->buildcluster(cmatrix, cRow, Dim, "", "kpp", "", "l2", cnum0);
    kmeans->saveCenters(dstfn, false);
    kmeans->refresh();
    cout<<"done\n";
    if(cmatrix != NULL)
    {
        delete [] cmatrix;
        cmatrix = NULL;
    }

    return clnum;
}

bool PQTrainer::buildPQuantizer(const char *rmatfn0, const unsigned int rnum0,
                               const unsigned int dv, const char *dstfn)
{
    unsigned int i = 0, j = 0, s = 0, loc0 = 0, loc1, loc2, k = 0;
    unsigned int irow = 0, rnum = 0, rCol = 0;
    char cnum[16] = "";
    cout<<"Generate Product quantizer ....... \n";
    cout<<endl;
    cout<<"Load training vectors ............ ";
    if(VString::endWith(rmatfn0, ".fvecs"))
    {
        cmatrix = PQTrainer::load_fvecs(rmatfn0, this->vqDim, SZ_RMAT0, step0, rnum);
    }else if(VString::endWith(rmatfn0, ".txt"))
    {
        cmatrix = IODelegator::loadTXTMatrix(rmatfn0, rnum, this->vqDim);
    }else{
        cout<<"Unknown format for input matrix '"<<rmatfn0<<"'"<<endl;
    }

    cout<<rnum<<"x"<<this->vqDim<<endl;
    if(this->_norm_)
    {
        PQMath::normVects(cmatrix, this->vqDim, rnum);
    }

    float *tmpvect = new float[this->vqDim];
    irow = 0;
    cout<<"Collecting residues .............. \n";
    for(i = 0; i < rnum; i++)
    {
        loc0  = i*this->vqDim;
        memcpy(tmpvect, cmatrix + loc0, this->vqDim*sizeof(float));

        if(PQTrainer::validate(tmpvect, this->vqDim))
        {
            irow++;
        }

        if(irow == SZ_RMAT0)
            break;

        printf("\r\r\r\t%8d", i);
    }
    cout<<endl<<endl;
       rRow = irow;
    rmatrix = new float[rRow*this->vqDim];
       loc1 = rRow*this->vqDim;
    memcpy(rmatrix, cmatrix, loc1*sizeof(float));
    delete [] cmatrix;
    cmatrix = NULL;
       rCol = this->vqDim/dv;
    pmatrix = new float[rRow*rCol];

    sprintf(cnum, "%d", rnum0);
    ofstream *outStrm = new ofstream(dstfn, ios_base::app);
    (*outStrm)<<rnum0<<" "<<rCol<<" "<<dv<<endl;
    outStrm->close();

    for(s = 0; s < dv; s++)
    {
        irow  = 0;
        for(i = 0; i < rRow; i++)
        {
            loc1 = irow*rCol;
            loc2 = i*this->vqDim;
               j = rCol*s;
            if(validate(rmatrix+loc2+j, rCol))
            {
                for(k = 0; k < rCol; k++)
                {
                    pmatrix[loc1+k] = rmatrix[loc2+j+k];
                }
                irow++;
            }
        }
        printf("\r\r\r%8d\t%d\n", s, irow);
        cout<<endl;
        kmeans->buildcluster(pmatrix, irow, rCol, "", "kpp", "", "l2", cnum);
        cout << "it is done\n";
        kmeans->saveCenters(dstfn, true);
        kmeans->refresh();
    }
    cout<<endl;
    delete [] rmatrix;
    delete [] pmatrix;
    delete [] tmpvect;
    rmatrix = pmatrix = tmpvect = NULL;

    return true;
}

bool PQTrainer::buildIVFPQuantizer(const char *rmatfn0, const unsigned int rnum0,
                                   const unsigned int dv, const char *dstfn)
{
    unsigned int i = 0, j = 0, s = 0, loc0 = 0, loc1, loc2, k = 0;
    unsigned int nnidx = 0, irow = 0, rnum = 0, rCol = 0;
    char cnum[16] = "";
    cout<<"Generate Product quantizer ....... \n";
    cout<<"Number of segments ............... "<<dv<<endl;
    cout<<"Number of sub-quantizer .......... "<<rnum0<<endl;
       rRow = SZ_RMAT0;
    rmatrix = new float[this->rRow*this->vqDim];
    memset(rmatrix, 0, sizeof(float)*this->rRow*this->vqDim);
    cout<<endl;
    irow = 0;
    cout<<"Load training vectors ............ ";
    if(VString::endWith(rmatfn0, ".fvecs"))
    {
        cmatrix = PQTrainer::load_fvecs(rmatfn0, this->vqDim, SZ_RMAT0, step0, rnum);
    }else if(VString::endWith(rmatfn0, ".txt"))
    {
        cmatrix = IODelegator::loadTXTMatrix(rmatfn0, rnum, this->vqDim);
    }else{
        cout<<"Unknown format for input matrix '"<<rmatfn0<<"'"<<endl;
    }

    cout<<rnum<<"x"<<this->vqDim<<endl;
    if(this->_norm_)
    {
        PQMath::normVects(cmatrix, this->vqDim, rnum);
    }
    float *tmpvect = new float[this->vqDim];

    cout<<"Collecting residues .............. \n";
    for(i = 0; i < rnum; i++)
    {
        loc0  = i*this->vqDim;
        loc1  = irow*this->vqDim;
        memcpy(tmpvect, cmatrix + loc0, this->vqDim*sizeof(float));

        nnidx = (*this.*nn_func)(tmpvect, this->vqDim);
        loc2  = nnidx*this->vqDim;

        for(j = 0; j < this->vqDim; j++)
        {
            rmatrix[loc1+j] = tmpvect[j] - medians[loc2+j];
        }

        if(PQTrainer::validate(rmatrix + loc1, this->vqDim))
        {
            irow++;
        }

        if(irow == SZ_RMAT0)
            break;

        printf("\r\r\r\t%8d", i);
    }
    cout<<endl;
    rRow = rnum < rRow?rnum:rRow;
    cout<<"\n";

    this->saveMat(this->vqVocab, this->vqNum, this->vqDim, dstfn);

       rCol = this->vqDim/dv;
    pmatrix = new float[rRow*rCol];
    sprintf(cnum, "%d", rnum0);
    ofstream *outStrm = new ofstream(dstfn, ios_base::app);
    (*outStrm)<<rnum0<<" "<<rCol<<" "<<dv<<endl;
    outStrm->close();

    for(s = 0; s < dv; s++)
    {
        irow  = 0;
        for(i = 0; i < rRow; i++)
        {
            loc1 = irow*rCol;
            loc2 = i*this->vqDim;
               j = rCol*s;
            if(validate(rmatrix+loc2+j, rCol))
            {
                for(k = 0; k < rCol; k++)
                {
                    pmatrix[loc1+k] = rmatrix[loc2+j+k];
                }
                irow++;
            }
        }
        printf("\r\r\r%8d\t%d\n", s, irow);
        cout<<endl;
        kmeans->buildcluster(pmatrix, irow, rCol, "", "kpp", "", "l2", cnum);
        kmeans->saveCenters(dstfn, true);
        kmeans->refresh();
    }
    cout<<endl;

    delete [] pmatrix;
    delete [] tmpvect;
    pmatrix = tmpvect = NULL;

    return true;
}

unsigned int PQTrainer::nnSearchl2(const float *query, const unsigned int dim)
{
    unsigned int idx  = 0, i = 0;
    float mdst = RAND_MAX, dst = 0;
    
    for(i = 0; i < this->vqNum; i++)
    {
        dst = PQMath::l2(query, 0, this->vqVocab, i, this->vqDim);
        if(dst < mdst)
        {
            idx  = i;
            mdst = dst;
        }
    }
    return idx;
}

bool PQTrainer::validate(const float *vect1, const unsigned int dim)
{
    unsigned int i = 0;
    float d1 = 0;
    for(i = 0; i < dim; i++)
    {
        d1 += vect1[i]*vect1[i];
    }

    if(d1 <= 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool PQTrainer::norm(float *vect1, const unsigned int dim)
{
    unsigned int i = 0;
    float lens = 0;
    for(i = 0; i < dim; i++)
    {
        lens += vect1[i]*vect1[i];
    }

    lens = sqrt(lens);

    if(lens > 0)
    {
        for(i = 0; i < dim; i++)
        {
            vect1[i] = vect1[i]/lens;
        }
        return true;
    }
    else
    {
        return false;
    }
}

void PQTrainer::clean2DMat(vector<vector<unsigned int> > &mat)
{
    vector<vector<unsigned int> >::iterator mit;
    for(mit = mat.begin(); mit != mat.end(); mit++)
    {
        vector<unsigned int> &vect = *mit;
        vect.clear();
    }
    mat.clear();
}

void PQTrainer::saveMat(const float *mat, const unsigned int r,
                        const unsigned int d, const char *dstfn)
{
    assert(mat);
    unsigned int i = 0, j = 0;
    ofstream *outStrm = new ofstream(dstfn, ios::out);
    (*outStrm)<<r<<"\t"<<d<<endl;
    for(i = 0; i < r; i++)
    {
        for(j = 0; j < d; j++)
        {
            (*outStrm)<<mat[i*d + j]<<" ";
        }
        (*outStrm)<<endl;
    }
    outStrm->close();

    return ;
}

void PQTrainer::test()
{
    const char *conf       = "./etc/pq-tc.conf";
    const char *rmatrixfn2 = "./etc/sift_learn.txt";
    const char *dstfn      = "./etc/sift_8k_kpp_pq256.txt";

    /**
    PQTrainer *mypqtrain   = new PQTrainer(conf);
    unsigned int rnum     = 256;
    unsigned int segment  = 64;
    mypqtrain->buildPQuantizer(rmatrixfn2, rnum, segment, dstfn);
    /**/
    //mypqtrain->buildIVFPQuantizer(rmatrixfn2, rnum, segment, dstfn);
}
