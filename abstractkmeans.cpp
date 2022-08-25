#include "abstractkmeans.h"
#include "hardwaresetup.h"
#include "timer.h"

#include <iostream>
#include <iomanip>
#include <cassert>
#include <fstream>
#include <cstring>
#include <cmath>


using namespace std;

const unsigned int AbstractKMeans::paraBound = 1024;
const float AbstractKMeans::smallVal0 = 0.0000001f;

AbstractKMeans::AbstractKMeans()
{
    this->ndim    = 0;
    this->labels  = NULL;
    this->data    = NULL;
    this->infoMap = NULL;
    this->visited = NULL;

    this->_INIT_  = false;
    this->nThrd   = 0;
    this->nThrd   = HardWareSetup::getCoreNum()/4;
    assert(this->nThrd > 0);
    this->visited = new unsigned char[this->nThrd];
    memset(visited, 0, this->nThrd*sizeof(unsigned char));
    strcpy(srcmatfn, "");

}

unsigned int AbstractKMeans::buildcluster(const char *srcfn, const char *dstfn, const char *lg_first,
        const char *crtrn, const char *distfunc, const char *numStr)
{
    if(kmMtd == _kppkmn_)
    cout<<"Threads for calcu. distance ...... "<<this->nThrd<<endl;

    this->config(lg_first, distfunc, crtrn, 1);
    this->clnumb = atoi(numStr);
    this->_INIT_ = this->init(srcfn);

    if(!this->_INIT_ || this->count == 0)
    {
        this->clnumb = 0;
    }
    else if(this->count <= this->clnumb)
    {
        this->infoMap = new CLSInfo[clnumb];
        this->clnumb  = this->nvclust(clnumb, dstfn, 1);
    }
    else
    {
        this->infoMap = new CLSInfo[clnumb];
        this->clnumb  = this->clust(clnumb, dstfn, 1);
    }

    return this->clnumb;
}

unsigned int AbstractKMeans::buildcluster(float *mat, const int row, const int dim, const char *dstfn,
        const char *lg_first, const char *crtrn, const char *distfunc, const char *numStr)
{
    assert(mat);
    assert(crtrn);
    assert(distfunc);
    assert(lg_first);

    this->config(lg_first, distfunc, crtrn, 0);
    this->clnumb = atoi(numStr);
    this->_INIT_ = this->init(mat, row, dim);

    if(!this->_INIT_ || this->count == 0)
    {
        this->clnumb = 0;
    }
    else if(this->count <= this->clnumb)
    {
        this->infoMap = new CLSInfo[clnumb];
        this->clnumb  = this->nvclust(clnumb, dstfn, 1);
    }
    else
    {
        this->infoMap = new CLSInfo[clnumb];
        ///cout<<"i am doing now\n";
        this->clnumb  = this->clust(clnumb, dstfn, 0);
        ///cout<<"i am done\n";
    }

    return this->clnumb;
}

unsigned int AbstractKMeans::nvclust(const unsigned int clnumb, const char *dstfn,  const int verbose)
{
    unsigned int i = 0;
    for(i = 0; i < clnumb; i++)
    {
        this->infoMap[i].n = 1;
        this->infoMap[i].E = 1.0f;
    }

    for(i = 0; i < this->count; i++)
    {
        this->labels[i] = i;
    }

    return this->count;
}

float *AbstractKMeans::loadMatrix(const char *srcfn, unsigned int &row,
                                  unsigned int &col)
{
    assert(srcfn);
    float vals[2] = {0};

    ifstream *inStrm = new ifstream();
    inStrm->open(srcfn, ios::in);
    if(inStrm->fail())
    {
        cout<<"Fail to read "<<srcfn<<endl;
        delete inStrm;
        exit(0);
    }

    (*inStrm)>>vals[0];
    (*inStrm)>>vals[1];
    row = (int)round(vals[0]);
    col = (int)round(vals[1]);
    float *mat = new float[row*col];
    float *tmp = new float[col];
    unsigned int irow, idim, loc;

    for(irow = 0; irow < row; irow++)
    {
        loc = irow*col;
        for(idim = 0; idim < col; idim++)
        {
            (*inStrm) >>tmp[idim];
        }
        for(idim = 0; idim < col; idim++)
        {
            mat[loc + idim] = tmp[idim];
        }
    }
    inStrm->close();
    assert(irow > 0);
    row = irow;
    delete inStrm;
    delete [] tmp;
    return mat;
}

