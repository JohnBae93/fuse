//
// Created by John on 2017-06-04.
//

/*
JNODE* search_jnode_path(char* path, JNODE* root) {
	JNODE *rpath = root;
	char* pathc = (char*)malloc(sizeof(char)*strlen(path));
	strcpy(pathc, path);

	char* p = strtok(pathc + 1 , "/");
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
*/

/*
JNODE *search_jnode(char *name, JNODE *dir) {
	JNODE *ret = NULL;
	JNODE *curr= dir->child;

	if (curr == NULL) { //not exist
		return ret;

	}
	if (S_ISREG(dir->st.st_mode)) { //reg file
		return ret;
	}

	do {
		if (strcmp(curr->fname, name) == 0) {
			ret = curr;
			return ret;
		}
		else {
			curr = curr->next;
		}
	} while (curr != dir->child && curr != NULL);

	return ret;

}
 */
