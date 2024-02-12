# AddressedArray

## A Dynamic C Data Structure

__Problem:__ As a C n00b it seems like a data structure either supports fast iteration (1 contiguous block of memory), OR fast lookups using a hash function and something that looks like a linked list.

### Goals: Create a datastructure and methods that support:
 - Fast Iteration
 - Fast Hash Lookups

Each AddressedArray instance contains 2 things:
 - Void Pointer that references a contiguous block of memory
 - KHASH data structure that maps object ID to memory offsets

When a slot is allocated: a pointer is generated which points to the last slot on the array.

When an element is deleted the last element of the array is moved to the vacated spot.


### Example Usage

```c

#include "addressed_array.h"
#include "base_object.h"


typedef struct TestObject {
    BaseObject base;
    int  a;
} TestObject;

AddressedArray* aa = aa_malloc_addressed_array(
    sizeof (TestElement), // Element Size
    10,                   // Initial Capacity
    10                    // Reallocation Size
);

// Reserve slots for 2 elements with ids 1337 and 42069 respectively.
TestObject *to1 = (TestObject*) aa_allocate_pointer_to_new_slot(aa, 1337);
TestObject *to2 = (TestObject*) aa_allocate_pointer_to_new_slot(aa, 42069);

// Write data to the array
to1->base.id = 1337;
to1->a = 123;
to2->base.id = 42069;
to2->a = 1337;

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
