unsigned int heap_pos = 0x100000;

void* malloc(unsigned int size) {
    void* ptr = (void*) heap_pos;
    heap_pos += size;
    return ptr;
}
