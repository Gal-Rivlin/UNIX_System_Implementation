#include "headers.h"

//this function gets an inode #, assuming that it is a dir, and returns the contents of the directory
dir *analyzedir(uint32_t inode , char *name){
    char *iname  = uint32_to_str(inode);
    FILE *file = fopen(iname , "rb");
    free(iname);
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

//initialize new directory structure
    dir *newdir = (dir *)malloc(sizeof(dir));
    (newdir -> size) = 0;
    (newdir -> data) = (dirfile *)malloc(8 * sizeof(dirfile));
    (newdir -> cap) = 5;

//fill directory structure
    int multiplier = 1;
    int count = 1;
    int ch;
//initialize first value
    (newdir -> data)[newdir -> size].inode_number = 0;

    while ((ch = fgetc(file)) != EOF){
        if (count == 37){
            //printf("inode number: %d\n" , (newdir -> data)[newdir -> size].inode_number);
            realloccheck(newdir);
            (newdir -> data)[newdir -> size].name[32] = '\0';
            count = 1;
            (newdir -> size)++;
            (newdir -> data)[newdir -> size].inode_number = 0;
            multiplier = 1;
        }
        if (count < 5){
            (newdir -> data)[newdir -> size].inode_number += multiplier * ch;
            multiplier = multiplier * 10;
            count++;
        }
        else if (count > 4 && count < 37 ){
            (newdir -> data)[newdir -> size].name[count - 5] = (char) ch;
            count++;
        }
        
    }
//run this one more time to add the last value
    (newdir -> data)[newdir -> size].name[32] = '\0';
    count = 1;
    (newdir -> size)++;
    (newdir -> data)[newdir -> size].inode_number = 0;

    fclose(file);

    strcpy((newdir -> name) , name);

    return newdir;
}

void *checked_malloc(int len)
{
   void *p = malloc(len);
   if (p == NULL)
   {
      printf("Ran out of memory!\n");
      exit(1);
   }
   return p;
}

char *uint32_to_str(uint32_t i)
{
   // pretend to print to a string to get length
   int length = snprintf(NULL, 0, "%lu", (unsigned long)i);

   // allocate space for the actual string
   char* str = (char *)checked_malloc(length + 1);

   // print to string
   snprintf(str, length + 1, "%lu", (unsigned long)i);

   return str;
}


void realloccheck(dir *newdir){
    if ((newdir -> size) == (newdir -> cap)){
        dirfile *newarr = realloc(newdir -> data , (8 + (newdir -> cap)) * sizeof(dirfile));
        if (newarr != NULL){
            newdir -> data = newarr;
            newdir -> cap += 5;
        }
        else{
            perror("Couldn't realloc");
        }      
    }
}