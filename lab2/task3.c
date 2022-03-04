#include <stdio.h>
#include <fcntl.h>

const int ExitSymbol = 6;

int main(int argc, char *argv[]) {

    if (argc != 2)
    {
        fprintf(stderr,"1 argument - Name of file to write\n");
        return 0;
    }

    int fileDesc = open(argv[1], O_WRONLY | O_CREAT, 0644);
    if (fileDesc < 0) {
        fprintf(stderr,"Could not open the file\n");
        return 0;
    }

    FILE *file =fdopen(fileDesc, "w");
    int inpChar;
    while ((inpChar = getchar()) != ExitSymbol){
        if (putc(inpChar, file) == EOF){
            fprintf(stderr,"Error writing\n");
            }
    }

    if (fclose(file) != 0){
        fprintf(stderr,"Could not close the file\n");
        }

    return 0;
}

