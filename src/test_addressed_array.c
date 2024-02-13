



#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"

#include "assert.h"

#include "addressed_array.h"
#include "base_object.h"



#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#define TEST_PASSED printf("OK: %s\n", __func__)

#endif


typedef struct TestElement {
    int  a;
    int  b;
    char c;
    char d;
} TestElement;

typedef struct TestObject {
    BaseAAObject base;
    int  a;
} TestObject;

int test_can_create_addressed_array_with_initial_dimensions() {
    AddressedArray* aa = aa_malloc_addressed_array(24, 10, 12);
    assert(aa->element_width == 24);
    assert(aa->capacity == 10);
    assert(aa->realloc_size == 12);
    assert(aa->count == 0);

    aa_free_addressed_array(aa);
    TEST_PASSED;
    return 1;
}

int test_a_created_addressed_array_has_an_initial_size_of_zero() {
    AddressedArray* aa = aa_malloc_addressed_array(24, 10, 10);
    assert(kh_size(aa->address_book) == 0);
    assert(aa->count == 0);

    aa_free_addressed_array(aa);
    TEST_PASSED;
    return 1;
}


int test_a_pointer_to_the_first_data_slot_can_be_calculated() {
    AddressedArray* aa = aa_malloc_addressed_array(sizeof (TestElement), 10, 10);
    TestElement *te = (TestElement*) aa_allocate_pointer_to_new_slot(aa, 1);

    // pointer to first data element should be same as the address
    // of the contiguously allocated memory.
    assert(te == aa->data);

    aa_free_addressed_array(aa);
    TEST_PASSED;
    return 1;
}

int test_a_pointer_to_the_second_data_slot_can_be_calculated() {
    AddressedArray* aa = aa_malloc_addressed_array(sizeof (TestElement), 10, 10);
    TestElement *te1 = (TestElement*) aa_allocate_pointer_to_new_slot(aa, 1);
    TestElement *te2 = (TestElement*) aa_allocate_pointer_to_new_slot(aa, 2);

    assert(te2 == ((TestElement*) aa->data) + 1);

    aa_free_addressed_array(aa);
    TEST_PASSED;
    return 1;
}

int test_allocating_a_new_pointer_updates_the_address_book() {
    AddressedArray* aa = aa_malloc_addressed_array(sizeof (TestElement), 10, 10);
    TestElement *te1 = (TestElement*) aa_allocate_pointer_to_new_slot(aa, 1337);
    TestElement *te2 = (TestElement*) aa_allocate_pointer_to_new_slot(aa, 42069);

    assert(kh_size(aa->address_book) == 2);
    assert(aa->count == 2);
    assert(
        kh_val(aa->address_book, kh_get(id_ix_map, aa->address_book, 1337)) == 0
    );
    assert(
        kh_val(aa->address_book, kh_get(id_ix_map, aa->address_book, 42069)) == 1
    );

    aa_free_addressed_array(aa);
    TEST_PASSED;
    return 1;
}

int test_a_data_pointer_can_be_returned_by_searching_by_id() {
    AddressedArray* aa = aa_malloc_addressed_array(sizeof (TestElement), 10, 10);
    TestElement *te1 = (TestElement*) aa_allocate_pointer_to_new_slot(aa, 1337);
    TestElement *te2 = (TestElement*) aa_allocate_pointer_to_new_slot(aa, 42069);
    TestElement *te3 = (TestElement*) aa_allocate_pointer_to_new_slot(aa, 12345);
    assert(kh_size(aa->address_book) == 3);
    assert(aa->count == 3);

    TestElement* q2 = (TestElement*) aa_get_pointer_from_id(aa, 42069);
    TestElement* q1 = (TestElement*) aa_get_pointer_from_id(aa, 1337);
    TestElement* q3 = (TestElement*) aa_get_pointer_from_id(aa, 12345);

    assert(((TestElement*) aa->data) == q1);
    assert(((TestElement*) aa->data) + 1 == q2);
    assert(((TestElement*) aa->data) + 2 == q3);

    aa_free_addressed_array(aa);
    TEST_PASSED;
    return 1;
}

