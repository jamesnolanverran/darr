# Dynamic Array Library

A small, minimal-overhead dynamic array implementation for C, inspired by Per Vognsen's *Bitwise* series and Sean Barrett's [`stb_ds`](https://github.com/nothings/stb/blob/master/stb_ds.h).

## Features

- Simple API with macros for dynamic array operations
- Custom allocator support
- Minimal boilerplate—just declare a pointer and use it
- Fast push, pop, and access operations
- Optional error handling

## API Overview

```c
// Declare an array of any type
int *arr = NULL;

darr_push(arr, 42);
darr_push(arr, 84);

for (int i = 0; i < darr_len(arr); i++) {
    printf("%d: %d\n", i, arr[i]);
}

darr_free(arr); // Free memory when done
```

### Macros & Functions

- `darr_push(arr, value)` – Append an element, resizing as needed.
- `darr_pop(arr)` – Remove and return the last element. User should check for NULL and empty array.
- `darr_peek(arr)` – Return the last element without removing it. User should check for NULL and empty array.
- `darr_pop_safe(arr, default)` – Safe pop with a default return value if empty.
- `darr_peek_safe(arr, default)` – Safe peek with a default return value if empty.
- `darr_len(arr)` – Get the number of elements.
- `darr_cap(arr)` – Get the capacity.
- `darr_clear(arr)` – Set length to zero without freeing.
- `darr_free(arr)` – Free the array.
- `darr_init(arr, initial_capacity, allocator)` – Initialize with custom capacity and allocator.
- `darr_set_error_handler(handler)` – Set a custom error handler.

## Implementation Details

Dynamic arrays use a small header (`DarrHdr`) stored before the array data. The user may provides an allocator function.

```c
typedef struct DarrHdr { 
    AllocatorFn alloc; // allocator must provide a single contiguous block of memory
    unsigned int len;
    unsigned int cap;
    _Alignas(DARR_ALIGNMENT) char data[]; 
} DarrHdr;

static inline DarrHdr *darr__hdr(void *arr) { 
    return (DarrHdr*)( (char*)(arr) - offsetof(DarrHdr, data)); 
}
```

## Credits

- Adapted from Per Vognsen's *Bitwise* series.
- Based on techniques from Sean Barrett's [`stb_ds`](https://github.com/nothings/stb/blob/master/stb_ds.h).

[Per Vognsen's *Bitwise* Series on YouTube](https://www.youtube.com/pervognsen)

## License
MIT License

