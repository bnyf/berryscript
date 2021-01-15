#include "utils.h"
#include "vm.h"
#include "parser.h"

std::string rootDir;

// 返回大于v的最小的2的幂
uint32_t ceilToPowerOf2(uint32_t v) {
    v += (v == 0);
    v--;
    // 将二进制v中从低位到高位，出现的第一个 '1'后面的位全置为'1'
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;

    return v;
}

std::string* readFile(const std::string &filename) {
    std::ifstream infile(filename);
    ASSERT(infile.is_open(), "Couldn't open file");
    std::string *content(new std::string((std::istreambuf_iterator<char>(infile)), // input iterator, 指向 infile 文件流中的第一个字节
            std::istreambuf_iterator<char>())); // input iterator的默认构造函数，指向 end
    infile.close();
    
    return content;
}

// 读取指定文件内容到 string 中
void runFile(std::string &filename) {
    size_t idx = filename.rfind('/');
    if(idx != std::string::npos) {
        rootDir = filename.substr(idx);
    }

    std::shared_ptr<VM> vm(new VM);

    std::unique_ptr<std::string>content(readFile(filename));
    if(content->size() == 0)
        return;
    vm->curParser = std::make_shared<Parser>(vm, filename, std::move(content));
    std::shared_ptr<Parser> &parser = vm->curParser;

    // 测试词法分析
    #include "token.list"
    while(parser->curToken.tokenType != TOKEN_EOF) {
        parser->getNextToken();
        Token_t curToken = parser->curToken;
        std::cout << curToken.lineNo << "L: " << \
        tokenArray[curToken.tokenType] << " [" << \
        parser->sourceCode->substr(curToken.startIdx, curToken.length) << "]\n";
    }
}

// 通用报错函数
void errorReport(Parser *parser, 
      ErrorType errorType, const char* fmt, ...) { //... 为可变参数
   char buffer[DEFAULT_BUFFER_SIZE] = {'\0'};
   va_list ap; // char *
   va_start(ap, fmt); //获取第一个可变参数的地址赋值给 ap
   vsnprintf(buffer, DEFAULT_BUFFER_SIZE, fmt, ap); //将可变参数ap，通过格式化字符串fmt，写入buffer
   va_end(ap); // 空函数，保持与 va_start 的成对出现

   switch (errorType) {
      case ERROR_IO:
      case ERROR_MEM:
	 fprintf(stderr, "%s:%d In function %s():%s\n",
	       __FILE__, __LINE__, __func__, buffer);
	 break;
      case ERROR_LEX:
      case ERROR_COMPILE:
	 fprintf(stderr, "%s:%d \"%s\"\n", parser->fileName.c_str(),
	       parser->preToken.lineNo, buffer);
	 break;
      case ERROR_RUNTIME:
	    fprintf(stderr, "%s\n", buffer);
	 break;
      default:
	    NOT_REACHED();
   }
   exit(1); // 非 0 为异常退出
}
