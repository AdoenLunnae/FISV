#ifndef _CMD_LINE_PARSER_HPP_
#define _CMD_LINE_PARSER_HPP_
#include <string>
    class CmdLineParser{
    private:
        int argc;
        char **argv;
    public:
        CmdLineParser(int _argc,char **_argv);

        bool operator[] (std::string param);

        std::string operator()(std::string param,std::string defvalue="-1");
    };
#endif