#ifndef STRINGMAGIC_H_INCLUDED
#define STRINGMAGIC_H_INCLUDED

#include <stdafx.h>

std::string trim(std::string& s,const std::string& drop = " \r\n");
void tokenize(const string& str, vector<string>& tokens,const string& delimiters  = " ");

#endif // STRINGMAGIC_H_INCLUDED
