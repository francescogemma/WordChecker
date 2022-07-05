#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// maximum number of characters in a word
#define MAX_WORD_LENGTH 100


/* typedef struct word_Node {
    struct word_Node *next;
    char word[];
} word_node_t; */

// struct for the red-black tree node for filtered words
typedef struct rb_Node {
    bool is_red;
    struct rb_Node* left;
    struct rb_Node* right;
    struct rb_Node* p;
    char word[];
} rb_node_t;

// struct for the lists of positions where a character must appear or cannot appear
typedef struct position_Node {
    unsigned short pos;
    struct position_Node* next;
} pos_node_t;

// struct for list of character constraints
typedef struct char_Node {
    char c;
    short appears_exactly;
    short appears_at_least;
    pos_node_t* appears_in;
    pos_node_t* not_appears_in;
    struct char_Node* next;
} char_node_t;


// global variables
unsigned short k;

// functions declarations
char* fastcpy(char* dest, const char* string_to_copy);
short fastcmp(const char* s1, const char* s2);
rb_node_t* left_rotate(rb_node_t* root, rb_node_t* x);
rb_node_t* right_rotate(rb_node_t* root, rb_node_t* x);
rb_node_t* rb_insert_fixup(rb_node_t* root, rb_node_t* z);
rb_node_t* rb_insert(rb_node_t* root, char* word);
/* word_node_t* word_list_insert(word_node_t* head, char* word);
void word_list_destroy(word_node_t* head); */
/* rb_node_t* tree_minimum(rb_node_t* root, rb_node_t* x);
rb_node_t* tree_successor(rb_node_t* root, rb_node_t* x);
rb_node_t* rb_delete_fixup(rb_node_t* root, rb_node_t* x);
rb_node_t* rb_delete(rb_node_t* root, rb_node_t* z); */
rb_node_t* rb_search(rb_node_t* root, char* word);
void rb_destroy(rb_node_t* root);
void read_input(char* input);
char_node_t* char_list_search(char_node_t* head, char c);
char_node_t* char_list_insert(char_node_t* head, char c);
pos_node_t* pos_list_insert(pos_node_t* head, unsigned short position);
pos_node_t* pos_list_search(pos_node_t* head, unsigned short position);
void destroy_pos_list(pos_node_t* head);
char_node_t* update_constraints(char_node_t* head, char* word_input, const char* result);
bool filter(char_node_t* head, const char* word);
void copy_filter_count_tree(rb_node_t* filtered_root, char_node_t* head, rb_node_t** new_filtered_root, unsigned* filtered_counter);
rb_node_t* update_tree(rb_node_t* old_filtered_root, char_node_t* head, unsigned* filtered_counter);
void initialize_tree(rb_node_t** filtered_root, rb_node_t* word_root);
void print_filtered(rb_node_t* root);
void game_init(rb_node_t** word_root);
void main_game(rb_node_t** word_root);
void add_words_to_dictionary_in_game(char_node_t* head, rb_node_t** root, rb_node_t** word_root);
void add_words_to_dictionary_not_in_game(rb_node_t** word_root);
char* give_result(char* word_to_find, char* word_input, char* result);


// functions definitions
char* fastcpy(char* dest, const char* string_to_copy) {
    unsigned short i = 0;
    while(string_to_copy[i] != '\0') {
        dest[i] = string_to_copy[i];
        i++;
    }
    dest[i] = '\0';

    return dest;
}

short fastcmp(const char* s1, const char* s2) {
    unsigned short i = 0;
    while(s1[i] == s2[i]) {
        if (s1[i] == '\0') {
            return 0;
        }
        i++;
    }
    if(s1[i] < s2[i]) {
        return -1;
    } else {
        return 1;
    }
}

/* word_node_t* word_list_insert(word_node_t* head, char* word) {
    word_node_t* z = malloc(sizeof(word_node_t) + k + 1);
    fastcpy(z->word, word);
    z->next = head;
    return z;
} */

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

