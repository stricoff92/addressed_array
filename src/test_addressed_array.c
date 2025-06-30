


#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "addressed_array.h"
#include "__test_helpers.h"


typedef struct test_object_1_t {
    aa_Array_Base base;
    int  a;
    int  b;
    char c;
    char d;
} test_object_1_t;

typedef struct test_object_2_t {
    aa_Array_Base base;
    int  a;
} test_object_2_t;

typedef struct test_object_3_t {
    aa_Array_Base base;
    char  name[24];
} test_object_3_t;



void test_can_get_arbitrary_objects_id_value() {
    aa_Array* aa = aa_malloc_addressed_array(sizeof (test_object_3_t), 10, 10);
    test_object_3_t* s1 = aa_allocate_pointer_to_new_slot(&aa, 75);
    s1->base.id = 75;
    strcpy(s1->name, "foobar");
    test_object_3_t* s2 = aa_allocate_pointer_to_new_slot(&aa, 76);
    s2->base.id = 76;
    strcpy(s2->name, "lorum");
    assert(aa_get_object_id_at_offset(aa, 0) == 75);
    assert(aa_get_object_id_at_offset(aa, 1) == 76);

    aa_free_addressed_array(aa);
    TEST_PASSED
}

void test_can_create_addressed_array_with_initial_dimensions() {
    aa_Array* aa = aa_malloc_addressed_array(24, 10, 12);
    assert(aa->element_width == 24);
    assert(aa->capacity == 10);
    assert(aa->realloc_size == 12);
    assert(aa->count == 0);

    aa_free_addressed_array(aa);
    TEST_PASSED
}

void test_a_created_addressed_array_has_an_initial_size_of_zero() {
    aa_Array* aa = aa_malloc_addressed_array(24, 10, 10);
    assert(kh_size(aa->address_book) == 0);
    assert(aa->count == 0);

    aa_free_addressed_array(aa);
    TEST_PASSED
}


void test_a_pointer_to_the_first_data_slot_can_be_calculated() {
    aa_Array* aa = aa_malloc_addressed_array(sizeof (test_object_1_t), 10, 10);
    test_object_1_t *te = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 1);

    // pointer to first data element should be same as the address
    // of the contiguously allocated memory.
    assert((void*)te == (void*)aa->data);

    aa_free_addressed_array(aa);
    TEST_PASSED
}

void test_a_pointer_to_the_second_data_slot_can_be_calculated() {
    aa_Array* aa = aa_malloc_addressed_array(sizeof (test_object_1_t), 10, 10);
    test_object_1_t *te1 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 1);
    test_object_1_t *te2 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 2);

    assert(te2 == ((test_object_1_t*) aa->data) + 1);

    aa_free_addressed_array(aa);
    TEST_PASSED
}

void test_allocating_a_new_pointer_updates_the_address_book() {
    aa_Array* aa = aa_malloc_addressed_array(sizeof (test_object_1_t), 10, 10);
    test_object_1_t *te1 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 1337);
    test_object_1_t *te2 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 42069);

    assert(kh_size(aa->address_book) == 2);
    assert(aa->count == 2);
    assert(
        kh_val(aa->address_book, kh_get(id_ix_map, aa->address_book, 1337)) == 0
    );
    assert(
        kh_val(aa->address_book, kh_get(id_ix_map, aa->address_book, 42069)) == 1
    );

    aa_free_addressed_array(aa);
    TEST_PASSED
}

void test_a_data_pointer_can_be_returned_by_searching_by_id() {
    aa_Array* aa = aa_malloc_addressed_array(sizeof (test_object_1_t), 10, 10);
    test_object_1_t *te1 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 1337);
    test_object_1_t *te2 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 42069);
    test_object_1_t *te3 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 12345);
    assert(kh_size(aa->address_book) == 3);
    assert(aa->count == 3);

    test_object_1_t* q2 = (test_object_1_t*) aa_get_pointer_from_id(aa, 42069);
    test_object_1_t* q1 = (test_object_1_t*) aa_get_pointer_from_id(aa, 1337);
    test_object_1_t* q3 = (test_object_1_t*) aa_get_pointer_from_id(aa, 12345);

    assert(((test_object_1_t*) aa->data) == q1);
    assert(((test_object_1_t*) aa->data) + 1 == q2);
    assert(((test_object_1_t*) aa->data) + 2 == q3);

    aa_free_addressed_array(aa);
    TEST_PASSED
}

