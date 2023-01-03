#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "pnmrdr.h"
#include "cputiming.h"

#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (0)

typedef struct ClosureStruct* ClosureStruct;
struct ClosureStruct {
        A2Methods_T methods;
        A2Methods_UArray2 a2_secondary;
};

static void
usage(const char* progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                "[-{row,col,block}-major] [filename]\n",
                progname);
        exit(1);
}

void populate_a2(FILE* file, A2Methods_T methods, A2Methods_UArray2* a2,
        A2Methods_UArray2* a2_secondary, bool rot);
void apply_0(int x, int y, A2Methods_UArray2 uarray2,
        void* elem, void* cl);
void apply_90(int x, int y, A2Methods_UArray2 uarray2,
        void* elem, void* cl);
void apply_180(int x, int y, A2Methods_UArray2 uarray2,
        void* elem, void* cl);
void apply_270(int x, int y, A2Methods_UArray2 uarray2,
        void* elem, void* cl);
void apply_flip_h(int x, int y, A2Methods_UArray2 uarray2,
        void* elem, void* cl);
void apply_flip_v(int x, int y, A2Methods_UArray2 uarray2,
        void* elem, void* cl);
void apply_transpose(int x, int y, A2Methods_UArray2 uarray2,
        void* elem, void* cl);
void write_timer_output(CPUTime_T timer, FILE* time_file, char* transform, 
        A2Methods_UArray2 uarray2, A2Methods_T methods);

int main(int argc, char* argv[])
{
        char* time_file_name = NULL;
        FILE* time_file = NULL;
        CPUTime_T timer = NULL;
        int   rotation = 0;
        bool trans = false;
        bool flip_h = false;
        bool flip_v = false;
        int   i;

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods);

        /* default to best map */
        A2Methods_mapfun* map = methods->map_default;
        assert(map);

        if (argc == 1) {
                usage(argv[0]);
        }

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major,
                                "row-major");
                }
                else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major,
                                "column-major");
                }
                else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                "block-major");
                }
                else if (strcmp(argv[i], "-rotate") == 0) {
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        char* endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                                rotation == 180 || rotation == 270)) {
                                fprintf(stderr,
                                        "Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) {    /* Not a number */
                                usage(argv[0]);
                        }
                }
                else if (strcmp(argv[i], "-flip") == 0) {
                        if (!(i + 1 < argc)) {      /* no flip value */
                                usage(argv[0]);
                        }
                        if (strcmp(argv[++i], "horizontal") == 0) {
                                flip_h = true;
                        }
                        else if (strcmp(argv[i], "vertical") == 0) {
                                flip_v = true;
                        }
                        else {
                                fprintf(stderr,
                                        "Flip must be horizontal or vertical\n"
                                );
                                usage(argv[0]);
                        }
                }
                else if (strcmp(argv[i], "-time") == 0) {
                        time_file_name = argv[++i];
                        time_file = fopen(time_file_name, "w");
                        if (!time_file) {
                                fprintf(stderr,
                                        "Could not open file %s\n", 
                                        time_file_name
                                );
                                usage(argv[0]);
                        }
                        timer = CPUTime_New();
                }
                else if (strcmp(argv[i], "-transpose") == 0) {
                        trans = true;
                }
                else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                        usage(argv[0]);
                }
                else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                }
                else {
                        break;
                }
        }

        FILE* file = fopen(argv[i], "r");
        if (file == NULL) {
                fprintf(stderr, "Last argument wasn't a file\n");
                usage(argv[0]);
        }

        A2Methods_UArray2 a2;
        ClosureStruct cls = malloc(sizeof(struct ClosureStruct));
        populate_a2(file, methods, &a2, &cls->a2_secondary, (rotation / 10) % 2);
        cls->methods = methods;
        fclose(file);

        if (timer != NULL) {
                CPUTime_Start(timer);
        }
        switch (rotation) {
                default:
                case (0): {
                        (*map)(a2, apply_0, (void*)cls);
                        write_timer_output(timer, time_file, "rotate 0",
                                a2, methods
                        );
                } break;
                case(90): {
                        (*map)(a2, apply_90, (void*)cls);
                        write_timer_output(timer, time_file, "rotate 90",
                                a2, methods
                        );
                } break;
                case (180): {
                        (*map)(a2, apply_180, (void*)cls);
                        write_timer_output(timer, time_file, "rotate 180",
                                a2, methods
                        );
                } break;
                case (270): {
                        (*map)(a2, apply_270, (void*)cls);
                        write_timer_output(timer, time_file, "rotate 270",
                                a2, methods
                        );
                } break;
        }

        if (flip_h) {
                if (timer != NULL) {
                        CPUTime_Start(timer);
                }
                (*map)(a2, apply_flip_h, (void*)cls);
                write_timer_output(timer, time_file, "flip horizontal", a2, 
                        methods
                );
        }
        if (flip_v) {
                if (timer != NULL) {
                        CPUTime_Start(timer);
                }
                (*map)(a2, apply_flip_v, (void*)cls);
                write_timer_output(timer, time_file, "flip vertical", a2,
                        methods
                );
        }
        if (trans) {
                if (timer != NULL) {
                        CPUTime_Start(timer);
                }
                (*map)(a2, apply_transpose, (void*)cls);
                write_timer_output(timer, time_file, "transpose", a2, methods);
        }

        printf("P6\n%i %i\n255\n", methods->width(cls->a2_secondary),
                methods->height(cls->a2_secondary)
        );
        for (int i = 0, max = methods->height(cls->a2_secondary); i < max; 
                i++) {
                for (int i2 = 0, max2 = methods->width(cls->a2_secondary); 
                        i2 < max2; i2++) {
                        char val = *(char*)methods->at(cls->a2_secondary, i2, i);
                        printf("%c", val);
                }
                printf("\n");
        }

        if (timer != NULL) {
                CPUTime_Free(&timer);
                fclose(time_file);
        }
        methods->free(&a2);
        methods->free(&cls->a2_secondary);
        free(cls);
}

