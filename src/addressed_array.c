
#include "addressed_array.h"



AddressedArray* aa_malloc_addressed_array(
    uint32_t element_width,
    uint32_t capacity,
    uint32_t realloc_size
) {
    AddressedArray *aa = (AddressedArray*) malloc(
        sizeof (AddressedArray)
        + (capacity * element_width)
    );
    aa->element_width = element_width;
    aa->capacity = capacity;
    aa->count = 0;
    aa->realloc_size = realloc_size;
    aa->address_book = kh_init(id_ix_map);
    return aa;
}

void aa_free_addressed_array(AddressedArray* aa) {
    kh_destroy(id_ix_map, aa->address_book);
    free(aa);
}

int aa_get_object_id_at_offset(AddressedArray *aa, uint32_t offset) {
    if(offset >= aa->count) {
        // slog_warn(
        //     "aa_get_object_id_at_offset() offset out of range");
        return -1;
    }
    return ((BaseAAObject*) (aa->data + offset * aa->element_width))->id;
}

void* aa_allocate_pointer_to_new_slot(AddressedArray *aa, AddressedArray **aap, unsigned long element_id) {
    if (kh_get(id_ix_map, aa->address_book, element_id) != kh_end(aa->address_book)) {
        // slog_error(
        //     "aa_allocate_pointer_to_new_slot() failed to allocate pointer, element_id already exists in address book.");
        return NULL;
    }
    if(aa->count < aa->capacity) {
        char* p = (aa->data) + (aa->count * aa->element_width);
        int retcode;
        khint_t i = kh_put(id_ix_map, aa->address_book, element_id, &retcode);
        kh_value(aa->address_book, i) = aa->count;
        aa->count++;
        return (void*) p;
    }
    void *expanded = realloc(
        aa,
        sizeof (AddressedArray) + ((aa->capacity + aa->realloc_size) * aa->element_width)
    );
    if(expanded) {
        aa = expanded;
        *aap = expanded;
        aa->capacity += aa->realloc_size;
        return aa_allocate_pointer_to_new_slot(aa, aap, element_id);
    } else {
        // slog_error(
        //     "aa_allocate_pointer_to_new_slot() failed to allocate, could not expand memory allocation.");
        return NULL;
    }
}

void* aa_get_pointer_from_id(AddressedArray *aa, unsigned long element_id) {
    khint_t i = kh_get(id_ix_map, aa->address_book, element_id);
    if (i == kh_end(aa->address_book)) {
        // slog_error("aa_get_pointer_from_id() could not find id in addressbook.");
        return NULL;
    }
    uint32_t ix = kh_val(aa->address_book, i);
    return aa->data + (ix * aa->element_width);
}

int aa_drop_element(AddressedArray *aa, unsigned long element_id) {
    khint_t _i;
    _i = kh_get(id_ix_map, aa->address_book, element_id);
    if (_i == kh_end(aa->address_book)) {
        // slog_error("aa_drop_element() could not find id in addressbook.");
        return -1;
    }
    uint32_t del_ix = kh_val(aa->address_book, _i);

    memset(
        aa->data + (del_ix * aa->element_width),
        0,
        aa->element_width
    );

    if(del_ix < aa->count - 1) {
        uint32_t move_ix = aa->count - 1;
        uint32_t id_to_move = aa_get_object_id_at_offset(aa, move_ix);
        memcpy(
            aa->data + (del_ix * aa->element_width),
            aa->data + (move_ix * aa->element_width),
            aa->element_width
        );
        memset(
            aa->data + (move_ix * aa->element_width),
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



