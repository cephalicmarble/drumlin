#ifndef _USES_STRIDES_H
#define _USES_STRIDES_H

#include "uses_allocator.h"

namespace drumlin {

class UsesStrides {
    typedef std::pair<heap_ptr_type, heap_ptr_type> strides_type;
    strides_type m_strides;
    size_t m_stride_length;
    UsesStrides();
    UsableBuffer *nextBuffer();
};

} // namespace drumlin

#endif // _USES_STRIDES_H
