Q4 Romney Duncan
V#00895553
I didn't implement Optimal replacement scheme since as far as I know, I have yet to accumulate magic powers

To implement the other schemes there is an if conditional in resolve_address for each scheme

FIFO:
added a global variable called counter, this is used in the FIFO conditional to replace the page at index counter % size_of_memory
after the replacement counter is iterated by + 1

LRU:
added a global variable called last_used that is iterated by one each time resolve_address is called, and added an int called lu to the page table struct
Whenever a page is refrenced added to the table, or replaced the value lu is set to last_used. 
In the LRU conditional the index with the min value of lu is found and replaced

SECONDCHANCE:
this uses the counter global variable, and added an int called refrenced into the page table struct
Whenever a page is refrenced, added or replaced the refrenced int is set to true.
In the SECONDCHANCE conditional the table is looped through while the refrence bit is true and sets it to false if it is. once the loop ends the page is swapped out at that index.
