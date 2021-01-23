#include "parser.h"
#include "utils.h"
#include "object/obj_string.h"

Parser::Parser(std::shared_ptr<VM> vm, std::string &fileName, std::unique_ptr<std::string> sourceCode, ObjModule* objModule, Parser* parent) {
    this->fileName = fileName;
    this->parent = parent;
    this->vm = vm;
    this->sourceCode = std::move(sourceCode);
    this->sourceCode->push_back('\0');
    this->curToken = Token_t();
    this->preToken = Token_t();
    this->nextIdx = 1;
    this->curChar = this->sourceCode->at(0);
    this->interpolationExpectRightParenNum = 0;
    this->codeSize = this->sourceCode->size();
    this->curModule = objModule;
}

// 判断是关键字还是标志符
TokenType_e Parser::idOrkeyword(std::string str) {
    int idx = 0;
    while(keywordsToken[idx].keyword != "") {
        if(keywordsToken[idx].keyword == str) {
            return keywordsToken[idx].tokenType;
        }
        idx++;
    }

    return TOKEN_ID;
}

// 获取源码下一个字符，不移动
char Parser::getNextChar() {
    ASSERT(nextIdx < codeSize, "outOfCodeSize");
    return sourceCode->at(nextIdx);
}

// 移动到下个字符
void Parser::moveAheadCurChar() {
    ASSERT(nextIdx < codeSize, "outOfCodeSize");
    curChar = sourceCode->at(nextIdx++);
}

// 匹配下一个字符并移动
bool Parser::matchNextChar(char expectedChar) {
    if(getNextChar() == expectedChar) {
        moveAheadCurChar();
        return true;
    }

    return false;
}

// 跳过空白符
void Parser::skipBlanks() {
    // 判断是否是空白符
    while(isspace(curChar)) {
        if(curChar == '\n') {
            curToken.lineNo++;
        }
        moveAheadCurChar();
    }
}

// 跳过一行
void Parser::skipALine() {
    while(curChar != '\0') {
        moveAheadCurChar();
        if(curChar == '\n') {
            curToken.lineNo++;
            moveAheadCurChar();
            break;
        }
    }
}

// 跳过注释
void Parser::skipComment() {
    char nextChar = getNextChar();
    if (curChar == '/') {  // 行注释
        skipALine();
    } else {   // 区块注释
        while(nextChar != '*' && nextChar != '\0') {
            moveAheadCurChar();
            if (curChar == '\n') {
                curToken.lineNo++;
            }
            nextChar = getNextChar();
        }
        if(matchNextChar('*')) {
            if (!matchNextChar('/')) {   //匹配*/
                LEX_ERROR(this, "expect '/' after '*'!");
	        }
            moveAheadCurChar();
        } else {
            LEX_ERROR(this, "expect '*/' before file end!");
        }
    }
    skipBlanks(); //注释之后有可能会有空白字符
}

// 获取当前变量的长度和类型
void Parser::parseId() {
    // isalnum 判断是否是字母或数字
    while(isalnum(curChar) || curChar == '_') {
        moveAheadCurChar();
    }
    curToken.length = nextIdx - curToken.startIdx - 1;
    curToken.tokenType = idOrkeyword(sourceCode->substr(curToken.startIdx, curToken.length));
}

