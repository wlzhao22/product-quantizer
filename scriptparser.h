#ifndef SCRIPTPARSER_H
#define SCRIPTPARSER_H

#include <string>
#include <vector>
#include <list>
#include <map>

using namespace std;

/*******************************************************************
@autor  Wanlei Zhao                                                *
@date   May.01.2014                                                *
@email: stonescx@gmail.com                                         *
                                                                   *
This code could not be redistributed without permission of the     *
author should be only used for research purpose                    *
                                                                   *
All rights are reserved by the author                              *
********************************************************************/

class ScriptParser
{

public:
    static const int STRLEN = 512;
	ScriptParser(){}

	static map<string, const char *> getConf(const char *fn);
	static vector<const char*> getNamelst(const char *fname);
	static map<unsigned int, map<string,const char*>* > getItmMaps(const char *fn);

	static void getStrPairs(const char* srcfn, vector<const char*> &strlst1,
                            vector<const char*> &strlst2);

	static void clearParaMap(map<string, const char *> &paras);
	static void clearItmMaps(map<unsigned int, map<string, const char*>* > &itmMaps);
	static bool verifyParaMap(const char *options[], const unsigned int num,
                               map<string, const char *> &paras);

	virtual ~ScriptParser(){}
	static void test();

};

#endif
