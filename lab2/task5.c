#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {

    if (argc != 3)
    {
        fprintf(stderr,"!Error!\n");
        fprintf(stderr,"First argument - the name of the file to read\n");
        fprintf(stderr,"Second - the name of the file to write\n");
        return 0;
    }

    int fSrc = open(argv[1], O_RDONLY);
    if (fSrc < 0)
    {
        fprintf(stderr,"Could not open the file\n");
        return 0;
    }
    
    struct stat srcstat,deststat;
    fstat(fSrc, &srcstat);
    
    int fDest = open(argv[2], O_WRONLY | O_CREAT, srcstat.st_mode);
    
    if (chmod(argv[2],srcstat.st_mode)==-1) fprintf(stderr,"Could not change mode\n");;
    	
    if (fDest < 0)
    {
        fprintf(stderr,"Could not open the file\n");
        return 0;
    }

    FILE *fileSrc = fdopen(fSrc, "r");
    FILE *fileDest = fdopen(fDest, "w");

    int c;
    while ((c = getc(fileSrc)) != EOF)
        putc(c, fileDest);

    if ((fclose(fileSrc) != 0) || (fclose(fileDest) != 0))
        fprintf(stderr,"Could not close the file\n");

    return 0;
}
