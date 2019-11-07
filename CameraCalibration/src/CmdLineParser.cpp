#include "CmdLineParser.hpp"
#include <string>

using std::string;

CmdLineParser::CmdLineParser(int _argc,char **_argv):argc(_argc),argv(_argv){};

bool CmdLineParser::operator[] (string param){
    int idx=-1;
    for(int i=0; i<argc && idx==-1; i++){
        if(string(argv[i]) == param) idx=i;
    }   
    return(idx!=-1);
}

string CmdLineParser::operator()(string param,string defvalue){
    int idx=-1;
    for(int i=0; i<argc && idx==-1; i++)
        if (string (argv[i]) == param ) idx=i;
    return (idx == -1)?defvalue:argv[idx+1];
}
