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
} rb_node_t;

typedef struct simple_Node {
    unsigned short pos;
    struct simple_Node* next;
} s_node_t;

typedef struct char_Node {
    char c;
    short appears_exactly;
    short appears_at_least;
    s_node_t* appears_in;
    s_node_t* not_appears_in;
    struct char_Node* next;
} char_node_t;


// global variables
unsigned short k;
unsigned short attempts;


// functions declarations
rb_node_t* left_rotate(rb_node_t* root, rb_node_t* x);
rb_node_t* right_rotate(rb_node_t* root, rb_node_t* x);
rb_node_t* rb_insert_fixup(rb_node_t* root, rb_node_t* z);
rb_node_t* rb_insert(rb_node_t* root, rb_node_t* z);
rb_node_t* rb_search(rb_node_t* root, char* word);
rb_node_t* set_all_filtered(rb_node_t* root);
void rb_destroy(rb_node_t* root);
void print_tree(rb_node_t* root);
void read_input(char* input);
unsigned count_filtered(rb_node_t* root);
char_node_t* list_search(char_node_t* head, char c);
char_node_t* list_insert(char_node_t* head, char c);
s_node_t* simple_list_insert(s_node_t* head, unsigned short position);
s_node_t* simple_list_search(s_node_t* head, unsigned short position);
// void destroy_list(char_node_t* head);
void destroy_simple_list(s_node_t* head);
// short simple_list_length(s_node_t* head);
char_node_t* update_constraints(char_node_t* head, char* word_input, char* result);
bool filter(char_node_t* head, char* word);
void game_init(rb_node_t** root);
void main_game(rb_node_t** root);
void add_words_to_tree(rb_node_t** root);
char* give_result(char* word_to_find, char* word_input, char* result);

