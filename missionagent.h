#ifndef MISSIONAGENT_H
#define MISSIONAGENT_H

#include <iostream>
#include <string>
#include <map>

using namespace std;

/**
Distribute tasks to different classes, that are
responsible. It is designed for the sake of clarity of
the code.

@author: Wanlei Zhao
@email:  stonescx@gmail.com
@date:   Mar-28-2015

**/

class MissionAgent
{
    public:
        MissionAgent(){}
        virtual ~MissionAgent(){}
        /**visual vocabulary generation**/
        static bool genVocab(map<string, const char*> &paras, const char *vkgenOpt[]);
        /**training tasks**/
        static bool runTrain(map<string, const char*> &paras, const char *trainOpt[]);

        /**encoding vector tasks**/
        static bool runEncode(map<string, const char*> &paras, const char *encodeOpt[]);

        /**fast nnSearch tasks with PQ and HE**/
        static bool runNNsearch(map<string, const char*> &paras, const char *nnsOpt[]);

};

#endif
