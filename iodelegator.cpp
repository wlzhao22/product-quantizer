#include <iostream>
#include <cstring>
#include <cassert>
#include <iomanip>
#include <cstdio>
#include <cmath>

#include "iodelegator.h"

using namespace std;

float  *IODelegator::loadTXTMatrix(const char *fn, unsigned int &row, unsigned int &col)
{
    assert(fn);
    float vals[2] = {0};

    ifstream *inStrm = new ifstream();
    inStrm->open(fn, ios::in);
    if(inStrm->fail())
    {
        cout<<"Fail to read "<<fn<<endl;
        delete inStrm;
        exit(0);
    }

    (*inStrm)>>vals[0];
    (*inStrm)>>vals[1];
    row = (int)round(vals[0]);
    col = (int)round(vals[1]);
    float *mat = new float[row*col];
    unsigned int irow, idim, loc;

    for(irow = 0; irow < row; irow++)
    {
        loc = irow*col;
        for(idim = 0; idim < col; idim++)
        {
            (*inStrm) >>mat[loc + idim];
        }
    }
    inStrm->close();
    delete inStrm;
    return mat;
}

float *IODelegator::loadFVECMatrix(const char *srcfn, unsigned int &row, unsigned int &col)
{
        ifstream *inStrm = new ifstream(srcfn, ios::in|ios::binary);
        long int bg, idx = 0;
        unsigned int bfsize = 0, i = 0, di = 0;
        if(!inStrm->is_open())
        {
            cout << "File " << srcfn << " cannot open!!\n";
            exit(0);
        }
        bg = inStrm->tellg();
        inStrm->read((char*)&col, sizeof(int));
        bfsize = col*sizeof(float);
        inStrm->seekg(0, ios::end);
        row = ((long int)inStrm->tellg() - bg)/(bfsize + sizeof(int));
        inStrm->close();

        float *mat   = new float[row*col];
        float *fvect = new float[col];

        inStrm = new ifstream(srcfn, ios::in|ios::binary);
        for(i = 0; i < row; i++)
        {
            inStrm->read((char*)&di, sizeof(int));
            inStrm->read((char*)fvect, bfsize);
            for(unsigned int j = 0; j < col; j++, idx++)
            {
                mat[idx] = fvect[j];
            }
        }
        inStrm->close();
        delete [] fvect;
        fvect = nullptr;

        return mat;
}

void IODelegator::loadMultiMap(const char *srcfn, vector<vector<unsigned int> > &sup_map)
{
    unsigned int val, size, numb, j = 0;
    ifstream *inStrm = new ifstream(srcfn);

    (*inStrm)>>numb;
    for(j = 0; j < numb; j++)
    {
        vector<unsigned int> row;
        sup_map.push_back(row);
    }

    while(!inStrm->eof())
    {
        (*inStrm)>>size;
        (*inStrm)>>j;
        vector<unsigned> &crntVect = sup_map[j];

        for(unsigned int i = 0; i < size; i++)
        {
            (*inStrm)>>val;
            crntVect.push_back(val);
        }
    }
    inStrm->close();

    return ;
}

bool IODelegator::loadWghs(const char *weightfn, map<unsigned int,float> &i2wMap,
                           const unsigned int offset)
{
    assert(weightfn);
    FILE *fp = fopen(weightfn,"r");
    if(fp== NULL)
    {
        cout<<"Open file "<<weightfn<<" error!\n";
        exit(1);
    }

    char oneline[LONGCHAR];
    unsigned int IMG_ID;
    int num = 0, nargs = 0;
    float weight = 0;
    ///cout<<weightfn<<endl;
    char *ptStr = NULL;

    while(!feof(fp))
    {
        ptStr = fgets(oneline, LONGCHAR, fp);

        if(ptStr == NULL)
        continue;

        if(!strcmp(ptStr, ""))
		continue;

        if(strcmp(oneline, ""))
        {
            nargs = sscanf(oneline,"%d %f %d",&IMG_ID, &weight, &num);

            if(nargs != 3)
            continue;

            IMG_ID = IMG_ID + offset;
            i2wMap.insert(pair<unsigned int, float>(IMG_ID, weight));
            strcpy(oneline,"");
        }
    }

    fclose(fp);

    return true;
}

