#ifndef TOPK_H
#define TOPK_H

#include "nnitem.h"

class TopK
{
private:
    unsigned topk;
    list<NNItem*> topk_lst;
    list<NNItem*>::reverse_iterator rear;
public:
    TopK(const int k0)
    {
        topk = k0;
        rear = topk_lst.rbegin();
    }

    void insert(const unsigned int idx, const float val)
    {
        if(topk_lst.size() < topk)
        {
            NNItem *crnt_item = new NNItem(idx, val);
            topk_lst.push_front(crnt_item);
            if(topk_lst.size() == topk)
            {
                topk_lst.sort(NNItem::LGcomparer);
                rear = topk_lst.rbegin();
            }
        }
        else
        {
            NNItem *_item = *rear;

            if(_item->val < val)
            {
                NNItem *crnt_item = new NNItem(idx, val);
                list<NNItem*>::iterator lit;
                lit = topk_lst.begin();
                _item = *lit;
                while(_item->val > val && lit != topk_lst.end())
                {
                    lit++;
                    _item = *lit;
                }
                topk_lst.insert(lit, crnt_item);
                lit = topk_lst.end();
                lit--;
                _item = *lit;
                delete _item;
                topk_lst.erase(lit);
                rear = topk_lst.rbegin();
            }
        }
    }

    void clear()
    {
        list<NNItem*>::iterator lit;
        NNItem *_item;
        for(lit = topk_lst.begin(); lit != topk_lst.end(); lit++)
        {
            _item = *lit;
            delete _item;
        }
        topk_lst.clear();
    }

    list<NNItem*>::iterator begin()
    {
        return topk_lst.begin();
    }

    list<NNItem*>::iterator end()
    {
        return topk_lst.end();
    }

    ~TopK()
    {
        list<NNItem*>::iterator lit;
        NNItem *_item;
        for(lit = topk_lst.begin(); lit != topk_lst.end(); lit++)
        {
            _item = *lit;
            delete _item;
        }
        topk_lst.clear();
    }
};

#endif