int test_data_can_be_written_to_and_read_from_data_pointers() {
    AddressedArray* aa = aa_malloc_addressed_array(sizeof (TestElement), 10, 10);
    TestElement *te1 = (TestElement*) aa_allocate_pointer_to_new_slot(aa, 111);
    TestElement *te2 = (TestElement*) aa_allocate_pointer_to_new_slot(aa, 222);

    te1->a = 60;
    te1->b = 61;
    te1->c = 'a';
    te1->d = 'b';

    te2->a = 62;
    te2->b = 63;
    te2->c = 'c';
    te2->d = 'd';

    TestElement* q2 = (TestElement*) aa_get_pointer_from_id(aa, 222);
    TestElement* q1 = (TestElement*) aa_get_pointer_from_id(aa, 111);

    assert(q1->a == 60);
    assert(q1->b == 61);
    assert(q1->c == 'a');
    assert(q1->d == 'b');
    assert(q2->a == 62);
    assert(q2->b == 63);
    assert(q2->c == 'c');
    assert(q2->d == 'd');

    aa_free_addressed_array(aa);
    TEST_PASSED;
    return 1;
}

int test_null_pointer_is_returned_when_element_id_is_not_found() {
    AddressedArray* aa = aa_malloc_addressed_array(sizeof (TestElement), 10, 10);
    TestElement *te1 = (TestElement*) aa_allocate_pointer_to_new_slot(aa, 111);
    TestElement *te2 = (TestElement*) aa_allocate_pointer_to_new_slot(aa, 222);

    TestElement* q1 = (TestElement*) aa_get_pointer_from_id(aa, 111); // exists
    TestElement* q2 = (TestElement*) aa_get_pointer_from_id(aa, 121); // doesn't exist

    assert(q1 != NULL); // exists
    assert(q2 == NULL); // doesn't exist

    aa_free_addressed_array(aa);
    TEST_PASSED;
    return 1;
}

int test_data_block_is_expanded_when_data_block_hits_capacity() {
    AddressedArray* aa = aa_malloc_addressed_array(
        sizeof (TestElement),
        4, // initial capacity (num. of elements)
        2 // reallocation size (num. of elements)
    );

    assert(aa->capacity == 4);
    assert(aa->realloc_size == 2);
    assert(aa->count == 0);

    aa_allocate_pointer_to_new_slot(aa, 11);
    assert(aa->capacity == 4);
    assert(aa->realloc_size == 2);
    assert(aa->count == 1);
    assert(kh_size(aa->address_book) == 1);


    aa_allocate_pointer_to_new_slot(aa, 12);
    assert(aa->capacity == 4);
    assert(aa->realloc_size == 2);
    assert(aa->count == 2);
    assert(kh_size(aa->address_book) == 2);
    aa_allocate_pointer_to_new_slot(aa, 13);
    assert(aa->capacity == 4);
    assert(aa->realloc_size == 2);
    assert(aa->count == 3);
    assert(kh_size(aa->address_book) == 3);
    aa_allocate_pointer_to_new_slot(aa, 14);
    assert(aa->capacity == 4);
    assert(aa->realloc_size == 2);
    assert(aa->count == 4);
    assert(kh_size(aa->address_book) == 4);

    // Adding this element will increase capacity
    aa_allocate_pointer_to_new_slot(aa, 15);
    assert(aa->capacity == 6);
    assert(aa->realloc_size == 2);
    assert(aa->count == 5);
    assert(kh_size(aa->address_book) == 5);

    aa_allocate_pointer_to_new_slot(aa, 16);
    assert(aa->capacity == 6);
    assert(aa->realloc_size == 2);
    assert(aa->count == 6);
    assert(kh_size(aa->address_book) == 6);

    // Adding this element will increase capacity
    aa_allocate_pointer_to_new_slot(aa, 17);
    assert(aa->capacity == 8);
    assert(aa->realloc_size == 2);
    assert(aa->count == 7);
    assert(kh_size(aa->address_book) == 7);

    TestElement* te1 = aa_get_pointer_from_id(aa, 11);
    assert(((TestElement*) aa->data) == te1);
    TestElement* te2 = aa_get_pointer_from_id(aa, 12);
    assert(((TestElement*) aa->data + 1) == te2);
    TestElement* te3 = aa_get_pointer_from_id(aa, 13);
    assert(((TestElement*) aa->data + 2) == te3);
    TestElement* te4 = aa_get_pointer_from_id(aa, 14);
    assert(((TestElement*) aa->data + 3) == te4);
    TestElement* te5 = aa_get_pointer_from_id(aa, 15);
    assert(((TestElement*) aa->data + 4) == te5);
    TestElement* te6 = aa_get_pointer_from_id(aa, 16);
    assert(((TestElement*) aa->data + 5) == te6);
    TestElement* te7 = aa_get_pointer_from_id(aa, 17);
    assert(((TestElement*) aa->data + 6) == te7);

    aa_free_addressed_array(aa);
    TEST_PASSED;
    return 1;
}

