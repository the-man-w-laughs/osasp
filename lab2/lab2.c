#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>

struct FileStuff {
    struct dirent *info;
    char *fullName; 
};

struct List {
    struct FileStuff *fileInfo; 
    struct List *next;
};

struct List *head = NULL;



// main function, fings same files
int findSame(char *dir1, char *dir2, FILE *file);

// returns dirname+dir->d_name
char* getFullPath(char* dirname, struct dirent *dir);

// compares files with names fn1 and fn2
int fileCompare(const char *fn1, const char *fn2);

// used in conjuction with findSame
// finds file, same to fileToFind
struct List *getSameFiles(char *dirName, struct dirent *fileToFind, char *dirToSearch);

// adds data to List
void add(struct List **head, struct FileStuff *data);

int checkList(struct List *head, struct dirent * data);

int main(int argc, char *argv[]) {

// errors check
    if (argc != 4) {
        fprintf(stderr,"error! wrong parameters:\n1- first dirName \n2 - second dirName \n3 - result file\n");
        return 1;
    }

    int fDest = open(argv[3], O_WRONLY | O_CREAT, 0644);
    if (fDest == -1) {
        
        fprintf(stderr,"error! could not open the file\n");
        
        return 1;
    }

    FILE *file= fdopen(fDest, "w");

    char *dir1, *dir2;

    dir1 = argv[1];
    dir2 = argv[2];

    if (findSame(dir1, dir2, file) != 0) {
        fprintf(stderr,"error! something went wrong while searching n");  
    }


    if (fclose(file) != 0) {
        
        fprintf(stderr,"error! could not close the file\n");
    }

    return 0;
}

int findSame(char *dir1, char *dir2, FILE *file){


    struct stat *fileStat1 = (struct stat *)calloc(1, sizeof(struct stat));
    struct stat *fileStat2 = (struct stat *)calloc(1, sizeof(struct stat));

    DIR *dirStream = opendir(dir1);
    if (dirStream == NULL) {
        
        fprintf(stderr,"error! could not open the dir %s\n",dir1);
        return 1;
    }

    
    struct dirent *curFile;

// reading all files in directory   
    while ((curFile = readdir(dirStream)) != NULL) {
        
//if the file is not dir
        if  ((curFile->d_type != DT_DIR) && (checkList(head,curFile))) {
        
// adds to List files the same to currFile     
            struct List *equalFiles = getSameFiles(dir1, curFile, dir2);
 	    
// if at list one file is equeal to curFile           
            if (equalFiles != NULL) {
//initializing iterator            
                char *fullName1 = getFullPath(dir1, curFile);

                struct List *curEl = equalFiles;
                while (curEl != NULL) {

// getting files info                
                    stat(fullName1, fileStat1);
                    stat(curEl->fileInfo->fullName, fileStat2);

                    printf("\npath: %s - %s\n", realpath(fullName1,NULL), realpath(curEl->fileInfo->fullName,NULL));
                    fprintf(file, "\npath: %s- %s\n", fullName1, curEl->fileInfo->fullName);

                    printf("inode number: %ld - %ld\n", fileStat1->st_ino, fileStat2->st_ino);
                    fprintf(file, "inode number: %ld - %ld\n", fileStat1->st_ino, fileStat2->st_ino);

                    printf("file type and mode: %lo - %lo\n",  (unsigned long)fileStat1->st_mode, (unsigned long)fileStat2->st_mode);
                    fprintf(file, "file type and mode: %lo - %lo\n",  (unsigned long)fileStat1->st_mode, (unsigned long)fileStat2->st_mode);
                                      
                    printf("size: %ldb - %ldb\n", fileStat1->st_size, fileStat2->st_size);
                    fprintf(file, "size: %ldb - %ldb\n", fileStat1->st_size, fileStat2->st_size);

                    printf("create time: %s - %s", ctime(&fileStat1->st_ctime), ctime(&fileStat2->st_ctime));
                    fprintf(file, "create time: %s - %s", ctime(&fileStat1->st_ctime), ctime(&fileStat2->st_ctime));                    
                    
                    curEl = curEl->next;
                }
            }
            // if the file is directory
        } else if ((curFile->d_type == DT_DIR) && (strcmp(curFile->d_name, ".") != 0) && (strcmp(curFile->d_name, "..") != 0)) {
            // getting full path to current directory   
            char *fullName1 = getFullPath(dir1, curFile);
            // recursion descent to current directory
            findSame(fullName1, dir2, file);
        }

    }

    free(fileStat1);
    free(fileStat2);
    if(closedir(dirStream) == -1) {
        fprintf(stderr,"error! could not close the dir %s\n",dir1);
        return 1;
    }    
    return 0;


}
void add(struct List **head, struct FileStuff *data) {
    struct List *tmp = (struct List*) malloc(sizeof(struct List));
    tmp->fileInfo = data;
    tmp->next = (*head);
    (*head) = tmp;
}


