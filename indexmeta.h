#ifndef INDEXMETA_H
#define INDEXMETA_H

#include <cstring>

class IndexMeta
{
public:
    IndexMeta(const unsigned int Id, const float w)
    {
        this->Id  = Id;
        this->wgh = w;
        this->pp  = 0;
        this->wgc = 1.0;
    }
public:
    unsigned int Id, pp;
    float wgh, wgc;

    static bool LGcomparer(const IndexMeta *a, const IndexMeta *b)
    {
        return (a->wgh > b->wgh);
    }

    static bool LLcomparer(const IndexMeta *a, const IndexMeta *b)
    {
        return (a->wgh < b->wgh);
    }
};

class RetriItem
{

public:
    RetriItem(const char *imgId0, const float w, const float wgc0)
    {
        strcpy(ImgId, imgId0);
        this->wgh = w;
        this->wgc = wgc0;
    }
public:
    char ImgId[96];
    float wgh, wgc;

    static bool LGcomparer(const RetriItem *a, const RetriItem *b)
    {
        if (a->wgh > b->wgh)
        {
            return true;
        }else if(a->wgh == b->wgh)
        {
           return (a->wgc > b->wgc);
        }else{
           return false;
        }
    }

    static bool LLcomparer(const RetriItem *a, const RetriItem *b)
    {
       if (a->wgh < b->wgh)
        {
            return true;
        }else if(a->wgh == b->wgh)
        {
           return (a->wgc < b->wgc);
        }else{
           return false;
        }
    }
};


#endif
