#include "binaryheap.h"

#include <algorithm>
#include <cstring>

BinaryHeap::BinaryHeap(const unsigned int capacity0, const char *comparer0)
{
    this->capacity = capacity0;
    this->array.reserve(capacity);
    if(!strcmp(comparer0, "lg"))
    {
        comparer = &IndexItem::LgComp;
        heaper   = &IndexItem::LtComp;
    }else if(!strcmp(comparer0, "lt"))
    {
        comparer = &IndexItem::LtComp;
        heaper   = &IndexItem::LgComp;
    }else{
        cout<<"Unknown comparer '"<<comparer0<<"'!";
        cout<<"Valid options are 'lg' or 'lt' for BinaryHeap!\n";
        exit(0);
    }
}

bool BinaryHeap::insert(const unsigned int idx, const float val, const float tmcode)
{
    IndexItem *nw_itm;
    if(this->capacity == this->array.size())
    {
        IndexItem tmp_itm, *left, *right;
        nw_itm = new IndexItem(idx, val);
        ///nw_itm->tmcode = tmcode;

        if((*comparer)(nw_itm, array[0]))
        {
            memcpy(&tmp_itm, array[0], sizeof(IndexItem));
            memcpy(array[0], nw_itm,   sizeof(IndexItem));
            memcpy(nw_itm,   &tmp_itm, sizeof(IndexItem));
        }

        unsigned int i = 1, idxl = 0;
        while(2*i < this->capacity)
        {
            idxl = 2*i - 1;
            left = array[2*i-1];    right = array[2*i];
            if((*comparer)(left, right))
            {
               if((*comparer)(nw_itm, right))
               {
                    memcpy(&tmp_itm, right, sizeof(IndexItem));
                    memcpy(right, nw_itm,   sizeof(IndexItem));
                    memcpy(nw_itm,   &tmp_itm, sizeof(IndexItem));
               }
               i = 2*i+1;
            }else{
               if((*comparer)(nw_itm, left))
               {
                    memcpy(&tmp_itm, left, sizeof(IndexItem));
                    memcpy(left, nw_itm,   sizeof(IndexItem));
                    memcpy(nw_itm,   &tmp_itm, sizeof(IndexItem));
               }
               i = 2*i;
            }
        }

        if(2*i == this->capacity)
        {
            idxl = 2*i-1;
            left = array[idxl];
            if((*comparer)(nw_itm, left))
            {
                memcpy(&tmp_itm, left,    sizeof(IndexItem));
                memcpy(left,    nw_itm,   sizeof(IndexItem));
                memcpy(nw_itm,  &tmp_itm, sizeof(IndexItem));
            }
        }
        delete nw_itm;
        return true;
    }else
    {
        nw_itm = new IndexItem(idx, val);
        ///nw_itm->tmcode = tmcode;
        array.push_back(nw_itm);

        if(array.size() == this->capacity)
        {
            make_heap(array.begin(), array.end(), (*heaper));
        }
        return true;
    }
}

bool BinaryHeap::updateRoot(const unsigned int idx, const float val, const float tmcode)
{
    if(this->capacity == this->array.size())
    {
        IndexItem tmp_itm, *left, *right, *crnt_itm;
        crnt_itm = array[0];
        crnt_itm->index = idx;    crnt_itm->val = val;
        ///crnt_itm->tmcode = tmcode;

        unsigned int i = 1, idxl = 0;
        while(2*i < this->capacity)
        {
            idxl = 2*i - 1;
            left = array[2*i-1];    right = array[2*i];
            if((*comparer)(left, right))
            {
               if((*comparer)(left, crnt_itm))
               {
                    memcpy(&tmp_itm, left, sizeof(IndexItem));
                    memcpy(left,    crnt_itm,   sizeof(IndexItem));
                    memcpy(crnt_itm,   &tmp_itm, sizeof(IndexItem));
                    crnt_itm = left;
               }
               i = 2*i;
            }else{
               if((*comparer)(right, crnt_itm))
               {
                    memcpy(&tmp_itm, right, sizeof(IndexItem));
                    memcpy(right,     crnt_itm,   sizeof(IndexItem));
                    memcpy(crnt_itm,   &tmp_itm, sizeof(IndexItem));
                    crnt_itm = right;
               }
               i = 2*i + 1;
            }
        }

        if(2*i == this->capacity)
        {
            idxl = 2*i-1;
            left = array[idxl];
            if((*comparer)(left, crnt_itm))
            {
                memcpy(&tmp_itm, left, sizeof(IndexItem));
                memcpy(left,     crnt_itm,   sizeof(IndexItem));
                memcpy(crnt_itm, &tmp_itm, sizeof(IndexItem));
            }
        }
        return true;
    }else{
        return false;
    }
}

bool BinaryHeap::root(IndexItem &root)
{
    if(array.size() == this->capacity)
    {
        memcpy(&root, array[0], sizeof(IndexItem));
        return true;
    }else{
        return false;
    }
}

void BinaryHeap::test()
{
    BinaryHeap *myheap = new BinaryHeap(3, "lt");
    int i, n = 13;
    for(i = 0; i < n; i++)
    {
        myheap->insert(i, random()/(RAND_MAX+1.0f), 0);
    }
    myheap->updateRoot(i, 0.5, 0);
    IndexItem root;
    myheap->root(root);
    cout<<root.val<<endl;
    cout<<"==========\n";
    myheap->print();

}