float *AbstractKMeans::load_fvecs(const char *fvecfn, unsigned int &d, unsigned int &r)
{
    float *mat = NULL, *vect = NULL, *ppmat = NULL;
    unsigned int di = 0, ir = 0;
    d = 0;   r = 0;
    unsigned long bg = 0, fsize = 0, bfsz = 0;

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
     fsize = (unsigned long)inStrm->tellg() - bg;
    inStrm->close();
         r = fsize/(sizeof(unsigned int) + bfsz);

    if(r == 0)
    {
        cout<<"No data has been loaded!\n";
        r = d = 0;
        return NULL;
    }
    vect   = new float[d];
    mat    = new float[r*d];
    ppmat  = mat;
    inStrm = new ifstream(fvecfn, ios::in|ios::binary);
    while(!inStrm->eof() && ir < r)
    {
        inStrm->read((char*)&di,  sizeof(int));

        if(di == 0)
        continue;

        bfsz = di*sizeof(float);
        inStrm->read((char*)vect, bfsz);
        memcpy(ppmat, vect, bfsz);
        memset(vect, 0, bfsz);
        ppmat = ppmat + d;
        di = 0;
        ir++;
    }

    delete [] vect;
    vect = NULL;
    inStrm->close();
    return mat;
}

float *AbstractKMeans::getCluster(const unsigned int clabel0,
                                  unsigned int &row, unsigned int &dim)
{
    vector<int>::iterator it;
    vector<int> vects;
    unsigned int i = 0;
    int loc1 = 0, loc2 = 0, v = 0;
    unsigned int cpysize = sizeof(float)*this->ndim;

    for(i = 0; i < this->count; i++)
    {
        if(clabel0 == this->labels[i])
        {
            vects.push_back(i);
        }
    }
    row = vects.size();
    float *cl_data = new float[this->infoMap[clabel0].n*this->ndim];

    dim = this->ndim;
    assert(row == this->infoMap[clabel0].n);
    for(it = vects.begin(), i = 0; it != vects.end(); it++)
    {
        v = *it;
        loc1 = v*this->ndim;
        loc2 = i*this->ndim;
        memcpy(cl_data+loc2, this->data+loc1, cpysize);
        i++;
    }
    vects.clear();

    return cl_data;
}

void AbstractKMeans::printClusters(const char *dstdir)
{
    unsigned int clabel = 0, i,  j, loc, counter = this->count;
    CLSInfo &crntinfo    = infoMap[0];
    FILE *fp = NULL;
    char matfn[1024];
    assert(dstdir);

    for(j = 0; j < this->clnumb; j++)
    {
        crntinfo = infoMap[j];
        sprintf(matfn, "%s%d.mat", dstdir, j);
        fp = fopen(matfn, "w");
        fprintf(fp,"%d %d\n", crntinfo.n, this->ndim);
        fclose(fp);
    }

    for(i = 0; i < this->count; i++)
    {
        clabel = labels[i];
        loc = i*this->ndim;

        sprintf(matfn, "%s%d.mat", dstdir, clabel);
        fp = fopen(matfn, "a");
        for(j = 0; j < this->ndim; j++)
        {
            fprintf(fp,"%f ", data[loc+j]);
        }
        fprintf(fp,"\n");
        fclose(fp);
        counter--;
        cout<<"\r\r"<<setw(8)<<counter;
    }
    cout<<endl;
}

void AbstractKMeans::printCluster(const char *dstfn)
{
    FILE *fp = fopen(dstfn, "w");
    if(fp == NULL)
    {
        cout<<"File '"<<dstfn<<"' cannot open for write!\n";
        return;
    }
    unsigned int i;

    for(i = 0; i < this->count; i++)
    {
        fprintf(fp, "%d\n", this->labels[i]);
    }

    fclose(fp);
    return ;
}

void AbstractKMeans::printCLInfo(const CLSInfo *info, const unsigned int n)
{
    cout<<"-------------------------------\n";
    for(unsigned int i = 0; i < n; i++)
    {
        cout<<i<<"\t"<<info[i].n<<endl;
    }
    cout<<"================================\n";
}

