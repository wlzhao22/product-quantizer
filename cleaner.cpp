#include "cleaner.h"

#include <cassert>

void Cleaner::eraseInvtList(InvtPQCBlock *header)
{
    if(header == NULL)
       return ;

    InvtPQCBlock *crntBlck = header->next;
    InvtPQCBlock *next_block = NULL;

    while(crntBlck != NULL)
    {
        next_block = crntBlck->next;
        delete crntBlck;
        crntBlck = next_block;
    }
}

void Cleaner::eraseInvtList(InvtPQOBlock *header)
{
    if(header == NULL)
       return ;

    InvtPQOBlock *crntBlck = header->next;
    InvtPQOBlock *next_block = NULL;

    while(crntBlck != NULL)
    {
        next_block = crntBlck->next;
        delete crntBlck;
        crntBlck = next_block;
    }
}

void Cleaner::eraseInvtList(InvtPQSBlock *header)
{
    if(header == NULL)
       return ;

    InvtPQSBlock *crntBlck = header->next;
    InvtPQSBlock *next_block = NULL;

    while(crntBlck != NULL)
    {
        next_block = crntBlck->next;
        delete crntBlck;
        crntBlck = next_block;
    }
}

void Cleaner::clearRankList(list<IndexMeta*> *rankList)
{
    if(rankList == NULL)
    return ;

    list<IndexMeta*>::iterator iter;
    IndexMeta *crntItem = NULL;

    for(iter = rankList->begin(); iter != rankList->end(); iter++)
    {
        crntItem = *iter;
        delete crntItem;
    }
    rankList->clear();
}

void Cleaner::clearRankList(vector<IndexMeta*> *rankList)
{
    if(rankList == NULL)
    return ;

    vector<IndexMeta*>::iterator iter;
    IndexMeta *crntItem = NULL;

    for(iter = rankList->begin(); iter != rankList->end(); iter++)
    {
        crntItem = *iter;
        delete crntItem;
    }
    rankList->clear();
}

void Cleaner::clearMatrix(vector<vector<unsigned int> > &matrix)
{
    vector<vector<unsigned int> >::iterator vit;
    for(vit = matrix.begin(); vit != matrix.end(); vit++)
    {
        vector<unsigned int> &crntVect = *vit;
        crntVect.clear();
    }
    matrix.clear();

}

void Cleaner::clearInvtList(InvtBlock *header)
{
    if(header == NULL)
       return ;

    InvtBlock *crntBlck = header->next;
    InvtBlock *next_block = NULL;

    while(crntBlck != NULL)
    {
        next_block = crntBlck->next;
        delete crntBlck;
        crntBlck = next_block;
    }
    header->next = NULL;
}

void Cleaner::freeItemMaps(map<unsigned int, map<string, const char*> *> &itmMaps)
{
    map<unsigned int, map<string,const char*>* >::iterator it;
    map<string, const char*>::iterator iter;
    map<string, const char*> *crntMap;
    string keystr;
    for(it = itmMaps.begin(); it != itmMaps.end(); it++)
    {
        crntMap = it->second;
        for(iter = crntMap->begin(); iter != crntMap->end(); iter++)
        {
            keystr = iter->first;
            /**i am not clear whether we should clear following
            strings by ourselves**/
            /**
            const char *val = iter->second;
            delete [] val;
            keystr.clear();
            **/
        }
        crntMap->clear();
        delete crntMap;
    }
    itmMaps.clear();
}

void Cleaner::clear_i2kMap(map<unsigned int, const char*> &i2kMap)
{
    map<unsigned int,const char*>::iterator mit;

    const char *val;
    for(mit = i2kMap.begin(); mit != i2kMap.end(); mit++)
    {
        val = mit->second;
        delete [] val;
    }
    i2kMap.erase(i2kMap.begin(),i2kMap.end());

}

void Cleaner::freeParaMap(map<string, const char*> &paras)
{
    map<string,const char*>::iterator mit;
    string crntstr;
    const char *val;
    for(mit = paras.begin(); mit != paras.end(); mit++)
    {
        val = mit->second;
        crntstr = mit->first;
        crntstr.erase(crntstr.begin(),crntstr.end());
        delete [] val;
    }
    paras.erase(paras.begin(),paras.end());

}

