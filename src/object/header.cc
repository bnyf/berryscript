#include "header.h"
#include "obj_string.h"
#include "obj_range.h"


Value::Value(ValueType _type, double _num) 
: type(_type), num(_num){}

bool Value::operator == (const Value &x) {
    //类型不同则无须进行后面的比较
    if (this->type != x.type) {
        return false;
    }

    //同为数字,比较数值
    if (this->type == VT_NUM) {
        return this->num == x.num;
    }

    //同为对象,若所指的对象是同一个则返回true
    if (this->objHeader == x.objHeader) {
        return true;
    }

    //对象类型不同无须比较
    if (this->objHeader->type != x.objHeader->type) {
        return false;
    }

    //以下处理类型相同的对象
    //若对象同为字符串
    if (this->objHeader->type == OT_STRING) {
        ObjString* strA = VALUE_TO_OBJSTR(*this);
        ObjString* strB = VALUE_TO_OBJSTR(x);
        
        return (strA->value.length == strB->value.length &&
        memcmp(strA->value.str, strB->value.str, strA->value.length) == 0);
    }

    // 若对象同为range
    if (this->objHeader->type == OT_RANGE) {
        ObjRange* rgA = VALUE_TO_OBJRANGE(*this);
        ObjRange* rgB = VALUE_TO_OBJRANGE(x);

        return (rgA->from == rgB->from && rgA->to == rgB->to);
    }

    return false;  //其它对象不可比较
}