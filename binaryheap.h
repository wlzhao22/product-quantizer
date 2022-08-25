#ifndef HEAP_H
#define HEAP_H

#include "indexitem.h"

#include <iostream>
#include <vector>

using namespace std;


 /******************************************************
 @Author     Wan-Lei Zhao
 @Date       02-Sep-2011
 @Copyrights Reserved by Wan-Lei Zhao

 BinaryHeap

 *****************************************************/


class BinaryHeap
{
public:
    explicit BinaryHeap( const unsigned int capacity0 = 100, const char *compare = "lg");

    bool isEmpty( ) const
    {
        return (array.size() == 0);
    }
    unsigned int size() const
    {
        return array.size();
    }

    bool insert(const unsigned int idx, const float val, const float tmcode);
    bool updateRoot(const unsigned int idx, const float val, const float tmcode);
    bool root(IndexItem &root);

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
    }
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
    ~BinaryHeap()
    {
        this->makeEmpty();
    }

    static void test();

private:
    unsigned int   capacity;
    vector<IndexItem*> array;
    bool  (*comparer)(const IndexItem *a, const IndexItem *b);
    bool  (*heaper)(const IndexItem *a, const IndexItem *b);
};
#endif