char* getFullPath(char* dirname, struct dirent *dir) {
    char *filepath = calloc(strlen(dirname) + strlen(dir->d_name)+2, 1);
    strcpy(filepath, dirname);
    strcat(filepath,"/");
    strcat(filepath,dir->d_name);
    
    return filepath;
}

struct List *getSameFiles(char *dirName, struct dirent *fileToFind, char *dirToSearch) {
    DIR *dirStream = opendir(dirToSearch);
    if (dirStream == NULL) {
        
        fprintf(stderr,"error! could not open the dir %s\n",dirToSearch);
        return NULL;
    }

    struct dirent *curFile;
    struct List *resList = NULL;
    

    while ((curFile = readdir(dirStream)) != NULL) {

        if  ((curFile->d_type != DT_DIR) && (curFile->d_ino != fileToFind->d_ino))  {
            char *curFileName = getFullPath(dirToSearch, curFile);
            char *mainFileName = getFullPath(dirName, fileToFind);
            

            if (fileCompare(mainFileName, curFileName)) {
                struct FileStuff *equalFile = malloc(sizeof(struct FileStuff));
                equalFile->info = curFile;
                equalFile->fullName = curFileName;

                add(&resList, equalFile);
                add(&head,equalFile);
            }

        } else if ((curFile->d_type == DT_DIR) && (strcmp(curFile->d_name, ".") != 0) && (strcmp(curFile->d_name, "..") != 0)) {
 
            char *fullName1 = getFullPath(dirToSearch, curFile);
            struct List *findRes = getSameFiles(dirName, fileToFind, fullName1);
            if (findRes != NULL) {
                struct List *curEl = findRes;
                while (curEl != NULL) {
                    add(&resList, curEl->fileInfo);
                    add(&head,curEl->fileInfo);
                    curEl = curEl->next;
                }
            }

        }

    }

    if(closedir(dirStream) == -1) {
        fprintf(stderr,"error! could not close the dir %s\n",dirToSearch);
    }
    return resList;
}



int fileCompare(const char *fn1, const char *fn2) {
    int file1open = open(fn1, O_RDONLY);
    if (file1open < 0) {

        fprintf(stderr,"error! could not open the file %s\n",fn1);
        return 0;
    }

    int file2open = open(fn2, O_RDONLY);
    if (file2open < 0) {

        fprintf(stderr,"error! could not open the file %s\n",fn2);
        return 0;
    }

    FILE *file1 = fdopen(file1open, "r");
    FILE *file2 = fdopen(file2open, "r");

    int ch1, ch2;

    int res;
    while(1) {
        ch1 = fgetc(file1);
        ch2 = fgetc(file2);

        if(ch1 != ch2) {
            res = 0;
            break;
        } else if((ch1 == EOF) && (ch2 == EOF)) {
            res = 1;
            break;
        }

    }
    fclose(file1);
    fclose(file2);

    return res;
}

int checkList(struct List *head, struct dirent * data){
	struct List *curEl = head;
	while (curEl != NULL) {
	if (curEl->fileInfo->info->d_ino == data->d_ino) return 0;
	curEl = curEl->next;
	}
	return 1;
}
