#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
//check
typedef struct jnode {
    struct stat st;
    char *name;
    struct jnode *next;
    struct jnode *child;
}JNODE;

static int jfs_getattr(){
}
static int jfs_readdir(){
}
static int jfs_mkdir(){
}
static int jfs_chmod(){
}
static int jfs_open(){
}
static int jfs_release(){
}
static int jfs_rmdir(){
}
static int jfs_rename(){
}
static int jfs_read(){
}
static int jfs_create(){
}
static int jfs_ultimens(){
}
static int jfs_unlink(){
}
static int jfs_write(){
}
static int truncate(){
}

static struct fuse_operation jfs_oper = {
    .getattr = ;
    .readdir = ;
    .mkdir = ;
    .chmod = ;
    .open = ;
    .release = ;
    .rmdir = ;
    .rename = ;
    .read = ;
    .create = ;
    .utimens = ;
    .unlink = ;
    .write = ;
    .truncate = ;
};
int main(){

            
}
