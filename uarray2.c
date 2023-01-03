/*
 *              ** uarray2.c **
 *    Authors: Adrien Lynch & Henry Nissen
 *                 jlynch07 & hnisse01
 *       Date: Oct 3, 2022
 * Assignment: HW2
 *    Summary: Implementation of the UArray2 interface,
 *             with all relevant functions and libraries
 *
 */

#include "uarray2.h"
#include "uarray.h"
#include "assert.h"

#define T UArray2_T
struct T {
	int width;
	int height;
	int size;
	UArray_T data;
};


T UArray2_new(int width, int height, int size) 
{
        assert(width > 0 && height > 0 && size > 0);

        /* 
           structured so that UArray instance exists as a row,
           then each column in row has nested UArray instance
        */
	UArray_T data = UArray_new(width, sizeof(UArray_T));
	for (int i = 0; i < width; i++) {
		UArray_T col_data = UArray_new(height, size);
                UArray_T *hold_col = UArray_at(data, i);
                *hold_col = col_data;
	}

	T ret = malloc(sizeof(struct UArray2_T));
	ret->width = width;
	ret->height = height;
	ret->data = data;
	ret->size = size;
	return ret;
}

void UArray2_free(T *UArray2) 
{
        assert(UArray2 != NULL);

        UArray_T data = (*UArray2)->data;

        /* free each column one by one */
	for (int i = 0; i < (*UArray2)->width; i++) {
                UArray_free(UArray_at(data, i));
	}
	UArray_free(&data);
	free(*UArray2);
}

int UArray2_width(T UArray2) 
{
        assert(UArray2 != NULL);
	return UArray2->width;
}

int UArray2_height(T UArray2) 
{
        assert(UArray2 != NULL);
	return UArray2->height;
}

int UArray2_size(T UArray2) 
{
        assert(UArray2 != NULL);
	return UArray2->size;
}

void* UArray2_at(T UArray2, int x, int y) 
{
        assert(UArray2 != NULL);

        /* confirm position/indices within bounds */
        assert(x >= 0 && x < UArray2->width);
        assert(y >= 0 && y < UArray2->height);
	
        UArray_T *curr_col = UArray_at(UArray2->data, x);
        return UArray_at(*curr_col, y);
}

void UArray2_map_row_major(T UArray2, 
                           void apply(int x, 
                                      int y, 
                                      T UArray2, 
                                      void *closure1, 
                                      void *closure),
                           void *closure) 
{
        assert(UArray2 != NULL);

        /* hit each element, with horizontal/width marking inner */ 
	for (int i = 0; i < UArray2->height; i++) {
		for (int j = 0; j < UArray2->width; j++) {
			void* val = UArray2_at(UArray2, j, i);
			if (val != NULL) {
				apply(j, i, UArray2, val, closure);
			}
		}
	}
}

void UArray2_map_col_major(T UArray2, 
                           void apply(int x, 
                                      int y, 
                                      T UArray2, 
                                      void *closure1, 
                                      void *closure),
                           void *closure) 
{
        assert(UArray2 != NULL);

        /* hit each element, with vertical/height marking inner */
	for (int i = 0; i < UArray2->width; i++) {
		for (int j = 0; j < UArray2->height; j++) {
			void* val = UArray2_at(UArray2, i, j);
			if (val != NULL) {
				apply(i, j, UArray2, val, closure);
			}
		}
	}
}

#undef T