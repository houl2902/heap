/* © 2015 Gordon Cichon */

#ifndef H_CSV
#define H_CSV

#include <stdio.h>

struct CsvSettings {
    char field_delim;
    char text_delim;
    char line_delim;
};

extern struct CsvSettings* csv_def_settings;

typedef char** CsvRow;
typedef CsvRow* CsvSheet;

int csv_num_columns(CsvRow row);
void csv_append_column(CsvRow *pRow);
void csv_append_char(CsvRow *pRow, char c);

CsvRow csv_read1(FILE* f, struct CsvSettings* settings);
int csv_read(FILE* f, struct CsvSettings* settings, int count, CsvSheet *pSheet);

int csv_write1(FILE* f, struct CsvSettings* settings, CsvRow row);
int csv_write(FILE* f, struct CsvSettings* settings, int count, CsvSheet sheet);

void csv_free1(CsvRow row);
void csv_free(CsvSheet *pSheet);

#endif /* H_CSV */