void test_data_can_be_written_to_and_read_from_data_pointers() {
    aa_Array* aa = aa_malloc_addressed_array(sizeof (test_object_1_t), 10, 10);
    test_object_1_t *te1 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 111);
    test_object_1_t *te2 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 222);

    te1->a = 60;
    te1->b = 61;
    te1->c = 'a';
    te1->d = 'b';

    te2->a = 62;
    te2->b = 63;
    te2->c = 'c';
    te2->d = 'd';

    test_object_1_t* q2 = (test_object_1_t*) aa_get_pointer_from_id(aa, 222);
    test_object_1_t* q1 = (test_object_1_t*) aa_get_pointer_from_id(aa, 111);

    assert(q1->a == 60);
    assert(q1->b == 61);
    assert(q1->c == 'a');
    assert(q1->d == 'b');
    assert(q2->a == 62);
    assert(q2->b == 63);
    assert(q2->c == 'c');
    assert(q2->d == 'd');

    aa_free_addressed_array(aa);
    TEST_PASSED
}

void test_null_pointer_is_returned_when_element_id_is_not_found() {
    aa_Array* aa = aa_malloc_addressed_array(sizeof (test_object_1_t), 10, 10);
    test_object_1_t *te1 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 111);
    test_object_1_t *te2 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 222);

    test_object_1_t* q1 = (test_object_1_t*) aa_get_pointer_from_id(aa, 111); // exists
    test_object_1_t* q2 = (test_object_1_t*) aa_get_pointer_from_id(aa, 121); // doesn't exist

    assert(q1 != NULL); // exists
    assert(q2 == NULL); // doesn't exist

    aa_free_addressed_array(aa);
    TEST_PASSED
}

void test_null_pointer_is_returned_when_allocating_a_pointer_for_an_id_that_is_already_in_use() {
    aa_Array* aa = aa_malloc_addressed_array(sizeof (test_object_1_t), 10, 10);
    test_object_1_t *te1 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 111);
    test_object_1_t *te2 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 222);
    assert(te1 != NULL);
    assert(te2 != NULL);

    test_object_1_t *te3 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 222); // Already using this id
    assert(te3 == NULL);

    TEST_PASSED
}

void test_attempt_to_get_id_of_offset_that_is_out_of_range() {
    aa_Array* aa = aa_malloc_addressed_array(
        sizeof (test_object_1_t),
        410,
        10
    );
    test_object_1_t *te1 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 111);
    te1->base.id = 111;
    test_object_1_t *te2 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 222);
    te2->base.id = 222;
    assert(aa_get_object_id_at_offset(aa, 0) == 111);
    assert(aa_get_object_id_at_offset(aa, 1) == 222);
    assert(aa_get_object_id_at_offset(aa, 2) == -1);

    TEST_PASSED
}

void test_data_block_is_expanded_when_data_block_hits_capacity() {
    aa_Array* aa = aa_malloc_addressed_array(
        sizeof (test_object_1_t),
        4, // initial capacity (num. of elements)
        2 // reallocation size (num. of elements)
    );

    assert(aa->capacity == 4);
    assert(aa->realloc_size == 2);
    assert(aa->count == 0);

    aa_allocate_pointer_to_new_slot(&aa, 11);
    assert(aa->capacity == 4);
    assert(aa->realloc_size == 2);
    assert(aa->count == 1);
    assert(kh_size(aa->address_book) == 1);


    aa_allocate_pointer_to_new_slot(&aa, 12);
    assert(aa->capacity == 4);
    assert(aa->realloc_size == 2);
    assert(aa->count == 2);
    assert(kh_size(aa->address_book) == 2);
    aa_allocate_pointer_to_new_slot(&aa, 13);
    assert(aa->capacity == 4);
    assert(aa->realloc_size == 2);
    assert(aa->count == 3);
    assert(kh_size(aa->address_book) == 3);
    aa_allocate_pointer_to_new_slot(&aa, 14);
    assert(aa->capacity == 4);
    assert(aa->realloc_size == 2);
    assert(aa->count == 4);
    assert(kh_size(aa->address_book) == 4);

    // Adding this element will increase capacity
    aa_allocate_pointer_to_new_slot(&aa, 15);
    assert(aa->capacity == 6);
    assert(aa->realloc_size == 2);
    assert(aa->count == 5);
    assert(kh_size(aa->address_book) == 5);

    aa_allocate_pointer_to_new_slot(&aa, 16);
    assert(aa->capacity == 6);
    assert(aa->realloc_size == 2);
    assert(aa->count == 6);
    assert(kh_size(aa->address_book) == 6);

    // Adding this element will increase capacity
    aa_allocate_pointer_to_new_slot(&aa, 17);
    assert(aa->capacity == 8);
    assert(aa->realloc_size == 2);
    assert(aa->count == 7);
    assert(kh_size(aa->address_book) == 7);

    test_object_1_t* te1 = aa_get_pointer_from_id(aa, 11);
    assert(((test_object_1_t*) aa->data) == te1);
    test_object_1_t* te2 = aa_get_pointer_from_id(aa, 12);
    assert(((test_object_1_t*) aa->data + 1) == te2);
    test_object_1_t* te3 = aa_get_pointer_from_id(aa, 13);
    assert(((test_object_1_t*) aa->data + 2) == te3);
    test_object_1_t* te4 = aa_get_pointer_from_id(aa, 14);
    assert(((test_object_1_t*) aa->data + 3) == te4);
    test_object_1_t* te5 = aa_get_pointer_from_id(aa, 15);
    assert(((test_object_1_t*) aa->data + 4) == te5);
    test_object_1_t* te6 = aa_get_pointer_from_id(aa, 16);
    assert(((test_object_1_t*) aa->data + 5) == te6);
    test_object_1_t* te7 = aa_get_pointer_from_id(aa, 17);
    assert(((test_object_1_t*) aa->data + 6) == te7);

    aa_free_addressed_array(aa);
    TEST_PASSED
}

