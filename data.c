void insert_data(DATA *node) {
	if (dptr.head == NULL) {
		dptr.head = node;
		dptr.tail = node;
	}
	else { //always insert at the end of list
		dptr.tail->next = node;
		node->pre = dptr.tail;
		dptr.tail = node;
	}
	return;
}

void del_data(DATA *dnode) {

	if (dptr.head == dnode) { //dnode is first node
		dptr.head = dptr.head->next;
		if (dptr.head != NULL) { //dnode has next node
			dptr.head->pre = NULL;
			dnode->pre = NULL;
			dnode->next = NULL;
		}
		else { //dptr.head == NULL : dnode is first and last node
			dptr.tail == NULL;
		}

	}
	else if (dptr.tail == dnode) { //dnode is not first, but last node
		dptr.tail = dnode->pre;
		dptr.tail->next = NULL;

	}
	else { //not the head, tail
		DATA *temp = dnode;
		if (dnode->pre != NULL) {
			dnode->pre->next = temp->next;
		}
		if (dnode->next != NULL) {
			dnode->next->pre = temp->pre;
		}
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

DATA *make_data(int inode) {
	DATA *n = (DATA *)calloc(1, sizeof(DATA));
	n->d_ino = inode;
	n->data = NULL;
	n->pre = NULL;
	n->next = NULL;
	return n;
}

