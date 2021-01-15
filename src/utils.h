#ifndef _UTILS_H
#define _UTILS_H

#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <memory>

class Parser;

typedef enum {
   ERROR_IO,
   ERROR_MEM,
   ERROR_LEX,
   ERROR_COMPILE,
   ERROR_RUNTIME
} ErrorType;

#define DEFAULT_BUFFER_SIZE 512
#define UNUSED __attribut__((unsued))
#define NOT_REACHED()\
    do {\
        fprintf(stderr, "NOT_REACHED: %s:%d In function %s()\n", \
                  __FILE__, __LINE__, __func__); \
        while(1);\
    } while(0);

#ifdef DEBUG
    #define ASSERT(condition, errMsg) \
        do {\
            if(!(condition)) {\
                fprintf(stderr, "ASSERT failed！ %s:%d In function %s(): %s\n", \
                __FILE__, __LINE__, __func__, errMsg); \
                abort();\
            }\
        } while(0);
#else
    #define ASSERT(condition, errMsg)  
#endif

void errorReport(Parser *parser, 
      ErrorType errorType, const char* fmt, ...);

#define IO_ERROR(...) \
   errorReport(nullptr, ERROR_IO, __VA_ARGS__)
#define MEM_ERROR(...) \
   errorReport(nullptr, ERROR_MEM, __VA_ARGS__)
#define LEX_ERROR(parser, ...) \
   errorReport(parser, ERROR_LEX, __VA_ARGS__)
#define COMPILE_ERROR(parser, ...) \
   errorReport(parser, ERROR_COMPILE, __VA_ARGS__)
#define RUN_ERROR(...) \
   errorReport(nullptr, ERROR_RUNTIME, __VA_ARGS__)

uint32_t ceilToPowerOf2(uint32_t v);
std::string* readFile(const std::string &filename);
void runFile(std::string &filename);

#endif