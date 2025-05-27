#include <engine/memory/LinearArray.h>

//#define ENGINE_REPLACE_NEW_DELETE 1

#if ENGINE_REPLACE_NEW_DELETE
void *operator new(std::size_t n) {
    printf("[Memory] new(%zu)\n", n);
    return malloc(n);
}

void operator delete(void *p) noexcept {
    printf("[Memory] delete(%p)\n", p);
    free(p);
}

void *operator new[](std::size_t s) {
    printf("[Memory] new[](%zu)\n", s);
    return malloc(s);
}

void operator delete[](void *p) noexcept {
    printf("[Memory] delete[](%p)\n", p);
    free(p);
}
#endif
