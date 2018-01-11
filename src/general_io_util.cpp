#include "../include/general_io_util.h"
#include <string.h>

std::string GIOUtil::loadTextFile(std::string path){
    FILE* file;
    std::string text;
    char buffer[256];

    if( (file = fopen(path.c_str(),"r+")) == NULL){
        throw "The file:\" "+path+"\" probably does not exist";
    }

    memset(buffer,0,256);
    while( fgets(buffer,256,file) ){
        text.append(buffer);
        memset(buffer,0,256);
    }
    fclose(file);

    return text;
}
