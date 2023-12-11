/* © 2015 Gordon Cichon */

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <langinfo.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "csv.h"
#include "ffppheap.h"

int verbose = 0;
char * input_filename = "birthdays.csv";
char * output_filename = "sorted-res.csv";

#if !HAVE_STRFTIME_NULL
# define MAX_DATE 100
#endif /* !HAVE_STRFTIME_NULL */

int
main(int argc, char *argv[])
{
    int c, i;
    FILE *f, *g;
    CsvSheet csv_in = NULL;
    char *csv_out[3] = { NULL, NULL, NULL };
    struct tm tm;
    char * s;
    char * date_fmt;
    FFPPHeap* my_heap;
    FFPPEntry ** my_entry;
#if !HAVE_STRFTIME_NULL
    char buffer[MAX_DATE];
#endif /* !HAVE_STRFTIME_NULL */

    date_fmt = "%d.%m.%y"; /* nl_langinfo (D_FMT); */
    
    opterr = 0;
    while ((c = getopt (argc, argv, "i:o:v")) != -1)
        switch (c) {
        case 'i':
            input_filename = optarg;
            break;
        case 'o':
            output_filename = optarg;
            break;
        case 'v':
            verbose = -1;
            break;
        case '?':
            if (optopt == 'i' || optopt == 'o')
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf (stderr,
                         "Unknown option character `\\x%x'.\n",
                         optopt);
            return 1;
        default:
            abort ();
        }

    f = fopen(input_filename, "r");
    if (f==NULL) {
        fprintf(stderr, "cannot open file %s: %s\n", 
                input_filename, strerror(errno));
        exit(1);
    }

    g = fopen(output_filename, "w");
    if (g==NULL) {
        fprintf(stderr, "cannot open file %s: %s\n", 
                output_filename, strerror(errno));
        fclose (f);
        exit(1);
    }
 
    /* read input file */
    c = csv_read (f, csv_def_settings, INT_MAX, &csv_in);
    if (verbose) {
        printf ("read %d record from file %s.\n", c, input_filename);
    }
    assert(c>1);
    fclose (f);

    my_heap =  FFPPHeap_new ();

    /* fill data into heap structure */
    for (i=1; i<c; i++) {
        /* assert that row has at least two fields */
        if (!csv_in[i][0] || !csv_in[i][1]) {
            fprintf(stderr,"invalid record in %s line %d\n",
                    input_filename, i);
        }

        if (verbose) {
            printf("input set %d:\n", i);
            csv_write1(stdout, csv_def_settings, csv_in[i]);
        }

        memset (&tm, '\0', sizeof (tm));
        s=strptime(csv_in[i][1], date_fmt, &tm);
        if (!s) {
            fprintf(stderr,"invalid date in %s line %d\n",
                    input_filename, i);
        }
        
        if (verbose) {
            printf ("converted date: 0x%8.8x\n", (unsigned int) mktime (&tm));
        }

        my_entry = FFPPHeap_enter (&my_heap, mktime (&tm), csv_in[i][0]);

        if (verbose) {
            printf ("new heap entry:\n");
            FFPPHeap_printEntry (stdout, *my_entry);
        }
    }
    
    /* output sorted data */
    /* output headline */
    if (1 != csv_write1(g, csv_def_settings, csv_in[0])) {
        assert(0);
    }
    while ((my_entry = FFPPHeap_top (my_heap))) {
        if (verbose) {
            printf ("current heap:\n");
            FFPPHeap_print (stdout, my_heap);
        }

        csv_out[0]=(*my_entry)->name;

        if (&tm != localtime_r (&(*my_entry)->birthday, &tm)) {
            assert (0);
        }
# if HAVE_STRFTIME_NULL
        c = strftime (NULL, INT_MAX, date_fmt, &tm);
        csv_out[1] = (char*) realloc (csv_out[1], c+1);
        if (c != strftime (csv_out[1], c+1, date_fmt, &tm)) {
            assert (0);
        }
# else /* !HAVE_STRFTIME_NULL */
        strftime (buffer, MAX_DATE, date_fmt, &tm);        
        csv_out[1]=buffer;
# endif /* HAVE_STRFTIME_NULL */

        if (1 != csv_write1(g, csv_def_settings, csv_out)) {
            assert(0);
        }

        FFPPHeap_remove (&my_heap, my_entry);
    }

    FFPPHeap_delete (my_heap);

    fclose (g);

    return 0;
}