// 处理当前字符串
void Parser::parseString() {
    std::shared_ptr<VM> vm_ptr = (this->vm).lock();
    std::string str;
    while(nextIdx < codeSize) {
        moveAheadCurChar();
        if(curChar == '"') {
            curToken.tokenType = TOKEN_STRING;
            break;
        }
        if(curChar == '\0')
            LEX_ERROR(this, "unterminated string!");
        if(curChar == '%') {
            if (nextIdx < codeSize && !matchNextChar('(')) {
                LEX_ERROR(this, "'%' should followed by '('!");
            }
            if (interpolationExpectRightParenNum > 0) {
                COMPILE_ERROR(this, "sorry, don`t support nest interpolate expression!");
            }
            interpolationExpectRightParenNum = 1;
            curToken.tokenType = TOKEN_STRING_BEFORE_INTERPOLATION;
            break;
        }
        if(curChar == '\\') {
            if(nextIdx < codeSize)
                moveAheadCurChar();
            switch (curChar) {
                case '0': 
                    str.push_back('\0'); 
                    break;
                case 'a': 
                    str.push_back('\a'); 
                    break;
                case 'b': 
                    str.push_back('\b'); 
                    break;
                case 'f':
                    str.push_back('\f'); 
                    break;
                case 'n': 
                    str.push_back('\n'); 
                    break;
                case 'r': 
                    str.push_back('\r'); 
                    break;
                case 't': 
                    str.push_back('\t'); 
                    break;
                case '\"': 
                    str.push_back('\"'); 
                    break;
                case '\'': 
                    str.push_back('\''); 
                    break;
                case '\\': 
                    str.push_back('\\');
                    break;
                default:
                    LEX_ERROR(this, "unsupport escape \\%c", curChar);
                    break;
            }
        } else {
            str.push_back(curChar);
        }
    }
    new ObjString(vm_ptr, str.c_str(), str.size());
}


//解析十六进制数字
void Parser::parseHexNum() {
   while (isxdigit(curChar)) {
      moveAheadCurChar();
   }
}

//解析八进制
void Parser::parseOctNum() {
   while(curChar >= '0' && curChar < '8') {
      moveAheadCurChar();
   }
}

//解析十进制数字
void Parser::parseDecNum() {
   while (isdigit(curChar)) {
      moveAheadCurChar();
   }

   //若有小数点
    if (curChar == '.' && isdigit(getNextChar())) {
        moveAheadCurChar();
        while (isdigit(curChar)) {
            moveAheadCurChar();
        }
   }
}

void Parser::parseNum() {
    if(curChar == '0' && matchNextChar('x')) { // 0x
        parseHexNum();
        this->curToken.length = this->nextIdx - this->curToken.startIdx - 1;
        this->curToken.value = NUM_TO_VALUE(static_cast<double>(
            strtol(sourceCode->substr(this->curToken.startIdx, 
            this->curToken.length).c_str(), nullptr, 16)
            ));
    } else if(curChar == 'O' && isdigit(getNextChar())) { // O
        moveAheadCurChar();
        parseOctNum();
        this->curToken.length = this->nextIdx - this->curToken.startIdx - 1;
        this->curToken.value = NUM_TO_VALUE(static_cast<double>(
            strtol(sourceCode->substr(this->curToken.startIdx, 
            this->curToken.length).c_str(), nullptr, 8)
            ));
    } else {
        parseDecNum();
        this->curToken.length = this->nextIdx - this->curToken.startIdx - 1;
        this->curToken.value = NUM_TO_VALUE(static_cast<double>(
            strtod(sourceCode->substr(this->curToken.startIdx, 
            this->nextIdx - this->curToken.startIdx - 1).c_str(), nullptr)
            ));
    }
    this->curToken.tokenType = TOKEN_NUM;
}

