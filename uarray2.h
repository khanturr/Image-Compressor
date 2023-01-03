/*
 *              ** uarray2.h **
 *    Authors: Adrien Lynch & Henry Nissen
 *                 jlynch07 & hnissen01  
 *       Date: Oct 3, 2022
 * Assignment: HW2
 *    Summary: The UArray2 interface, with all relevant functions and libraries
 * 
 */

#ifndef UARRAY2_INCLUDED
#define UARRAY2_INCLUDED
#include <stdio.h>
#include <stdlib.h>

#define T UArray2_T
typedef struct T* T;

/* UArray2_new
    Purpose: Instantiates a new UArray2 using the width and height parameters.
    Expectations: This function assumes that all arguments will be greater 
                  than 0.
    Input: Width & height of desired UArray2 structure, amount of space in 
           memory needed to hold structure based on type.
    Output: An instance of a newly-allocated UArray2, of type UArray2_T.
*/
extern T UArray2_new(int width, int height, int size);

/* UArray2_free
   Purpose: Frees memory allocated for the UArray2 instance specified as 
            parameter.
   Expectations: This function assumes that the UArray2 exists and has had 
                 memory allocated.
   Input: Pointer to existing UArray2.
   Output: N/A - void function - end result: memory freed.
*/
extern void UArray2_free(T *UArray2);

/* UArray2_width
    Purpose: For a given instance of a UArray2, returns the length of the 
             one-dimensional (horizontal) array within the two-dimensional 
             structure.
    Expectations: This function assumes that the UArray2 passed is currently 
                  allocated.
    Input: Existing instance of UArray2.
    Output: An integer indicating width of supplied UArray2.
*/
extern int UArray2_width(T UArray2);

/* UArray2_height
    Purpose: For a given instance of a UArray2, returns the length of the 
             two-dimensional (vertical) array.
    Expectations: This function assumes that the UArray2 passed is currently 
                  allocated.
    Input: Existing instance of UArray2.
    Output: An integer indicating height of supplied UArray2.
*/
extern int UArray2_height(T UArray2);

/* UArray2_size
   Purpose: For a given UArray2 instance, returns the size of the 2-dimensional 
            array (i.e. the amount of space that the array occupies, given the 
            type of its contents).
   Expectations: This function assumes that the UArray2 provided as parameter 
                 exists and has 0 or more elements.
   Input: Existing instance of UArray2.
   Output: An integer indicating size of supplied UArray2.
*/
extern int UArray2_size(T UArray2);

/* UArray2_at 
    Purpose: Returns the value (pointed to by void pointer) at a given position 
             of a UArray2 instance.
    Expectations: This function assumes that the UArray2 passed to the function 
                  is a valid instance, and that both index variables are 
                  between 0 and the maximum respective width/height of the 
                  UArray2.
    Input: Existing instance of UArray2, horizontal position/index in UArray2,
           vertical position/index in UArray2.
    Output: Void pointer pointing to value of element at the specified position 
            in UArray2 instance.
*/
extern void* UArray2_at(T UArray2, int x, int y);

/* UArray2_map_row_major
    Purpose: Iterate across all elements on a given UArray2 row by row, 
             executing a given function on all of its elements.
    Expectations: This function assumes that the UArray2 passed to the 
                  function is a valid instance, and that the function passed 
                  returns void and takes a pointer to a void pointer which 
                  represents a given element of the UArray2.
    Input: Existing instance of UArray2, outside function that takes position 
           (horizontal & vertical) of element, spare pointers for closure
    Output: N/A - void function - end result: given function “apply” executed 
            upon all elements in supplied UArray2 instance.
*/
extern void UArray2_map_row_major(T UArray2, 
                                  void apply(int x, 
                                             int y, 
                                             T UArray2, 
                                             void *closure1, 
                                             void *closure),
                                  void *closure);

/* UArray2_map_column_major
    Purpose: Iterate across all elements on a given UArray2 column by column, 
             executing a given function on all of its elements.
    Expectations: This function assumes that the UArray2 passed to the 
                  function is a valid instance, and that the function passed 
                  returns void and takes a pointer to a void pointer which 
                  represents a given element of the UArray2.
    Input: Existing instance of UArray2, outside function that takes position 
           (horizontal & vertical) of element, spare pointers for closure
    Output: N/A - void function - end result: given function “apply” executed 
            upon all elements in supplied UArray2 instance.
*/
extern void UArray2_map_col_major(T UArray2, 
                                  void apply(int x, 
                                             int y, 
                                             T UArray2, 
                                             void *closure1, 
                                             void *closure),
                                  void *closure);

#undef T
#endif /* UARRAY2_INCLUDED */