// functions definitions
rb_node_t* left_rotate(rb_node_t* root, rb_node_t* x) {
    rb_node_t* y = x->right;
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

rb_node_t* right_rotate(rb_node_t* root, rb_node_t* x) {
    rb_node_t* y = x->left;
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
rb_node_t* rb_insert_fixup(rb_node_t* root, rb_node_t* z) {
    if(z == root) {
        root->is_red = false;
    } else {
        rb_node_t* x = z->p;
        if(x->is_red) {
            if(x == x->p->left) {
                rb_node_t* y = x->p->right;
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
                rb_node_t* y = x->p->left;
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

rb_node_t* rb_insert(rb_node_t* root, rb_node_t* z) {
    rb_node_t* y = NULL;
    rb_node_t* x = root;
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

rb_node_t* rb_search(rb_node_t* root, char* word) {
    if(root == NULL || !strcmp(word, root->word)) {
        return root;
    }
    if(strcmp(word, root->word) < 0) {
        return rb_search(root->left, word);
    } else {
        return rb_search(root->right, word);
    }
}

void rb_destroy(rb_node_t* root) {
    if(root != NULL) {
        rb_destroy(root->left);
        rb_destroy(root->right);
        free(root);
    }
}

void print_tree(rb_node_t* root) {
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

void add_words_to_tree(rb_node_t** root) {
    char word[MAX_WORD_LENGTH];
    read_input(word);
    while(strcmp(word, "+inserisci_fine") != 0) {
        rb_node_t* z = malloc(sizeof(rb_node_t) + k + 1);
        strcpy(z->word, word);
        *root = rb_insert(*root, z);
        read_input(word);
    }
}

rb_node_t* set_all_filtered(rb_node_t* root) {
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

unsigned count_filtered(rb_node_t* root) {
    if(root == NULL) {
        return 0;
    }
    if(root->is_filtered) {
        return 1 + count_filtered(root->left) + count_filtered(root->right);
    } else {
        return count_filtered(root->left) + count_filtered(root->right);
    }
}

// list search function
char_node_t* list_search(char_node_t* head, char c) {
    char_node_t* x = head;
    while(x != NULL && x->c != c) {
        x = x->next;
    }
    return x;
}

char_node_t* list_insert(char_node_t* head, char c) {
    char_node_t* x = malloc(sizeof(char_node_t));
    x->c = c;
    x->appears_at_least = -1;
    x->appears_exactly = -1;
    x->appears_in = NULL;
    x->not_appears_in = NULL;
    x->next = head;
    return x;
}

s_node_t* simple_list_insert(s_node_t* head, unsigned short position) {
    s_node_t* x = malloc(sizeof(s_node_t));
    x->pos = position;
    x->next = head;
    return x;
}

s_node_t* simple_list_search(s_node_t* head, unsigned short position) {
    s_node_t* x = head;
    while(x != NULL && x->pos != position) {
        x = x->next;
    }
    return x;
}

/* short simple_list_length(s_node_t* head) {
    short i = 0;
    s_node_t* x = head;
    while(x != NULL) {
        i++;
        x = x->next;
    }
    return i;
} */

/* void destroy_list(char_node_t* head) {
    char_node_t* x = head;
    while(x != NULL) {
        char_node_t* y = x;
        x = x->next;
        free(y);
    }
} */

void destroy_simple_list(s_node_t* head) {
    s_node_t* x = head;
    while(x != NULL) {
        s_node_t* y = x;
        x = x->next;
        free(y);
    }
}

char_node_t* update_constraints(char_node_t* head, char* word_input, char* result) {
    for(int i = 0; i < k; i++) {
        if(result[i] == '+') {
            char_node_t* x = list_search(head, word_input[i]);
            if(x == NULL) {
                head = list_insert(head, word_input[i]);
                x = head;
            }
            if(simple_list_search(x->appears_in, i) == NULL) {
                x->appears_in = simple_list_insert(x->appears_in, i);
            }
        }
    }

    for(int i = 0; i < k; i++) {
        if(result[i] == '/') {
            char_node_t* x = list_search(head, word_input[i]);
            if(x == NULL) {
                head = list_insert(head, word_input[i]);
                x = head;
            }
            if(simple_list_search(x->not_appears_in, i) == NULL) {
                x->not_appears_in = simple_list_insert(x->not_appears_in, i);
            }
            short counter = 0;
            for(int j = 0; j < k; j++) {
                if((result[j] == '|' || result[j] == '+') && word_input[j] == word_input[i]) {
                    counter++;
                }
            }
            x->appears_exactly = counter;
        }
    }

    for(int i = 0; i < k; i++) {
        if(result[i] == '|') {
            char_node_t* x = list_search(head, word_input[i]);
            if(x == NULL) {
                head = list_insert(head, word_input[i]);
                x = head;
            }
            if(simple_list_search(x->not_appears_in, i) == NULL) {
                x->not_appears_in = simple_list_insert(x->not_appears_in, i);
            }
            short counter = 0;
            for(int j = 0; j < k; j++) {
                if((result[j] == '|' || result[j] == '+') && word_input[j] == word_input[i]) {
                    counter++;
                }
            }
            if(counter > x->appears_at_least) {
                x->appears_at_least = counter;
            }
        }
    }
    return head;
}

bool filter(char_node_t* head, char* word) {
    short counter;
    //short appears_plus_length = 0;
    char_node_t* x = head;
    while(x != NULL) {
        counter = 0;
        s_node_t* curr = x->appears_in;
        //appears_plus_length = simple_list_length(curr);
        while(curr != NULL) {
            if(word[curr->pos] != x->c) {
                return false;
            }
            curr = curr->next;
        }
        curr = x->not_appears_in;
        while(curr != NULL) {
            if(word[curr->pos] == x->c) {
                return false;
            }
            curr = curr->next;
        }
        if(x->appears_exactly != -1) {
            for(int i = 0; i < k; i++) {
                if(word[i] == x->c) counter++;
            }
            if(counter != x->appears_exactly) return false;
        } else if(x->appears_at_least != -1) {
            counter = 0;
            for(int i = 0; i < k; i++) {
                if(word[i] == x->c) counter++;
            }
            if(counter < x->appears_at_least) return false;
        }
        x = x->next;
    }
    return true;
}

rb_node_t* filter_tree(rb_node_t* root, char_node_t* head) {
    if(root != NULL) {
        filter_tree(root->left, head);
        if(root->is_filtered == true && filter(head, root->word) == false) {
            root->is_filtered = false;
        }
        filter_tree(root->right, head);
    }
    return root;
}

void print_filtered(rb_node_t* root) {
    if(root != NULL) {
        print_filtered(root->left);
        if(root->is_filtered == true) {
            puts(root->word);
        }
        print_filtered(root->right);
    }
}

void game_init(rb_node_t** root) {
    char input[MAX_WORD_LENGTH];
    read_input(input);
    k = (unsigned short)atoi(input);
    read_input(input);
    while(strcmp(input, "+nuova_partita") != 0) {
        rb_node_t* z = malloc(sizeof(rb_node_t) + k + 1);
        strcpy(z->word, input);
        *root = rb_insert(*root, z);
        read_input(input);
    }
}

void main_game(rb_node_t** root) {
    char_node_t* head = NULL;
    char_node_t* curr;

    char word_to_find[k+1];
    read_input(word_to_find);

    char input[MAX_WORD_LENGTH];
    read_input(input);
    attempts = (unsigned short)atoi(input);

    while(attempts > 0) {
        read_input(input);
        if(!strcmp(input, word_to_find)) {
            puts("ok");

            while(head != NULL) {
                curr = head;
                head = head->next;
                destroy_simple_list(curr->appears_in);
                destroy_simple_list(curr->not_appears_in);
                free(curr);
            }

            return;
        } else if(!strcmp(input, "+inserisci_inizio")) {
            add_words_to_tree(root);
            *root = filter_tree(*root, head);
        } else if(!strcmp(input, "+stampa_filtrate")) {
            print_filtered(*root);
        } else {
            if(rb_search(*root, input) != NULL) {
                char result[k+1];
                char word_to_find_copy[k+1];
                char input_copy[k+1];
                strcpy(word_to_find_copy, word_to_find);
                strcpy(input_copy, input);
                strcpy(result, give_result(word_to_find_copy, input_copy, result));
                puts(result);

                head = update_constraints(head, input, result);
                *root = filter_tree(*root, head);

                fprintf(stdout, "%d\n", count_filtered(*root));

                attempts--;
            } else {
                puts("not_exists");
            }
        }
    }
    puts("ko");

    while(head != NULL) {
        curr = head;
        head = head->next;
        destroy_simple_list(curr->appears_in);
        destroy_simple_list(curr->not_appears_in);
        free(curr);
    }
}

// main function
int main() {
    rb_node_t* root = NULL;
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
