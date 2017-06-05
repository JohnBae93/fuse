#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

typedef struct _jnode {
    struct stat st;
    char *fname;
    struct _jnode *next;
    struct _jnode *child;
    struct _jnode *parent;
} JNODE;

typedef struct _data {
    int d_ino;
    char *data;
    struct _data *pre;
    struct _data *next;
} DATA;

typedef struct data_pointer {
    DATA *head;
    DATA *tail;
} DATA_P;

DATA_P dptr;
short inode_num;
JNODE *root;

/*
 *  jnode function
 */
// given directory path, return leaf file name
const char *get_leaf_fname(const char *path) {
    int len = strlen(path);

    if(len == 1 && path[0] == '/' )
        return path;

    const char *name = &path[len - 1];

    while (*name != '/') {
        name--;
    }
    name++;

    return name;
}

//get parent directory path
char *get_parent_path(const char *path) {
    int len = strlen(path);
    const char *name = &path[len - 1];

    int len_leaf_fname = 0;

    while (*name != '/') {
        name--;
        len_leaf_fname++;
    }
    int len_parent_path = len - len_leaf_fname;
    if(len_parent_path == 1)
        return "/";
    char *parent_path = (char *) malloc(sizeof(char) * len_parent_path);
    strncpy(parent_path, path, len_parent_path);
    parent_path[len_parent_path - 1] = '\0';
    return parent_path;
}

// given parent node and new node, insert
void insert_jnode(JNODE *parent_node, JNODE *new_node) {
    JNODE *tmp;
    if (parent_node->child == NULL)
        parent_node->child = new_node;
    else {
        tmp = parent_node->child;
        parent_node->child = new_node;
        new_node->next = tmp;
    }
    new_node->parent = parent_node;
}

// given file name and mode, make new node
JNODE *make_jnode(const char *fname, mode_t mode) {
    DATA *make_data(int);
    void insert_data(DATA*);
    JNODE *new_jnode = (JNODE *) malloc(sizeof(JNODE));
    assert(new_jnode);
    int len_fname = strlen(fname);

    new_jnode->fname = (char*)malloc(sizeof(char) * len_fname + 1);
    assert(new_jnode->fname);

    strcpy(new_jnode->fname, fname);

    (new_jnode->fname)[len_fname] = '\0';

    new_jnode->st.st_ino = ++inode_num;
    new_jnode->st.st_mode = mode;
    new_jnode->st.st_uid = fuse_get_context()->uid;
    new_jnode->st.st_gid = fuse_get_context()->gid;
    new_jnode->st.st_atime = time(NULL);
    new_jnode->st.st_mtime = time(NULL);
    new_jnode->st.st_ctime = time(NULL);

    if (S_ISDIR(mode))
        new_jnode->st.st_nlink = 2;
    else
        new_jnode->st.st_nlink = 1;

    new_jnode->child = NULL;
    new_jnode->parent = NULL;
    new_jnode->next = NULL;

    DATA *data = make_data(new_jnode->st.st_ino);
    insert_data(data);
   
    return new_jnode;
}

//given file`s path, find corresponding jnode.
JNODE *search_jnode(const char *path) {
    JNODE *tmp_jnode = root;
    if(strlen(path) == 1 && path[0] == '/')
        return tmp_jnode;

    const char *tmp_path = path;
    const char *tmp_fname = path;
    unsigned int len = 1;
    while (tmp_jnode) {

        while (*tmp_path != '/' && *tmp_path != '\0') {
            len++;
            tmp_path++;
        }

        while (tmp_jnode) {
            if (strncmp(tmp_fname, tmp_jnode->fname, len) == 0) {
                if (*tmp_path == '\0') // find!
                    return tmp_jnode;
                tmp_jnode = tmp_jnode->child;
                tmp_path++;
                tmp_fname = tmp_path;
                break;
            } else {
                tmp_jnode = tmp_jnode->next;
            }
        }
        len = 0;
    }
    return NULL; // not exist!!

}

