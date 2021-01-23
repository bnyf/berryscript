#ifndef _PARSER_H
#define _PARSER_H
#include <cstdint>
#include <cctype>
#include <string>
#include "object/header.h"
#include "object/obj_class.h"

class VM;
template<class T>
class DataBuffer;

typedef enum {
   TOKEN_UNKNOWN,

   // 数据类型
   TOKEN_NUM,		       //数字
   TOKEN_STRING,     	   //字符串
   TOKEN_ID,	     	   //变量名 
   TOKEN_STRING_BEFORE_INTERPOLATION,     //内嵌表达式前的字符串

   // 关键字(系统保留字)
   TOKEN_VAR,		   //'var'
   TOKEN_FUN,		   //'fun'
   TOKEN_IF,		   //'if'
   TOKEN_ELSE,	     	   //'else'	
   TOKEN_TRUE,	     	   //'true'
   TOKEN_FALSE,	     	   //'false'
   TOKEN_WHILE,	     	   //'while'
   TOKEN_FOR,	     	   //'for'
   TOKEN_BREAK,	     	   //'break'
   TOKEN_CONTINUE,         //'continue'
   TOKEN_RETURN,     	   //'return'
   TOKEN_NULL,	     	   //'null'

   //以下是关于类和模块导入的token
   TOKEN_CLASS,	     	   //'class'
   TOKEN_THIS,	     	   //'this'
   TOKEN_STATIC,     	   //'static'
   TOKEN_IS,		   // 'is'
   TOKEN_SUPER,	     	   //'super'
   TOKEN_IMPORT,     	   //'import'

   //分隔符
   TOKEN_COMMA,		   //','
   TOKEN_COLON,		   //':'
   TOKEN_LEFT_PAREN,	   //'('
   TOKEN_RIGHT_PAREN,	   //')'
   TOKEN_LEFT_BRACKET,	   //'['
   TOKEN_RIGHT_BRACKET,	   //']'
   TOKEN_LEFT_BRACE,	   //'{'
   TOKEN_RIGHT_BRACE,	   //'}'
   TOKEN_DOT,		   //'.'
   TOKEN_DOT_DOT,	   //'..'
   
   //简单双目运算符
   TOKEN_ADD,		   //'+'
   TOKEN_SUB,		   //'-'
   TOKEN_MUL,		   //'*' 
   TOKEN_DIV,		   //'/' 
   TOKEN_MOD,		   //'%'

   //赋值运算符
   TOKEN_ASSIGN,	   //'='

   // 位运算符
   TOKEN_BIT_AND,	   //'&'
   TOKEN_BIT_OR,	   //'|'
   TOKEN_BIT_NOT,	   //'~'
   TOKEN_BIT_SHIFT_RIGHT,  //'>>'
   TOKEN_BIT_SHIFT_LEFT,   //'<<'

   // 逻辑运算符
   TOKEN_LOGIC_AND,	   //'&&'
   TOKEN_LOGIC_OR,	   //'||'
   TOKEN_LOGIC_NOT,	   //'!'

   //关系操作符
   TOKEN_EQUAL,		   //'=='
   TOKEN_NOT_EQUAL,	   //'!='
   TOKEN_GREATE,	   //'>'
   TOKEN_GREATE_EQUAL,	   //'>='
   TOKEN_LESS,		   //'<'
   TOKEN_LESS_EQUAL,	   //'<='

   TOKEN_QUESTION,	   //'?'

   //文件结束标记,仅词法分析时使用
   TOKEN_EOF		   //'EOF'
} TokenType_e;

typedef struct Token_s{
    Token_s(TokenType_e _tokenType = TOKEN_UNKNOWN, uint32_t _startIdx = 0, \
    uint32_t _length = 0, uint32_t _lineNo = 1) \
    : tokenType(_tokenType), startIdx(_startIdx), length(_length), lineNo(_lineNo) {}

    TokenType_e tokenType;
    uint32_t startIdx; //token 起始位置下标
    uint32_t length;
    uint32_t lineNo;
    Value value;
} Token_t;

//关键字(保留字)结构体
typedef struct keywordToken_s{
    keywordToken_s(std::string _keyword = "", uint32_t _length = 0, \
    TokenType_e _tokenType = TOKEN_UNKNOWN) \
    : keyword(_keyword), length(_length), tokenType(_tokenType){}

    std::string keyword;
    uint32_t     length;
    TokenType_e   tokenType;
} keywordToken_t;

const int keywordTokenNum = 18;

//关键字查找表
const keywordToken_t keywordsToken[] = {
   keywordToken_t("var",	  3,	TOKEN_VAR), 
   keywordToken_t("fun",	  3,	TOKEN_FUN), 
   keywordToken_t("if",	  2,	TOKEN_IF), 
   keywordToken_t("else",	  4,  	TOKEN_ELSE),
   keywordToken_t("true",	  4,  	TOKEN_TRUE), 
   keywordToken_t("false",	  5,  	TOKEN_FALSE), 
   keywordToken_t("while",	  5,  	TOKEN_WHILE), 
   keywordToken_t("for",	  3,  	TOKEN_FOR), 
   keywordToken_t("break",	  5,  	TOKEN_BREAK), 
   keywordToken_t("continue",   8,    TOKEN_CONTINUE),
   keywordToken_t("return",	  6,  	TOKEN_RETURN), 
   keywordToken_t("null",	  4,  	TOKEN_NULL), 
   keywordToken_t("class",	  5,  	TOKEN_CLASS),
   keywordToken_t("is",	  2,  	TOKEN_IS),
   keywordToken_t("static",	  6,  	TOKEN_STATIC),
   keywordToken_t("this",	  4,  	TOKEN_THIS),
   keywordToken_t("super",	  5,  	TOKEN_SUPER),
   keywordToken_t("import",	  6,  	TOKEN_IMPORT),
   keywordToken_t()
};

class Parser {
    public:
        std::string fileName; // 文件名
        std::unique_ptr<std::string> sourceCode; // 源码串
        Token_t curToken; // 当前 Token
        Token_t preToken; // 上一个 Token
        ObjModule* curModule;

        Parser(std::shared_ptr<VM> vm, std::string &fileName, std::unique_ptr<std::string> sourceCode, ObjModule *objmodule = nullptr, Parser* parent = nullptr);
        void getNextToken(); 
        void consumeCurToken(TokenType_e expected, const char* errMsg);
        void consumeNextToken(TokenType_e expected, const char* errMsg);

    private:
        uint32_t nextIdx; // sourceCode 中的当前字符下标
        char curChar; // sourceCode 中的下一个字符
        int interpolationExpectRightParenNum; // 用于内嵌表达式的括号匹配
        uint32_t codeSize;
        Parser *parent;  // 指向父parser
        std::weak_ptr<VM> vm;

        char getNextChar(); 
        bool matchToken(TokenType_e expected);

        static TokenType_e idOrkeyword(std::string str); 
        void moveAheadCurChar();
        bool matchNextChar(char expectedChar);
        void skipBlanks();
        void parseId(); 
        void parseString();// 解析字符串
        void parseNum();
        void skipALine();
        void skipComment();
        void parseHexNum();
        void parseOctNum();
        void parseDecNum();

};

#endif
