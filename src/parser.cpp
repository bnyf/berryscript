#include "parser.h"
#include "dataBuffer.h"
#include "utils.h"

Parser::Parser(std::weak_ptr<VM> vm, std::string fileName, std::string *sourceCode, Parser* parent) {
    this->fileName = fileName;
    this->parent = parent;
    this->vm = vm;
    this->sourceCode = sourceCode;

    this->curToken = Token_t();
    this->preToken = Token_t();
    this->nextIdx = 1;
    this->curChar = sourceCode->at(0);
    this->interpolationExpectRightParenNum = 0;
    this->codeSize = sourceCode->size();
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

// 获取源码下一个字符
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
bool Parser::skipBlanks() {
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
    while(nextIdx < codeSize) {
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
    if (curChar == '/') {  // 行注释
        skipALine();
    } else {   // 区块注释
        do {
            while(nextIdx < codeSize &&  getNextChar() != '*') {
                moveAheadCurChar();
                if (curChar == '\n') {
                    curToken.lineNo++;
                }
            }
            moveAheadCurChar();
        } while(matchNextChar('\\'));
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
    DataBuffer<char> str;
    while(nextIdx < codeSize) {
        moveAheadCurChar();
        if(curChar == '"') {
            curToken.tokenType = TOKEN_STRING;
            break;
        }
        if(curChar == '%') {
            if (nextIdx < codeSize && !matchNextChar('(')) {
                LEX_ERROR(*this, "'%' should followed by '('!");
            }
            if (interpolationExpectRightParenNum > 0) {
                COMPILE_ERROR(*this, "sorry, don`t support nest interpolate expression!");
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
                    str.bufferAdd('\0'); 
                    break;
                case 'a': 
                    str.bufferAdd('\a'); 
                    break;
                case 'b': 
                    str.bufferAdd('\b'); 
                    break;
                case 'f':
                    str.bufferAdd('\f'); 
                    break;
                case 'n': 
                    str.bufferAdd('\n'); 
                    break;
                case 'r': 
                    str.bufferAdd('\r'); 
                    break;
                case 't': 
                    str.bufferAdd('\t'); 
                break;
                case '\"': 
                    str.bufferAdd('\"'); 
                    break;
                case '\'': 
                    str.bufferAdd('\''); 
                    break;
                case '\\': 
                    str.bufferAdd('\\');
                    break;
                default:
                    LEX_ERROR(*this, "unsupport escape \\%c", curChar);
                break;
            }
        } else {
            str.bufferAdd(curChar);
        }
    }
    str.bufferClear();
    if(curToken.tokenType == TOKEN_EOF)
        LEX_ERROR(*this, "unterminated string!");
}

// 获取下一个 Token
void Parser::getNextToken() {
    preToken = curToken;
    curToken = Token_t(TOKEN_EOF, nextIdx - 1, 0, curToken.lineNo);
    skipBlanks();
    while(nextIdx <= codeSize) {
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
                } else {
                    if (curChar == '#' && matchNextChar('!')) {
                        skipALine();
                        curToken.startIdx = nextIdx - 1; // 重置下一个token起始地址
                        continue;
                    } 
                    LEX_ERROR(*this, "unsupport char: \'%c\', quit.", curChar);
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
        COMPILE_ERROR(*this, errMsg);
    getNextToken();
}

// 断言下一个 Token
void Parser::consumeNextToken(TokenType_e expected, const char* errMsg) {
    getNextToken();
    if(curToken.tokenType != expected)
        COMPILE_ERROR(*this, errMsg);
}
