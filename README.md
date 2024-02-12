# AddressedArray

## A Dynamic C Data Structure

__Problem:__ As a C n00b it seems like a data structure either supports fast iteration (1 contiguous block of memory), OR fast lookups using a hash function and something that looks like a linked list.

### Goals: Create a datastructure and methods that support:
 - Fast Iteration
 - Fast Hash Lookups

Each AddressedArray instance contains 2 things:
 - Void Pointer that references a contiguous block of memory
 - KHASH data structure that maps object ID to memory offsets
