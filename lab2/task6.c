#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

void PrintDir(char *dir)
{
    DIR *dirStream;
    if ((dirStream = opendir(dir)) == NULL)
    {
        fprintf(stderr,"Can not open the dir\n");
        return;
    }
    struct dirent *curDir;
    while ((curDir = readdir(dirStream)) != NULL)
        printf("%s\n", curDir->d_name);

    if(closedir(dirStream) == -1)
        fprintf(stderr,"Can not close the dir\n");
}

int main(int argc, char *argv[]) {
    char curDir[256];
    getcwd(curDir, 256);
    PrintDir(curDir);

    printf("\n\n\n     **********\n");
    PrintDir("/");
    return 0;
}
