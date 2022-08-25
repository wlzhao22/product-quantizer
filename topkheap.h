#ifndef TOPKHEAP_H
#define TOPKHEAP_H

#include "binaryheap.h"
#include "indexitem.h"

#include <iostream>
#include <vector>

using namespace std;

class TopkHeap
{
public:
    explicit TopkHeap(const unsigned int capacity0 = 100, const char *compare = "lg");

    bool isEmpty( ) const;
    unsigned int size() const
    {
        return array.size();
    }

    bool insert(const unsigned int idx,  const float val,
                const unsigned int vkid, long pcode,
                const float dp, const float dc);
    bool insert(const unsigned int idx, const float val);

    vector<IndexItem*>::iterator begin()
    {
        return array.begin();
    }

    vector<IndexItem*>::iterator end()
    {
        return array.end();
    }

    void makeEmpty( )
    {
        vector<IndexItem*>::iterator it;
        IndexItem* crnt_itm;

        for(it = array.begin(); it != array.end(); it++)
        {
            crnt_itm = *it;
            delete crnt_itm;
        }
        array.clear();
        leafHeap->makeEmpty();
    }
    void sort();
    void print()
    {
        vector<IndexItem*>::iterator it;
        IndexItem* crnt_itm;
        cout<<"-------------------------\n";
        for(it = array.begin(); it != array.end(); it++)
        {
            crnt_itm = *it;
            cout<<crnt_itm->index<<" "<<crnt_itm->val<<endl;
        }
        cout<<"--------------------------\n";
    }

    void printLeaf()
    {
        leafHeap->print();
    }

    ~TopkHeap()
    {
        this->makeEmpty();
        leafHeap->makeEmpty();
        delete leafHeap;
    }

    static void test();

private:
    unsigned int capacity, leaf_num;
    vector<IndexItem*> array;
    BinaryHeap *leafHeap;
    bool  (*comparer)(const IndexItem *a, const IndexItem *b);
    bool  (*heaper)(const IndexItem *a, const IndexItem *b);
};

#endif
