#define _GNU_SOURCE
#include <stdio.h>
#include <dirent.h> 
#include <errno.h> 
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


//this is for inodes
typedef struct inodeEntry {
    uint32_t inode_number;
    char file_type;
} inodeEntry;

typedef struct{
    inodeEntry *ptr;
    int size;
}nodelist;


//this is for dir files
typedef struct{
    uint32_t inode_number;
    char name[33];
}dirfile;

typedef struct{
    char name[33];
    dirfile *data;
    int size;
    int cap;
}dir;



int verifydir(char *argv[]);
FILE *loadInode(DIR *dir);
nodelist *emulatebin(char *filename);
void *checked_malloc(int len);
char *uint32_to_str(uint32_t i);
dir *analyzedir(uint32_t inode , char *name);
void run(nodelist *myinodes , dir *currentdir);
void dols(dir *curdir);
void movedir(dir *curdir , nodelist *myinodes , char *line);
void freeptrs(nodelist *myinodes , dir *currentdir , char *line);
int nodup(dir *curdir , char *name);
void newinode(nodelist *myinodes , char filetype);
void makedir(dir *curdir , nodelist *myinodes , char *line);
void writetoinodelist(int inode_number , char type);
void addtodirectory(dir *curdir , char *filename , uint32_t inode);
void writetodir(uint32_t desdirInode , char *filename , uint32_t inode);
void touchdir(dir *curdir , nodelist *myinodes , char *line);
void realloccheck(dir *newdir);