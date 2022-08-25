#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>

#include "scriptparser.h"
#include "binaryheap.h"
#include "kppmeans.h"
#include "pqtrainer.h"
#include "pqencoder.h"
#include "topkheap.h"
#include "vstring.h"
#include "topk.h"

#include "invtflloader.h"
#include "pqnnsearch.h"

#include "hardwaresetup.h"
#include "missionagent.h"

#include "timer.h"

#include <unistd.h>
#include <iomanip>
#include <thread>

using namespace std;

void help()
{
    const char *version = "Version 1.10 Release (2014-2015)";
    const char *contact = "Email to Wan-Lei Zhao, stonescx@gmail.com";

    cout<<" Usage: pq [-vc|-ec|-nc|-tc] conf <detailed options>\n";
    cout<<"\tOptions (exclusive) '-vc', '-ec', '-nc' and '-tc'\n\n";

    cout<<" Build visual vocabulary (-vc)\n";
    cout<<"\tpq -vc matrixfile -d dstvocab -k num\n";
    cout<<"\t-vc\tSource matrix (features sampled randomly)\n";
    cout<<"\t-d\tDestine vocabulary file\n";
    cout<<"\t-k\tNum. of visual words\n\n";

    cout<<" Encode for nearest neighbor search (-ec)\n";
    cout<<"\tpq -ec conf -i itmtab -o [pq|ivfpq]\n";
    cout<<"\t-ec\tConfiguration file for encoding\n";
    cout<<"\t-i\titem table (source and destine file paths) list\n";
    cout<<"\t-o\tProduct-quantizer (pq), IVF product-quantizer (ivfpq)\n\n";

    cout<<" Nearest neighbor search (-nc)\n";
    cout<<"\tpq -nc conf -q qryfn -o [pqs|pqa] -d dstfn\n";
    cout<<"\t-nc\tConfiguration file for NN search\n";
    cout<<"\t-q\tQuery file\n";
    cout<<"\t-o\tSymmetric PQ (pqs), Asymmetric PQ (pqa)\n";
    cout<<"\t-d\tDestine file for nn search results\n\n";

    cout<<" Training tasks (-tc)\n";
    cout<<"\tpq -tc conf -s srcdir|srcfn -o [pq|ivfpq] -m seg -k size -d dstfn\n";
    cout<<"\t-s\tSource dir for feature files used for training\n";
    cout<<"\t-o pq\tTrain product-quantizer\n";
    cout<<"\t-d\tDestine file for the trained results\n\n";
    cout<<" Copyright notice (-c)\n";
    cout<<"\tpq -c\n";

    cout<<"\n\t"<<version<<endl;
    cout<<"\t"<<contact<<endl;
}

void license()
{
    cout<<" Xiamen University, China\n";
    cout<<" Author: Wan-Lei Zhao\n";
}

void test()
{
    PQnnSearch::test();

    ///PQEncoder::test();

    ///PQTrainer::test();

    ///FeatureCollector::test();
    ///HardWareSetup::test();
    ///ScriptParser::test();
    ///InvtFLLoader::test();
    ///IODelegator::test();
    ///BinaryHeap::test();
    ///KppMeans::test();
    ///TopkHeap::test();

    ///VString::test();
    ///KeyPIO::test();
    ///Timer::test();

    ///license();
    return ;
}

int main(int argc, const char *argv[])
{
    /**
    test();
    return 0;
    /**/
    const char *vkgen_opt[3] = {"-vc", "-d", "-k"};
    const char *train_opt[6] = {"-tc", "-d", "-o",  "-s", "-m", "-k"};
    const char *nnqry_opt[4] = {"-nc", "-d", "-o",  "-q"};
    const char *endcd_opt[3] = {"-ec", "-i", "-o"};

    map<string, const char*> paras;

    if(argc < 5)
    {
        if(argc > 1 && !strcmp(argv[1], "-c"))
        {
            license();
        }
        else
        {
            help();
        }
        return 0;
    }

    for(int i = 1; i < argc; i = i + 2)
    {
        paras.insert(pair<string, const char*>(argv[i], argv[i+1]));
    }
    cout<<endl;

    if(paras.find("-vc") != paras.end())
    {
        MissionAgent::genVocab(paras, vkgen_opt);
    }
    else if(paras.find("-tc") != paras.end())
    {
        MissionAgent::runTrain(paras, train_opt);
    }
    else if(paras.find("-ec") != paras.end())
    {
        MissionAgent::runEncode(paras, endcd_opt);
    }
    else if(paras.find("-nc") != paras.end())
    {
        MissionAgent::runNNsearch(paras, nnqry_opt);
    }
    else
    {
        cout<< "No valid option is selected\n";
    }
    return 0;
}
