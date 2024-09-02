
#ifndef ADDRESSED_ARRAY_H
#define ADDRESSED_ARRAY_H


#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "khash.h"



typedef uint8_t     u8;
typedef int8_t      i8;
typedef uint16_t    u16;
typedef int16_t     i16;
typedef uint32_t    u32;
typedef int32_t     i32;
typedef uint64_t    u64;
typedef int64_t     i64;
typedef float       f32;
typedef double      f64;

#define U8(v)  ((u8)(v))
#define I8(v)  ((i8)(v))
#define U16(v) ((u16)(v))
#define I16(v) ((i16)(v))
#define U32(v) ((u32)(v))
#define I32(v) ((i32)(v))
#define U64(v) ((u64)(v))
#define I64(v) ((i64)(v))
#define F32(v) ((f32)(v))
#define F64(v) ((f64)(v))




KHASH_MAP_INIT_INT(id_ix_map, u32)

typedef struct addressed_array_base_t {
    u32 id;
} addressed_array_base_t;

typedef struct addressed_array_t
{
    u32 element_width;
    u32 capacity;
    u32 count;
    u32 realloc_size;
    u32 supplemental_data_ix; // used to paginate through supplemental data published by the engine
    khash_t(id_ix_map) *address_book;

    /* Flexible array member (last member, not counted in sizeof)
        Data points to the beginning of the array of elements.
    */
    u8 data[ ];
} addressed_array_t;


addressed_array_t* aa_malloc_addressed_array(
    u32 element_width,
    u32 capacity,
    u32 realloc_size
);


void aa_free_addressed_array(addressed_array_t* aa);

int aa_get_object_id_at_offset(addressed_array_t *aa, u32 offset);

void* aa_allocate_pointer_to_new_slot(
    addressed_array_t **aap,
    u32 element_id
);

void* aa_get_pointer_from_id(
    addressed_array_t *aa,
    u32 element_id
);

int aa_drop_element(addressed_array_t *aa, u32 element_id);

int aa_get_next_supplemental_data_ix(addressed_array_t *aa);

#endif
