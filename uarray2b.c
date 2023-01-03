#include "uarray2b.h"
#include "uarray.h"
#include "assert.h"

#define T UArray2b_T
struct T {
	int width;
	int height;
	int size;
	int blocksize;
	UArray_T data;
};


T UArray2b_new(int width, int height, int size, int blocksize) 
{
        assert(width > 0 && height > 0 && size > 0 && blocksize > 1);

	int w_blocks = (width / blocksize);
	if (width % blocksize) {
		w_blocks++;
	}
	int h_blocks = (height / blocksize);
	if (height % blocksize) {
		h_blocks++;
	}

	UArray_T data = UArray_new(w_blocks, sizeof(UArray_T));
	for (int i = 0; i < width; i++) {
		UArray_T col_data = UArray_new(h_blocks, size * blocksize * blocksize);
                UArray_T *hold_col = UArray_at(data, i);
                *hold_col = col_data;
	}

	T ret = malloc(sizeof(struct UArray2b_T));
	ret->width = width;
	ret->height = height;
	ret->blocksize = blocksize;
	ret->data = data;
	ret->size = size;
	return ret;
}

T UArray2b_new_64K_block(int width, int height, int size) {
	assert(width > 0 && height > 0 && size > 0);
	int blocksize = 256;

	int w_blocks = (width / blocksize);
	if (width % blocksize) {
		w_blocks++;
	}
	int h_blocks = (height / blocksize);
	if (height % blocksize) {
		h_blocks++;
	}

	UArray_T data = UArray_new(w_blocks, sizeof(UArray_T));
	for (int i = 0; i < w_blocks; i++) {
		UArray_T col_data = UArray_new(h_blocks, size * blocksize * blocksize);
		UArray_T* hold_col = UArray_at(data, i);
		*hold_col = col_data;
	}

	T ret = malloc(sizeof(struct UArray2b_T));
	ret->width = width;
	ret->height = height;
	ret->data = data;
	ret->size = size;
	ret->blocksize = blocksize;
	return ret;
}

void UArray2b_free(T *UArray2) 
{
        assert(UArray2 != NULL);

        UArray_T data = (*UArray2)->data;

	int w_blocks = ((*UArray2)->width / (*UArray2)->blocksize);

        /* free each column one by one */
	for (int i = 0; i < w_blocks; i++) {
                UArray_free(UArray_at(data, i));
	}
	UArray_free(&data);
	free(*UArray2);
}

int UArray2b_width(T UArray2) 
{
        assert(UArray2 != NULL);
	return UArray2->width;
}

int UArray2b_height(T UArray2) 
{
        assert(UArray2 != NULL);
	return UArray2->height;
}

int UArray2b_size(T UArray2) 
{
        assert(UArray2 != NULL);
	return UArray2->size;
}

int UArray2b_blocksize(T UArray2) 
{
	assert(UArray2 != NULL);
	return UArray2->blocksize;
}

void* UArray2b_at(T UArray2, int x, int y) 
{
        assert(UArray2 != NULL);

        /* confirm position/indices within bounds */
        assert(x >= 0 && x < UArray2->width);
        assert(y >= 0 && y < UArray2->height);
	
	int blocksize = UArray2->blocksize;

        UArray_T *curr_col = UArray_at(UArray2->data, x / blocksize);
	int* block = UArray_at(*curr_col, y / blocksize);
	return &block[blocksize * (x % blocksize) + (y % blocksize)];
}

void UArray2b_map(T UArray2, 
                           void apply(int x, 
                                      int y, 
                                      T UArray2, 
                                      void *elem, 
                                      void *cl),
                           void *cl) 
{
        assert(UArray2 != NULL);
	int blocksize = UArray2->blocksize;

        /* hit each element, with vertical/height marking inner */
	for (int x = 0; x < UArray2->width; x++) {
		UArray_T* curr_col = UArray_at(UArray2->data, x / blocksize);
		for (int y = 0; y < UArray2->height; y++) {
			int* block = UArray_at(*curr_col, y / blocksize);
			apply(x, y, UArray2, &block[blocksize * (x % blocksize) + (y % blocksize)], cl);
		}
	}
}

#undef T