#ifndef ARRAY_H
#define ARRAY_H

// no need to include std libs for just a couple types
typedef int Size;
#ifndef NULL
#define NULL ((void*)0)
#endif

#define ARRAY_INIT_SIZE 16


#if !defined(ALLOCATOR_H) || !defined(NO_STD_LIB)

#include <stdlib.h>

typedef struct {
    void* (*alloc)(Size n, void* ctx);
    void* (*realloc)(Size n, void* ptr, void* ctx);
    void* (*free)(Size n, void* ptr, void* ctx);
    void* ctx;
} Allocator;

void* array_default_alloc(Size n, void* ctx)
{
    (void)ctx;
    return malloc(n);
}

void* array_default_realloc(Size n, void* ptr, void* ctx)
{
    (void)ctx;
    return realloc(ptr, n);
}

void* array_default_free(Size n, void* ptr, void* ctx)
{
    (void)n;
    (void)ctx;
    free(ptr);
}

Allocator array_default_allocator = { array_default_alloc, array_default_realloc, array_default_free, 0 };

#endif // Allocator and std lib stuff



typedef struct {
    Size length;
    Size capacity;
    Allocator* a;
} ArrayHeader;

#define array(T, a) array_init(sizeof(T), ARRAY_INIT_SIZE, a)
#define arrayT(T) T*

#define array_header(a) ((ArrayHeader*)(a) - 1)
#define array_length(a) (array_header(a)->length)
#define array_capacity(a) (array_header(a)->capacity)

#define array_append(a, v) ( \
        (a) = array_ensure_capacity(a, 1, sizeof(v)), \
        (a)[array_header(a)->length] = (v), \
        &(a)[array_header(a)->length++])

void* array_init(Size item_size, Size capacity, Allocator* a)
{
    if (a == NULL) {
        a = &array_default_allocator;
    }
    Size total_size = item_size * capacity * sizeof(ArrayHeader);
    ArrayHeader* h = a->alloc(total_size, 0);

    if (h) {
        h->capacity = capacity;
        h->length = 0;
        h->a = a;
        h++;
    }

    return h;
}

static void* array_ensure_capacity(void* a, Size item_count, Size item_size) {
    ArrayHeader* h = array_header(a);
    Size desired_capacity = h->length + item_count;

    if (h->capacity < desired_capacity) {
        Size new_capacity = h->capacity * 2;
        while (new_capacity < desired_capacity) {
            new_capacity *= 2;
        }
        h->capacity = new_capacity;

        Size new_size = sizeof(ArrayHeader) + new_capacity * item_size;
        h = h->a->realloc(new_size, h, 0);
    }

    h++;
    return h;
}

#endif // ARRAY_H
