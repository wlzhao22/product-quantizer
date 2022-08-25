#include "kppmeans.h"

#include "iodelegator.h"
#include "vstring.h"
#include "cleaner.h"
#include "nnitem.h"

#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <ctime>

using namespace std;

const unsigned int KppMeans::M_ITER = 32;
const unsigned int KppMeans::RN0    = 30;
const float KppMeans::EPS  = 0.5f;
const float KppMeans::Err0 = 10.0;


KppMeans::KppMeans()
{
    arrayD = tmparrayD = NULL;
    _INIT_ = false;
    kmMtd  = _kppkmn_;
    cout<<"Method ........................... k-means++\n";
}

bool KppMeans::init(const char *srcfn)
{
    cout<<"Loading matrix ................... ";
    assert(srcfn);
    strcpy(srcmatfn, srcfn);

    if(!this->_REFER_)
    {
        if(this->data != NULL)
        {
            delete [] this->data;
            this->data = NULL;
        }
    }else
    {
        if(this->data != NULL)
        {
            this->data = NULL;
        }
    }

    if(VString::endWith(srcfn, ".txt"))
    {
        this->data = KppMeans::loadMatrix(srcfn,  this->count, this->ndim);
    }
    else if(VString::endWith(srcfn, ".fvecs"))
    {
        this->data = KppMeans::load_fvecs(srcfn, this->ndim, this->count);
    }
    else
    {
        cout<<"Unrecognizable input file format!!!\n";
        this->data = NULL;
        exit(0);
    }
    cout<<this->count<<"x"<<this->ndim<<endl;

    if(this->data == NULL)
    {
        cout<<"Exceptions ....................... Loading matrix failed!\n";
        exit(0);
    }

    this->labels = new int[this->count];
    memset(this->labels, 0 , sizeof(int)*this->count);
    this->_INIT_  = true;
    this->_REFER_ = false;

    return true;
}

bool KppMeans::init(float *mat, const int row, const int dim)
{
    if(!this->_REFER_)
    {
        if(this->data != NULL)
        {
            delete [] this->data;
            this->data = NULL;
        }
    }else
    {
        this->data = NULL;
    }

    this->data  = mat;
    this->count = row;
    this->ndim  = dim;

    if(this->labels != NULL)
    {
        delete [] this->labels;
        this->labels = NULL;
    }

    if(this->infoMap != NULL)
    {
        delete [] this->infoMap;
        this->infoMap = NULL;
    }

    if(this->arrayD != NULL)
    {
        delete [] this->arrayD;
        this->arrayD = NULL;
    }

    if(this->tmparrayD != NULL)
    {
        delete [] this->tmparrayD;
        this->tmparrayD = NULL;
    }

    this->labels = new int[this->count];
    memset(this->labels,  0, sizeof(int)*this->count);
    memset(this->visited, 0, this->nThrd*sizeof(unsigned char));
    this->_INIT_  = true;
    this->_REFER_ = true;
    return true;
}

bool KppMeans::refresh()
{
    this->count  = 1;
    this->ndim   = 0;
    this->_INIT_ = false;

    if(this->_REFER_)
    {
        if(this->data != NULL)
        {
            this->data = NULL;
        }
    }
    else{
        if(this->data != NULL)
        {
            delete [] this->data;
            this->data = NULL;
        }
    }

    if(this->labels != NULL)
    {
        delete [] this->labels;
        this->labels = NULL;
    }

    if(this->infoMap != NULL)
    {
        delete [] this->infoMap;
        this->infoMap = NULL;
    }

    if(this->arrayD != NULL)
    {
        delete [] this->arrayD;
        this->arrayD = NULL;
    }

    if(this->tmparrayD != NULL)
    {
        delete [] this->tmparrayD;
        this->tmparrayD = NULL;
    }
    memset(this->visited, 0, nThrd*sizeof(unsigned char));
    return true;
}

