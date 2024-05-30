/* Public domain, by rilysh <nightquick@proton.me>. */

#include <stdlib.h>
#include <string.h>

#include "dya.h"

/* Initialize the dya structure members. */
int dya_init(struct dya *dya)
{
	dya->ptr = calloc((size_t)1, sizeof(char));
	if (dya->ptr == NULL)
	        return (DYA_ALLOC_FAILED);

	/* Start with zeros, except pre_alloc_size. */
	dya->idx = 0;
	dya->old_size = 0;
	dya->pre_alloc_size = DYA_PRE_ALLOC_SIZE;
	dya->nelems_size = 0;

        return (DYA_ALLOC_FAILED);
}

/* Overwrite the pre-allocation size. */
void dya_set_pre_alloc_size(struct dya *dya, size_t new_size)
{
	dya->pre_alloc_size = new_size;
}

/* Get the pre-allocation size. */
size_t dya_get_pre_alloc_size(const struct dya *dya)
{
	return (dya->pre_alloc_size);
}

/* Push data to the memory. */
int dya_push(struct dya *dya, const void *data)
{
	const unsigned char *pdata;
	char *rptr;
	size_t asz;

	pdata = data;
	asz = strlen((const char *)pdata) + (size_t)1;
	
	/* Do not include the null terminated byte. */
	dya->old_size += asz;

	/* When dya->idx is lower or equal (<=) to 1, we will need to re-allocate
	   again. If the former one is false, and if we are out of index, in case
	   if newly passed data size is larger than our current pre-allocated
	   capacity, we will need to re-allocate the memory region again with the
	   newly passed data size plus the amount of pre-allocation we will do
	   (pre-allocation is optional). */
	if (dya->idx <= 1 || asz >= dya->idx) {
		dya->idx = dya->pre_alloc_size;
	        rptr = realloc(dya->ptr, dya->old_size + dya->idx);
		if (rptr == NULL) {
			free(dya->ptr);
			return (DYA_REALLOC_FAILED);
		}
		dya->ptr = rptr;
	} else {
		/* If the previously passed data size was bigger than our current
		   capacity of the index then it means that we cannot subtract that
		   previous size from the index, as it will cause an overflow. So
		   set the index value to zero (0), which means that we have exhausted
		   our index, and next time we need to do a re-allocation. */
		if (dya->prev_size >= dya->idx)
			dya->idx = 0;
		else
			dya->idx -= dya->prev_size;
	}

	/* Set the previous size later as we will use this before here we set. */
	dya->prev_size = asz;

	/* C89 does not allow implicit casting for a type that is
	   a void * if pointer arithmetic is being used on that pointer. */
	memcpy((unsigned char *)dya->ptr + dya->nelems_size, pdata, asz);

	/* Number of elements, without the length of the terminating null byte.
	   (Behave like strlen()). */
	dya->nelems_size += asz - 1;
	return (DYA_PUSH_SUCCESS);
}

/* Pop data (last element) from the memory. */
int dya_pop(struct dya *dya)
{
	unsigned char *aptr;

	if (dya->nelems_size > 1) {
		aptr = dya->ptr;
		/* Number of elements (nelems_size): Hello world
		   Previous pushed data size (prev_size): 11 */
		aptr[dya->nelems_size - dya->prev_size + 1] = '\0';
		return (DYA_POP_SUCCESS);
	} else {
		return (DYA_BUFFER_EMPTY);
	}
}

/* Cut data from left to right. */
int dya_from_left_cut(struct dya *dya, size_t left, size_t right)
{
	unsigned char *aptr;

	if (dya->nelems_size > 1) {
		aptr = dya->ptr;
		aptr[right - left] = '\0';
	        return (DYA_CUT_SUCCESS);
	} else {
	        return (DYA_BUFFER_EMPTY);
	}
}

/* Cut data from right to left. */
int dya_from_right_cut(struct dya *dya, size_t left, size_t right)
{
        unsigned char *aptr;

	if (dya->nelems_size > 1) {
		aptr = dya->ptr;
		aptr[left - right] = '\0';
	        return (DYA_CUT_SUCCESS);
	} else {
		return (DYA_BUFFER_EMPTY);
	}
}

void dya_clear(struct dya *dya)
{
        unsigned char *aptr;

	aptr = dya->ptr;

	/* Just set the first value as a null terminated. */
	/* NOTE: Maybe use memset() to clear the entire buffer? */
	*aptr = '\0';

	/* Reset the nelems_size, so we can count again from zero,
	   if push is called after the clear operation. */
	dya->nelems_size = 0;
}

/* Free the main pointer, and reset other members
   of the dya structure. */
void dya_free(struct dya *dya)
{
	if (dya->ptr) {
		free(dya->ptr);
		dya->ptr = NULL;
		dya->idx = 0;
		dya->old_size = 0;
		/* If the pre-allocation size was set manually before
		   this will reset it to the default value. */
		dya->pre_alloc_size = DYA_PRE_ALLOC_SIZE;
		dya->nelems_size = 0;
	}
}
