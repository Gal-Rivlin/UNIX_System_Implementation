#include "headers.h"

void run(nodelist *myinodes , dir *currentdir){
    char *acceptable[] = {"exit" , "cd" , "ls" , "mkdir" , "touch" };

    char *line = NULL;
    size_t size = 0;

    while (1){
        printf("%s: " , (currentdir -> name));
        if (getline(&line , &size , stdin) == EOF){
            freeptrs(myinodes , currentdir , line);
            printf("\n");

            return;
        }
        
        int i = 0;
        char buffer[100];
        while(line[i] != '\n' &&line[i] != ' ' && line[i] != '\0' && i < 99){
            buffer[i] = line[i];
            i++;
        }
        buffer[i] = '\0';
        char op[i + 1];
        strcpy(op , buffer);
        if (strcmp(op , acceptable[0]) == 0){
            freeptrs(myinodes , currentdir , line);
            //exit
            return;
        }
        else if(strcmp(op , acceptable[1]) == 0){
            //cd
            movedir(currentdir , myinodes, line);
            
        }
        else if(strcmp(op , acceptable[2]) == 0){ //ls
            dols(currentdir);
        }
        else if(strcmp(op , acceptable[3]) == 0){
            makedir(currentdir , myinodes , line);
            //mkdir
        }
        else if(strcmp(op , acceptable[4]) == 0){
            //touch
            touchdir(currentdir , myinodes , line);
        }
        else{
            printf("not a valid command\n");
        }
        size = 0;
        free(line);
    }
    

}

void freeptrs(nodelist *myinodes , dir *currentdir, char *line){
    free(myinodes -> ptr);
    free(myinodes);
    free(currentdir -> data);
    free(currentdir);
    free(line);
}


void dols(dir *curdir){
    for (int j = 0 ; j < (curdir -> size) ; j++){
        printf("%d   %s\n"   , (curdir ->data)[j].inode_number , (curdir ->data)[j].name);
    }
}

void movedir(dir *curdir , nodelist *myinodes , char *line){
    int k = 3;
    char buffer[100];
    while (line[k] != '\n' && line[k] != '\0'){
        buffer[k - 3] = line[k];
        k++;
    }
    buffer[k - 3] = '\0';
    char filename[k - 2];
    strcpy(filename , buffer);

    uint32_t myid = 5000; 
    int found = 0;

    if (filename[0] == '.' && filename[1] != '.'){
        return;
    }
    for (int j = 0 ; j < (curdir ->size) ; j ++){
        if (strcmp((curdir ->data)[j].name , filename) == 0){
            found = 1;
            myid = (curdir ->data)[j].inode_number;
        }
    }
    
    if (!found || (myinodes ->ptr)[myid].file_type == 'f'){
        printf("not a directory\n");
        return;
    }

    dir *newdir = analyzedir(myid , filename );
    (curdir -> size) = (newdir -> size);
    (curdir -> cap) = (newdir -> cap);
    free(curdir -> data);
    (curdir -> data) = (newdir -> data);
    free(newdir);

    if (filename[0] == '.' && filename[1] == '.'){
        if((curdir -> data)[0].inode_number == (curdir -> data)[1].inode_number){
            strcpy(filename , "root");
        }
        else{
            dir *finddir = analyzedir((curdir -> data)[1].inode_number , filename);
            for (int m = 0 ; m < (finddir -> size) ; m++){
                if ((finddir -> data)[m].inode_number == myid){
                    strcpy(filename , (finddir -> data)[m].name);
                }
            }
            free(finddir -> data);
            free(finddir);
        }
    }
    
    strcpy((curdir -> name) , filename);
}

//helper functions for mkdir and touch

int nodup(dir *curdir , char *name){
    int ret = 0;
    //printf("name of string to be added: %s" , name);
    for(int i = 0 ; i < (curdir ->size) ; i++){
        if (strcmp((curdir ->data)[i].name , name) == 0){
            ret = 1;
        }
    }
    return ret;
}

void newinode(nodelist *myinodes , char filetype){
    inodeEntry addnew;
    addnew.file_type = filetype;
    addnew.inode_number = (myinodes ->size);

    (myinodes ->ptr)[(myinodes ->size)] = addnew;
    (myinodes ->size)++;
}

