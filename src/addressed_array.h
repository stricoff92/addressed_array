
#ifndef ADDRESSED_ARRAY_H
#define ADDRESSED_ARRAY_H

#include "stddef.h"

#include "khash.h"

#include "base_object.h"



KHASH_MAP_INIT_INT(id_ix_map, uint32_t)

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
) {
    AddressedArray *aa = (AddressedArray*) malloc(sizeof (AddressedArray));
    aa->element_width = element_width;
    aa->capacity = capacity;
    aa->count = 0;
    aa->realloc_size = realloc_size;
    aa->address_book = kh_init(id_ix_map);
    aa->data = malloc(element_width * capacity);
    return aa;
}

void aa_free_addressed_array(AddressedArray* aa) {
    free(aa->data);
    free(aa);
}

void* aa_allocate_pointer_to_new_slot(AddressedArray *aa, unsigned long element_id) {
    if (kh_get(id_ix_map, aa->address_book, element_id) != kh_end(aa->address_book)) {
        slog_error("aa_allocate_pointer_to_new_slot() failed to allocate pointer, element_id already exists in address book.");
        return NULL;
    }
    if(aa->count < aa->capacity) {
       // casting to (char*) shouldn't be needed when using gcc.
        char* p = ((char *) aa->data) + (aa->count * aa->element_width);

        int _ret_code;
        khint_t i = kh_put(id_ix_map, aa->address_book, element_id, &_ret_code);
        kh_value(aa->address_book, i) = aa->count;

        aa->count++;
        return p;
    }
    void *expanded = realloc(
        aa->data,
        (aa->capacity + aa->realloc_size)
            * aa->element_width
    );
    if(expanded) {
        aa->data = expanded;
        aa->capacity += aa->realloc_size;
        return aa_allocate_pointer_to_new_slot(aa, element_id);
    } else {
        slog_error(
            "aa_allocate_pointer_to_new_slot() failed to allocate, could not expand memory allocation.");
        return NULL;
    }
}

void* aa_get_pointer_from_id(AddressedArray *aa, unsigned long element_id) {
    khint_t i = kh_get(id_ix_map, aa->address_book, element_id);
    if (i == kh_end(aa->address_book)) {
        slog_error("aa_get_pointer_from_id() could not find id in addressbook.");
        return NULL;
    }
    size_t ix = kh_val(aa->address_book, i);
    return ((char*) aa->data) + (ix * aa->element_width);
}

int aa_drop_element(AddressedArray *aa, unsigned long element_id) {
    khint_t _i;
    _i = kh_get(id_ix_map, aa->address_book, element_id);
    if (_i == kh_end(aa->address_book)) {
        slog_error("aa_drop_element() could not find id in addressbook.");
        return -1;
    }
    size_t del_ix = kh_val(aa->address_book, _i);

    memset(
        (((char*) aa->data) + del_ix * aa->element_width),
        0,
        aa->element_width
    );

    if(del_ix < aa->count - 1) {
        uint32_t move_ix = aa->count - 1;
        uint32_t id_to_move = ((BaseObject*) (((char*) aa->data) + move_ix * aa->element_width))->id;
        memcpy(
            (((char*)aa->data) + del_ix * aa->element_width),
            (((char*)aa->data) + move_ix * aa->element_width),
            aa->element_width
        );
        memset(
            (((char*)aa->data) + move_ix * aa->element_width),
            0,
            aa->element_width
        );

        _i = kh_get(id_ix_map, aa->address_book, id_to_move);
        if(_i != kh_end(aa->address_book))
            kh_del(id_ix_map, aa->address_book, _i);
        int _ret_code;
        _i = kh_put(id_ix_map, aa->address_book, id_to_move, &_ret_code);
        kh_value(aa->address_book, _i) = del_ix;
    }

    _i = kh_get(id_ix_map, aa->address_book, element_id);
    if(_i != kh_end(aa->address_book))
        kh_del(id_ix_map, aa->address_book, _i);

    aa->count--;
    return (int) del_ix;
}


#endif
