/* © 2015 Gordon Cichon */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csv.h"

static struct CsvSettings 
def_settings = {
    ',', '\"', '\n'
};

struct CsvSettings* 
csv_def_settings = &def_settings;

int
csv_num_columns(CsvRow row)
{
    int num = 0;
    if(row)
	while(row[num])
	    num++;
    return(num);
}

int 
csv_write1(FILE* f, struct CsvSettings* settings, CsvRow row)
{
    int i, j;

    if(!row) return(0);
    for(i=0; row[i]; i++) {
	if(fputc(settings->text_delim, f)==EOF) return(0);
	for(j=0; row[i][j] != '\0'; j++) {
	    if(fputc(row[i][j], f)==EOF) return(0);
	    if(row[i][j] == settings->text_delim)
		if(fputc(settings->text_delim, f)==EOF) return(0);
	}
	if(fputc(settings->text_delim, f)==EOF) return(0);
	if(row[i+1]) {
	    if(fputc(settings->field_delim, f)==EOF) return(0);
	} else {
	    if(fputc(settings->line_delim, f)==EOF) return(0);
	}
    }
    return(1);
}

int 
csv_write(FILE * f, struct CsvSettings* settings, 
	  int count, CsvSheet sheet)
{
    int i=0;
    if(sheet)
        for(i=0; i<count&&sheet[i]; i++)
            if(csv_write1(f, settings, sheet[i])==0)
                break;
    return(i);
}

void
csv_append_column(CsvRow *pRow)
{
    int num_columns = csv_num_columns(*pRow);
    *pRow = (char**) realloc(*pRow, (num_columns+2)*sizeof(char*));
    (*pRow)[num_columns] = (char*) malloc(1);
    (*pRow)[num_columns][0] = '\0';
    (*pRow)[num_columns+1] = NULL;
}

void
csv_append_char(CsvRow *pRow, char c)
{
    int num_columns = csv_num_columns(*pRow);
    int column_len = strlen((*pRow)[num_columns-1]);
    (*pRow)[num_columns-1] = (char*) realloc
	((*pRow)[num_columns-1], column_len+2);
    (*pRow)[num_columns-1][column_len] = c;
    (*pRow)[num_columns-1][column_len+1] = '\0';
}

CsvRow
csv_read1(FILE* f, struct CsvSettings* settings)
{
    int c;
    CsvRow result = (char**) malloc(sizeof(char*));
    result[0] = NULL;
    
    while(1) {
	c=fgetc(f);
 	if(c==settings->line_delim||c==EOF) break;

	csv_append_column(&result);
	while(c!=settings->field_delim) {
	    if(c!=settings->text_delim) {
		csv_append_char(&result, c);
                c=fgetc(f);
            } else {
		while(1) {
		    while((c=fgetc(f))!=settings->text_delim)
			csv_append_char(&result, c);
		    c=fgetc(f);
		    if(c==settings->text_delim)
			csv_append_char(&result, c);
		    else
			break;
		}
	    }
	    if(c==settings->line_delim||c==EOF) break;
	}
	if(c==settings->line_delim||c==EOF) break;
    }

    return result;
}

int 
csv_read(FILE* f, struct CsvSettings* settings, 
	 int count, CsvSheet *pSheet)
{
    int i,n;
    n=0; if(*pSheet) while((*pSheet)[n++]) n++;
    for(i=0; i < count; i++) {
        *pSheet = (CsvRow*) realloc(*pSheet, (n+1)*sizeof(CsvRow));
	(*pSheet)[n++] = csv_read1(f, settings);
	if(feof(f)) break;
    }
    return(i);
}

void
csv_free1(CsvRow row)
{
    int i;
    if(row) {
	for(i=0; row[i]; i++)
	    free(row[i]);
	free(row);
    }
}

void
csv_free(CsvSheet *pSheet)
{
    int i;
    if(*pSheet) {
        for(i=0; (*pSheet)[i]; i++)
            csv_free1((*pSheet)[i]);
        free(*pSheet);
        *pSheet = NULL;
    }
}
