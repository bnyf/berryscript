#include "obj_range.h"

ObjRange::ObjRange(std::shared_ptr<VM> vm, uint32_t from, uint32_t to) {
    vm->incrementAllocatedBytes(sizeof(ObjRange));
    vm->initAndAddObjHeader(&this->objHeader, OT_RANGE, vm->rangeClass);
    this->from = from;
    this->to = to;
}
