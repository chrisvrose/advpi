
/**
 * Copy code array into a buffer
 */
void initmemory(void* physicalMemory, void* code, size_t codeLen) {
    memcpy(physicalMemory, code, codeLen);
}