void test_the_address_array_pointer_is_updated_if_realloc_is_called() {
    // arrange
    /* It's important to allocate a large enough block of memory to
        ensure that the realloc() call will have to move the
        memory block to a new location. If the block is not large
        enough, the realloc() function may simply expand the block
        in place. If this happens this test will fail.
    */
    aa_Array* aa = aa_malloc_addressed_array(
        sizeof (test_object_1_t),
        100, // initial capacity (num. of elements)
        300  // reallocation size (num. of elements)
    );
    u64 initial_address = (u64) aa;

    // act
    for(u32 i = 0; i < 100; i++)
        aa_allocate_pointer_to_new_slot(&aa, i+1);
    assert(aa->capacity == 100);
    assert(aa->realloc_size == 300);
    assert(aa->count == 100);
    assert(((u64) aa) == initial_address);
    aa_allocate_pointer_to_new_slot(&aa, 101);
    assert(aa->capacity == 400);
    assert(aa->realloc_size == 300);
    assert(aa->count == 101);

    // assert
    // The address of the aa_Array struct should have changed.
    // This test may fail if the realloc() function simply expanded
    // the block in place, without moving it.
    assert(((u64) aa) != initial_address);

    aa_free_addressed_array(aa);
}

void test_delete_only_element() {
    aa_Array* aa = aa_malloc_addressed_array(sizeof (test_object_2_t), 10, 10);
    test_object_2_t *te1 = (test_object_2_t*) aa_allocate_pointer_to_new_slot(&aa, 111);
    te1->base.id = 111;
    te1->a = 69;
    assert(aa->count == 1);
    assert(kh_size(aa->address_book) == 1);

    aa_drop_element(aa, 111);
    assert(aa->count == 0);
    assert(kh_size(aa->address_book) == 0);

    aa_free_addressed_array(aa);
    TEST_PASSED
}

void test_delete_element_that_doesnt_exist() {
    aa_Array* aa = aa_malloc_addressed_array(sizeof (test_object_2_t), 10, 10);
    test_object_2_t *te1 = (test_object_2_t*) aa_allocate_pointer_to_new_slot(&aa, 111);
    te1->base.id = 111;
    te1->a = 69;
    assert(aa->count == 1);
    assert(kh_size(aa->address_book) == 1);

    int ret = aa_drop_element(aa, 222);
    assert(ret == -1);
    assert(aa->count == 1);
    assert(kh_size(aa->address_book) == 1);

    test_object_2_t* q1 = (test_object_2_t*) aa_get_pointer_from_id(aa, 111);
    assert(q1->base.id == 111);
    assert(q1->a == 69);

    aa_free_addressed_array(aa);
    TEST_PASSED
}

void test_deleting_the_last_data_element_does_not_cause_a_reshuffling() {
    aa_Array* aa = aa_malloc_addressed_array(sizeof (test_object_2_t), 10, 10);
    test_object_2_t *te1 = (test_object_2_t*) aa_allocate_pointer_to_new_slot(&aa, 111);
    test_object_2_t *te2 = (test_object_2_t*) aa_allocate_pointer_to_new_slot(&aa, 222);
    assert(aa->count == 2);

    te1->base.id = 111;
    te1->a = 69;
    te2->base.id = 222;
    te2->a = 420;

    // Act: drop the last element.
    int dropped_ix = aa_drop_element(aa, 222);
    assert(dropped_ix == 1);
    assert(aa->count == 1);

    // Assert: first element unsullied
    assert(((test_object_2_t*) aa->data)->base.id == 111);
    assert(((test_object_2_t*) aa->data)->a == 69);

    khint_t i = kh_get(id_ix_map, aa->address_book, 111);
    u32 ix = kh_val(aa->address_book, i);
    assert(ix == 0);

    aa_free_addressed_array(aa);
    TEST_PASSED
}

