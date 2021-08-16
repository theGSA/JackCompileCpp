#include "stdafx.h"
#include <dirent.h>
#include <iostream>
#include "JackCompiler.h"
#include "CompileEngine.h"


JackCompiler::JackCompiler(int arc, char* argv[])
{
    DIR* dir = opendir(argv[1]);
    std::vector<std::string> fileList;

    if (dir)
    {
        struct dirent* file;
        while ((file = readdir(dir)) != NULL)
        {           
            if(file->d_type != S_IFDIR) {
                std::cout << (file->d_name) << " File" << std::endl;
                if (strstr(file->d_name, ".jack"))
                {
                    std::cout << "compilando" << " " << file->d_name << std::endl;
                    
                    CompileEngine sintaxer(argv[1] + std::string("\\")+std::string(file->d_name));
                    fileList.push_back(file->d_name);
                }
            }
        }
    }
    else
    {
        std::cout << "Error opening directory" << argv[0] << std::endl;
    }

    for (unsigned i = 0; i < fileList.size(); i++)
        std::cout << "compilou " << fileList[i] << std::endl;
}

JackCompiler::~JackCompiler()
{
}