
# Design decisions

## Data structures
### Bufferpool:
The BufferPool uses an array of Page objects to store the actual pages and a list of PageNode objects to track and manage the pages.

### PageNode: 
Each PageNode contains the following:

- PageId: A unique identifier for the page, consisting of the file ID and the page number.
- pindex: The index of the page within the array.
- dirty: A boolean flag that indicates whether the page has been modified and needs to be flushed to disk.


## Managing bufferpool

### Accessing and adding pages:

When a page is accessed, its corresponding PageNode is moved to the head of the list, indicating that it has been recently used.

If a new page needs to be loaded and the list has reached its maximum capacity, the PageNode at the tail of the list (representing the least recently used page) will be evicted. The corresponding page in the array will be replaced by the new page.



### Writing and flushing pages:

When a page is modified, its PageNode sets the dirty flag to true, marking the page for flushing.

If a dirty page is evicted from the BufferPool, it will be written back to disk before being replaced, ensuring no data is lost.


# Time Spent and Challenges
I spent approximately 6 hours working on this assignment. The most challenging part was setting up the development environment and getting familiar with C++, especially with managing memory with stl containers and references in C++.