bool IODelegator::loadWghs(const char *weightfn, map<unsigned int,float> &i2wMap, map<unsigned int, unsigned int> &pmap,
                           map<unsigned int, unsigned int>  &_pmap, map<unsigned int, int>  &tmap, const unsigned int kf_offset)
{
    assert(weightfn);
    FILE *fp = fopen(weightfn,"r");
    if(fp== NULL)
    {
        cout<<"Open file "<<weightfn<<" error!\n";
        exit(1);
    }

    ///cout<<"i am here\n";

    char oneline[LONGCHAR];
    unsigned int IMG_ID, crnt_vid, pre_vid = 0, tm_code;
    int nargs = 0;
    float weight = 0;
    unsigned vid_offset =  pmap.size();
    char *ptStr = NULL;

    while(!feof(fp))
    {
        ptStr = fgets(oneline, LONGCHAR, fp);

        if(ptStr == NULL)
        continue;

        if(!strcmp(ptStr, ""))
        continue;

        if(strcmp(oneline, ""))
        {
            nargs = sscanf(oneline,"%d %d %d %f",&IMG_ID, &crnt_vid, &tm_code, &weight);

            if(nargs != 4)
            continue;

            IMG_ID = IMG_ID + kf_offset;
            _pmap.insert(pair<unsigned int,unsigned int>(IMG_ID, crnt_vid+vid_offset));
            if(crnt_vid != pre_vid)
            {
                pmap.insert(pair<unsigned int,unsigned int>(crnt_vid+vid_offset, IMG_ID));
            }
            i2wMap.insert(pair<unsigned int,float>(IMG_ID,weight));
            tmap.insert(pair<unsigned int,int>(IMG_ID,tm_code));
            strcpy(oneline,"");
            pre_vid = crnt_vid;
        }
    }

    fclose(fp);

    return true;
}

void IODelegator::load_k2imap(const char *srcfn, map<string, unsigned int> &referTab)
{
    FILE *fp = fopen(srcfn, "r");

    if(fp == NULL)
        return ;

    char key[256];
    char *crnt_key = NULL;
    int klen = 0, label = 0, nargs = 0;

    while(!feof(fp))
    {
        nargs = fscanf(fp, "%s %d", key, &label);

        if(nargs != 2)
        continue;

        klen = strlen(key)+1;
        crnt_key = new char[klen];

        strcpy(crnt_key, key);

        referTab.insert(pair<const char*, unsigned int>(crnt_key, label));
    }
    fclose(fp);
    return ;
}

void IODelegator::load_k2imap(const char *srcfn, map<string, int> &referTab)
{
    FILE *fp = fopen(srcfn, "r");

    if(fp == NULL)
        return ;

    char key[256];
    char *crnt_key;
    int klen = 0, label = 0, nargs = 0;

    while(!feof(fp))
    {
        nargs = fscanf(fp, "%s %d", key, &label);

        if(nargs != 2)
        continue;

            klen = strlen(key) + 1;
        crnt_key = new char[klen];
        strcpy(crnt_key, key);

        referTab.insert(pair<const char*, int>(crnt_key, label));
    }
    fclose(fp);

    return ;
}

void IODelegator::load_i2kmap(const char *nmapfn, map<unsigned int, const char*> &i2kMap,
                              const unsigned int offset)
{
    assert(nmapfn);

    FILE *fp = fopen(nmapfn, "r");

    if(fp == NULL)
    {
        cout<<"Open file '"<<nmapfn<<"' error!\n";
        exit(1);
    }

    char oneline[LONGCHAR];
    char fname[FNLEN];

    int nargs = 0;
    unsigned int IMG_ID;
    char *name = NULL, *ptStr = NULL;

    while(!feof(fp))
    {
        ptStr = fgets(oneline, LONGCHAR, fp);

        if(ptStr == NULL)
        continue;

        if(!strcmp(ptStr, ""))
        continue;

        if(!strcmp(oneline, "\n")||!strcmp(oneline, ""))
            continue;

        if(oneline[0] == '#')
            continue;

        nargs = sscanf(oneline,"%d %s\n",&IMG_ID, fname);

        if(nargs != 2)
        continue;

        name = new char[strlen(fname)+1];
        strcpy(name, fname);
        name[strlen(fname)] = '\0';
        IMG_ID = IMG_ID +  offset;

        i2kMap.insert(pair<unsigned int, const char*>(IMG_ID, name));
        strcpy(oneline, "");
    }
    fclose(fp);
    return ;
}

vector<const char*> IODelegator::loadNameList(const char *kflstfn, const char *msg)
{
    assert(kflstfn);
    FILE *fp = fopen(kflstfn,"r");

    vector<const char*> namelst;

    if(fp == NULL)
    {
        cout<<"Open file "<<kflstfn<<" error!\n";
        exit(1);
    }

    cout<<msg;

    char oneline[LONGCHAR];
    char fname[FNLEN];

    unsigned int IMG_ID = 0, i = 0;
    char *name = NULL, *ptStr = NULL;
    int nargs = 0;

    while(!feof(fp))
    {
        ptStr = fgets(oneline, LONGCHAR, fp);

        if(ptStr == NULL)
        continue;

        if(!strcmp(ptStr, ""))
        continue;

        if(!strcmp(oneline, "\n")||!strcmp(oneline, ""))
        {
            continue;
        }

        if(oneline[0] != '#' || oneline[0] != '\n')
        {
            nargs = sscanf(oneline, "%d %s\n", &IMG_ID, fname);

            if(nargs != 2)
            continue;

             name = new char[strlen(fname)+1];
            strcpy(name,fname);

            name[strlen(fname)] = '\0';
            namelst.push_back(name);
            i++;
        }
        strcpy(oneline,"");
    }
    fclose(fp);
    cout<<i<<endl;

    return namelst;
}

