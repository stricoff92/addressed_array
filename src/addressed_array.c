

#include "addressed_array.h"



aa_Array* aa_malloc_addressed_array(
    u32 element_width,
    u32 capacity,
    u32 realloc_size
) {
    aa_Array *aa = malloc(
        sizeof (aa_Array)      // Allocate space for the aa_Array struct
        + (capacity * element_width) // Allocate space for the array of elements
    );
    if(!aa) return NULL;
    aa->element_width = element_width;
    aa->capacity = capacity;
    aa->count = 0;
    aa->realloc_size = realloc_size;
    aa->supplemental_data_ix = 0;
    aa->address_book = kh_init(id_ix_map);
    if(!aa->address_book) {
        free(aa);
        return NULL;
    }
    return aa;
}

void aa_free_addressed_array(aa_Array* aa) {
    kh_destroy(id_ix_map, aa->address_book);
    free(aa);
}

int aa_get_object_id_at_offset(aa_Array *aa, u32 offset) {
    if(offset >= aa->count) {
        return -1;
    }
    return ((aa_Array_Base*) (aa->data + offset * aa->element_width))->id;
}

void* aa_allocate_pointer_to_new_slot(
    aa_Array **aap,
    u32 element_id
) {
    if (kh_get(id_ix_map, (*aap)->address_book, element_id) != kh_end((*aap)->address_book)) {
        return NULL;
    }
    if((*aap)->count < (*aap)->capacity) {
        void *p = (void *) (((*aap)->data) + ((*aap)->count * (*aap)->element_width));
        memset(p, 0, (*aap)->element_width);
        ((aa_Array_Base*) p)->id = element_id;
        int retcode;
        khint_t i = kh_put(id_ix_map, (*aap)->address_book, element_id, &retcode);
        kh_value((*aap)->address_book, i) = (*aap)->count;
        (*aap)->count++;
        return p;
    }
    void *expanded = realloc(
        (*aap),
        sizeof (aa_Array) + (((*aap)->capacity + (*aap)->realloc_size) * (*aap)->element_width)
    );
    if(expanded) {
        *aap = expanded;
        (*aap)->capacity += (*aap)->realloc_size;
        return aa_allocate_pointer_to_new_slot(aap, element_id);
    } else {
        return NULL;
    }
}

void* aa_get_pointer_from_id(aa_Array *aa, u32 element_id) {
    khint_t i = kh_get(id_ix_map, aa->address_book, element_id);
    if (i == kh_end(aa->address_book)) {
        return NULL;
    }
    u32 ix = kh_val(aa->address_book, i);
    return aa->data + (ix * aa->element_width);
}

int aa_drop_element(aa_Array *aa, u32 element_id) {
    khint_t _i;
    _i = kh_get(id_ix_map, aa->address_book, element_id);
    if (_i == kh_end(aa->address_book)) {
        return -1;
    }
    u32 del_ix = kh_val(aa->address_book, _i);

    memset(
        aa->data + (del_ix * aa->element_width),
        0,
        aa->element_width
    );

    if(del_ix < aa->count - 1) {
        u32 move_ix = aa->count - 1;
        u32 id_to_move = aa_get_object_id_at_offset(aa, move_ix);
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

int aa_get_next_supplemental_data_ix(aa_Array *aa) {
    if(!aa->count) return -1;
    if(aa->supplemental_data_ix >= aa->count)
        aa->supplemental_data_ix = 0;
    return aa->supplemental_data_ix++;
}