int test_delete_only_element() {
    AddressedArray* aa = aa_malloc_addressed_array(sizeof (TestObject), 10, 10);
    TestObject *te1 = (TestObject*) aa_allocate_pointer_to_new_slot(aa, 111);
    te1->base.id = 111;
    te1->a = 69;
    assert(aa->count = 1);
    assert(kh_size(aa->address_book) == 1);

    aa_drop_element(aa, 111);
    assert(aa->count == 0);
    assert(kh_size(aa->address_book) == 0);

    aa_free_addressed_array(aa);
    TEST_PASSED;
    return 1;
}

int test_delete_element_that_doesnt_exist() {
    AddressedArray* aa = aa_malloc_addressed_array(sizeof (TestObject), 10, 10);
    TestObject *te1 = (TestObject*) aa_allocate_pointer_to_new_slot(aa, 111);
    te1->base.id = 111;
    te1->a = 69;
    assert(aa->count = 1);
    assert(kh_size(aa->address_book) == 1);

    int ret = aa_drop_element(aa, 222);
    assert(ret == -1);
    assert(aa->count == 1);
    assert(kh_size(aa->address_book) == 1);

    TestObject* q1 = (TestObject*) aa_get_pointer_from_id(aa, 111);
    assert(q1->base.id == 111);
    assert(q1->a == 69);

    aa_free_addressed_array(aa);
    TEST_PASSED;
    return 1;
}

int test_deleting_the_last_data_element_does_not_cause_a_reshuffling() {
    AddressedArray* aa = aa_malloc_addressed_array(sizeof (TestObject), 10, 10);
    TestObject *te1 = (TestObject*) aa_allocate_pointer_to_new_slot(aa, 111);
    TestObject *te2 = (TestObject*) aa_allocate_pointer_to_new_slot(aa, 222);
    assert(aa->count = 2);

    te1->base.id = 111;
    te1->a = 69;
    te2->base.id = 222;
    te2->a = 420;

    // Act: drop the last element.
    int dropped_ix = aa_drop_element(aa, 222);
    assert(dropped_ix == 1);
    assert(aa->count = 1);

    // Assert: first element unsullied
    assert(((TestObject*) aa->data)->base.id == 111);
    assert(((TestObject*) aa->data)->a == 69);

    khint_t i = kh_get(id_ix_map, aa->address_book, 111);
    size_t ix = kh_val(aa->address_book, i);
    assert(ix == 0);

    aa_free_addressed_array(aa);
    TEST_PASSED;
    return 1;
}

