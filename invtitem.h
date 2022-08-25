#ifndef INVTITEM_H
#define INVTITEM_H

#include <iostream>
#include <cassert>

using namespace std;

/***************unit definition region*******************/
/**Everything is not necessarily simple, but must be simpler, by Albert Einstein **/

class AbstractUnit
{
public:
    unsigned int Id;
};

class AbstractVnit
{
public:
    unsigned short Id;
};

class InvtUnit :public AbstractUnit
{
public:
    unsigned char tf;
} __attribute__((__packed__));

class InvtPQUnit: public  AbstractUnit
{
public:
    ///unsigned char *pqc;
    unsigned long lw64;
    unsigned long up64; ///if the sz of PQ codebook is larger than 256
                        ///but it is not suggested
} __attribute__((__packed__));

class InvtPQCUnit: public  AbstractUnit
{
public:
    unsigned char *pqc;
} __attribute__((__packed__));


class InvtPQOUnit: public  AbstractUnit
{
public:
    unsigned char *pqc;
    float sc;
} __attribute__((__packed__));

/**for encoding FCVLAD**/
class InvtPQSUnit: public  AbstractUnit
{
public:
    unsigned char pqc;
} __attribute__((__packed__));
/***************block definition region*******************/

class AbstractBlock
{
public:
    unsigned int length;
    unsigned short uIdx;

    AbstractBlock(const unsigned int sz)
    {
        length = sz;
        uIdx = 0;
    }
};

class AbstractHead
{
public:
    unsigned int Id;
    unsigned int bIdx;
};

class InvtBlock : public AbstractBlock
{

public:
    InvtBlock *next;
    InvtUnit *block;

public:
    InvtBlock(const unsigned int sz):AbstractBlock(sz)
    {
        if(sz > 0)
        {
            block = new InvtUnit[sz];
        }
        else
        {
            block = NULL;
        }
        uIdx = 0;
        next = NULL;
    }
    inline InvtUnit* operator[](const unsigned int i)
    {
        return &block[i];
    }
    ~InvtBlock()
    {
        if(block != NULL)
        {
            delete [] block;
            block = NULL;
        }
        next = NULL;
    }
};

class InvtPQBlock: public AbstractBlock
{

public:
    InvtPQBlock *next;
    InvtPQUnit *block;

public:
    InvtPQBlock(const unsigned int sz):AbstractBlock(sz)
    {
        if(sz > 0)
        {
            block = new InvtPQUnit[sz];
        }
        else
        {
            block = NULL;
        }
        uIdx = 0;
        next = NULL;
    }
    inline InvtPQUnit* operator[](const unsigned int i)
    {
        return &block[i];
    }
    ~InvtPQBlock()
    {
        if(block != NULL)
        {
            delete [] block;
            block = NULL;
        }
        next = NULL;
    }
};


class InvtPQCBlock: public AbstractBlock
{

public:
    InvtPQCBlock *next;
    InvtPQCUnit *block;

public:
    InvtPQCBlock(const unsigned int sz, const unsigned int nBt):AbstractBlock(sz)
    {
        if(sz > 0)
        {
            block = new InvtPQCUnit[sz];
            for(unsigned int i = 0; i < sz; i++)
            {
                block[i].pqc = new unsigned char[nBt];
            }
        }
        else
        {
            block = NULL;
        }
        uIdx = 0;
        next = NULL;
    }
    inline InvtPQCUnit* operator[](const unsigned int i)
    {
        return &block[i];
    }
    ~InvtPQCBlock()
    {
        if(block != NULL)
        {
            unsigned char *ppqc = NULL;
            for(unsigned int i = 0; i < length; i++)
            {
                ppqc = block[i].pqc;
                delete [] ppqc;
                ppqc = NULL;
            }

            delete [] block;
            block = NULL;
        }
        next = NULL;
    }
};