void populate_a2(FILE* file, A2Methods_T methods, A2Methods_UArray2* a2,
        A2Methods_UArray2* a2_secondary, bool rot) {
        Pnmrdr_T pnm = Pnmrdr_new(file);
        Pnmrdr_mapdata pnm_data = Pnmrdr_data(pnm);

        assert(pnm_data.width > 0 && pnm_data.height > 0);

        *a2 = methods->new(pnm_data.width, pnm_data.height,
                sizeof(int));
        if (rot) {
                *a2_secondary = methods->new(pnm_data.height, pnm_data.width,
                        sizeof(int));
        }
        else {
                *a2_secondary = methods->new(pnm_data.width, pnm_data.height,
                        sizeof(int));
        }

        for (int y = 0; y < (int)pnm_data.height; y++) {
                for (int x = 0; x < (int)pnm_data.width; x++) {
                        int val = (int)Pnmrdr_get(pnm);
                        int* val_a2 = methods->at(*a2, x, y);
                        *val_a2 = val;
                }
        }
}

void apply_0(int x, int y, A2Methods_UArray2 uarray2,
        void* elem, void* cl) {
        (void)uarray2;
        ClosureStruct cls = (ClosureStruct)cl;
        int val = *(int*)elem;
        int* val_a2 = cls->methods->at(cls->a2_secondary, x, y);
        *val_a2 = val;
}

void apply_90(int x, int y, A2Methods_UArray2 uarray2,
        void* elem, void* cl) {
        ClosureStruct cls = (ClosureStruct)cl;
        int val = *(int*)elem;
        int height = cls->methods->height(uarray2);

        int* val_a2 = cls->methods->at(cls->a2_secondary, (height - y - 1), x);
        *val_a2 = val;
}

void apply_180(int x, int y, A2Methods_UArray2 uarray2,
        void* elem, void* cl) {
        ClosureStruct cls = (ClosureStruct)cl;
        int val = *(int*)elem;
        int width = cls->methods->width(uarray2);
        int height = cls->methods->height(uarray2);

        int* val_a2 = cls->methods->at(cls->a2_secondary, (width - x - 1), 
                (height - y - 1)
        );
        *val_a2 = val;
}

void apply_270(int x, int y, A2Methods_UArray2 uarray2,
        void* elem, void* cl) {
        ClosureStruct cls = (ClosureStruct)cl;
        int val = *(int*)elem;
        int width = cls->methods->width(uarray2);

        int* val_a2 = cls->methods->at(cls->a2_secondary, y, (width - x - 1));
        *val_a2 = val;
}

void apply_flip_h(int x, int y, A2Methods_UArray2 uarray2,
        void* elem, void* cl) {
        ClosureStruct cls = (ClosureStruct)cl;
        int val = *(int*)elem;
        int width = cls->methods->width(uarray2);

        int* val_a2 = cls->methods->at(cls->a2_secondary, (width - x - 1), y);
        *val_a2 = val;
}

void apply_flip_v(int x, int y, A2Methods_UArray2 uarray2,
        void* elem, void* cl) {
        ClosureStruct cls = (ClosureStruct)cl;
        int val = *(int*)elem;
        int height = cls->methods->height(uarray2);

        int* val_a2 = cls->methods->at(cls->a2_secondary, x, (height - y - 1));
        *val_a2 = val;
}

void apply_transpose(int x, int y, A2Methods_UArray2 uarray2,
        void* elem, void* cl) {
        ClosureStruct cls = (ClosureStruct)cl;
        int val = *(int*)elem;
        int width = cls->methods->width(uarray2);
        int height = cls->methods->height(uarray2);

        int* val_a2 = cls->methods->at(cls->a2_secondary, 
                (width - (height - y - 1) - 1), (width - x - 1));
        *val_a2 = val;
}

void write_timer_output(CPUTime_T timer, FILE* time_file, char* transform,
        A2Methods_UArray2 uarray2, A2Methods_T methods) {
        if (timer == NULL) {
                return;
        }
        double val = CPUTime_Stop(timer);
        double num_elements = methods->width(uarray2) *
                methods->height(uarray2);
        fprintf(time_file, "Total nanoseconds spent on operation %s: %lf. "
                "Nanoseconds spent per element: %lf\n", transform, val, 
                val / num_elements
        );
}