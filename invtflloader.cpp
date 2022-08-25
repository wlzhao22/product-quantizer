#include "invtflloader.h"

#include "scriptparser.h"
#include "iodelegator.h"
#include "cleaner.h"

#include <unordered_map>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>

using namespace std;


/**load IVF+PQ codes for IVFADC search**/
unsigned long InvtFLLoader::load_ivfpq_InvtF(Hdr_InvtPQBlock *invtTable, unsigned int *offsets,  map<unsigned int, map<string, const char *>* > &itmMaps,
        const int vqNum, const unsigned int s0)
{
    cout<<"Building Inverted File ........... PQ\n";
    if(s0 > 16)
    {
        cout<<"PQ only supports no more than 16 bytes code!!!\n";
        exit(0);
    }

    InvtPQBlock *crntBlock = NULL;
    InvtPQUnit  *crntUnit  = NULL;

    unsigned long kpid = 1, ptNum = 0;
    map<string, const char*> *crntParaMap;
    unsigned int   i = 0, nTab = itmMaps.size();
    unsigned int   tidx = 0;
    unsigned char *pCodes = NULL;
    unsigned int  *codes = new unsigned int[s0];
    long wId = -1;

    char crntInvtFn[FNLEN];
    ifstream *inStrm = NULL;

    for(tidx = 0; tidx < nTab; tidx++)
    {
        crntParaMap = itmMaps[tidx];
        strcpy(crntInvtFn, (*crntParaMap)["pqctab"]);
        inStrm = new ifstream(crntInvtFn);

        if(inStrm->fail())
        {
            cout<<"Read Inverted File '"<<(*crntParaMap)["pqctab"]<<"' fail!"<<endl;
            delete inStrm;
            continue;
        }
        while(!inStrm->eof())
        {
            (*inStrm)>>wId;
            if(wId == -1)
                continue;

            for(i = 0; i < s0; i++)
            {
                (*inStrm)>>codes[i];
            }

            Hdr_InvtPQBlock &headerpt = invtTable[wId];
            headerpt.length = headerpt.length + 1;
            crntBlock = headerpt.next;

            if(headerpt.uIdx == InvtFLLoader::BLK_SIZE || crntBlock == NULL)
            {
                crntBlock = new InvtPQBlock(InvtFLLoader::BLK_SIZE);
                crntBlock->next = headerpt.next;
                headerpt.next = crntBlock;
                crntBlock->length = InvtFLLoader::BLK_SIZE;
                headerpt.uIdx = 0;
            }
            crntUnit = (*crntBlock)[crntBlock->uIdx];
            crntBlock->uIdx = crntBlock->uIdx +  1;
            crntUnit->Id = kpid;
            if(s0 <= 8)
            {
                pCodes = (unsigned char*)&(crntUnit->lw64);
                for(i = 0; i < s0; i++)
                {
                    (*pCodes) = codes[i];
                    pCodes++;
                }
            }
            else
            {
                pCodes = (unsigned char*)&(crntUnit->lw64);
                for(i = 0; i < 8; i++)
                {
                    (*pCodes) = codes[i];
                    pCodes++;
                }
                pCodes = (unsigned char*)&(crntUnit->up64);
                for(; i < s0; i++)
                {
                    (*pCodes) = codes[i];
                    pCodes++;
                }
            }
            headerpt.uIdx++;
            if(!inStrm->eof())
            {
                cout<<"\r\r\r\t"<<kpid;
                kpid++;
            }
            wId = -1;
        }///while(inStrm)
        inStrm->close();
        delete inStrm;
        cout<<endl;
    }///for(tidx)
    ptNum = kpid - 1;

    delete [] codes;
    codes = NULL;

    return ptNum;
}

