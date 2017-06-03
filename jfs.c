#defsdfaine FUSE_USE_VERSION 26

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
	char* data;
	struct _data* pre;
	struct _data* next;
}DATA;

typedef struct data_pointer{
	DATA* head;
	DATA* tail;
}DATA_P;

DATA_P dptr;
short inode_num;
JNODE *root;

/*
 *  jnode function
 */
// given directory path, return leaf file name
char *get_leaf_fname(char *path) {
    int len = strlen(path);
    char *name = &path[len - 1];

    while (*name != '/') {
        name--;
    }
    name++;

    return name;
}


JNODE* search_jnode_path(char* path, JNODE* root) {
	JNODE *rpath = root;
	char* pathc = (char*)malloc(sizeof(char)*strlen(path));
	strcpy(pathc, path);

	char* p = strtok(pathc + 1 / "/");
	if (p != NULL) { //search first
		rpath = search_jnode(p, rpath); 
		if (rpath == NULL) {
			return rpath; //no result. just null
		}
	}

	while (p != NULL) {
		p = strtok(NULL, "/");
		if (p != NULL) {
			rpath = search_jnode(p, rpath);
		}
		if (rpath == NULL) {
			return NULL;
		}

	}
	return rpath;
}

//get directory path
char* get_dir_path(char* path) {
	int len = strlen(path);
	char* p = &path[len - 1];
	do {
		p--;
	} while (*p != '/');
	do {
		p--;
	} while (*p != '/');
	char *dir_p = p;
	dir_p = strtok(dir_p + 1."/");

	return dir_p;

}




void insert_jnode(JNODE *parent_node, JNODE *new_node) {
    ////// check ENOTDIR

    JNODE *tmp;
    if (parent_node->child == NULL)
        parent_node->child = new_node;
    else {
        tmp = parent_node->child;
        parent_node->child = new_node;
        new_node->next = tmp;
    }
}

JNODE *make_jnode(char *fname, mode_t mode, uid_t uid, gid_t gid) {
	JNODE *new_jnode = (JNODE*)malloc(sizeof(JNODE));
	assert(new_jnode);

	new_jnode->fname = fname;
	new_jnode->st.st_ino = ++inode_num;
	new_jnode->st.st_mode = mode;
	new_jnode->st.uid = st_uid;
	new_jnode->st.gid = st_gid;
	new_jnode->st.st_atime = time(NULL);
	new_jnode->st.st_mtime = time(NULL);
	new_jnode->st.st_atime = time(NULL);

	if(S_ISDIR(mode))
		new_jnode->st.st_nlink = 2;
	else
		new_jnode->st.st_nlink = 1;

	return new_jnode;
}


JNODE *search_jnode(char *name, JNODE *dir) {
	JNODE *ret = NULL;
	JNODE *curr= dir->child;
	
	if (curr == NULL) { //not exist
		return ret;
		
	}
	if (S_ISREG(dir->st.st_mode) { //reg file
		return ret;
	}
	
	do {
		if (strcmp(curr->fname, name) == 0) {
			res = cur;
			return ret;
		}
		else {
			curr = curr->next;
		}
	} while (curr != dir->child && curr != NULL);

	return ret;

}

void delete_jnode(JNODE* node) {

	if (node == NULL) {//not exist 
		return;
	}
	if (S_ISDIR(node->st.st_mode) && node->child != NULL) {
		return;
	}

	if (node->parent->child == node) { //delete the first child
		if (node->next == NULL) { //only one child
			node->parent->child = NULL;
		}
		else {
			node->parent->child = node->next;
		}
	}
	else{ //delete not the first child
		JNODE* temp;
		temp = node->parent->child;
		while (temp->next != node) {
			temp = temp->next;
		} //temp->next is node
		temp->next = node->next;
		node->next = NULL;
	}
	free(node);
}



/*
 * data function
 */
void insert_data(DATA* node) {
	if (dptr.head == NULL) {
		dptr.head = node;
		dptr.tail = node;
	}
	else { //always insert at the end of list
		dptr.tail->next = node;
		node->pre = dptr.tail;
		dptr.tail = node;
	}
}

void del_data(DATA* dnode) {

	if (dptr.head == dnode) { //dnode is first node
		dptr.head = dptr.head->next;
		if (dptr.head != NULL) { //dnode has next node
			dptr.head->pre = NULL;
			dnode->pre = NULL;
			dnode->next = NULL;
		}
	}
	else { 
		DATA* temp = dnode;
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

DATA* search_data(int inode) {
	
	if (dptr.head == NULL) { 
		return NULL;
	}
	DATA* curr = dptr.head;
	while (curr != NULL &&curr->d_ino < inode) {
		if (curr->next == NULL) {
			break;
		}
		else {
			curr = curr->next;
		}
	}
	//search end
	
	if (curr->d_ino == inode) { //exist
		return curr;
	}
	else {
		return NULL; //not exist
	}
}

DATA* make_data(int inode) {
	DATA* n = (DATA*)calloc(1, sizeof(DATA));
	n->d_ino = inode;
	n->data = NULL;
	n->pre = NULL;
	n->next = NULL;
	return n;
}



/*
 * fuse function
 */
static int jfs_getattr(const char *path, struct stat *stbuf) {
    int ret = 0;

    memset(stbuf, 0, sizeof(struct stat));

    JNODE *jnode = search_node(path);
    ////////
    if (jnode) {
        stbuf->st_ino = jnode->st.st_ino;
        stbuf->st_mode = jnode->st.st_mode;
        stbuf->st_nlink = jnode->st.nlink;
        stbuf->st_uid = jnode->st.st_uid;
        stbuf->st_gid = jnode->st.st_gid;
        stbuf->st_size = jnode->st.st_size;
        stbuf->st_atime = jnode->st.st_atime;
        stbuf->st_mtime = jnode->st.st_mtime;
        stbuf->st_ctime = jnode->st.ct_ctime;
    } else {
        return -ENOENT;
    }
}

static int jfs_readdir() {
}

static int jfs_mkdir(const char *path, mode_t mode) {
    if(!S_ISDIR(mode))
		return -ENOTDIR;

	char *fname = get_leaf_fname(path);

    // EEXIST
    // ENOENT

    char *parent_path = get_parent_path(path);

    JNODE *new_jnode = make_jnode(fname, mode, getuid(), getgid());/////
    JNODE *parent_jnode = search_jnode(parent_path);

	insert_jnode(parent_jnode, new_jnode);//////

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

static int jfs_utimens() {
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
