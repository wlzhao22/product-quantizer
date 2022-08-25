#include "missionagent.h"

#include "pqnnsearch.h"
#include "pqtrainer.h"
#include "pqencoder.h"
#include "vstring.h"

/**
Distribute the tasks

**/

bool MissionAgent::genVocab(map<string, const char*> &paras, const char *vkgenOpt[])
{
    for(unsigned int i = 0; i < 3; i++)
    {
        if(paras.find(vkgenOpt[i]) == paras.end())
        {
            cout<<"Option '"<<vkgenOpt[i]<<"' unset!\n";
            cout<<"pq -vc matrix -d dstfn -k num\n";
            exit(1);
        }
    }
    if(!VString::validatePath(paras["-d"]))
    {
        cout<<"Destine path '"<<paras["-d"]<<"' is invalid!\n";
        exit(0);
    }

    PQTrainer::buildVQuantizer(paras["-vc"], paras["-k"], paras["-d"]);
    return true;
}

bool MissionAgent::runTrain(map<string, const char*> &paras, const char *trainOpt[])
{
    for(unsigned int i = 0; i < 4; i++)
    {
        if(paras.find(trainOpt[i]) == paras.end())
        {
            cout<<"Option '"<<trainOpt[i]<<"' unset!\n";
            cout<<"pq -tc sotu-tc.conf -o mat|pq -s srcdir -d dstfn\n";
            exit(1);
        }
    }

    if(!VString::validatePath(paras["-d"]))
    {
        cout<<"Path '"<<paras["-d"]<<"' is invalid!\n";
        exit(0);
    }

    if(!strcmp(paras["-o"], "pq"))
    {
        if(paras.find("-k") == paras.end())
        {
            cout<<"Required parameter '-k' is missing!\n";
            exit(0);
        }
        if(paras.find("-m") == paras.end())
        {
            cout<<"Required parameter '-m' is missing!\n";
            exit(0);
        }
        unsigned int k = atoi(paras["-k"]); ///codebook size of sub-quantizer
        unsigned int m = atoi(paras["-m"]); ///num of segments
        PQTrainer *mypq = new PQTrainer(paras["-tc"]);
        mypq->buildPQuantizer(paras["-s"], k, m, paras["-d"]);
        delete mypq;
    }else if(!strcmp(paras["-o"], "ivfpq"))
    {
        if(paras.find("-k") == paras.end())
        {
            cout<<"Required parameter '-k' is missing!\n";
            exit(0);
        }
        if(paras.find("-m") == paras.end())
        {
            cout<<"Required parameter '-m' is missing!\n";
            exit(0);
        }
        unsigned int k = atoi(paras["-k"]); ///codebook size of sub-quantizer
        unsigned int m = atoi(paras["-m"]); ///num of segments
        PQTrainer *mypq = new PQTrainer(paras["-tc"]);
        mypq->buildIVFPQuantizer(paras["-s"], k, m, paras["-d"]);
        delete mypq;
    }
    else
    {
        cout<<"Valid options for '-o' are 'he', 'pq', 'mat' and 'avg'\n";
    }
    return true;
}


bool MissionAgent::runEncode(map<string, const char*> &paras, const char *encodeOpt[])
{
    char dtOption[16];

    for(unsigned int i = 0; i < 3; i++)
    {
        if(paras.find(encodeOpt[i]) == paras.end())
        {
            cout<<"Option '"<<encodeOpt[i]<<"' unset!\n";
            cout<<"\tpq -ec pq-ec.conf -s itmtab -o [pq|ivfpq]\n";
            exit(0);
        }
    }

    if(paras.find("-o") != paras.end())
    {
        strcpy(dtOption, paras["-o"]);
    }

    if(!strcmp(dtOption, "pq"))
    {
        PQEncoder *encoder = new PQEncoder(paras["-ec"], "pq");
        encoder->performPQEncodeMat(paras["-i"]);
        delete encoder;
    }else if(!strcmp(dtOption, "ivfpq"))
    {
        PQEncoder *encoder = new PQEncoder(paras["-ec"], "ivfpq");
        encoder->performPQEncodeMat(paras["-i"]);
        delete encoder;
    }else{
        cout<<"Unknown encoding option '"<<dtOption<<"'!\n";
        exit(0);
    }
    return true;
}

bool MissionAgent::runNNsearch(map<string, const char*> &paras, const char *nnsOpt[])
{
    char dtOption[16];

    for(unsigned int i = 0; i < 4; i++)
    {
        if(paras.find(nnsOpt[i]) == paras.end())
        {
            cout<<"Option '"<<nnsOpt[i]<<"' unset!\n";
            cout<<"\tpq -nc conf -q qryfn -o [pqa|pqs] -d dstfn\n";
            exit(0);
        }
    }

    if(paras.find("-o") != paras.end())
    {
        strcpy(dtOption, paras["-o"]);
    }

    if(!VString::validatePath(paras["-d"]))
    {
        cout<<"Destine path '"<<paras["-d"]<<"' is invalid!\n";
        exit(0);
    }

   if(!strcmp(dtOption, "pqa"))
    {
        PQnnSearch *search = new PQnnSearch(paras["-nc"], "pqa");
        search->performIVFADCNNSearch(paras["-q"], paras["-d"]);
        // delete search;
    }
    else if(!strcmp(dtOption, "pqs"))
    {
        PQnnSearch *search = new PQnnSearch(paras["-nc"], "pqs");
        search->performIVFSDCNNSearch(paras["-q"], paras["-d"]);
        // delete search;
    }else{
        cout<<"Unknown NN search option '"<<dtOption<<"'!\n";
        exit(0);
    }

    return true;
}


