#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>

int main(int argc, char *argv[]) {
    if (argc != 3)
    {
        fprintf(stderr,"First argument - the name of the file to read\n");
        fprintf(stderr,"Second - number of lines to print, 0 - solid text\n");
        return 0;
    }
    char *endptr = NULL;
    int N = strtol (argv[2], &endptr, 10 );
    if (argv[2] == endptr){
    	fprintf(stderr, "Line \"%s\" not a number\n", argv[2]);
        return 0;
    }

    int fileDesc = open(argv[1], O_RDONLY);
    if (fileDesc < 1)
    {
        fprintf(stderr,"Could not open the file\n");
        return 0;
    }

    FILE *file = fdopen(fileDesc, "r");

    int line = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF)  {
    	putc(ch,stdout);
        if (ch == '\n') line++;
        if ((N!=0) && (line==N)){
        	getc(stdin);
        	line = 0;
        }
    }

    if (fclose(file) != 0)
        fprintf(stderr,"Could not close the file\n");

    return 0;
}
