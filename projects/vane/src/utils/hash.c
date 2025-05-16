#include "vane/utils/hash.h"

u32 hash_finalizer(u32 hash) {
    hash ^= hash >> 16;
    hash *= 0x3243f6a9U;
    hash ^= hash >> 16;
    return hash;
}

u32 get_cstr_hash(const char* cstr) {
    assert(cstr != NULL);

    u32 hash = 5381;
    char c = '\0';
    while ((c = *cstr++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash_finalizer(hash);
}