vector<const char*> IODelegator::loadStrings(const char *kflstfn)
{
    assert(kflstfn);
    FILE *fp = fopen(kflstfn,"r");

    vector<const char*> namelst;

    if(fp == NULL)
    {
        cout<<"Open file "<<kflstfn<<" error!\n";
        exit(1);
    }

    cout<<"Loading Name List ......... ";

    char oneline[LONGCHAR];
    char fname[FNLEN];

    char *name = NULL, *ptStr = NULL;
    int i = 0;

    while(!feof(fp))
    {
        ptStr = fgets(oneline, LONGCHAR, fp);

        if(ptStr == NULL)
        continue;

        if(!strcmp(ptStr, ""))
        continue;

        if(!strcmp(oneline, "\n")||!strcmp(oneline, ""))
        {
            continue;
        }

        if(oneline[0] != '#' || oneline[0] != '\n')
        {
            sscanf(oneline, "%s\n", fname);
            name = new char[strlen(fname)+1];
            strcpy(name,fname);

            name[strlen(fname)] = '\0';
            namelst.push_back(name);
            i++;
        }
        strcpy(oneline, "");
    }
    fclose(fp);
    cout<<i<<endl;
    return namelst;
}

void IODelegator::load_map(const char *srcfn, map<string,unsigned char> &referTab)
{
    FILE *fp = fopen(srcfn, "r");
    if(fp == NULL)
        return ;

    char key[256];
    char *crnt_key = NULL;
    int  klen = 0, nargs = 0;

    while(!feof(fp))
    {
        nargs = fscanf(fp, "%s", key);

        if(nargs != 1)
        continue;

            klen = strlen(key)+1;
        crnt_key = new char[klen];

        strcpy(crnt_key,key);

        referTab.insert(make_pair(crnt_key,1));
    }
    fclose(fp);
}

void IODelegator::parseLine(vector<char> &oneline,vector<float> &vals,const char splitter)
{
    int size = 0;
    char word[30];
    char ch;
    float val;

    vector<char>::iterator it;
    for(it = oneline.begin(); it != oneline.end(); it++)
    {
        ch = *it;
        if(ch != splitter)
        {
            word[size] = ch;
            size++;
        }
        else
        {
            word[size] = '\0';
            val = atof(word);
            vals.push_back(val);
            size = 0;
        }
    }

    if(size > 0)
    {
        word[size]='\0';
        val=atof(word);
        vals.push_back(val);
    }
}

void IODelegator::parse(const char *oneline, list<float> &vals, const char splitter)
{
    assert(oneline);
    int len = strlen(oneline);
    int size = 0;
    char word[30];
    int i;
    float val;

    vector<char>::iterator it;
    for(i = 0; i < len; i++)
    {
        if(oneline[i] != splitter && oneline[i] != '\n')
        {
            word[size] = oneline[i];
            size++;
        }
        else
        {
            word[size] = '\0';
            if(size > 0)
            {
                val = atof(word);
                vals.push_back(val);
                size = 0;
            }
        }
    }

    if(size > 0)
    {
        word[size]='\0';
        val=atof(word);
        vals.push_back(val);
    }

    return ;
}

void IODelegator::parse(const char *oneline, list<unsigned int> &vals, const char splitter)
{
    assert(oneline);
    int len = strlen(oneline);
    int size = 0;
    char word[30];
    int i;
    unsigned int val;

    vector<char>::iterator it;
    for(i = 0; i < len; i++)
    {
        if(oneline[i] != splitter && oneline[i] != '\n' && oneline[i] != ',')
        {
            word[size] = oneline[i];
            size++;
        }
        else
        {
            word[size] = '\0';
            if(size > 0)
            {
                val = (unsigned int)atoi(word);
                vals.push_back(val);
                size = 0;
            }
        }
    }

    if(size > 0)
    {
        word[size]='\0';
        val=(unsigned int)atoi(word);
        vals.push_back(val);
    }

    return ;
}

void IODelegator::parseLine(vector<char> &oneline, vector<float> &vals, const char splitters[])
{
    char word[30], ch;
    float val;
    unsigned int i, size = 0;
    unsigned int len = strlen(splitters);

    vector<char>::iterator it;
    bool SPLIT = false;
    for(it = oneline.begin(); it != oneline.end(); it++)
    {
        ch = *it;
        SPLIT = false;
        for(i = 0; i < len; i++)
        {
            if(ch == splitters[i])
            {
                if(size > 0)
                {
                    word[size] = '\0';
                    val = atof(word);
                    vals.push_back(val);
                    size = 0;
                }
                SPLIT = true;
                break;
            }
        }

        if(!SPLIT)
        {
            word[size] = ch;
            size++;
        }
    }

    if(size > 0)
    {
        word[size] = '\0';
        val = atof(word);
        vals.push_back(val);
    }
    return ;
}

void IODelegator::test()
{
}
