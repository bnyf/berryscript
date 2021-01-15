#include "utils.h"

int main(int argc, const char* argv[]) {
    if(argc > 1) { // 执行文件
        std::string filename(argv[1]);
        runFile(filename);
    }
    else { // 执行CLI
        ;
    }
}
