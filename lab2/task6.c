#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

void PrintDir(char *dir)
{
    DIR *dirStream = opendir(dir);
    if (dirStream == NULL)
    {
        perror("Can not open the dir: ");
        return;
    }
    struct dirent *curDir;
    while ((curDir = readdir(dirStream)) != NULL)
        printf("%s\n", curDir->d_name);

    if(closedir(dirStream) == -1)
        perror("Can not close the dir: ");
}

int main(int argc, char *argv[]) {
    char curDir[256];
    getcwd(curDir, 256);
    
    printf("\n\n\n  ***************\n");
    printf("  Current directory: \n");
    PrintDir(curDir);  

    
    printf("\n\n\n  ***************\n");
    printf("  Root directory: \n");
    PrintDir("/");
    return 0;
}
