#include <sys/wait.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int MaxChildNumber;
int ClildNumber;

// main function, fings same files
int scanDirectory(char* dir);

// returns dirname+dir->d_name
char* getFullPath(char* dirname, struct dirent* dir);

// compares files with names fn1 and fn2
int scanFile(const char* fileName, int* byteCount, int* wordCount);

int main(int argc, char* argv[])
{
    // errors check
    if (argc != 3) {
        perror("ERROR! Wrong number of parameters:\n1 - dirName \n2 - MaxChildNumber");
        return 1;
    }

    MaxChildNumber = atoi(argv[2]);

    if (MaxChildNumber < 1) {
        perror("MaxChildNumber should be greater than 1.");
        return 1;
    }
    char* dir;

    dir = argv[1];
    ClildNumber = 0;
    printf("---------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("| pid |                                                         full name                                                               |   bc  |   wc  | \n");
    if (scanDirectory(dir) != 0) {
        printf("---------------------------------------------------------------------------------------------------------------------------------------------------------\n");

        fprintf(stderr, "|%5d| Can't open directory \"%-100s\"                     | \n", getpid(), dir);
    }
	
    while (wait(NULL) != -1) {
    }
    printf("---------------------------------------------------------------------------------------------------------------------------------------------------------\n");

    return 0;
}

int scanDirectory(char* dir)
{
    struct dirent* curFile;
    char* fullName;
    int byteCount, wordCount;

    DIR* dirStream = opendir(dir);
    if (dirStream == NULL) {
        fprintf(stderr, "error! could not open the dir %s\n", dir);
        return 1;
    }
    int scanRes;
    // reading all files in directory
    while ((curFile = readdir(dirStream)) != NULL) {
        //if the file is not dir
        if (curFile->d_type != DT_DIR) {
            if (MaxChildNumber <= ClildNumber) // not enough place to create new process
            {
                // wait for somebody
                wait(NULL);
                // dec ctr
                ClildNumber--;
            }
            ClildNumber++;
            //printf("-%d",ClildNumber);            
            
            // create new process
            switch(fork()){
            case (pid_t)0:{
                fullName= getFullPath(dir, curFile);
                
                if (scanFile(fullName, &byteCount, &wordCount) == 0) {
                    printf("---------------------------------------------------------------------------------------------------------------------------------------------------------\n");
                    printf("|%5d|%-129s|%7d|%7d|\n", getpid(), realpath(fullName, NULL), byteCount, wordCount);
                }
                else {
                    printf("---------------------------------------------------------------------------------------------------------------------------------------------------------\n");
                    printf("|%5d|%-129s|  can't open   |\n", getpid(), realpath(fullName, NULL));
                }
                exit(0); 
                }       
            case (pid_t)-1: {
                // ERROR cant create fork
                perror("ERROR cant create fork");
                return 1;                               
            }          
            }
        }
        // if the file is directory
        else if ((curFile->d_type == DT_DIR) && (strcmp(curFile->d_name, ".") != 0) && (strcmp(curFile->d_name, "..") != 0)) {
            // getting full path to current directory
            char* fullName = getFullPath(dir, curFile);
            // recursion descent to current directory
            scanDirectory(fullName);
        }
    }
    if (closedir(dirStream) == -1) {
        fprintf(stderr, "error! could not close the dir %s\n", dir);
        return 1;
    }
    return 0;
}

char* getFullPath(char* dirname, struct dirent* dir)
{
    char* filepath = calloc(strlen(dirname) + strlen(dir->d_name) + 2, 1);
    strcpy(filepath, dirname);
    strcat(filepath, "/");
    strcat(filepath, dir->d_name);

    return filepath;
}

int scanFile(const char* fileName, int* byteCount, int* wordCount)
{

    (*byteCount) = 0;
    (*wordCount) = 0;
    int fileopen = open(fileName, O_RDONLY);
    if (fileopen < 0) {
        return 1;
    }

    FILE* file = fdopen(fileopen, "r");

    int ch;

    char isWord = 0;
    while (1) {
        ch = fgetc(file);
        (*byteCount)++;
        // if the pointer is in word
        if ((ch != ' ') && (ch != ';') && (ch != '.') && (ch != '-') && (ch != '!') && (ch != '?') && (ch != '\n') && (ch != '\r')) {
            // if the pointer is in the beginning of word
            if (isWord == 0) {
                isWord = 1;
                (*wordCount)++;
            }
        }
        // if tehe pointer is in the end of word
        else if (isWord == 1) {
            isWord = 0;
        }
        if (ch == EOF) {
            break;
        }
    }
    (*wordCount)--;
    (*byteCount) -= 2;
    if ((*byteCount) == -1)
        (*byteCount) = 0;
    fclose(file);

    return 0;
}

