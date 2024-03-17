
#ifndef ADDRESSED_ARRAY_H
#define ADDRESSED_ARRAY_H

#include "stddef.h"
#include "string.h"
#include "stdlib.h"

// https://github.com/attractivechaos/klib/blob/master/khash.h
#include "khash.h"


KHASH_MAP_INIT_INT(id_ix_map, uint32_t)


typedef struct BaseAAObject {
    uint32_t id;
} BaseAAObject;

typedef struct AddressedArray
{
    uint32_t element_width;
    uint32_t capacity;
    uint32_t count;
    uint32_t realloc_size;
    khash_t(id_ix_map) *address_book;
    char data[ ];
} AddressedArray;


AddressedArray* aa_malloc_addressed_array(
    uint32_t element_width,
    uint32_t capacity,
    uint32_t realloc_size
);


void aa_free_addressed_array(AddressedArray* aa);

int aa_get_object_id_at_offset(AddressedArray *aa, uint32_t offset);

void* aa_allocate_pointer_to_new_slot(AddressedArray *aa, AddressedArray **aap, unsigned long element_id);

void* aa_get_pointer_from_id(AddressedArray *aa, unsigned long element_id);

int aa_drop_element(AddressedArray *aa, unsigned long element_id);


#endif
