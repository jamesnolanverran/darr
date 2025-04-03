#ifndef DARR_H
#define DARR_H

#include <stddef.h>
#include <stdalign.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DARR_ALIGNMENT
    #define DARR_ALIGNMENT 16
#endif

#ifndef DARR_DEFAULT_MAX_SIZE
    // 2GB for testing
    #define DARR_DEFAULT_MAX_SIZE (1ULL << 31)
#endif // DARR_DEFAULT_MAX_SIZE

#define DARR_INITIAL_CAPACITY 16
#define DARR_GROWTH_MULTIPLIER 2.0f
#if defined(__cplusplus)
    #define DARR_TYPEOF(d) (decltype((d) + 0))
#elif defined(__clang__) && defined(_MSC_VER)
    // clang-cl behaves like MSVC, no typeof
    #define DARR_TYPEOF(d)
#elif defined(__clang__) || defined(__GNUC__)  
    #define DARR_TYPEOF(d) (typeof(d))
#else
    #define DARR_TYPEOF(d)
#endif

typedef void *(*AllocatorFn)(void *hdr, size_t new_total_size);

typedef struct DarrHdr { 
    AllocatorFn alloc; // allocator must provide a single contiguous block of memory
    unsigned int len;
    unsigned int cap;
    _Alignas(DARR_ALIGNMENT) char data[]; 
} DarrHdr;

static inline DarrHdr *darr__hdr(void *arr) { 
    return (DarrHdr*)( (char*)(arr) - offsetof(DarrHdr, data)); 
}
static inline size_t darr_len(void *a) { return a ? darr__hdr(a)->len : 0; }
static inline size_t darr_cap(void *a) { return a ? darr__hdr(a)->cap : 0; }
static inline void darr_clear(void *a) { if (a)  darr__hdr(a)->len = 0; }
static inline void darr_free(void *a) { if (a) darr__hdr(a)->alloc(darr__hdr(a), 0); } // call alloc w/ zero size to free

// internal - used by macros
void *darr__grow(void *arr, size_t elem_size);
void *darr__init(void *arr, size_t initial_capacity, size_t elem_size, AllocatorFn alloc);
void darr__free(void *a);
#define darr__fit(a, n) ((n) <= darr_cap(a) ? 0 : ((a) = DARR_TYPEOF(a) darr__grow((a), sizeof(*(a)))))

#define darr_end(a) ((a) + darr_len(a))

#define darr_push(a, ...) (darr__fit((a), 1 + darr_len(a)), (a)[darr__hdr(a)->len] = (__VA_ARGS__), &(a)[darr__hdr(a)->len++]) // returns ptr 

// optional init: allow users to define initial capacity and allocator
// darr_init(myarr, 0, NULL); //  '0' & 'NULL' indicates the user will use default values
#define darr_init(a, initial_capacity, alloc) (a) = darr__init(a, initial_capacity, sizeof(*(a)), alloc) 

#define darr_pop(a) ((a)[darr__hdr(a)->len-- - 1])  // it's up to the user to null check etc, or see darr_pop_safe() below
#define darr_peek(a) ((a)[darr__hdr(a)->len - 1] ) 

#define darr_pop_safe(a, default) (darr_len(a) ? darr_pop(a) : (default)) // supply a return value 'default' if 'a' is empty or NULL
#define darr_peek_safe(a, default) (darr_len(a) ? darr_peek(a) : (default)) 

void darr_set_error_handler(void (*handler)(char* err_msg));

#ifdef __cplusplus
}
#endif
#endif // DARR_H