// delete jnode.
int delete_jnode(JNODE *node) {

    if (node == NULL) {//not exist
        return 0;
    }

    // directory not empty.
    if (S_ISDIR(node->st.st_mode) && node->child != NULL) {
        return -1;
    }

    // delete
    if (node->parent->child == node) { //delete the first child
        if (node->next == NULL) { //only one child
            node->parent->child = NULL;
        } else {
            node->parent->child = node->next;
        }
    } else { //delete not the first child
        JNODE *temp;
        temp = node->parent->child;
        while (temp->next != node) {
            temp = temp->next;
        } //temp->next is node
        temp->next = node->next;
        node->next = NULL;
    }
    free(node);
    return 1;
}


/*
 * data function
 */
void insert_data(DATA *node) {
    if (dptr.head == NULL) {
        dptr.head = node;
        dptr.tail = node;
    } else { //always insert at the end of list
        dptr.tail->next = node;
        node->pre = dptr.tail;
        dptr.tail = node;
    }
}

void del_data(DATA *dnode) {

    if (dptr.head == dnode) { //dnode is first node
        dptr.head = dptr.head->next;
        if (dptr.head != NULL) { //dnode has next node
            dptr.head->pre = NULL;
            dnode->pre = NULL;
            dnode->next = NULL;
        }
    } else {
        DATA *temp = dnode;
        if (dnode->pre != NULL) {
            dnode->pre->next = temp->next;
        }
        if (dnode->next != NULL) {
            dnode->next->pre = temp->pre;
        }
        dnode->pre = NULL;
        dnode->next = NULL;

    }

    free(dnode->data);
    free(dnode);

    return;
}

DATA *search_data(int inode) {

    if (dptr.head == NULL) {
        return NULL;
    }
    DATA *curr = dptr.head;
    while (curr != NULL && curr->d_ino < inode) {
        if (curr->next == NULL) {
            break;
        } else {
            curr = curr->next;
        }
    }
    //search end

    if (curr->d_ino == inode) { //exist
        return curr;
    } else {
        return NULL; //not exist
    }
}

DATA *make_data(int inode) {
    DATA *n = (DATA *) calloc(1, sizeof(DATA));
    n->d_ino = inode;
    n->data = NULL;
    n->pre = NULL;
    n->next = NULL;
    return n;
}


/*
 * fuse function
 * All error checking is done hear
 */
// ls -> access getattr
static int jfs_getattr(const char *path, struct stat *stbuf) {
    
    int ret = 0;
    
    //memset(stbuf, 0, sizeof(struct stat));

    JNODE *jnode = search_jnode(path);
    ////////
    if (jnode) {
        stbuf->st_ino = jnode->st.st_ino;
        stbuf->st_mode = jnode->st.st_mode;
        stbuf->st_nlink = jnode->st.st_nlink;
        stbuf->st_uid = jnode->st.st_uid;
        stbuf->st_gid = jnode->st.st_gid;
        stbuf->st_size = jnode->st.st_size;
        stbuf->st_atime = jnode->st.st_atime;
        stbuf->st_mtime = jnode->st.st_mtime;
        stbuf->st_ctime = jnode->st.st_ctime;
    } else {
        return -ENOENT;
   }
   return 0;
}

static int jfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi) {
    (void) offset;
    (void) fi;

    JNODE* jnode = search_jnode(path);
    JNODE* current;

    if(jnode == NULL)
    {
        return -ENOENT; //no such directory
    }

    if(!S_ISDIR(jnode->st.st_mode))
    {
        return -ENOTDIR;	//not a directory
    }

    filler(buf, ".", NULL, 0);	//current
    filler(buf, "..", NULL, 0);	//parent

    current = jnode->child;
    while(current) {
        filler(buf, current->fname, NULL,0);
        current = current->next;
    }

    return 0;
}