int test_deleting_the_first_data_element_does_cause_a_reshuffling() {
    AddressedArray* aa = aa_malloc_addressed_array(sizeof (TestObject), 10, 10);
    TestObject *te1 = (TestObject*) aa_allocate_pointer_to_new_slot(aa, 111);
    TestObject *te2 = (TestObject*) aa_allocate_pointer_to_new_slot(aa, 222);
    TestObject *te3 = (TestObject*) aa_allocate_pointer_to_new_slot(aa, 333);
    assert(aa->count = 3);

    te1->base.id = 111;
    te1->a = 69;
    te2->base.id = 222;
    te2->a = 420;
    te3->base.id = 333;
    te3->a = 1999;

    assert(((TestObject*) aa->data)->base.id == 111);
    assert(((TestObject*) aa->data)->a == 69);

    // Act: drop the first element.
    int dropped_ix = aa_drop_element(aa, 111);
    assert(dropped_ix == 0);
    assert(aa->count = 2);

    // Assert: first element now holds object 333
    assert(((TestObject*) aa->data)->base.id == 333);
    assert(((TestObject*) aa->data)->a == 1999);
    khint_t i = kh_get(id_ix_map, aa->address_book, 333);
    size_t ix = kh_val(aa->address_book, i);
    assert(ix == 0);

    // Assert: second element still holds object 222
    assert((((TestObject*) aa->data) + 1)->base.id == 222);
    assert((((TestObject*) aa->data) + 1)->a == 420);
    i = kh_get(id_ix_map, aa->address_book, 222);
    ix = kh_val(aa->address_book, i);
    assert(ix == 1);

    // Assert object 111 is gone
TestObject* q_deleted = (TestObject*) aa_get_pointer_from_id(aa, 111);
    assert(q_deleted == NULL);

    aa_free_addressed_array(aa);
    TEST_PASSED;
    return 1;
}

int test_deleting_a_middle_element_does_cause_a_reshuffling() {
    AddressedArray* aa = aa_malloc_addressed_array(sizeof (TestObject), 10, 10);
    TestObject *te1 = (TestObject*) aa_allocate_pointer_to_new_slot(aa, 111);
    TestObject *te2 = (TestObject*) aa_allocate_pointer_to_new_slot(aa, 222);
    TestObject *te3 = (TestObject*) aa_allocate_pointer_to_new_slot(aa, 333);
    assert(aa->count = 3);

    te1->base.id = 111;
    te1->a = 69;
    te2->base.id = 222;
    te2->a = 420;
    te3->base.id = 333;
    te3->a = 1999;


    // Act: drop the middle element.
    int dropped_ix = aa_drop_element(aa, 222);
    assert(dropped_ix == 1);
    assert(aa->count = 2);

    // Assert: first element still holds object 111
    assert(((TestObject*) aa->data)->base.id == 111);
    assert(((TestObject*) aa->data)->a == 69);
    khint_t i = kh_get(id_ix_map, aa->address_book, 111);
    size_t ix = kh_val(aa->address_book, i);
    assert(ix == 0);

    // Assert: second element now holds object 333
    assert((((TestObject*) aa->data) + 1)->base.id == 333);
    assert((((TestObject*) aa->data) + 1)->a == 1999);
    i = kh_get(id_ix_map, aa->address_book, 333);
    ix = kh_val(aa->address_book, i);
    assert(ix == 1);

    // Assert object 222 is gone
    TestObject* q_deleted = (TestObject*) aa_get_pointer_from_id(aa, 222);
    assert(q_deleted == NULL);

    aa_free_addressed_array(aa);
    TEST_PASSED;
    return 1;
}


int main() {
    clock_t start, end;
    start = clock();
    int c = 0;

    c += test_can_create_addressed_array_with_initial_dimensions();
    c += test_a_created_addressed_array_has_an_initial_size_of_zero();
    c += test_a_pointer_to_the_first_data_slot_can_be_calculated();
    c += test_a_pointer_to_the_second_data_slot_can_be_calculated();
    c += test_allocating_a_new_pointer_updates_the_address_book();
    c += test_a_data_pointer_can_be_returned_by_searching_by_id();
    c += test_data_can_be_written_to_and_read_from_data_pointers();
    c += test_null_pointer_is_returned_when_element_id_is_not_found();
    c += test_data_block_is_expanded_when_data_block_hits_capacity();
    c += test_delete_only_element();
    c += test_delete_element_that_doesnt_exist();
    c += test_deleting_the_last_data_element_does_not_cause_a_reshuffling();
    c += test_deleting_the_first_data_element_does_cause_a_reshuffling();
    c += test_deleting_a_middle_element_does_cause_a_reshuffling();

    end = clock();
    double duration = ((double)end - start)/CLOCKS_PER_SEC;
    printf("%d in %.2f ms ", c, duration * 1000);
    return 0;
}