// 获取下一个 Token
void Parser::getNextToken() {
    preToken = curToken;
    skipBlanks();
    curToken = Token_t(TOKEN_EOF, nextIdx - 1, 0, curToken.lineNo);
    while(curChar != '\0') {
        switch(curChar) {
            case ',':
                curToken.tokenType = TOKEN_COMMA;
                break;
            case ':':
                curToken.tokenType = TOKEN_COLON;
                break;
            case '(':
                if (interpolationExpectRightParenNum > 0) {
                    interpolationExpectRightParenNum++;
                }
                curToken.tokenType = TOKEN_LEFT_PAREN;
                break;
            case ')':
                    if (interpolationExpectRightParenNum > 0) {
                    interpolationExpectRightParenNum--;
                    if (interpolationExpectRightParenNum == 0) {
                        parseString();
                        break;
                    }
                }
                curToken.tokenType = TOKEN_RIGHT_PAREN;
                break;
            case '[':
                curToken.tokenType = TOKEN_LEFT_BRACKET;
                break;
            case ']':
                curToken.tokenType = TOKEN_RIGHT_BRACKET;
                break;
            case '{':
                curToken.tokenType = TOKEN_LEFT_BRACE;
                break;
            case '}':
                curToken.tokenType = TOKEN_RIGHT_BRACE;
                break;
            case '.':
                if (matchNextChar('.')) {
                    curToken.tokenType = TOKEN_DOT_DOT;
                } else {
                    curToken.tokenType = TOKEN_DOT;
                }
                break;
            case '=':
                if (matchNextChar('=')) {
                    curToken.tokenType = TOKEN_EQUAL;
                } else {
                    curToken.tokenType = TOKEN_ASSIGN;
                }
                break;
            case '+':
                curToken.tokenType = TOKEN_ADD;
                break;
            case '-':
                curToken.tokenType = TOKEN_SUB;
                break;
            case '*':
                curToken.tokenType = TOKEN_MUL;
                break;
            case '/':
                // 跳过注释 '//'或'/*'
                if (matchNextChar('/') || matchNextChar('*')) {
                    skipComment();

                    // 重置下一个token起始地址
                    curToken.startIdx = nextIdx - 1;
                    continue;
                } else { // 除号
                    curToken.tokenType = TOKEN_DIV;
                }
                break;
            case '%':
                curToken.tokenType = TOKEN_MOD;
                break;
            case '&':
                if (matchNextChar('&')) {
                    curToken.tokenType = TOKEN_LOGIC_AND;
                } else {
                    curToken.tokenType = TOKEN_BIT_AND;
                }
                break;
            case '|':
                if (matchNextChar('|')) {
                    curToken.tokenType = TOKEN_LOGIC_OR;
                } else {
                    curToken.tokenType = TOKEN_BIT_OR;
                }
                break;
            case '~':
                curToken.tokenType = TOKEN_BIT_NOT;
                break;
            case '?':
                curToken.tokenType = TOKEN_QUESTION;
                break;
            case '>':
                if (matchNextChar('=')) {
                    curToken.tokenType = TOKEN_GREATE_EQUAL;
                } else if (matchNextChar('>')) {
                    curToken.tokenType = TOKEN_BIT_SHIFT_RIGHT;
                } else {
                    curToken.tokenType = TOKEN_GREATE;
                }
                break;
            case '<':
                if (matchNextChar('=')) {
                    curToken.tokenType = TOKEN_LESS_EQUAL;
                } else if (matchNextChar('<')) {
                    curToken.tokenType = TOKEN_BIT_SHIFT_LEFT;
                } else {
                    curToken.tokenType = TOKEN_LESS;
                }
                break;
            case '!':
                if (matchNextChar('=')) {
                    curToken.tokenType = TOKEN_NOT_EQUAL;
                } else {
                    curToken.tokenType = TOKEN_LOGIC_NOT;
                }
                break;
            case '"':
                parseString();
                break;
            default:    
                //首字符是字母或'_'则是变量名或者关键字
                if (isalpha(curChar) || curChar == '_') {
                    parseId(); // 解析变量名其余的部分
                } else if(isdigit(curChar)) {
                    parseNum();
                }else {
                    if (curChar == '#' && matchNextChar('!')) {
                        skipALine();
                        curToken.startIdx = nextIdx - 1; // 重置下一个token起始地址
                        continue;
                    } 
                    LEX_ERROR(this, "unsupport char: \'%c\', quit.", curChar);
                }
                return;
        }
        //大部分case的出口
        curToken.length = (uint32_t)(nextIdx - curToken.startIdx);
        moveAheadCurChar();
        return;
    }
}

// 匹配当前 Token，成功读入下一个返回 true，否则返回false
bool Parser::matchToken(TokenType_e expected) {
    if(curToken.tokenType == expected) {
        getNextToken();
        return true;
    } else {
        return false;
    }
}

// 断言当前 Token，匹配则读入下一个 Token
void Parser::consumeCurToken(TokenType_e expected, const char* errMsg) {
    if(curToken.tokenType != expected)
        COMPILE_ERROR(this, errMsg);
    getNextToken();
}

// 读入并断言下一个 Token
void Parser::consumeNextToken(TokenType_e expected, const char* errMsg) {
    getNextToken();
    if(curToken.tokenType != expected)
        COMPILE_ERROR(this, errMsg);
}
