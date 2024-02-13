
#ifndef ADDRESSED_ARRAY_H
#define ADDRESSED_ARRAY_H

#include "stddef.h"

#include "khash.h"


KHASH_MAP_INIT_INT(id_ix_map, uint32_t)


typedef struct BaseAAObject {
    uint32_t id;
} BaseAAObject;



typedef struct AddressedArray
{
    size_t element_width;
    size_t capacity;
    size_t count;
    size_t realloc_size;
    void* data;
    khash_t(id_ix_map) *address_book;
} AddressedArray;


AddressedArray* aa_malloc_addressed_array(
    size_t element_width,
    size_t capacity,
    size_t realloc_size
);

void aa_free_addressed_array(AddressedArray* aa);

void* aa_allocate_pointer_to_new_slot(AddressedArray *aa, unsigned long element_id);

void* aa_get_pointer_from_id(AddressedArray *aa, unsigned long element_id);

int aa_drop_element(AddressedArray *aa, unsigned long element_id);


#endif
