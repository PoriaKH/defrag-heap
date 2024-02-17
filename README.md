# defrag-heap
defrag-heap is a custom heap memory, in essence, I created customized versions of the functionsmalloc,realloc, andfreethat when being used, it optimizes the de-fragmentation of heap address allocation.

### Usage
Write your own code in `./mm_alloc/mm_alloc.c`.<br/>
<br/>
You can use methods such as `mm_malloc()`, `mm_free()`, `mm_realloc()` which are optimized simulation of `malloc()`, `free()`, `realloc()`<br/>
</br>
Example:
```
int* data = mm_malloc(sizeof(int * 4));
```
### Build
Use `make` to build the project. The executable file will be in the `./build` directory. <br/>
`make clean` to clean the artifacts. <br/>
