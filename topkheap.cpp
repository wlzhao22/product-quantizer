 #include "topkheap.h"

 #include <algorithm>
  #include <cstring>
 #include <cassert>
 #include <cstdlib>
 #include <cmath>

 /******************************************************
 @Author Wan-Lei Zhao
 @Date   02-Sep-2014
 @Copyrights Reserved by Wan-Lei Zhao

 Always keep top-k of the list, and it is also Heap-structured

 *****************************************************/

TopkHeap::TopkHeap(const unsigned int capacity0, const char *comparer0)
{
    this->capacity = capacity0;
    assert(capacity >= 1);

    this->array.reserve(capacity);
    this->leaf_num = capacity - capacity/2;

    if(!strcmp(comparer0, "lg"))
    {
        comparer = &IndexItem::LgComp;
        heaper   = &IndexItem::LtComp;
        leafHeap = new BinaryHeap(leaf_num, "lt");
    }else if(!strcmp(comparer0, "lt"))
    {
        comparer = &IndexItem::LtComp;
        heaper   = &IndexItem::LgComp;
        leafHeap = new BinaryHeap(leaf_num, "lg");
    }else{
        cout<<"Unknown comparer '"<<comparer0<<"'!";
        cout<<"Valid options are 'lg' or 'lt' for BinaryHeap!\n";
        exit(0);
    }
}

bool TopkHeap::insert(const unsigned int idx0, const float val,
                      const unsigned int vkid, long pcodes,
                      const float dp, const float dc)
{
    IndexItem *nw_itm = new IndexItem(idx0, val);
    nw_itm->vkid      = vkid;
    nw_itm->pcode     = pcodes;
    nw_itm->dp        = dp;
    nw_itm->dc        = dc;

    if(this->capacity == this->array.size())
    {
        IndexItem root, *parent, *crnt_itm, tmp_itm;
        leafHeap->root(root);
        if((*comparer)(&root, nw_itm))
        {
            delete nw_itm;
            return false;
        }else{
            int idx = root.index;
            memcpy(array[idx], nw_itm, sizeof(IndexItem));
            crnt_itm = array[idx];
            idx = (idx+1)/2 - 1;
            while(idx >= 0)
            {
                parent = array[idx];
                if((*comparer)(crnt_itm, parent))
                {
                    memcpy(&tmp_itm, parent,   sizeof(IndexItem));
                    memcpy(parent,   crnt_itm, sizeof(IndexItem));
                    memcpy(crnt_itm, &tmp_itm, sizeof(IndexItem));
                    if(idx == 0)
                    {
                        break;
                    }else{
                        idx = (idx+1)/2 - 1;
                        crnt_itm = parent;
                    }
                }else{
                    break;
                }
            }
            idx = root.index;
            leafHeap->updateRoot(idx, array[idx]->val, 0);
            //leafHeap->print();

            delete nw_itm;
            return true;
        }

    }else
    {
        array.push_back(nw_itm);
        if(array.size() == this->capacity)
        {
            make_heap(array.begin(), array.end(), (*heaper));
            int i;
            unsigned int idx = array.size();
            for(i = leaf_num; i > 0; i--)
            {
                idx--;
                leafHeap->insert(idx, array[idx]->val, 0);
            }
            //leafHeap->print();
        }
        return true;
    }
}


bool TopkHeap::insert(const unsigned int idx0, const float val)
{
    IndexItem *nw_itm = new IndexItem(idx0, val);

    if(this->capacity == this->array.size())
    {
        IndexItem root, *parent, *crnt_itm, tmp_itm;
        leafHeap->root(root);
        if((*comparer)(&root, nw_itm))
        {
            delete nw_itm;
            return false;
        }else{
            unsigned int idx = root.index;
            memcpy(array[idx], nw_itm, sizeof(IndexItem));
            crnt_itm = array[idx];
            idx = (idx+1)/2 - 1;
            while(idx >= 0)
            {
                parent = array[idx];
                if((*comparer)(crnt_itm, parent))
                {
                    memcpy(&tmp_itm, parent,   sizeof(IndexItem));
                    memcpy(parent,   crnt_itm, sizeof(IndexItem));
                    memcpy(crnt_itm, &tmp_itm, sizeof(IndexItem));
                    if(idx == 0)
                    {
                        break;
                    }else{
                        idx = (idx+1)/2 - 1;
                        crnt_itm = parent;
                    }
                }else{
                    break;
                }
            }
            idx = root.index;
            leafHeap->updateRoot(idx, array[idx]->val, 0);
            //leafHeap->print();
            delete nw_itm;
            return true;
        }
    }else
    {
        array.push_back(nw_itm);
        if(array.size() == this->capacity)
        {
            make_heap(array.begin(), array.end(), (*heaper));
            int i;
            unsigned int idx = array.size();
            for(i = leaf_num; i > 0; i--)
            {
                idx--;
                leafHeap->insert(idx, array[idx]->val, 0);
            }
            //leafHeap->print();
        }
        return true;
    }
}


void TopkHeap::sort()
{
    stable_sort(array.begin(), array.end(), (*comparer));
}

void TopkHeap::test()
{
    TopkHeap *myheap = new TopkHeap(1, "lt");
    int i, n = 200;
    IndexItem root;
    for(i = 0; i < 200; i++)
    {
        myheap->insert(i, n, 0, 0 ,0, 0);
        n--;
    }
    //myheap->printLeaf();
    /**/
    myheap->insert(i, 0.01,  0, 0, 0, 0); i++;
    myheap->insert(i, 0.00016,  0, 0, 0, 0); i++;
    myheap->insert(i, 0.03,  0, 0, 0, 0); i++;
    /**
    myheap->insert(i, 11.06, 0, 0); i++;
    myheap->insert(i, 300, 0, 0); i++;
    myheap->insert(i, 306, 0, 0); i++;
    myheap->sort();
    **/
    myheap->print();
    cout<<myheap->size()<<endl;
    /**/
    ///myheap->printLeaf();

}