bool KppMeans::config(const char *_seeding_, const char *distfunc,
                      const char *crtrn, int verbose)
{
    if(verbose)
        cout<<"Distance function ................ ";
    if(!strcmp(distfunc, "l2"))
    {
        dist_func  = &KppMeans::l2;
        EXTRM_VAL0 = RAND_MAX;
        if(verbose)
            cout<<"l2\n";
    }
    else
    {
        dist_func  = &KppMeans::l2;
        EXTRM_VAL0 = RAND_MAX;
        if(verbose)
            cout<<"l2\n";
    }

    if(verbose)
        cout<<"Seeds ............................ ";
    if(!strcmp(_seeding_, "rand"))
    {
        if(verbose)
            cout<<"random\n";
        seed_func = &KppMeans::Rndseeds;
    }
    else if(!strcmp(_seeding_, "kpp"))
    {
        if(verbose)
            cout<<"k-means++\n";
        ///seed_func = &KppMeans::Rndseeds;
        seed_func = &KppMeans::Kppseeds;
        //seed_func = &KppMeans::paraKppseeds;
    }
    else
    {
        ///seed_func = &KppMeans::Rndseeds;
        seed_func = &KppMeans::Kppseeds;
        ///seed_func = &KppMeans::paraKppseeds;
        if(verbose)
        {
            cout<<"k-means++\n";
        }
        exit(0);
    }
    return true;
}


unsigned int KppMeans::clust(const unsigned int clust_num, const char *dstfn, const int verbose)
{
    unsigned int j = 0, clabel = 0, nfouls = 0;
    unsigned int i = 0, loc, d, di = 0, niter = 0;
    double dist  = 0, mindist = 0, err = 0, crnt_err = RAND_MAX;
    int *rseeds  = new int[clust_num];

    arrayD = new double[this->ndim*clust_num];
    memset(arrayD, 0, this->ndim*clust_num*sizeof(double));

    tmparrayD = new double[this->ndim*clust_num];
    memset(tmparrayD, 0, this->ndim*clust_num*sizeof(double));

    this->Kppseeds(clust_num, rseeds, count);
    for(j = 0; j < clust_num; j++)
    {
        loc = rseeds[j]*this->ndim;
        di  = j*this->ndim;
        for(d = 0; d < ndim; d++)
        {
            arrayD[di+d] = this->data[loc+d];
        }
    }
    if(verbose)
        cout<<"Clustering ....................... on progress\n";
    do
    {
             err = crnt_err;
        crnt_err = 0;
        for(j = 0; j < clust_num; j++)
        {
            this->infoMap[j].n = 0;
        }
        for(i = 0; i < this->count; i++)
        {
            mindist = EXTRM_VAL0;
            for(j = 0; j < clust_num; j++)
            {
                dist = this->l2(this->data, i, this->arrayD, j, this->ndim);
                if(dist < mindist)
                {
                    mindist = dist;
                    clabel  = j;
                }
            }///for(j)
            crnt_err       += mindist;
            this->labels[i] = clabel;
            loc             = i*this->ndim;
            di              = clabel*this->ndim;
            for(d = 0; d < this->ndim; d++)
            {
                this->tmparrayD[di+d] += this->data[loc+d];
            }
            this->infoMap[clabel].n = this->infoMap[clabel].n + 1;
            this->infoMap[clabel].E = this->infoMap[clabel].E + mindist;
        }///for(i)

        nfouls = di = 0;
        for(j = 0; j < clust_num; j++)
        {

            if(this->infoMap[j].n == 0)
            {
                this->Kppseeds(clust_num, rseeds, count);
                for(j = 0; j < clust_num; j++)
                {
                    di  = j*this->ndim;
                    loc = rseeds[j]*ndim;
                    for(d = 0; d < ndim; d++)
                    {
                        arrayD[di+d] = this->data[loc+d];
                    }
                }
                nfouls = 1;
                break;
            }
            else
            {
                di = j*this->ndim;
                for(d = 0; d < ndim; d++)
                {
                    this->arrayD[di+d] = this->tmparrayD[di+d]/this->infoMap[j].n;
                }
                this->infoMap[j].E = this->infoMap[j].E/this->infoMap[j].n;
            }
        }///for(j)
        memset(this->tmparrayD, 0, sizeof(double)*this->ndim*this->clnumb);
        niter++;
        ///cout<<err<<"\t"<<crnt_err<<"\t"<<niter<<endl;
    }while(fabs(err-crnt_err) > KppMeans::Err0 && niter < KppMeans::M_ITER);

    if(nfouls == 1)
    {
        cout<<"Exceptions ....................... failed to generate "<<clust_num<<" clusters\n";
        exit(0);
    }

    for(j = 0; j < clust_num; j++)
    {
        NNItem *crnt_item = new NNItem(j, infoMap[j].E, infoMap[j].n);
        sorted_stack.push_back(crnt_item);
    }

    if(strlen(dstfn) > 0)
    {
        char infofn[512];
        VString::parsePath(infofn, dstfn);
        strcat(infofn, "_large1_info.txt");
        KppMeans::printvect(sorted_stack, infofn);
        KppMeans::save_clust(dstfn);
    }
    KppMeans::clearVector(sorted_stack);
    delete [] rseeds;
    rseeds = NULL;

    return clust_num;
}

