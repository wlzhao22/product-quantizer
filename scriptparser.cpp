#include "scriptparser.h"
#include "vstring.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <list>

using namespace std;

map<string, const char *> ScriptParser::getConf(const char *fn)
{
    assert(fn);
    FILE *fp = fopen(fn, "r");
	if(fp == NULL)
	{
		printf("Configure '%s' cannot be open!\n", fn);
		exit(0);
	}

	map<string, const char *> paras;

    string fst_str = "", sec_str = "";
    char *pairf = NULL, *pairs = NULL, result[STRLEN];
    char *ptStr = NULL;

	while(!feof(fp))
	{
		ptStr = fgets(result, STRLEN, fp);

		if(ptStr == NULL)
		continue;

		if(!strcmp(ptStr, ""))
		continue;

		if(result[0] == '#')
		continue;

		VString::trimEnd(result, '\n');
		VString::trimEnd(result, '\r');
		VString::parsePair(result, fst_str, sec_str, '=');

		pairf = new char[fst_str.length()+1];
		strcpy(pairf, fst_str.c_str());

		pairs = new char[sec_str.length()+1];
		strcpy(pairs, sec_str.c_str());

		paras.insert(pair<const char *, const char*>(pairf, pairs));

		fst_str.erase(fst_str.begin(), fst_str.end());
		sec_str.erase(sec_str.begin(), sec_str.end());
		strcpy(result, "");
	}
	fclose(fp);

	return paras;
}

map<unsigned int, map<string, const char*>* > ScriptParser::getItmMaps(const char *fn)
{
    assert(fn);

    unsigned int index = 0;
    FILE *fp = fopen(fn, "r");
	if(fp == NULL)
	{
		printf("Configure '%s' cannot be open!\n", fn);
		exit(0);
	}

	char result[STRLEN];

	map<unsigned int, map<string, const char*>* > data;
	map<string, const char*> *crnt_para_map;
    string fst_str = "", sec_str = "";
    char *pairf = NULL, *pairs = NULL;
    char *ptStr = NULL;

	while(!feof(fp))
	{
		ptStr = fgets(result, STRLEN, fp);

		if(ptStr == NULL)
		continue;

		if(!strcmp(ptStr, ""))
		continue;

		VString::trimEnd(result, '\n');
        VString::trimEnd(result, '\r');

		if(!strcmp(result, "<item>"))
		{
		    crnt_para_map = new map<string, const char*>;
		    ptStr = fgets(result, STRLEN, fp);

		    if(!strcmp(result, ""))
		    continue;

		    if(result[0] == '#')
		    continue;

		    VString::trimEnd(result, '\n');
		    VString::trimEnd(result, '\r');
		    while(strcmp(result, "</item>"))
		    {
		        VString::parsePair(result, fst_str, sec_str, '=');
		        pairf = new char[fst_str.length()+1];
		        strcpy(pairf, fst_str.c_str());

		        pairs = new char[sec_str.length()+1];
		        strcpy(pairs, sec_str.c_str());
		        crnt_para_map->insert(pair<const char*,const char*>(pairf,pairs));
		        fst_str.erase(fst_str.begin(),fst_str.end());
		        sec_str.erase(sec_str.begin(),sec_str.end());

		        ptStr = fgets(result, STRLEN, fp);

		        if(!strcmp(ptStr, ""))
		        continue;

		        VString::trimEnd(result, '\n');
		        VString::trimEnd(result, '\r');
		    }
		    data.insert(pair<int,map<string,const char*>* >(index, crnt_para_map));
		    index++;
		}
		strcpy(result, "");
	}
	fclose(fp);
	return data;
}

