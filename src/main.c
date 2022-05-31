#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct rb_Node{
    bool is_red;
    bool is_filtered;
    struct rb_Node *left;
    struct rb_Node *right;
    struct rb_Node *p;
    char word[];
} node_t;

node_t *left_rotate(node_t *root, node_t *x){
    node_t *y = x->right;
    x->right = y->left;
    if(y->left != NULL)
        y->left->p = x;
    y->p = x->p;
    if(x->p == NULL)
        root = y;
    else if(x == x->p->left)
        x->p->left = y;
    else x->p->right = y;
    y->left = x;
    x->p = y;

    return root;
}

node_t *right_rotate(node_t *root, node_t *y){
    node_t *x = y->left;
    y->left = x->right;
    if(x->right != NULL)
        x->right->p = y;
    x->p = y->p;
    if(y->p == NULL)
        root = x;
    else if(y == y->p->left)
        y->p->left = x;
    else y->p->right = x;
    x->right = y;
    y->p = x;

    return root;
}

node_t *rb_insert_fixup(node_t *root, node_t *z) {
    if(z == root)
        z->is_red = false;
    else {
        node_t *x = z->p;
        if(x->is_red == true) {
            if(x == x->p->left) {
                node_t *y = x->p->right;
                if(y->is_red == true) {
                    x->is_red = false;
                    y->is_red = false;
                    x->p->is_red = true;
                    rb_insert_fixup(root, x->p);
                } else if(z == x->right) {
                    z = x;
                    root = left_rotate(root, z);
                    x = z->p;
                }
                x->is_red = false;
                x->p->is_red = true;
                root = right_rotate(root, x->p);
            }
            else {
                node_t *y = x->p->left;
                if(y->is_red == true) {
                    x->is_red = false;
                    y->is_red = false;
                    x->p->is_red = true;
                    rb_insert_fixup(root, x->p);
                } else if(z == x->left) {
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
    return root;
}

node_t *rb_insert(node_t *root, node_t *z){
    node_t *y = NULL;
    node_t *x = root;
    while(x != NULL){
        y = x;
        if(strcmp(z->word, x->word) < 0)
            x = x->left;
        else x = x->right;
    }
    z->p = y;
    if(y == NULL)
        root = z;
    else if(strcmp(z->word, y->word) < 0)
        y->left = z;
    else y->right = z;
    z->left = NULL;
    z->right = NULL;
    z->is_red = true;
    z->is_filtered = true;
    rb_insert_fixup(root, z);

    return root;
}

int main() {
    return 0;
}
