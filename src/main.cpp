#include "utils.h"

int main(int argc, const char* argv[]) {
    if(argc > 1) { // 执行文件
        runFile(std::string(argv[1]));
    }
    else { // 执行CLI
        ;
    }
}