void AbstractKMeans::printCLInfo(const CLSInfo *info, const unsigned int n,
                                 const char *dstfn)
{
    ofstream *outStrm = new ofstream(dstfn, ios::out);
    for(unsigned int i = 0; i < n; i++)
    {
        (*outStrm)<<i<<"\t"<<info[i].n<<endl;
    }
    outStrm->close();
    return ;
}

void AbstractKMeans::printvect(vector<NNItem*> &vects)
{
    vector<NNItem*>::iterator vit;
    NNItem* crntItm = NULL;
    cout<<"Solution------------------\n";
    cout<<"  "<<vects.size()<<" ways of clustering\n";
    cout<<"--------------------------\n";
    cout<<" Clabel\t  Size\tISim\t|\n";
    cout<<"--------------------------\n";
    for(vit = vects.begin(); vit != vects.end(); vit++)
    {
        crntItm = *vit;
        cout<<setw(4)<<crntItm->index<<"\t"<<setw(4)<<crntItm->size<<"\t+"<<setprecision(4)<<crntItm->val<<"|"<<endl;
    }
    cout<<"--------------------------\n";
}

void AbstractKMeans::printvect(vector<NNItem*> &vects, const char *dst_info_fn)
{
    assert(dst_info_fn);
    ofstream *outStrm = new ofstream(dst_info_fn, ios::out);
    vector<NNItem*>::iterator vit;
    NNItem* crntItm;
    (*outStrm)<<"Clabel\tSize\tISim\n";
    for(vit = vects.begin(); vit != vects.end(); vit++)
    {
        crntItm = *vit;
        (*outStrm)<<crntItm->index<<"\t"<<crntItm->size<<"\t"<<setprecision(4)<<crntItm->val<<endl;
    }
    (*outStrm).close();
}


void AbstractKMeans::normVects(float *vects, const unsigned int d0,
                       const unsigned int n0, float *lens)
{
    assert(vects);
    assert(lens);
    unsigned int i = 0, j = 0, loc = 0;
    float len = 0;
    for(i = 0; i < n0; i++)
    {
        len = 0;
        for(j = 0; j < d0; j++)
        {
            len += vects[loc+j]*vects[loc+j];
        }
        len     = sqrt(len);
        lens[i] = len;
        if(len > AbstractKMeans::smallVal0)
        {
            for(j = 0; j < d0; j++)
            {
                vects[loc+j] = vects[loc+j]/len;
            }
        }else{
            lens[i] = 0;
        }
        loc += d0;
    }
    return ;
}

bool AbstractKMeans::save_clust(const char *dstfn)
{
    if(dstfn == NULL || !strcmp(dstfn,""))
        return false;

    ofstream *outStrm = new ofstream(dstfn, ios::out);
    if(!outStrm->is_open())
    {
        cout<<"\nDestine file '"<<dstfn<<"' cannot open for write!\n";
        return false;
    }

    for(unsigned int i = 0; i < this->count; i++)
    {
        (*outStrm)<<this->labels[i]<<endl;
    }
    outStrm->close();
    return true;
}

bool AbstractKMeans::clearVector(vector<NNItem*> &vects)
{
    vector<NNItem*>::iterator vit;
    NNItem *crntItm;
    for(vit = vects.begin(); vit != vects.end(); vit++)
    {
        crntItm = *vit;
        delete crntItm;
    }
    vects.clear();
    return true;
}

void AbstractKMeans::clearInfoMap(map<unsigned int, CLSInfo*> &infoMap)
{
    map<unsigned int, CLSInfo*>::iterator mit;
    CLSInfo *crnt_info;
    for(mit = infoMap.begin(); mit != infoMap.end(); mit++)
    {
        crnt_info = mit->second;
        delete crnt_info;
    }
    infoMap.clear();
    return ;
}

AbstractKMeans::~AbstractKMeans()
{

    if(this->labels != NULL)
    {
        delete [] this->labels;
        this->labels = NULL;
    }

    if(!this->_REFER_)
    {
        if(this->data != NULL)
        {
            delete [] this->data;
            this->data = NULL;
        }
    }
    else
    {
        this->data = NULL;
    }

    if(this->infoMap != NULL)
    {
        delete [] this->infoMap;
        this->infoMap = NULL;
    }

    if(this->visited != NULL)
    {
        delete [] this->visited;
        this->visited = NULL;
    }
}
