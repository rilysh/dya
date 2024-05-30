#ifndef DYA_H
# define DYA_H

/* Pre-allocation size. */
#define DYA_PRE_ALLOC_SIZE    (4096 * 18)

struct dya {
	/* (Public) Directs to the memory pointer. */
	void *ptr;

	/* (Private) Keep adding the size of the data
	   that is going to be pushed. */ 
        size_t old_size;

	/* (Private) Size of the data that was previously
	   pushed, if any. */
	size_t prev_size;

	/* (Private) Size of the memory that will be allocated
	   and if re-allocation is not needed, it will be subtracted
	   from the prev_size. */
        size_t idx;

	/* (Public) Pre-allocation size or extra size, that will
	   be allocated alongside the length of the input data and
	   the total size of the memory region. */
	size_t pre_alloc_size;

	/* (Private) Number of elements or the size of the pushed
	   data. More pushed data size will be added with the previous
	   number of elements size. */
	size_t nelems_size;
};

/* Constants. */
enum {
	/* Allocation failed. */
	DYA_ALLOC_FAILED   = -1,
	/* Reallocation failed. */
	DYA_REALLOC_FAILED = -2,
	/* Buffer is empty. */
	DYA_BUFFER_EMPTY   = -3,
	/* Allocation succeed. */
	DYA_ALLOC_SUCCESS  = 1,
	/* Push succeed. */ 
	DYA_PUSH_SUCCESS   = 2,
	/* Pop succeed. */
	DYA_POP_SUCCESS    = 3,
	/* Cut succeed. */
	DYA_CUT_SUCCESS    = 4
};

/* Prototypes. */
int dya_init(struct dya *dya);
void dya_set_pre_alloc_size(struct dya *dya, size_t new_size);
size_t dya_get_pre_alloc_size(const struct dya *dya);
int dya_push(struct dya *dya, const void *data);
int dya_pop(struct dya *dya);
int dya_from_left_cut(struct dya *dya, size_t left, size_t right);
int dya_from_right_cut(struct dya *dya, size_t left, size_t right);
void dya_clear(struct dya *dya);
void dya_free(struct dya *dya);

#endif /* DYA_H */