//mkdir -> mkdir
static int jfs_mkdir(const char *path, mode_t mode) {
    //if (!S_ISDIR(mode))
      //  return -ENOTDIR;
    
    if(search_jnode(path))
        return -EEXIST;

    const char *fname = get_leaf_fname(path);
    char *parent_path = get_parent_path(path);

    JNODE *new_jnode = make_jnode(fname, mode | S_IFDIR);
    JNODE *parent_jnode = search_jnode(parent_path);
    
    if(!parent_jnode)
        return -ENOENT;
    if(!S_ISDIR(parent_jnode->st.st_mode))
        return -ENOTDIR;

    insert_jnode(parent_jnode, new_jnode);
    
    parent_jnode->st.st_nlink++;
    
    return 0;
}

//done
static int jfs_chmod(const char* path, mode_t mode) {
    JNODE *jnode = search_jnode(path);
    if(!jnode)
        return -ENOENT;

    jnode->st.st_mode = mode;
    return 0;
}

//cat -> open
static int jfs_open(const char *path, struct fuse_file_info *fi) {
    JNODE *jnode = search_jnode(path);

    if(!jnode)
        return -ENOENT;
    if(S_ISDIR(jnode->st.st_mode))
        return -EISDIR;

    return 0;

}

static int jfs_release(const char* path, struct fuse_file_info* fi) {
    (void)fi;
	(void)path;
	
	return 0;
}

static int jfs_rmdir(const char* path) {
    JNODE *jnode = search_jnode(path);

    if(!jnode)
        return -ENOENT;
    if(jnode->child)
        return -ENOTEMPTY;

    delete_jnode(jnode);
    return 0;
}

// done
static int jfs_rename(const char *old_path, const char *new_path) {
    JNODE *jnode = search_jnode(old_path);
    if(!jnode)
        return -ENOENT;

    const char *new_fname = get_leaf_fname(new_path);
    jnode->fname = (char*)realloc(jnode->fname, sizeof(char) * strlen(new_fname) + 1);
    strcpy(jnode->fname, new_fname);

    return 0;
}

//read data from opened file
static int jfs_read(const char *path, char *buf, size_t size, off_t offset) {
    
	JNODE *jnode = search_jnode(path);
	if (jnode == NULL) { //no jnode
		return -ENOENT;
	}

	DATA *jdata = search_data(jnode->st.st_ino);
	if (jdata == NULL) { //no data node
		return -ENOENT;
	}

	int leng = jnode->st.st_size;
	if (leng > offset) {
		if (offset + size > leng) {
			size = leng - offset;
		}
		memcpy(buf, jdata->data + offset, size); //data to buf
	}
	else {
		size = 0;
	}

	return size; //number of bytes
}

//create and open new file
//touch -> create
static int jfs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    if (search_jnode(path)) { //already exist name
		return -EEXIST;
	}

	char *leaf = get_leaf_fname(path);
	char *parent = get_parent_path(path);

	JNODE *newnode = make_jnode(leaf, mode);
	JNODE *pnode = search_jnode(parent);
	pnode->st.st_nlink++;
	insert_jnode(pnode, newnode);

	return 0;
}


static int jfs_utimens(const char* path, const struct timespec tv[2]) {
    return 0;
}

static int jfs_unlink(const char *path) {
    JNODE *jnode = search_jnode(path);
    if(!jnode)
        return -ENOENT;
    if(jnode->child)
        return -ENOTEMPTY;

    DATA *data = search_data(jnode->st.st_ino);
    del_data(data);
    delete_jnode(jnode);

    return 0;
}

//write data to opened file
// echo -> wirte
static int jfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    (void)fi;

	JNODE *jnode = search_jnode(path);
	if (jnode == NULL) {//no jnode
		return -ENOENT;
	}

	DATA *jdata = search_data(jnode->st.st_ino);
	if (jdata == NULL) { //no data
		return -ENOENT;
	}

	jdata->data = (char*)realloc(jdata->data, size);
	strcpy(jdata->data, buf);
	jnode->st.st_size = size;
	return size;
}

static int jfs_truncate(const char* path, off_t length) {
    return 0;
}

static struct fuse_operations jfs_oper = {
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
    root = make_jnode("/", S_IFDIR);
    return fuse_main(argc, argv, &jfs_oper, NULL);
}
