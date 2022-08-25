#ifndef INDEXITEM_H
#define INDEXITEM_H

#include "invtitem.h"

class IndexItem
{
public:
    int index;
    unsigned int vkid;
    ///float tmcode;
    unsigned long pcode;
    float val, dc, dp;

    IndexItem(const unsigned int index0, const float val0)
    {
        index   = index0;
        val     = val0;
        pcode   = 0;
        vkid    = 0;
        dc = dp = 0;
    }

    IndexItem()
    {
        index = val = 0;
        pcode = 0;
        vkid  = 0;
        dc = dp = 0;
    }

    static bool LtComp (const IndexItem *a, const IndexItem *b)
    {
        return (a->val < b->val);
    }

    static bool LgComp (const IndexItem *a, const IndexItem *b)
    {
        return (a->val > b->val);
    }

};

class UIndexItem
{

public:
    UIndexItem(const unsigned int index0, const unsigned int val0)
    {
        index = index0;
        val   = val0;
    }
    UIndexItem()
    {
        index = val = 0;
    }
public:
    unsigned int index;
    unsigned int val;

    static bool LtComp (const UIndexItem *a,const UIndexItem *b)
    {
        return (a->index < b->index);
    }

    friend bool operator> (const UIndexItem &a, const UIndexItem &b)
    {
        return (a.val > b.val);
    }

    friend bool operator< (const UIndexItem &a, const UIndexItem &b)
    {
        return (a.val < b.val);
    }

    friend bool operator== (const UIndexItem &a, const UIndexItem &b)
    {
        return (a.val == b.val);
    }


};

#endif