/** random k seeds in the way of k-means++ **/
bool KppMeans::Kppseeds(const unsigned int k, int rseeds[], const unsigned int bound)
{
    unsigned int i = 0, j = 0, newsel = 0;
    double sum = 0.0f, rd = 0;
    if(this->_INIT_ == false)
    {
        cout<<"Error ........................ initialize K-Means++ first!\n";
        exit(0);
    }
    float *disbest = new float[bound];
    float *distmp  = new float[bound];
    for(i = 0; i < bound; i++)
    {
        disbest[i] = RAND_MAX;
    }

    unsigned int sed0 = (unsigned int)time(NULL);
    float tmp = 0;

    rseeds[0] = rand_r(&sed0) % bound;
    for (i = 1 ; i < k ; i++)
    {
        newsel = rseeds[i-1];
        for(j = 0 ; j < bound; j++)
        {
            tmp = this->l2f(this->data, j, this->data, newsel, this->ndim);
            if(tmp < disbest[j])
            {
                disbest[j] = tmp;
            }
        }

        ///convert the best distances to probabilities
        memcpy(distmp, disbest, bound * sizeof (float));
        sum = 0;
        for(j = 0; j < bound; j++)
        {
            sum += distmp[j];
        }
        for(j = 0; j < bound; j++)
        {
            distmp[j] = distmp[j]/sum;
        }
        rd = rand_r(&sed0)/(RAND_MAX+1.0f);
        for (j = 0; j < bound-1; j++)
        {
          rd -= distmp[j];
          if (rd < 0)
           break;
        }
        rseeds[i] = j;
    }///for(i)

   
    // cout<<"k-means++ Version ................ 1.54\n";

    delete [] disbest;
    delete [] distmp;
    return true;
}