class InvtPQOBlock: public AbstractBlock
{

public:
    InvtPQOBlock *next;
    InvtPQOUnit *block;

public:
    InvtPQOBlock(const unsigned int sz, const unsigned int nBt):AbstractBlock(sz)
    {
        if(sz > 0)
        {
            block = new InvtPQOUnit[sz];
            for(unsigned int i = 0; i < sz; i++)
            {
                block[i].pqc = new unsigned char[nBt];
            }
        }
        else
        {
            block = NULL;
        }
        uIdx = 0;
        next = NULL;
    }
    inline InvtPQOUnit* operator[](const unsigned int i)
    {
        return &block[i];
    }
    ~InvtPQOBlock()
    {
        if(block != NULL)
        {
            unsigned char *ppqc = NULL;
            for(unsigned int i = 0; i < length; i++)
            {
                ppqc = block[i].pqc;
                delete [] ppqc;
                ppqc = NULL;
            }

            delete [] block;
            block = NULL;
        }
        next = NULL;
    }
};

class InvtPQSBlock: public AbstractBlock
{

public:
    InvtPQSBlock *next;
    InvtPQSUnit *block;

public:
    InvtPQSBlock(const unsigned int sz):AbstractBlock(sz)
    {
        if(sz > 0)
        {
            block = new InvtPQSUnit[sz];
        }
        else
        {
            block = NULL;
        }
        uIdx = 0;
        next = NULL;
    }
    inline InvtPQSUnit* operator[](const unsigned int i)
    {
        return &block[i];
    }
    ~InvtPQSBlock()
    {
        if(block != NULL)
        {
            delete [] block;
            block = NULL;
        }
        next = NULL;
    }
};
/***************header pointer definition region*******************/

class Hdr_InvtBlock: public InvtBlock, public AbstractHead
{

public:
    Hdr_InvtBlock(unsigned int i):InvtBlock(0)
    {
        Id = i;
        next = NULL;
        length = bIdx = uIdx = 0;
    }
    Hdr_InvtBlock():InvtBlock(0)
    {
        Id = 0;
        next = NULL;
        length = bIdx = uIdx = 0;
    }
};


class Hdr_InvtPQBlock: public InvtPQBlock, public AbstractHead
{

public:
    Hdr_InvtPQBlock(unsigned int i):InvtPQBlock(0)
    {
        Id = i;
        next = NULL;
        length = bIdx = uIdx = 0;
    }
    Hdr_InvtPQBlock():InvtPQBlock(0)
    {
        Id = 0;
        next = NULL;
        length = bIdx = uIdx = 0;
    }
};


class Hdr_InvtPQCBlock: public InvtPQCBlock, public AbstractHead
{

public:
    Hdr_InvtPQCBlock(unsigned int i, unsigned int nBt):InvtPQCBlock(0, 0)
    {
          Id = i;
        next = NULL;
        length = bIdx = uIdx = 0;
    }
    Hdr_InvtPQCBlock():InvtPQCBlock(0,0)
    {
        Id = 0;
        next = NULL;
        length = bIdx = uIdx = 0;
    }
};

class Hdr_InvtPQOBlock: public InvtPQOBlock, public AbstractHead
{

public:
    Hdr_InvtPQOBlock(unsigned int i, unsigned int nBt):InvtPQOBlock(0, 0)
    {
          Id = i;
        next = NULL;
        length = bIdx = uIdx = 0;
    }
    Hdr_InvtPQOBlock():InvtPQOBlock(0,0)
    {
        Id = 0;
        next = NULL;
        length = bIdx = uIdx = 0;
    }
};

class Hdr_InvtPQSBlock: public InvtPQSBlock, public AbstractHead
{

public:
    Hdr_InvtPQSBlock(unsigned int i):InvtPQSBlock(0)
    {
        Id = i;
        next = NULL;
        length = bIdx = uIdx = 0;
    }
    Hdr_InvtPQSBlock():InvtPQSBlock(0)
    {
        Id = 0;
        next = NULL;
        length = bIdx = uIdx = 0;
    }
};



#endif
