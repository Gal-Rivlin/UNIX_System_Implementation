#include "headers.h"

int main(int argc , char *argv[])
{
    int i = verifydir(argv);
    if (!i){
        return 1;
    }

    if (chdir(argv[1]) != 0) {
        perror("Failed to change directory");
        return 1;
    }

    nodelist *myinodes = emulatebin("inodes_list");
    dir *rootdir = analyzedir(0 , "root");

    run(myinodes , rootdir);

    return 0;

}


int verifydir(char *argv[]){
    DIR *dir; // Pointer to the directory
    dir = opendir(argv[1]); 
    if (dir == NULL) {
        perror("directory does not exist");
        return 0;
    }
    closedir(dir);
    return 1;
}


nodelist *emulatebin(char *filename){
    FILE *file = fopen(filename , "rb"); // Open the file in binary mode
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    nodelist *ret = (nodelist *)malloc(sizeof(nodelist));

    inodeEntry *mynodes = (inodeEntry *)malloc(1024 * sizeof(inodeEntry));

    (ret -> ptr) = mynodes;
    (ret -> size) = 0;

    int ch;
    int count = 1;
    int cur_inode = 0;
    int multiplier = 1;

    inodeEntry cur;
    while ((ch = fgetc(file)) != EOF) {
        if (count % 5 == 0){
            count = 1;
            
            cur.inode_number = (uint32_t) cur_inode;
            cur.file_type = (char) ch;

            (ret -> ptr)[(ret -> size)] = cur;
            (ret -> size)++;

            multiplier = 1;
            cur_inode = 0;

        }
        else{
            cur_inode += multiplier * ch;
            multiplier = multiplier * 10; 
            count++;
        }
    }

    fclose(file);
    return ret;
}

