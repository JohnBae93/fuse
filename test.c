//
//  Created by John on 2017-06-03.
//
// This code is made to debug jfs`s
//  jnode operting function.
//


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

typedef struct _jnode {
    char *fname;
    struct _jnode *next;
    struct _jnode *child;
    struct _jnode *parent;
} JNODE;

JNODE *root;

/*
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
    new_node->parent = parent_node;
}

JNODE *search_jnode(const char *path) {
    JNODE *tmp_jnode = root;
    const char *tmp_path = path;
    const char *tmp_fname = path;
    unsigned int len = 1;
    while(tmp_jnode) {

        while(*tmp_path != '/' && *tmp_path != '\0'){
            len++;
            tmp_path++;
        }

        while(tmp_jnode) {
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

JNODE *make_jnode(char *fname) {
    JNODE* new_jnode = (JNODE*)malloc(sizeof(JNODE));
    new_jnode->fname = fname;
    new_jnode->next = new_jnode->child = new_jnode->parent = NULL;
}
*/

char *get_parent_path(char *path) {
    int len = strlen(path);
    char *name = &path[len - 1];

    int len_leaf_fname = 0;

    while (*name != '/') {
        name--;
        len_leaf_fname++;
    }
    int len_parent_path = len - len_leaf_fname;
    char *parent_path = (char *) malloc(sizeof(char) * len_parent_path);
    strncpy(parent_path, path, len_parent_path);
    parent_path[len_parent_path - 1] = '\0';
    return parent_path;


}

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

JNODE *make_jnode(const char *fname) {
    JNODE *new_jnode = (JNODE *) malloc(sizeof(JNODE));
    assert(new_jnode);
    int len_fname = strlen(fname);

    new_jnode->fname = (char*)malloc(sizeof(char) * len_fname + 1);

    strcpy(new_jnode->fname, fname);

    (new_jnode->fname)[len_fname] = '\0';

    return new_jnode;
}

//given file`s path, find corresponding jnode.
JNODE *search_jnode(const char *path) {
    JNODE *tmp_jnode = root;
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

int delete_jnode(JNODE *node) {
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
int main() {
    char root_fname = '/';
    root = (JNODE*)malloc(sizeof(JNODE));
    root->fname = &root_fname;
    root->child = NULL;


    //   / - abc - lol
    //              |
    //             xyz - mom
    //                    |
    //                   nan

    JNODE *a = make_jnode("abc");
    insert_jnode(root, a);

    JNODE *x = make_jnode("xyz");
    insert_jnode(a, x);

    JNODE *n = make_jnode("nan");
    insert_jnode(x,n);

    JNODE *m = make_jnode("mom");
    insert_jnode(x, m);

    JNODE *l = make_jnode("lol");
    insert_jnode(a,l);


    JNODE *find = search_jnode("/abc/xyz/nan");
    delete_jnode(n); // del nan
    //find = search_jnode("/abc/xyz/nan");

    if(find)
        printf("find jname : %s", find->fname);
    else
        printf("0");

    printf("\n");
    printf("%s", get_parent_path("/abc/xyz/nan"));
}

