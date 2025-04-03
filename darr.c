#include "darr.h"
#include <stdint.h>
#include <stdlib.h>

#define DARR_MAX_CAPACITY ((size_t)UINT32_MAX - 2)
#ifndef MAX
#define MAX(x, y) ((x) >= (y) ? (x) : (y))
#endif

static void darr_default_error_handler(char* err_msg) {
    perror(err_msg);
    exit(1); 
}
static void (*darr_error_handler)(char* err_msg) = darr_default_error_handler;

void darr_set_error_handler(void (*handler)(char* err_msg)) {
    darr_error_handler = handler ? handler : darr_default_error_handler; // fallback to default
}

// optionally specify an initial capacity and/or an allocation type. Default allocator: realloc
void *darr__init(void *arr, size_t initial_capacity, size_t elem_size, AllocatorFn alloc){
    if(arr) {
        darr_error_handler("darr_init: array already initialized (argument must be null)");
    }
    if(alloc == NULL){
        alloc = realloc;
    }
    DarrHdr *new_hdr = NULL;
    size_t new_cap = MAX(DARR_INITIAL_CAPACITY, initial_capacity); 
    size_t size_in_bytes = offsetof(DarrHdr, data) + (new_cap * elem_size);
    if(new_cap > DARR_MAX_CAPACITY){
        darr_error_handler("Error: Max capacity exceeded.\n");
    }
    if(size_in_bytes > DARR_DEFAULT_MAX_SIZE){
        darr_error_handler("Error: Max size exceeded. #define DMAP_DEFAULT_MAX_SIZE to overide default.");
    }
    new_hdr = alloc(arr, size_in_bytes);
    if(!new_hdr) {
        darr_error_handler("Out of memory 4");
    }
    new_hdr->alloc = alloc;
    new_hdr->len = 0;
    new_hdr->cap = (unsigned int)new_cap;

    return new_hdr->data;
}
void *darr__grow(void *arr, size_t elem_size) { 
    if (!arr) {
        // when this is the case we just want the defaults
        return darr__init(arr, 0, elem_size, NULL);
    }
    DarrHdr *dh = darr__hdr(arr);
    DarrHdr *new_hdr = NULL;
    size_t old_cap = dh->cap;
    if(old_cap > (size_t)((float)DARR_MAX_CAPACITY / DARR_GROWTH_MULTIPLIER)){
        darr_error_handler("Error: Max capacity exceeded.\n");
    }
    size_t new_cap = (size_t)((float)old_cap * DARR_GROWTH_MULTIPLIER);
    size_t total_size_in_bytes = offsetof(DarrHdr, data) + new_cap * elem_size;
    if(total_size_in_bytes > DARR_DEFAULT_MAX_SIZE){
        darr_error_handler("Error: Max size exceeded. #define DMAP_DEFAULT_MAX_SIZE to overide default.");
    }
    AllocatorFn alloc = dh->alloc;
    new_hdr = alloc(dh, total_size_in_bytes);
    if(!new_hdr) {
        darr_error_handler("Out of memory 5");
    }
    new_hdr->cap = (unsigned int)new_cap;
    // assert(((size_t)&new_hdr->data & (DARR_ALIGNMENT - 1)) == 0); // Ensure alignment
    return &new_hdr->data;
}