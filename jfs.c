#defsdfaine FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

typedef struct jnode {
    struct stat st;
    char *fname;
    struct jnode *next;
    struct jnode *child;
} JNODE;

// given directory path, return leaf file name
char *get_leaf_name(char *path) {
    int len = strlen(path);
    char *name = path[len - 1];

    while (*name != '/') {
        name--;
    }
    name++;

    return name;
}

void insert_node(JNODE *parent_node, JNODE *new_node) {
    ////// check ENOTDIR

    JNODE *tmp;
    if (parent_node->child == NULL || *(parent_node->child) == NULL)
        parent_node->child = new_node;
    else {
        tmp = parent_node->child;
        parent_node->child = new_node;
        new_node->next = tmp;
    }

}

static int jfs_getattr(const char *path, struct stat *stbuf) {
    int ret = 0;

    memset(stbuf, 0, sizeof(struct stat));

    JNODE *node = search_node(path);
    ////////   
    if (node) {
        stbuf->st_ino = node->st.st_ino;
        stbuf->st_mode = node->st.st_mode;
        stbuf->st_nlink = node->st.nlink;
        stbuf->st_uid = node->st.st_uid;
        stbuf->st_gid = node->st.st_gid;
        stbuf->st_rdev = node->st.st_rdev;
        stbuf->st_size = node->st.st_size;
        stbuf->st_atime = node->st.st_atime;
        stbuf->st_mtime = node->st.st_mtime;
        stbuf->st_xtime = node->st.st_xtime;
    } else {
        return -ENOENT;
    }

}

static int jfs_readdir() {
}

static int jfs_mkdir(const char *path, mode_t mode) {
    char *fname = get_leaf_name(path);

    // EEXIST 
    // ENOENT 
    char *parent_path = get_parent_path(path);

    JNODE *new_node = make_jnode();/////
    JNODE *parent_node = search_node(parent_path);
    insert_node(parent_node, new_node);//////

    return 0;
}

static int jfs_chmod() {
}

static int jfs_open() {
}

static int jfs_release() {
}

static int jfs_rmdir() {
}

static int jfs_rename() {
}

static int jfs_read() {
}

static int jfs_create() {
}

static int jfs_ultimens() {
}

static int jfs_unlink() {
}

static int jfs_write() {
}

static int jfs_truncate() {
}

static struct fuse_operation jfs_oper = { // flush?
        .getattr = jfs_getattr,
        .readdir = jfs_readdir,
        .mkdir = jfs_mkdir,
        .chmod = jfs_chmod,
        .open = jfs_open,
        .release = jfs_release,
        .rmdir = jfs_rmdir,
        .rename = jfs_rename,
        .read = jfs_read,
        .create = jfs_create,
        .utimens = jfs_utimens,
        .unlink = jfs_unlink,
        .write = jfs_write,
        .truncate = jfs_truncate,
};

int main(int argc, char *argv[]) {

    return fuse_main(argc, argv, &jfs_oper, NULL);

}
