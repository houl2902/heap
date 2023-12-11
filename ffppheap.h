/* © 2015 Gordon Cichon */

#ifndef FFPP_HEAP_H
#define FFPP_HEAP_H

#include <stdio.h>
#include <time.h>

typedef struct FFPPEntry_t
{
    time_t birthday; /* birthday of person */
    char name[1]; /* name of person, directly following */
} FFPPEntry;

typedef struct FFPPHeap_t
{
    int n; /* number of elements in heap */
    FFPPEntry *entry[1]; /* pointers to entries, directly following */
} FFPPHeap;

extern FFPPHeap* FFPPHeap_new ();
extern FFPPEntry ** FFPPHeap_enter (FFPPHeap**, time_t, const char *);
extern FFPPEntry ** FFPPHeap_top (FFPPHeap*);
extern void FFPPHeap_remove (FFPPHeap**, FFPPEntry**);
extern void FFPPHeap_delete (FFPPHeap*);

extern void FFPPHeap_printEntry(FILE*, FFPPEntry*);
extern void FFPPHeap_print(FILE*, FFPPHeap*);

#endif /* FFPP_HEAP_H */