/**load IVF+PQ codes for IVFADC search**/
unsigned long InvtFLLoader::load_ivfpq_InvtF(Hdr_InvtPQCBlock *invtTable, unsigned int *offsets,  map<unsigned int, map<string, const char *>* > &itmMaps,
        const int vqNum, const unsigned int dv0)
{
    cout<<"Building Inverted File ........... PQC\n";

    InvtPQCBlock *crntBlock = NULL;
    InvtPQCUnit  *crntUnit  = NULL;

    unsigned long kpid = 1, ptNum = 0;
    map<string, const char*> *crntParaMap;
    unsigned int   i = 0, nTab = itmMaps.size();
    unsigned int   tidx = 0;
    unsigned char *pCodes = NULL;
    unsigned int  *codes = new unsigned int[dv0];
    long wId = -1;

    char crntInvtFn[FNLEN];
    ifstream *inStrm = NULL;

    for(tidx = 0; tidx < nTab; tidx++)
    {
        crntParaMap = itmMaps[tidx];
        strcpy(crntInvtFn, (*crntParaMap)["pqctab"]);
        inStrm = new ifstream(crntInvtFn);

        if(inStrm->fail())
        {
            cout<<"Read Inverted File '"<<(*crntParaMap)["pqctab"]<<"' fail!"<<endl;
            delete inStrm;
            continue;
        }
        while(!inStrm->eof()) //
        {
            (*inStrm)>>wId;
            if(wId == -1)
                continue;
            for(i = 0; i < dv0; i++)
            {
                (*inStrm)>>codes[i];
            }

            Hdr_InvtPQCBlock &headerpt = invtTable[wId];
            crntBlock = headerpt.next;

            if(headerpt.uIdx == InvtFLLoader::BLK_SIZE || crntBlock == NULL)
            {
                      crntBlock = new InvtPQCBlock(InvtFLLoader::BLK_SIZE, dv0);
                crntBlock->next = headerpt.next;
                  headerpt.next = crntBlock;
              crntBlock->length = InvtFLLoader::BLK_SIZE;
                  headerpt.uIdx = 0;
            }
            crntUnit = (*crntBlock)[crntBlock->uIdx];
            crntBlock->uIdx = crntBlock->uIdx +  1;
            crntUnit->Id = kpid;
            pCodes = crntUnit->pqc;
            for(i = 0; i < dv0; i++)
            {
                (*pCodes) = codes[i];
                pCodes++;
            }
            headerpt.uIdx++;
            if(!inStrm->eof())
            {
                cout<<"\r\r\r\t"<<kpid;
                kpid++;
            }
            wId = -1;
        }///while(inStrm)
        inStrm->close();
        delete inStrm;
        cout<<endl;
    }///for(tidx)
    ptNum = kpid - 1;

    delete [] codes;
    codes = NULL;

    return ptNum;
}


