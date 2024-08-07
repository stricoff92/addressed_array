# AddressedArray

## A Dynamic C Data Structure

### Goals: Create a datastructure and methods that support:
 - Fast Iteration
 - Fast Hash Lookups

Each AddressedArray instance contains 2 things:
 - Meta information describing array contents: (element width, count)
 - KHASH data structure that maps object ID to memory offsets
 - Flexible array member

When a slot is allocated: a pointer is generated which points to the last slot on the array.

When an element is deleted the last element of the array is moved to the vacated spot.


### Example Usage

```c

#include "addressed_array.h"
#include "base_object.h"

// The first prop. of the elem. should be an uint32_t with the field name "id".
typedef struct TestObject {
    BaseAAObject base;
    int  a;
} TestObject;

AddressedArray* aa = aa_malloc_addressed_array(
    sizeof (TestElement), // Element Size
    10,                   // Initial Capacity
    10                    // Reallocation Size
);

// Reserve slots for 2 elements with ids 1337 and 42069 respectively.
// We pass a pointer to aa so the caller's reference can be updated if
// aa_allocate_pointer_to_new_slot calls realloc.
TestObject *to1 = (TestObject*) aa_allocate_pointer_to_new_slot(aa, &aa, 1337);
TestObject *to2 = (TestObject*) aa_allocate_pointer_to_new_slot(aa, &aa, 42069);

// Write data to the data structure
to1->base.id = 1337;
to1->a = 123;
to2->base.id = 42069;
to2->a = 1337;

// write data using memcpy
TestObject data;
data.base.id = 1337;
data.a = 123;
TestObject *to1 = (TestObject*) aa_allocate_pointer_to_new_slot(aa, &aa, 1337);
memcpy((void*) to1, (void*) &data, sizeof (TestObject))

// Query object 1337
TestObject *q1 = (TestObject*) aa_get_pointer_from_id(aa, 1337)
printf("id = %d\n", q1->base.id);

// Loop through elements
TestObject* o;
for(int i = 0; i < aa->count; i++)
{
    o = ((TestObject*) aa->data) + i;
    printf("%d %d\n", o->base.id, o->a)
}

// Delete object 1337 and move object 42069 to the zero'th slot.
aa_drop_element(aa, 1337);


// Clean up
aa_free_addressed_array(aa);

```

This code was compiled on a `x86_64 GNU/Linux` machine. KHASH is required to use AddressedArrays.


![](https://media.giphy.com/media/QQKhpfeRQqz6M/giphy.gif)