void Cleaner::freeStrVect(vector<const char*> &str_vect)
{
    const char *crntChrs;
    vector<const char*>::iterator it;
    for(it = str_vect.begin(); it != str_vect.end(); it++)
    {
        crntChrs = *it;
        delete [] crntChrs;
    }
    str_vect.clear();
    return ;
}

void Cleaner::releaseInvtList(map<unsigned int, vector<unsigned int>* > &invtLst)
{
    map<unsigned int, vector<unsigned int>* >::iterator mit;
    vector<unsigned int>* crntlst = NULL;
    for(mit =  invtLst.begin(); mit != invtLst.end(); mit++)
    {
        crntlst = mit->second;
        crntlst->clear();
        delete crntlst;
    }
    invtLst.clear();
}

void Cleaner::releaseInvtList(Hdr_InvtPQCBlock *invertLst, const unsigned int vqNum)
{
    if(invertLst != NULL)
    {
        for(unsigned int i = 0; i < vqNum; i++)
        {
            Cleaner::eraseInvtList(&invertLst[i]);
        }
        delete [] invertLst;
        invertLst = NULL;
    }
}

void Cleaner::releaseInvtList(Hdr_InvtPQOBlock *invertLst, const unsigned int vqNum)
{
    if(invertLst != NULL)
    {
        for(unsigned int i = 0; i < vqNum; i++)
        {
            Cleaner::eraseInvtList(&invertLst[i]);
        }
        delete [] invertLst;
        invertLst = NULL;
    }
}

void Cleaner::releaseInvtList(Hdr_InvtPQSBlock *invertLst, const unsigned int vqNum)
{
    if(invertLst != NULL)
    {
        for(unsigned int i = 0; i < vqNum; i++)
        {
            Cleaner::eraseInvtList(&invertLst[i]);
        }
        delete [] invertLst;
        invertLst = NULL;
    }
}


void Cleaner::clear_k2iMap(map<string, unsigned int> &refTab)
{
    map<string, unsigned int>::iterator mit;
    string key;

    for(mit = refTab.begin(); mit != refTab.end(); mit++)
    {
        key = mit->first;
        key.clear();
    }

    refTab.clear();
}

void Cleaner::clear_k2iMap(map<string, unsigned char> &refTab)
{
    map<string, unsigned char>::iterator mit;
    string key;

    for(mit = refTab.begin(); mit != refTab.end(); mit++)
    {
        key = mit->first;
        key.clear();
    }

    refTab.clear();
}

void Cleaner::clearNNList(list<NNItem*> &nnItems)
{
    NNItem *crnt_itm = NULL;
    list<NNItem*>::iterator itm_iter;

    for(itm_iter = nnItems.begin(); itm_iter != nnItems.end(); itm_iter++)
    {
        crnt_itm = *itm_iter;
        delete crnt_itm;
    }
    nnItems.erase(nnItems.begin(), nnItems.end());
}

void Cleaner::clearRetriList(list<RetriItem*> &nnItems)
{
    RetriItem *crntItm = NULL;
    list<RetriItem*>::iterator itm_iter;

    for(itm_iter = nnItems.begin(); itm_iter != nnItems.end(); itm_iter++)
    {
        crntItm = *itm_iter;
        delete crntItm;
    }
    nnItems.erase(nnItems.begin(), nnItems.end());
}

void Cleaner::clearRetriList(vector<RetriItem*> &nnItems)
{
    RetriItem *crntItm = NULL;
    vector<RetriItem*>::iterator itm_iter;

    for(itm_iter = nnItems.begin(); itm_iter != nnItems.end(); itm_iter++)
    {
        crntItm = *itm_iter;
        delete crntItm;
    }
    nnItems.erase(nnItems.begin(),nnItems.end());
}

void Cleaner::clearNNList(list<MiniNN*> &nnItems)
{
    MiniNN *crnt_itm = NULL;
    list<MiniNN*>::iterator itm_iter;

    for(itm_iter = nnItems.begin(); itm_iter != nnItems.end(); itm_iter++)
    {
        crnt_itm = *itm_iter;
        delete crnt_itm;
    }
    nnItems.erase(nnItems.begin(),nnItems.end());
}

