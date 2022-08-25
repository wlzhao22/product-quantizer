#include "abstractpquantizer.h"

#include "hardwaresetup.h"
#include "scriptparser.h"
#include "iodelegator.h"
#include "cleaner.h"
#include "pqmath.h"
#include "timer.h"

#include <thread>


AbstractPQuantizer::AbstractPQuantizer()
{
    this->vqVocab  = NULL;
    this->visited  = NULL;
    this->_timeon_ = false;

    strcpy(this->tmLogFn, "");
    strcpy(this->vocabfn,  "");
}

bool AbstractPQuantizer::init_nnSearch(map<string, const char*> &paras)
{
    cout<<"-----Settings for NN search------\n";
    cout<<"Dist. measure in quantization ... ";
    if(paras.find("dist") != paras.end())
    {
        if(!strcmp(paras["dist"], "l2"))
        {
            dst_msr = __l2__;
        }
        else
        {
            cout<<"\nDistance measure '"<<paras["dist"]<<"' is not supported\n";
            exit(0);
        }
    }
    else
    {
        dst_msr = __l2__;
        cout<<"default = l2\n";
    }

    cout<<"Initialize NN search ............ ";
    nnVQFunc = NULL;
    nnFunc = NULL;

    cout<<"flat\n";
    _nns_opt = nns_no;
    nnFunc   = &AbstractPQuantizer::nnSearchl2;

    return true;
}

unsigned int AbstractPQuantizer::nnSearchl2(const float *query, const unsigned int dim)
{
        unsigned int idx  = 0, i = 0;
    float mdst = RAND_MAX, dst = 0;
    
    for(i = 0; i < this->vqNum; i++)
    {
        dst = PQMath::l2(query, 0, this->vqVocab, i, dim);
        if(dst < mdst)
        {
            idx  = i;
            mdst = dst;
        }
    }
    return idx;
}

void AbstractPQuantizer::nnSearch_SegL2(const float *query, const unsigned int dim,
                                       const unsigned int nSeg, unsigned int *idx)
{
    unsigned int sDim = dim/nSeg;
    assert(idx);
    assert(query);
    assert(sDim > 0);
    assert(dim == this->ftDim);
    const float *pVect1 = NULL, *pVect2 = NULL;
    unsigned int i = 0, j = 0, k = 0;
    float dst = 0, minDst = 0;

    for(i = 0; i < nSeg; i++)
    {
        pVect1 = query + i*sDim;
        minDst = RAND_MAX;
        for(j = 0; j < this->vqNum; j++)
        {
            pVect2 = vqVocab + j*this->ftDim + i*sDim;
               dst = PQMath::l2(pVect1, 0, pVect2, 0, sDim);
            if(dst < minDst)
            {
                minDst = dst;
                     k = j;
            }
        }
        idx[i] = k;
    }//for(i)

    return ;
}

float *AbstractPQuantizer::load_VQVocab(const char *vocfn, unsigned int &vdim1, unsigned int &vsize1)
{
    assert(vocfn);
    ifstream *inStrm = new ifstream(vocfn);
    if(!inStrm->is_open())
    {
        cout<<"Open file '"<<vocfn<<" failed!"<<endl;
        exit(0);
    }
    (*inStrm)>>vsize1;
    (*inStrm)>>vdim1;
    float *vocab1 = new float[vdim1*vsize1];
    unsigned int irow, idim, loc;
    for(irow = 0; irow < vsize1; irow++)
    {
        loc = irow*vdim1;
        for(idim = 0; idim < vdim1; idim++)
        {
            (*inStrm) >>vocab1[loc + idim];
        }
    }
    inStrm->close();
    return vocab1;
}

float *AbstractPQuantizer::load_PQVocab(const char *vocfn, unsigned int &vdim2, unsigned int &vsize2, unsigned int &segment)
{
    assert(vocfn);

    ifstream *inStrm = new ifstream(vocfn);
    if(!inStrm->is_open())
    {
        cout<<"Open file '"<<vocfn<<" failed!"<<endl;
        exit(0);
    }
    unsigned int vsize1, vdim1;
    (*inStrm)>>vsize1;
    (*inStrm)>>vdim1;
    float vocab1 = 0;
    unsigned int irow = 0, idim = 0, loc = 0;
    for(irow = 0; irow < vsize1; irow++)
    {
        loc = irow*vdim1;
        for(idim = 0; idim < vdim1; idim++)
        {
            (*inStrm) >>vocab1;
        }
    }
    (*inStrm)>>vsize2;
    (*inStrm)>>vdim2;
    (*inStrm)>>segment;

    float *vocab2 = new float[vdim2*vsize2*segment];
    unsigned int iseg = 0, loc0 = 0;

    for(iseg = 0; iseg < segment; iseg++)
    {
        loc0 = iseg*vsize2*vdim2;
        for(irow = 0; irow < vsize2; irow++)
        {
            loc = loc0 + irow*vdim2;
            for(idim = 0; idim < vdim2; idim++)
            {
                (*inStrm) >>vocab2[loc + idim];
            }
        }
    }

    inStrm->close();
    return vocab2;
}

AbstractPQuantizer::~AbstractPQuantizer()
{
    Cleaner::freeItemMaps(itmMaps);
    ScriptParser::clearParaMap(paras);

    cout<<"\nRelease Visual words ............ ";
    if(this->vqVocab != nullptr)
    {
        delete [] this->vqVocab;
        this->vqVocab = nullptr;
    }

    if(this->visited != nullptr)
    {
        delete [] this->visited;
        this->visited = nullptr;
    }

    Cleaner::clearMatrix(this->supv_2_vocab);
    cout<<"done\n";
}