bool KppMeans::Rndseeds(const unsigned int k, int rseeds[], const unsigned int bound)
{
    srand(time(NULL));
    unsigned int NITER, it = 0, sed0 = (unsigned int)time(NULL);
    float v     = rand_r(&sed0)/(RAND_MAX+1.0f);
    bool FAILED = false;
    unsigned int i = 1, j = 0;
    int r = 0;

    rseeds[0] = (int)floor(v*bound);
    NITER = 20*k;

    while(i < k && it < NITER)
    {
        v      = rand_r(&sed0)/(RAND_MAX+1.0f);
        r      = (int)floor(v*bound);
        FAILED = false;
        for(j = 0; j < i; j++)
        {
            if(r == rseeds[j])
            {
                FAILED = true;
                break;
            }
        }
        if(!FAILED)
        {
            rseeds[i] = r;
            i++;
        }
        it++;
    }

    if(FAILED && it == NITER)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void KppMeans::saveCenters(const char *dstfn, bool append)
{
    unsigned int clabel = 0, j, loc;
    ofstream *outStrm  = NULL;
    if(append)
    {
        outStrm = new ofstream(dstfn, ios::app);
    }
    else
    {
        outStrm = new ofstream(dstfn, ios::out);
        (*outStrm)<<this->clnumb<<" "<<this->ndim<<endl;;
    }

    for(clabel = 0; clabel < this->clnumb; clabel++)
    {
        loc  = clabel*this->ndim;
        for(j = 0; j < this->ndim; j++)
        {
            (*outStrm)<<this->arrayD[loc+j]<<" ";
        }
        (*outStrm)<<endl;
    }

    outStrm->close();
    return ;
}

int KppMeans::fetchCenters(float *centers)
{
    unsigned int clabel = 0, j = 0, loc = 0, idxi = 0;
    assert(centers);

    if(!this->_INIT_||this->clnumb == 0)
    {
        memset(centers, 0, this->clnumb*this->ndim*sizeof(float));
        return 0;
    }

    for(clabel = 0; clabel < this->clnumb; clabel++)
    {
        loc   = clabel*this->ndim;
        for(j = 0; j < this->ndim; j++)
        {
            centers[idxi + j] = this->arrayD[loc+j];
        }
        idxi += this->ndim;
    }
    return this->clnumb;
}

double KppMeans::l2f(const float vect1[], const unsigned int s1,
                     const float vect2[], const unsigned int s2,
                     const unsigned int dim0)
{
    double w1 = 0.0f, w2 = 0.0f;
    unsigned int loc1 = dim0 * s1;
    unsigned int loc2 = dim0 * s2;

    for(unsigned int i = 0; i < dim0; i++)
    {
        w1 = vect1[loc1+i] - vect2[loc2+i];
        w2 += w1*w1;
    }

    w2 = sqrt(w2);

    return w2;
}

double KppMeans::l2(const float vect1[], const unsigned int s1,
                    const double vect2[], const unsigned int s2,
                    const unsigned int dim0)
{
    double w1 = 0.0f, w2 = 0.0f;
    unsigned int loc1 = dim0*s1;
    unsigned int loc2 = dim0*s2;

    for(unsigned int i = 0; i < dim0; i++)
    {
        w1 = vect1[loc1+i] - vect2[loc2+i];
        w2 += w1*w1;
    }

    w2 = sqrt(w2);

    return w2;
}

double KppMeans::fvec_norm(const float *v, const long n, const double norm)
{
    if(norm == 0)
        return n;

    long i = 0;
    double s = 0;

    if(norm == 1)
    {
        for(i = 0 ; i < n ; i++)
            s += fabs(v[i]);
        return s;
    }

    if(norm == 2)
    {
        for(i = 0 ; i < n ; i++)
        {
            s += v[i]*v[i];
        }
        return sqrt(s);
    }

    if(norm == -1)
    {
        for(i = 0 ; i < n ; i++)
            if(fabs(v[i]) > s)
                s = fabs(v[i]);
        return s;
    }

    for (i = 0 ; i < n ; i++)
    {
        s += pow(v[i], norm);
    }

    return pow(s, 1/norm);
}

bool KppMeans::fvec_scale(float *v, const long n, const double sc)
{
    assert(sc != 0.0f);

    for(long i = 0; i < n; i++)
    {
        v[i] = v[i]*sc;
    }
    return true;
}

KppMeans::~KppMeans()
{
    if(arrayD != NULL)
    {
        delete [] arrayD;
        arrayD = NULL;
    }
    if(tmparrayD != NULL)
    {
        delete [] tmparrayD;
        tmparrayD = NULL;
    }

}

void KppMeans::test()
{
    const char *srcfn = "/home/wanlei/datasets/mat/paris60k_hess-rnsift_65k.txt";
    const char *dstfn = "/home/wanlei/datasets/vocab/paris60k_hess-rnsift_65k_200_kpp.txt";

    KppMeans *mykmean = new KppMeans();
    /**/
    mykmean->buildcluster(srcfn, "", "kpp", "", "l2", "200");

    mykmean->saveCenters(dstfn, false);

    delete mykmean;
}