rb_node_t* rb_insert_fixup(rb_node_t* root, rb_node_t* z) {
    while(z->p != NULL && z->p->is_red == true) {
        if(z->p == z->p->p->left) {
            rb_node_t* y = z->p->p->right;
            if(y != NULL && y->is_red == true) {
                z->p->is_red = false;
                y->is_red = false;
                z->p->p->is_red = true;
                z = z->p->p;
            } else {
                if(z == z->p->right) {
                    z = z->p;
                    root = left_rotate(root, z);
                }
                z->p->is_red = false;
                z->p->p->is_red = true;
                root = right_rotate(root, z->p->p);
            }
        } else {
            rb_node_t* y = z->p->p->left;
            if(y != NULL && y->is_red == true) {
                z->p->is_red = false;
                y->is_red = false;
                z->p->p->is_red = true;
                z = z->p->p;
            } else {
                if(z == z->p->left) {
                    z = z->p;
                    root = right_rotate(root, z);
                }
                z->p->is_red = false;
                z->p->p->is_red = true;
                root = left_rotate(root, z->p->p);
            }
        }
    }
    root->is_red = false;
    return root;
}

rb_node_t* rb_insert(rb_node_t* root, char* word) {
    rb_node_t* z = malloc(sizeof(rb_node_t) + k + 1);
    fastcpy(z->word, word);

    rb_node_t* y = NULL;
    rb_node_t* x = root;
    while(x != NULL) {
        y = x;
        if(fastcmp(z->word, x->word) < 0) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    z->p = y;
    if(y == NULL) {
        root = z;
    } else if(fastcmp(z->word, y->word) < 0) {
        y->left = z;
    } else {
        y->right = z;
    }
    z->left = NULL;
    z->right = NULL;
    z->is_red = true;

    root = rb_insert_fixup(root, z);

    return root;
}

rb_node_t* rb_search(rb_node_t* root, char* word) {
    if(root == NULL || !fastcmp(word, root->word)) {
        return root;
    }
    if(fastcmp(word, root->word) < 0) {
        return rb_search(root->left, word);
    } else {
        return rb_search(root->right, word);
    }
}

/* rb_node_t* tree_minimum(rb_node_t* root, rb_node_t* x) {
    while(x->left != NULL) {
        x = x->left;
    }
    return x;
}

rb_node_t* tree_successor(rb_node_t* root, rb_node_t* x) {
    if(x->right != NULL) {
        return tree_minimum(root, x->right);
    }
    rb_node_t* y = x->p;
    while(y != NULL && x == y->right) {
        x = y;
        y = y->p;
    }
    return y;
}

rb_node_t* rb_delete_fixup(rb_node_t* root, rb_node_t* x) {
    while(x != root && x->is_red == false) {
        if(x->p != NULL && x == x->p->left) {
            rb_node_t *w = x->p->right;
            if (w != NULL && w->is_red == true) {
                w->is_red = false;
                x->p->is_red = true;
                root = left_rotate(root, x->p);
                w = x->p->right;
            }
            if(w->right != NULL && w->left != NULL) {
                if (w->left->is_red == false && w->right->is_red == false) {
                    w->is_red = true;
                    x = x->p;
                } else {
                    if (w->right->is_red == false) {
                        w->left->is_red = false;
                        w->is_red = true;
                        root = right_rotate(root, w);
                        w = x->p->right;
                    }
                    w->is_red = x->p->is_red;
                    x->p->is_red = false;
                    w->right->is_red = false;
                    root = left_rotate(root, x->p);
                    x = root;
                }
            }
        } else {
            rb_node_t *w = x->p->left;
            if (w != NULL && w->is_red == true) {
                w->is_red = false;
                x->p->is_red = true;
                root = right_rotate(root, x->p);
                w = x->p->left;
            }
            if(w->right != NULL && w->left != NULL) {
                if (w->right->is_red == false && w->left->is_red == false) {
                    w->is_red = true;
                    x = x->p;
                } else {
                    if (w->left->is_red == false) {
                        w->right->is_red = false;
                        w->is_red = true;
                        root = left_rotate(root, w);
                        w = x->p->left;
                    }
                    w->is_red = x->p->is_red;
                    x->p->is_red = false;
                    w->left->is_red = false;
                    root = right_rotate(root, x->p);
                    x = root;
                }
            }
        }
    }
    x->is_red = false;
    return root;
}

rb_node_t* rb_delete(rb_node_t* root, rb_node_t* z) {
    rb_node_t* y;
    rb_node_t* x;
    if(z->left == NULL || z->right == NULL) {
        y = z;
    } else y = tree_successor(root, z);
    if(y->left != NULL) {
        x = y->left;
    } else x = y->right;
    if(x != NULL) {
        x->p = y->p;
    }
    if(y->p == NULL) {
        root = x;
    } else if(y == y->p->left) {
        y->p->left = x;
    } else y->p->right = x;
    if(y != z) {
        fastcpy(z->word, y->word);
    }
    if(y->is_red == false) {
        root = rb_delete_fixup(root, x);
    }
    return y;
}*/

void rb_destroy(rb_node_t* root) {
    if(root != NULL) {
        rb_destroy(root->left);
        rb_destroy(root->right);
        free(root);
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

void add_words_to_dictionary_in_game(char_node_t* head, rb_node_t** root, rb_node_t** word_root) {
    char word[MAX_WORD_LENGTH];
    read_input(word);
    while(word[0] != '+') { // finchè non c'è un +inserisci_fine
        *word_root = rb_insert(*word_root, word);
        if(filter(head, word) == true) {
            *root = rb_insert(*root, word);
        }
        read_input(word);
    }
}

void add_words_to_dictionary_not_in_game(rb_node_t** word_root) {
    char word[MAX_WORD_LENGTH];
    read_input(word);
    while(word[0] != '+') { // finchè non c'è un +inserisci_fine
        *word_root = rb_insert(*word_root, word);
        read_input(word);
    }
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

char_node_t* char_list_search(char_node_t* head, char c) {
    char_node_t* x = head;
    while(x != NULL && x->c != c) {
        x = x->next;
    }
    return x;
}

char_node_t* char_list_insert(char_node_t* head, char c) {
    char_node_t* x = malloc(sizeof(char_node_t));
    x->c = c;
    x->appears_at_least = -1;
    x->appears_exactly = -1;
    x->appears_in = NULL;
    x->not_appears_in = NULL;
    x->next = head;
    return x;
}

pos_node_t* pos_list_insert(pos_node_t* head, unsigned short position) {
    pos_node_t* x = malloc(sizeof(pos_node_t));
    x->pos = position;
    x->next = head;
    return x;
}

pos_node_t* pos_list_search(pos_node_t* head, unsigned short position) {
    pos_node_t* x = head;
    while(x != NULL && x->pos != position) {
        x = x->next;
    }
    return x;
}

void destroy_pos_list(pos_node_t* head) {
    pos_node_t* x = head;
    while(x != NULL) {
        pos_node_t* y = x;
        x = x->next;
        free(y);
    }
}

char_node_t* update_constraints(char_node_t* head, char* word_input, const char* result) {
    for(int i = 0; i < k; i++) {
        if(result[i] == '+') {
            char_node_t* x = char_list_search(head, word_input[i]);
            if(x == NULL) {
                head = char_list_insert(head, word_input[i]);
                x = head;
            }
            if(pos_list_search(x->appears_in, i) == NULL) {
                x->appears_in = pos_list_insert(x->appears_in, i);
            }
        }
    }

    for(int i = 0; i < k; i++) {
        if(result[i] == '/') {
            char_node_t* x = char_list_search(head, word_input[i]);
            if(x == NULL) {
                head = char_list_insert(head, word_input[i]);
                x = head;
            }
            if(pos_list_search(x->not_appears_in, i) == NULL) {
                x->not_appears_in = pos_list_insert(x->not_appears_in, i);
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
            char_node_t* x = char_list_search(head, word_input[i]);
            if(x == NULL) {
                head = char_list_insert(head, word_input[i]);
                x = head;
            }
            if(pos_list_search(x->not_appears_in, i) == NULL) {
                x->not_appears_in = pos_list_insert(x->not_appears_in, i);
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

bool filter(char_node_t* head, const char* word) {
    short counter;
    //short appears_plus_length = 0;
    char_node_t* x = head;
    while(x != NULL) {
        counter = 0;
        pos_node_t* curr = x->appears_in;
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

/* void word_list_destroy(word_node_t* head) {
    word_node_t* x = head;
    while(x != NULL) {
        word_node_t* y = x;
        x = x->next;
        free(y);
    }
} */

void copy_filter_count_tree(rb_node_t* filtered_root, char_node_t* head, rb_node_t** new_filtered_root, unsigned* filtered_counter) {
    if(filtered_root != NULL) {
        copy_filter_count_tree(filtered_root->left, head, new_filtered_root, filtered_counter);
        if(filter(head, filtered_root->word) == true) {
            *new_filtered_root = rb_insert(*new_filtered_root, filtered_root->word);
            (*filtered_counter)++;
        }
        copy_filter_count_tree(filtered_root->right, head, new_filtered_root, filtered_counter);
    }
}

rb_node_t* update_tree(rb_node_t* old_filtered_root, char_node_t* head, unsigned* filtered_counter) {
    rb_node_t* new_filtered_root = NULL;
    copy_filter_count_tree(old_filtered_root, head, &new_filtered_root, filtered_counter);
    rb_destroy(old_filtered_root);
    return new_filtered_root;
}

void print_filtered(rb_node_t* root) {
    if(root != NULL) {
        print_filtered(root->left);
        puts(root->word);
        print_filtered(root->right);
    }
}

void initialize_tree(rb_node_t** filtered_root, rb_node_t* word_root) {
    if(word_root != NULL) {
        initialize_tree(filtered_root, word_root->left);
        *filtered_root = rb_insert(*filtered_root, word_root->word);
        initialize_tree(filtered_root, word_root->right);
    }
}

/* word_node_t* word_search(word_node_t* head, const char* word) {
    word_node_t* x = head;
    while(x != NULL) {
        if(fastcmp(x->word, word) == 0) {
            return x;
        }
        x = x->next;
    }
    return NULL;
} */

void game_init(rb_node_t** word_root) {
    char input[MAX_WORD_LENGTH];
    read_input(input);
    k = (unsigned short)atoi(input);
    read_input(input);
    while(input[0] != '+') {
        *word_root = rb_insert(*word_root, input);
        read_input(input);
    }
}

void main_game(rb_node_t** word_root) {
    unsigned attempts;
    rb_node_t* filtered_root = NULL;
    char_node_t* char_list_head = NULL;
    char_node_t* curr;

    char word_to_find[k+1];
    read_input(word_to_find);

    char input[MAX_WORD_LENGTH];
    read_input(input);
    attempts = (unsigned)atoi(input);
    initialize_tree(&filtered_root, *word_root);

    while(attempts > 0) {
        read_input(input);
         if(input[0] == '+') {    // if there is a command in input
            if(input[1] == 'i') {   // in the command is +inserisci_inizio
                add_words_to_dictionary_in_game(char_list_head, &filtered_root, word_root);
            } else {    // if the command is +stampa_filtrate
                print_filtered(filtered_root);
            }
        } else {
             if(!fastcmp(input, word_to_find)) {
                 puts("ok");

                 while(char_list_head != NULL) {
                     curr = char_list_head;
                     char_list_head = char_list_head->next;
                     destroy_pos_list(curr->appears_in);
                     destroy_pos_list(curr->not_appears_in);
                     free(curr);
                 }
                 rb_destroy(filtered_root);
                 return;
             } else if(rb_search(*word_root, input) != NULL) {
                 char result[k+1];
                 char word_to_find_copy[k+1];
                 char input_copy[k+1];
                 fastcpy(word_to_find_copy, word_to_find);
                 fastcpy(input_copy, input);
                 fastcpy(result, give_result(word_to_find_copy, input_copy, result));
                 puts(result);

                 char_list_head = update_constraints(char_list_head, input, result);

                 unsigned filtered_counter = 0;
                 filtered_root = update_tree(filtered_root, char_list_head, &filtered_counter);

                 fprintf(stdout, "%i\n", filtered_counter);

                 --attempts;
             } else {
                 puts("not_exists");
             }
        }
    }
    puts("ko");

    while(char_list_head != NULL) {
        curr = char_list_head;
        char_list_head = char_list_head->next;
        destroy_pos_list(curr->appears_in);
        destroy_pos_list(curr->not_appears_in);
        free(curr);
    }
    rb_destroy(filtered_root);
}

// main function
int main() {
    rb_node_t* word_root = NULL;
    char command[MAX_WORD_LENGTH];

    game_init(&word_root);

    main_game(&word_root);
    while(fgets(command, MAX_WORD_LENGTH, stdin) != NULL) {
        if(command[0] == '+') { // if there is a command in input
            if(command[1] == 'n') { // if the command is +nuova_partita
                main_game(&word_root);
            } else if(command[1] == 'i') {  // if the command is +inserisci_inizio
                add_words_to_dictionary_not_in_game(&word_root);
            }
        }
    }

    rb_destroy(word_root);
    return 0;
}