void test_deleting_the_first_data_element_does_cause_a_reshuffling() {
    aa_Array* aa = aa_malloc_addressed_array(sizeof (test_object_2_t), 10, 10);
    test_object_2_t *te1 = (test_object_2_t*) aa_allocate_pointer_to_new_slot(&aa, 111);
    test_object_2_t *te2 = (test_object_2_t*) aa_allocate_pointer_to_new_slot(&aa, 222);
    test_object_2_t *te3 = (test_object_2_t*) aa_allocate_pointer_to_new_slot(&aa, 333);
    assert(aa->count == 3);

    te1->base.id = 111;
    te1->a = 69;
    te2->base.id = 222;
    te2->a = 420;
    te3->base.id = 333;
    te3->a = 1999;

    assert(((test_object_2_t*) aa->data)->base.id == 111);
    assert(((test_object_2_t*) aa->data)->a == 69);

    // Act: drop the first element.
    int dropped_ix = aa_drop_element(aa, 111);
    assert(dropped_ix == 0);
    assert(aa->count == 2);

    // Assert: first element now holds object 333
    assert(((test_object_2_t*) aa->data)->base.id == 333);
    assert(((test_object_2_t*) aa->data)->a == 1999);
    khint_t i = kh_get(id_ix_map, aa->address_book, 333);
    u32 ix = kh_val(aa->address_book, i);
    assert(ix == 0);

    // Assert: second element still holds object 222
    assert((((test_object_2_t*) aa->data) + 1)->base.id == 222);
    assert((((test_object_2_t*) aa->data) + 1)->a == 420);
    i = kh_get(id_ix_map, aa->address_book, 222);
    ix = kh_val(aa->address_book, i);
    assert(ix == 1);

    // Assert object 111 is gone
    test_object_2_t* q_deleted = (test_object_2_t*) aa_get_pointer_from_id(aa, 111);
    assert(q_deleted == NULL);

    aa_free_addressed_array(aa);
    TEST_PASSED
}

void test_deleting_a_middle_element_does_cause_a_reshuffling() {
    aa_Array* aa = aa_malloc_addressed_array(sizeof (test_object_2_t), 10, 10);
    test_object_2_t *te1 = (test_object_2_t*) aa_allocate_pointer_to_new_slot(&aa, 111);
    test_object_2_t *te2 = (test_object_2_t*) aa_allocate_pointer_to_new_slot(&aa, 222);
    test_object_2_t *te3 = (test_object_2_t*) aa_allocate_pointer_to_new_slot(&aa, 333);
    assert(aa->count == 3);

    te1->base.id = 111;
    te1->a = 69;
    te2->base.id = 222;
    te2->a = 420;
    te3->base.id = 333;
    te3->a = 1999;


    // Act: drop the middle element.
    int dropped_ix = aa_drop_element(aa, 222);
    assert(dropped_ix == 1);
    assert(aa->count == 2);

    // Assert: first element still holds object 111
    assert(((test_object_2_t*) aa->data)->base.id == 111);
    assert(((test_object_2_t*) aa->data)->a == 69);
    khint_t i = kh_get(id_ix_map, aa->address_book, 111);
    u32 ix = kh_val(aa->address_book, i);
    assert(ix == 0);

    // Assert: second element now holds object 333
    assert((((test_object_2_t*) aa->data) + 1)->base.id == 333);
    assert((((test_object_2_t*) aa->data) + 1)->a == 1999);
    i = kh_get(id_ix_map, aa->address_book, 333);
    ix = kh_val(aa->address_book, i);
    assert(ix == 1);

    // Assert object 222 is gone
    test_object_2_t* q_deleted = (test_object_2_t*) aa_get_pointer_from_id(aa, 222);
    assert(q_deleted == NULL);

    aa_free_addressed_array(aa);
    TEST_PASSED
}

