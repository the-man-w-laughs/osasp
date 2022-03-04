#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

struct FILERES {
    struct dirent *info;
    char *fileDir;
};

struct List {
    struct FILERES *file;
    struct List *next;
};

void findFile(char *dirSource, char *dirSearch, FILE *pFile);
char* getFilepath (char* dirname, struct dirent *dir);
struct List *findEqualFile(char *fileDir, struct dirent *mainFile, char *dirToFind);
int CompareFiles(const char *fn1, const char *fn2);

int main(int argc, char *argv[]) {

// arguments err
    if (argc != 4)
    {
        fprintf(stderr,"!Error!\n");
        fprintf(stderr,"First argument - dir 1\n");
        fprintf(stderr,"Second - dir 2\n");
        fprintf(stderr,"Third - file to write result\n");
    }


// destfile
    int fDest = open(argv[3], O_WRONLY | O_CREAT, 0644);
    if (fDest < 0)
    {
        fprintf(stderr,"Could not open the file\n");
        return 0;
    }

    FILE *file= fdopen(fDest, "w");

// dirnames
    char *dirName1, *dirName2;

    dirName1 = argv[1];
    dirName2 = argv[2];
    
// main function
    findFile(dirName1, dirName2, file);


//close err
    if ((fclose(file) != 0))
        fprintf(stderr,"Could not close the file\n");

    return 0;
}

// dirSource, dirSearch - directories to find, pFile - dstfile
void findFile(char *dirSource, char *dirSearch, FILE *pFile) {

    //Info about files
    struct stat *fileStat1 = (struct stat *)calloc(1, sizeof(struct stat));
    struct stat *fileStat2 = (struct stat *)calloc(1, sizeof(struct stat));

    DIR *dirStream;
    if ((dirStream = opendir(dirSource)) == NULL)
    {
        fprintf(stderr,"Can not open the dir\n");
        return;
    }

    //file handler
    struct dirent *curDir;
    
    //read each file from dir
    while ((curDir = readdir(dirStream)) != NULL) {
        //if is file -> find the same in dirSearch, else open new dir
        if  (curDir->d_type != DT_DIR)
        {
            struct List *equalFiles = findEqualFile(dirSource, curDir, dirSearch);

            //get full info about two files
            if (equalFiles != NULL) {
                char *dirNewPath = getFilepath(dirSource, curDir);

                struct List *curEl = equalFiles;
                while (curEl != NULL)
                {
                    stat(dirNewPath, fileStat1);
                    stat(curEl->file->fileDir, fileStat2);

                    printf("\nFiles\n");
                    printf("Path: %s - %s\n", dirNewPath, curEl->file->fileDir);
                    printf("Size: %lldb - %lldb\n", fileStat1->st_size, fileStat2->st_size);
                    printf("Create time: %s - %s", ctime(&fileStat1->st_ctim), ctime(&fileStat2->st_ctim));
                    printf("Mode: %lo - %lo\n",  (unsigned long)fileStat1->st_mode, (unsigned long)fileStat2->st_mode);
                    printf("Ind desc: %ld - %ld\n", fileStat1->st_ino, fileStat2->st_ino);

                    fprintf(pFile, "\nFiles\n");
                    fprintf(pFile, "Path: %s- %s\n", dirNewPath, curEl->file->fileDir);
                    fprintf(pFile, "Size: %lldb - %lldb\n", fileStat1->st_size, fileStat2->st_size);
                    fprintf(pFile, "Create time: %s - %s", ctime(&fileStat1->st_ctim), ctime(&fileStat2->st_ctim));
                    fprintf(pFile, "Mode: %lo - %lo\n",  (unsigned long)fileStat1->st_mode, (unsigned long)fileStat2->st_mode);
                    fprintf(pFile, "Ind desc: %ld - %ld\n", fileStat1->st_ino, fileStat2->st_ino);

                    curEl = curEl->next;
                }
            }
        }
        else if ((curDir->d_type == DT_DIR) && (strcmp(curDir->d_name, ".") != 0) && (strcmp(curDir->d_name, "..") != 0))
        {
            //Get path of next folder
            char *dirNewPath = getFilepath(dirSource, curDir);
            findFile(dirNewPath, dirSearch, pFile);
        }

    }

    free(fileStat1);
    free(fileStat2);
    if(closedir(dirStream) == -1)
        fprintf(stderr,"Can not close the dir\n");
}

// adding elemant to list
void Add(struct List **head, struct FILERES *data) {
    struct List *tmp = (struct List*) malloc(sizeof(struct List));
    tmp->file = data;
    tmp->next = (*head);
    (*head) = tmp;
}


struct List *findEqualFile(char *fileDir, struct dirent *mainFile, char *dirToFind) {
    DIR *dirStream;
    if ((dirStream = opendir(dirToFind)) == NULL)
    {
        fprintf(stderr,"Can not open the dir\n");
        return NULL;
    }

    struct List *array = NULL;

    //file handler
    struct dirent *curDir;
    
    //read each file from dir
    while ((curDir = readdir(dirStream)) != NULL) {
        //if is file -> cmp two files, else open new dir
        if  (curDir->d_type != DT_DIR)
        {
            char *file2NewPath = getFilepath(dirToFind, curDir);
            char *file1Path = getFilepath(fileDir, mainFile);
            int cmpRes = CompareFiles(file1Path, file2NewPath);
            //Files equals
            if (cmpRes == 1)
            {
                struct FILERES *answer = malloc(sizeof(struct FILERES));
                answer->info = curDir;
                answer->fileDir = file2NewPath;

                Add(&array, answer);
            }

        }
        else if ((curDir->d_type == DT_DIR) && (strcmp(curDir->d_name, ".") != 0) && (strcmp(curDir->d_name, "..") != 0))
        {
            //Get path of next folder
            char *dirNewPath = getFilepath(dirToFind, curDir);
            struct List *findRes = findEqualFile(fileDir, mainFile, dirNewPath);
            if (findRes != NULL)
            {
                struct List *curEl = findRes;
                while (curEl != NULL)
                {
                    Add(&array, curEl->file);
                    curEl = curEl->next;
                }
            }

        }

    }

    if(closedir(dirStream) == -1)
        fprintf(stderr,"Can not close the dir\n");

    return array;
}

//get path of new folder
char* getFilepath (char* dirname, struct dirent *dir)
{
    char *filepath = calloc(strlen(dirname) + strlen(dir->d_name)+2, 1);
    strcpy(filepath, dirname);
    strcat(filepath,"/");
    strcat(filepath,dir->d_name);
       
    return filepath;
}

int CompareFiles(const char *fn1, const char *fn2) {
    int fFirstFile = open(fn1, O_RDONLY);
    if (fFirstFile < 0)
    {
        fprintf(stderr,"Could not open the file\n");
        return NULL;
    }

    int fSecFile = open(fn2, O_RDONLY);
    if (fSecFile < 0)
    {
        fprintf(stderr,"Could not open the file\n");
        return NULL;
    }

    FILE *fp1 = fdopen(fFirstFile, "r");
    FILE *fp2 = fdopen(fSecFile, "r");

    int   ch1, ch2, c, r;

    c = r = 0;
    while(1) {
        ch1 = fgetc(fp1);
        ch2 = fgetc(fp2);
        if(ch1 != ch2)
            break;
        else if((ch1 == '\n') && (ch2 == '\n')) {
            ++r;
            c = 0;
        } else if((ch1 == EOF) && (ch2 == EOF))
            return 1;

        ++c;
    }
    fclose(fp1);
    fclose(fp2);

    return 0;
}