void ScriptParser::getStrPairs(const char *srcfn, vector<const char*> &strlst1,
                                     vector<const char*> &strlst2)
{
    assert(srcfn);
    printf("Read string pairs ........... ");
	FILE *fp = fopen(srcfn, "r");
	if(fp == NULL)
	{
		printf("File '%s' cannot be open!\n", srcfn);
		return ;
	}

	char result[STRLEN];
	char former[STRLEN];
	char latter[STRLEN];

	char *first = NULL, *second = NULL;
	unsigned int i = 0, len1 = 0, len2 = 0;
	int nargs = 0;

	while(!feof(fp))
	{
		nargs = fscanf(fp, "%s\n", result);

		if(nargs == 0)
		continue;

		if((!strcmp(result, "NULL"))||(!strcmp(result, "")))
		{
		      continue;
		}
		else
		{
		    VString::split_twin(former, latter, result, '^');
		    len1   = strlen(former) + 1;
		    len2   = strlen(latter) + 1;
		    first  = new char[len1];
		    second = new char[len2];

		    strcpy(first,  former);
		    strcpy(second, latter);

		    strlst1.push_back(first);
		    strlst2.push_back(second);
		    i++;
		}
		strcpy(result, "");
	}
	printf("%d\n", i);
	fclose(fp);
	return ;
}

vector<const char*> ScriptParser::getNamelst(const char *fname)
{
    assert(fname);
    vector<const char*> kflst;

	FILE *fp = fopen(fname, "r");
	if(fp == NULL)
	{
		printf("File '%s' cannot be open!\n",fname);
		return kflst;
	}

	char result[ScriptParser::STRLEN];
	char *crntKeyFrm = NULL;
	int i = 0, len = 0, nargs = 0;

	while(!feof(fp))
	{
		nargs = fscanf(fp, "%s\n", result);

		if(nargs == 0)
		continue;

		len = strlen(result) + 1;
		if((!strcmp(result, "NULL"))||(!strcmp(result, "")))
		{
		      continue;
		}
		else
		{
             crntKeyFrm = new char[len];
             strcpy(crntKeyFrm, result);
		     kflst.push_back(crntKeyFrm);
		     i++;
		}
		strcpy(result, "");
	}
	fclose(fp);
	return kflst;
}


bool ScriptParser::verifyParaMap(const char *options[], const unsigned int n,
                                map<string, const char*> &paras)
{
    unsigned int i;
    for(i = 0; i < n; i++)
    {
        if(paras.find(options[i]) == paras.end())
        {
            cout<<"Option '"<<options[i]<<"' has not been set!\n";
        }
    }
    return true;
}

void ScriptParser::clearParaMap(map<string, const char *> &paras)
{
    string key;
    const char *val;
    map<string, const char *>::iterator mit;

    for(mit = paras.begin(); mit != paras.end(); mit++)
    {
        key = mit->first;
        key.clear();
        val = mit->second;
        delete [] val;
    }
    paras.clear();
}

void ScriptParser::clearItmMaps(map<unsigned int, map<string, const char*>* > &itmMaps)
{
    unsigned int idx = 0;
    map<string, const char*>* crntMap;
    for(idx  = 0; idx < itmMaps.size(); idx++)
    {
        crntMap = itmMaps[idx];
        ScriptParser::clearParaMap(*crntMap);
        delete crntMap;
    }
    itmMaps.clear();
}

void ScriptParser::test()
{
	vector<const char *> kflst1;
	vector<const char *> kflst2;

	const char *fn    = "e:/wanlei/vcdlab/itmtabs.txt";

	map<unsigned int, map<string, const char*>* > itmMaps;
	itmMaps = ScriptParser::getItmMaps(fn);

	map<unsigned int, map<string, const char*>* >::iterator it;
	map<string, const char*>::iterator iter;
	map<string, const char*> *crntMap;
	string keystr;
	for(it = itmMaps.begin(); it != itmMaps.end(); it++)
	{
	    cout<<it->first<<endl;
	    crntMap = it->second;
	    for(iter = crntMap->begin(); iter != crntMap->end(); iter++)
	    {
	        cout<<iter->first<<"\t"<<iter->second<<endl;
	        keystr =  iter->first;
	        delete [] iter->second;
	        keystr.clear();
	    }
	    cout<<endl;
	    crntMap->clear();
	    delete crntMap;
	}
	itmMaps.clear();
}
