#ifndef _CMD_LINE_PARSER_HPP_
#define _CMD_LINE_PARSER_HPP_
    class CmdLineParser(){
    private:
        int argc;
        char **argv;
    public:
        CmdLineParser::CmdLineParser(int _argc,char **_argv);

        bool CmdLineParser::operator[] (string param);

        string CmdLineParser::operator()(string param,string defvalue="-1");
    }
#endif