void test_aa_get_next_supplemental_data_ix_step_through_data() {
    aa_Array* aa = aa_malloc_addressed_array(sizeof (test_object_1_t), 10, 10);

    test_object_1_t *obj1 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 15);
    assert(obj1 != NULL);
    test_object_1_t *obj2 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 16);
    assert(obj2 != NULL);
    test_object_1_t *obj3 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 17);
    assert(obj3 != NULL);
    test_object_1_t *obj4 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 18);
    assert(obj4 != NULL);
    test_object_1_t *obj5 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 19);
    assert(obj5 != NULL);

    assert(aa->count == 5);
    assert(aa_get_next_supplemental_data_ix(aa) == 0);
    assert(aa_get_next_supplemental_data_ix(aa) == 1);
    assert(aa_get_next_supplemental_data_ix(aa) == 2);
    assert(aa_get_next_supplemental_data_ix(aa) == 3);
    assert(aa_get_next_supplemental_data_ix(aa) == 4);
    assert(aa->supplemental_data_ix == 5);

    aa_free_addressed_array(aa);
    TEST_PASSED
}

void test_aa_get_next_supplemental_data_ix_counter_resets_to_zero_at_end() {
    aa_Array* aa = aa_malloc_addressed_array(sizeof (test_object_1_t), 10, 10);

    test_object_1_t *obj1 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 15);
    assert(obj1 != NULL);
    test_object_1_t *obj2 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 16);
    assert(obj2 != NULL);
    test_object_1_t *obj3 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 17);
    assert(obj3 != NULL);
    test_object_1_t *obj4 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 18);
    assert(obj4 != NULL);
    test_object_1_t *obj5 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 19);
    assert(obj5 != NULL);

    assert(aa->count == 5);
    aa->supplemental_data_ix = 5;
    assert(aa_get_next_supplemental_data_ix(aa) == 0);
    assert(aa->supplemental_data_ix == 1);
    assert(aa_get_next_supplemental_data_ix(aa) == 1);

    aa_free_addressed_array(aa);
    TEST_PASSED
}

void test_aa_get_next_supplemental_data_ix_counter_resets_to_zero_if_array_shrinks_below_ix() {
    aa_Array* aa = aa_malloc_addressed_array(sizeof (test_object_1_t), 10, 10);

    test_object_1_t *obj1 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 15);
    assert(obj1 != NULL);
    test_object_1_t *obj2 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 16);
    assert(obj2 != NULL);
    test_object_1_t *obj3 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 17);
    assert(obj3 != NULL);
    test_object_1_t *obj4 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 18);
    assert(obj4 != NULL);
    test_object_1_t *obj5 = (test_object_1_t*) aa_allocate_pointer_to_new_slot(&aa, 19);
    assert(obj5 != NULL);

    assert(aa_get_next_supplemental_data_ix(aa) == 0);
    assert(aa_get_next_supplemental_data_ix(aa) == 1);
    assert(aa_get_next_supplemental_data_ix(aa) == 2);
    assert(aa_get_next_supplemental_data_ix(aa) == 3);

    assert(aa_drop_element(aa, 18) != -1);
    assert(aa_drop_element(aa, 19) != -1);
    assert(aa_get_next_supplemental_data_ix(aa) == 0);

    aa_free_addressed_array(aa);
    TEST_PASSED
}

int main() {
    clock_t start = clock();

    test_can_create_addressed_array_with_initial_dimensions();
    test_can_get_arbitrary_objects_id_value();
    test_a_created_addressed_array_has_an_initial_size_of_zero();
    test_a_pointer_to_the_first_data_slot_can_be_calculated();
    test_a_pointer_to_the_second_data_slot_can_be_calculated();
    test_allocating_a_new_pointer_updates_the_address_book();
    test_a_data_pointer_can_be_returned_by_searching_by_id();
    test_data_can_be_written_to_and_read_from_data_pointers();
    test_null_pointer_is_returned_when_element_id_is_not_found();
    test_null_pointer_is_returned_when_allocating_a_pointer_for_an_id_that_is_already_in_use();
    test_attempt_to_get_id_of_offset_that_is_out_of_range();
    test_data_block_is_expanded_when_data_block_hits_capacity();
    test_the_address_array_pointer_is_updated_if_realloc_is_called();
    test_delete_only_element();
    test_delete_element_that_doesnt_exist();
    test_deleting_the_last_data_element_does_not_cause_a_reshuffling();
    test_deleting_the_first_data_element_does_cause_a_reshuffling();
    test_deleting_a_middle_element_does_cause_a_reshuffling();

    test_aa_get_next_supplemental_data_ix_step_through_data();
    test_aa_get_next_supplemental_data_ix_counter_resets_to_zero_at_end();
    test_aa_get_next_supplemental_data_ix_counter_resets_to_zero_if_array_shrinks_below_ix();

    SUITE_END(start)
}