unsigned long InvtFLLoader::load_ivfpq_InvtF(Hdr_InvtPQSBlock *invtTable,   unsigned int *offsets, map<unsigned int, map<string, const char *>* > &itmMaps,
        const int vqNum, const unsigned int s0, const unsigned int dv0)
{
    cout<<"Building Inverted File ........... FVLAD-PQC\n";
    InvtPQSBlock *crntBlock = NULL;
    InvtPQSUnit  *crntUnit  = NULL;

    unsigned long kpid = 1, ptNum = 0;
    map<string, const char*> *crntParaMap = NULL;
    unsigned int   i = 0, nTab = itmMaps.size();
    unsigned int   tidx = 0, is = 0, nCd = s0/dv0;
    unsigned int  *codes = new unsigned int[nCd];
    unsigned char *pCodes = NULL;
    long wId = -1;

    assert(nCd > 0);

    char crntInvtFn[FNLEN];
    ifstream *inStrm = NULL;

    for(tidx = 0; tidx < nTab; tidx++)
    {
        crntParaMap = itmMaps[tidx];
        strcpy(crntInvtFn, (*crntParaMap)["pqctab"]);
        inStrm = new ifstream(crntInvtFn);

        if(inStrm->fail())
        {
            cout<<"Read Inverted File '"<<(*crntParaMap)["pqctab"]<<"' fail!"<<endl;
            delete inStrm;
            continue;
        }
        while(!inStrm->eof())
        {
           (*inStrm)>>wId;

           if(wId == -1)
              continue;

            for(is = 0; is < s0; is++)
            {
                for(i = 0; i < nCd; i++)
                {
                    (*inStrm)>>codes[i];
                    ///cout<<codes[i]<<" ";
                }
                      wId = vqNum*is + wId;
                Hdr_InvtPQSBlock &headerpt = invtTable[wId];
                crntBlock = headerpt.next;

                if(headerpt.uIdx == InvtFLLoader::BLK_SIZE || crntBlock == NULL)
                {
                          crntBlock = new InvtPQSBlock(InvtFLLoader::BLK_SIZE);
                    crntBlock->next = headerpt.next;
                      headerpt.next = crntBlock;
                      headerpt.uIdx = 0;
                  crntBlock->length = InvtFLLoader::BLK_SIZE;
                }
                       crntUnit = (*crntBlock)[crntBlock->uIdx];
                crntBlock->uIdx = crntBlock->uIdx +  1;
                   crntUnit->Id = kpid;
                         pCodes = (unsigned char*)&(crntUnit->pqc);

                for(i = 0; i < nCd; i++)
                {
                    (*pCodes) = codes[i];
                    pCodes++;
                }
                headerpt.uIdx++;
                wId = -1;
            }//for(is)
            ///cout<<endl;
            ///exit(0);
            if(!inStrm->eof())
            {
                cout<<"\r\r\r\t"<<kpid;
                kpid++;
            }
        }///while(inStrm)
        inStrm->close();
        delete inStrm;
        inStrm = NULL;
        cout<<endl;
    }///for(tidx)
    ptNum = kpid - 1;

    delete [] codes;
    codes = NULL;

    return ptNum;
}
/**load PQ codes for ADC search**/
unsigned char* InvtFLLoader::load_pq_Codes(map<unsigned int, map<string, const char *>* > &itmMaps, unsigned long &refImgSz, unsigned int &dim)
{
    cout<<"Loading PQ codes file ........... PQ\n";
    unsigned int Num = 0, Dim = 0, i = 0, tidx = 0, j = 0;;
    unsigned char * pqCodes = NULL, pqcode = 0;
    map<string, const char*> *crntParaMap;
    unsigned long ptNum = 0, imgID = 0;
    unsigned int  nTab = itmMaps.size();
    unsigned long pp = 0;

    char crntInvtFn[FNLEN];
    ifstream *inStrm = NULL;

    for(tidx = 0; tidx < nTab; tidx++)
    {
        crntParaMap =  itmMaps[tidx];
        strcpy(crntInvtFn, (*crntParaMap)["pqctab"]);
        inStrm = new ifstream(crntInvtFn);

        if(inStrm->fail())
        {
            cout<<"Read inverted File '"<<(*crntParaMap)["pqctab"]<<"' fail!"<<endl;
            delete inStrm;
            continue;
        }
        (*inStrm)>>Num; (*inStrm)>>Dim;
        ptNum = ptNum + Num;
        inStrm->close();
    }

    pqCodes = new unsigned char[ptNum*Dim];

    for(tidx = 0; tidx < nTab; tidx++)
    {
        crntParaMap = itmMaps[tidx];
        strcpy(crntInvtFn, (*crntParaMap)["bowtab"]);
        inStrm = new ifstream(crntInvtFn);

        if(inStrm->fail())
        {
            cout<<"Read Inverted File '"<<(*crntParaMap)["bowtab"]<<"' fail!"<<endl;
            delete inStrm;
            continue;
        }
        Num = 0;
        if(!inStrm->eof())
        {
            (*inStrm)>>Num; (*inStrm)>>Dim;
            ptNum = ptNum + Num;

            for(i = 0; i < Num && !inStrm->eof(); i++)
            {
                for(j = 0; j < Dim; j++, pp++)
                {
                    (*inStrm)>>pqcode;
                    pqCodes[pp] = pqcode;
                }///for(j)
            }///for(i)
            imgID += Num;
            cout<<"\r\r\r\t"<<imgID;
        }///if(inStrm)
        inStrm->close();
        delete inStrm;
        cout<<endl;
    }///for(tidx)

    dim = Dim;
    refImgSz = imgID;
    return pqCodes;
}

void InvtFLLoader::test()
{

}