void writetoinodelist(int inode_number , char type){
    int fd = open("inodes_list" , O_WRONLY | O_APPEND | O_CREAT, 0644);
    char newdata[5];
    int divider = 1000;
    for(int i = 3 ; i > -1 ; i--){
        int addthis = inode_number / divider;

        if(addthis){
            inode_number = inode_number - (addthis * divider);
        }
        divider = divider / 10;
        newdata[i] = (char) addthis;
    }
    newdata[4] = type;
    write(fd, newdata, sizeof(newdata));
    close(fd);
}

void addtodirectory(dir *curdir , char *filename , uint32_t inode){
    realloccheck(curdir);
    dirfile addnew;
    addnew.inode_number = inode;
    strncpy(addnew.name , filename , sizeof(addnew.name));
    addnew.name[32] = '\0';

    (curdir -> data)[curdir ->size] = addnew;
    (curdir -> size)++;
}


void writetodir(uint32_t desdirInode , char *filename , uint32_t inode){
    char *name = uint32_to_str(desdirInode);
    int fd = open(name , O_WRONLY | O_APPEND | O_CREAT, 0644);
    free(name);

    int inode_number = (int) inode;
    char newdata[36];
    int divider = 1000;
    for(int i = 3 ; i > -1 ; i--){
        int addthis = inode_number / divider;

        if(addthis){
            inode_number = inode_number - (addthis * divider);
        }
        divider = divider / 10;
        newdata[i] = (char) addthis;
    }
    int j = 4;
    while(filename[j - 4] != '\0'){
        newdata[j] = filename[j - 4];
        j++;
    }
    while(j < 36){
        newdata[j] = '\0';
        j++;
    }
    write(fd, newdata, sizeof(newdata));
    close(fd);
}


void makedir(dir *curdir , nodelist *myinodes , char *line){
    int k = 6;
    char buffer[100];
    while (line[k] != '\n' && line[k] != '\0'){
        buffer[k - 6] = line[k];
        k++;
    }
    buffer[k - 6] = '\0';
    char filename[33];
    strncpy(filename , buffer , sizeof(filename));
    filename[32] = '\0';

    int check = nodup(curdir , filename);
    if (check){
        printf("file name taken: could not add file\n");
        return;
    }

//add new inodeentry to inode list
    newinode(myinodes , 'd');
//write to inode file and add the new entry
    writetoinodelist((myinodes ->size) - 1 , 'd');
//add new dirfile struct entry to dir
    addtodirectory(curdir , filename , (uint32_t) (myinodes ->size) - 1 );
//add the information about the file to the dir entry
    writetodir((curdir -> data)[0].inode_number , filename , (u_int32_t) (myinodes ->size) - 1);
//create new file that represents the dir file and write into it

    FILE *file;
    char *name = uint32_to_str((uint32_t) (myinodes ->size) - 1);
    file = fopen(name , "w");
    free(name);
    fclose(file);
    writetodir(  (u_int32_t) (myinodes ->size) - 1 , "." , (u_int32_t) (myinodes ->size) - 1);
    writetodir(  (u_int32_t) (myinodes ->size) - 1 , ".." , (curdir -> data)[0].inode_number);
}


void touchdir(dir *curdir , nodelist *myinodes , char *line){
    //add new inodeentry to inode list
    int k = 6;
    char buffer[100];
    while (line[k] != '\n' && line[k] != '\0'){
        buffer[k - 6] = line[k];
        k++;
    }
    buffer[k - 6] = '\0';
    char filename[33];
    strncpy(filename , buffer , sizeof(filename));
    filename[32] = '\0';

    int check = nodup(curdir , filename);
    if (check){
        printf("file name taken: could not add file\n");
        return;
    }
    //write to inode file and add the new entry in the file itself
    newinode(myinodes , 'f');
    //write to inode_list file
    writetoinodelist((myinodes ->size) - 1 , 'f');
    //add new dirfile structs to the dir
    addtodirectory(curdir , filename , (uint32_t) (myinodes ->size) - 1 );
    //updates contents of file itself    
    writetodir((curdir -> data)[0].inode_number , filename , (u_int32_t) (myinodes ->size) - 1);
    //create a new file that represents the writing file and into it the file name
    FILE *file;
    char *name = uint32_to_str((uint32_t) (myinodes ->size) - 1);
    file = fopen(name , "w");
    free(name);

    fwrite(filename , sizeof(char) , strlen(filename) , file );


    fclose(file);

}


