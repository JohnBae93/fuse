#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

typedef struct jnode {
    struct stat st;
    char *name;
    struct jnode *next;
    struct jnode *child;
}JNODE;

int main(){
    JNODE jnode;
    jnode.st.st_ino = 6;
    printf("inode number : %d", jnode.st.st_ino);   
}
