#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LENGTH 100

typedef struct rb_Node {
    bool is_red;
    bool is_filtered;
    struct rb_Node* left;
    struct rb_Node* right;
    struct rb_Node* p;
    char word[];
} node_t;


node_t* left_rotate(node_t* root, node_t* x) {
    node_t* y = x->right;
    x->right = y->left;
    if(y->left != NULL) {
        y->left->p = x;
    }
    y->p = x->p;
    if(x->p == NULL) {
        root = y;
    } else if(x == x->p->left) {
        x->p->left = y;
    } else {
        x->p->right = y;
    }
    y->left = x;
    x->p = y;

    return root;
}

node_t* right_rotate(node_t* root, node_t* x) {
    node_t* y = x->left;
    x->left = y->right;
    if(y->right != NULL) {
        y->right->p = x;
    }
    y->p = x->p;
    if(x->p == NULL) {
        root = y;
    } else if(x == x->p->right) {
        x->p->right = y;
    } else {
        x->p->left = y;
    }
    y->right = x;
    x->p = y;

    return root;
}

node_t* rb_insert_fixup(node_t* root, node_t* z) {
    if(z == root) {
        root->is_red = false;
    } else {
        node_t* x = z->p;
        if(x->is_red) {
            if(x == x->p->left) {
                node_t* y = x->p->right;
                if(y->is_red) {
                    x->is_red = false;
                    y->is_red = false;
                    x->p->is_red = true;
                    root = rb_insert_fixup(root, x->p);
                } else {
                    if(z == x->right) {
                        z = x;
                        root = left_rotate(root, z);
                        x = z->p;
                    }
                    x->is_red = false;
                    x->p->is_red = true;
                    root = right_rotate(root, x->p);
                }
            } else {
                node_t* y = x->p->left;
                if(y->is_red) {
                    x->is_red = false;
                    y->is_red = false;
                    x->p->is_red = true;
                    root = rb_insert_fixup(root, x->p);
                } else {
                    if(z == x->left) {
                        z = x;
                        root = right_rotate(root, z);
                        x = z->p;
                    }
                    x->is_red = false;
                    x->p->is_red = true;
                    root = left_rotate(root, x->p);
                }
            }
        }
    }
    return root;
}

node_t* rb_insert(node_t* root, node_t* z) {
    node_t* y = NULL;
    node_t* x = root;
    while(x != NULL) {
        y = x;
        if(strcmp(z->word, x->word) < 0) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    z->p = y;
    if(y == NULL) {
        root = z;
    } else if(strcmp(z->word, y->word) < 0) {
        y->left = z;
    } else {
        y->right = z;
    }
    z->left = NULL;
    z->right = NULL;
    z->is_red = true;
    z->is_filtered = true;
    root = rb_insert_fixup(root, z);

    return root;
}

/* void rb_destroy(node_t* root) {
    if(root != NULL) {
        rb_destroy(root->left);
        rb_destroy(root->right);
        free(root);
    }
} */

void print_tree(node_t* root) {
    if(root != NULL) {
        print_tree(root->left);
        printf("%s\n", root->word);
        print_tree(root->right);
    }
}

void read_input(char* input) {
    int i = 0;
    char c;
    while((c = getc_unlocked(stdin)) != '\n' && c != EOF) {
        input[i++] = c;
    }
    input[i] = '\0';
}

void game_init(int* k, node_t** root) {
    char input[MAX_LENGTH];
    read_input(input);
    *k = atoi(input);
    read_input(input);
    while(strcmp(input, "+nuova_partita") != 0) {
        node_t *z = malloc(sizeof(node_t) + *(k) + 1);
        strcpy(z->word, input);
        *root = rb_insert(*root, z);
        read_input(input);
    }
}

int main() {
    int k;
    node_t* root = NULL;
    game_init(&k, &root);
    print_tree(root);
    return 0;
}
