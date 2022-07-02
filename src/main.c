#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// maximum number of characters in a word
#define MAX_WORD_LENGTH 100

// struct for the red-black tree node
typedef struct rb_Node {
    bool is_red;
    bool is_filtered;
    struct rb_Node* left;
    struct rb_Node* right;
    struct rb_Node* p;
    char word[];
} node_t;


// global variables
unsigned short k;
unsigned short attempts;


// functions declarations
node_t* left_rotate(node_t* root, node_t* x);
node_t* right_rotate(node_t* root, node_t* x);
node_t* rb_insert_fixup(node_t* root, node_t* z);
node_t* rb_insert(node_t* root, node_t* z);
node_t* set_all_filtered(node_t* root);
void rb_destroy(node_t* root);
void print_tree(node_t* root);
void read_input(char* input);
void game_init(node_t** root);
void main_game(node_t** root);
void add_words_to_tree(node_t** root);
char* give_result(char* word_to_find, char* word_input, char* result);

// functions definitions
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

// TODO: change rb_insert_fixup from recursive to iterative
node_t* rb_insert_fixup(node_t* root, node_t* z) {
    if(z == root) {
        root->is_red = false;
    } else {
        node_t* x = z->p;
        if(x->is_red) {
            if(x == x->p->left) {
                node_t* y = x->p->right;
                if(y != NULL && y->is_red) {
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
                if(y != NULL && y->is_red) {
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

node_t* rb_search(node_t* root, char* word) {
    if(root == NULL || !strcmp(word, root->word)) {
        return root;
    }
    if(strcmp(word, root->word) < 0) {
        return rb_search(root->left, word);
    } else {
        return rb_search(root->right, word);
    }
}

void rb_destroy(node_t* root) {
    if(root != NULL) {
        rb_destroy(root->left);
        rb_destroy(root->right);
        free(root);
    }
}

void print_tree(node_t* root) {
    if(root != NULL) {
        print_tree(root->left);
        printf("%s\n", root->word);
        print_tree(root->right);
    }
}

void read_input(char* input) {
    unsigned short i = 0;
    char c;
    while((c = getc_unlocked(stdin)) != '\n' && c != EOF) {
        input[i++] = c;
    }
    input[i] = '\0';
}

void add_words_to_tree(node_t** root) {
    char word[MAX_WORD_LENGTH];
    read_input(word);
    while(strcmp(word, "+inserisci_fine") != 0) {
        node_t* z = malloc(sizeof(node_t) + k + 1);
        strcpy(z->word, word);
        *root = rb_insert(*root, z);
        read_input(word);
    }
}

node_t* set_all_filtered(node_t* root) {
    if(root != NULL) {
        root->is_filtered = true;
        set_all_filtered(root->left);
        set_all_filtered(root->right);
    }
    return root;
}

char* give_result(char* word_to_find, char* word_input, char* result) {
    bool found;
    unsigned short i, j;

    for(i = 0; i < k; i++) {
        if(word_input[i] == word_to_find[i]) {
            word_to_find[i] = word_input[i] = '*';
            result[i] = '+';
        }
    }

    for(i = 0, found = false; i < k; i++, found = false) {
        if(word_input[i] != '*') {
            for(j = 0; j < k && !found; j++) {
                if(word_input[i] == word_to_find[j]) {
                    found = true;
                    result[i] = '|';
                    word_to_find[j] = '*';
                }
            }
            if(!found) {
                result[i] = '/';
            }
        }
    }
    result[k] = '\0';

    return result;
}

void game_init(node_t** root) {
    char input[MAX_WORD_LENGTH];
    read_input(input);
    k = (unsigned short)atoi(input);
    read_input(input);
    while(strcmp(input, "+nuova_partita") != 0) {
        node_t* z = malloc(sizeof(node_t) + k + 1);
        strcpy(z->word, input);
        *root = rb_insert(*root, z);
        read_input(input);
    }
}

void main_game(node_t** root) {
    char word_to_find[k+1];
    read_input(word_to_find);

    char input[MAX_WORD_LENGTH];
    read_input(input);
    attempts = (unsigned short)atoi(input);

    while(attempts > 0) {
        read_input(input);
        if(!strcmp(input, word_to_find)) {
            puts("ok");
            return;
        } else if(!strcmp(input, "+inserisci_inizio")) {
            add_words_to_tree(root);
        } else if(!strcmp(input, "+stampa_filtrate")) {
            // TODO: make the stampa_filtrate
        } else {
            // TODO: give number of words that may be correct
            if(rb_search(*root, input) != NULL) {
                char result[k+1];
                char word_to_find_copy[k+1];
                char input_copy[k+1];
                strcpy(word_to_find_copy, word_to_find);
                strcpy(input_copy, input);
                strcpy(result, give_result(word_to_find_copy, input_copy, result));
                puts(result);
                attempts--;
            } else {
                puts("not_exists");
            }
        }
    }
    puts("ko");
}

// main function
int main() {
    node_t* root = NULL;
    char command[MAX_WORD_LENGTH];

    game_init(&root);

    main_game(&root);
    while(fgets(command, MAX_WORD_LENGTH, stdin) != NULL) {
        if(!strcmp(command, "+nuova_partita\n")) {
            root = set_all_filtered(root);
            main_game(&root);
        } else if(!strcmp(command, "+inserisci_inizio\n")) {
            add_words_to_tree(&root);
        }
    }

    // print_tree(root);
    rb_destroy(root);
    return 0;
}
