#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


// maximum number of characters in a word
#define MAX_WORD_LENGTH 100


// struct red-black trees
typedef struct rb_Node {
    bool is_red;
    struct rb_Node* left;
    struct rb_Node* right;
    struct rb_Node* parent;
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
rb_node_t* rb_insert(rb_node_t* root, char* word_to_be_inserted);
rb_node_t* rb_search(rb_node_t* root, char* word_to_be_searched);
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
rb_node_t* update_tree_first_time(rb_node_t* word_root, char_node_t* head, unsigned* filtered_counter);
void print_filtered(rb_node_t* root);
unsigned short game_init(rb_node_t** word_root);
void main_game(rb_node_t** word_root);
void add_words_to_dictionary_and_filter(char_node_t* head, rb_node_t** root, rb_node_t** word_root);
void add_words_to_dictionary(rb_node_t** word_root);
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

rb_node_t* left_rotate(rb_node_t* root, rb_node_t* x) {
    rb_node_t* y = x->right;
    x->right = y->left;
    if(y->left != NULL) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if(x->parent == NULL) {
        root = y;
    } else if(x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;

    return root;
}

rb_node_t* right_rotate(rb_node_t* root, rb_node_t* x) {
    rb_node_t* y = x->left;
    x->left = y->right;
    if(y->right != NULL) {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if(x->parent == NULL) {
        root = y;
    } else if(x == x->parent->right) {
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;

    return root;
}

rb_node_t* rb_insert_fixup(rb_node_t* root, rb_node_t* z) {
    while(z->parent != NULL && z->parent->is_red == true) {
        if(z->parent == z->parent->parent->left) {
            rb_node_t* y = z->parent->parent->right;
            if(y != NULL && y->is_red == true) {
                z->parent->is_red = false;
                y->is_red = false;
                z->parent->parent->is_red = true;
                z = z->parent->parent;
            } else {
                if(z == z->parent->right) {
                    z = z->parent;
                    root = left_rotate(root, z);
                }
                z->parent->is_red = false;
                z->parent->parent->is_red = true;
                root = right_rotate(root, z->parent->parent);
            }
        } else {
            rb_node_t* y = z->parent->parent->left;
            if(y != NULL && y->is_red == true) {
                z->parent->is_red = false;
                y->is_red = false;
                z->parent->parent->is_red = true;
                z = z->parent->parent;
            } else {
                if(z == z->parent->left) {
                    z = z->parent;
                    root = right_rotate(root, z);
                }
                z->parent->is_red = false;
                z->parent->parent->is_red = true;
                root = left_rotate(root, z->parent->parent);
            }
        }
    }
    root->is_red = false;
    return root;
}

rb_node_t* rb_insert(rb_node_t* root, char* word_to_be_inserted) {
    rb_node_t* z = malloc(sizeof(rb_node_t) + k + 1);
    fastcpy(z->word, word_to_be_inserted);

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
    z->parent = y;
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

rb_node_t* rb_search(rb_node_t* root, char* word_to_be_searched) {
    if(root == NULL || !fastcmp(word_to_be_searched, root->word)) {
        return root;
    }
    if(fastcmp(word_to_be_searched, root->word) < 0) {
        return rb_search(root->left, word_to_be_searched);
    } else {
        return rb_search(root->right, word_to_be_searched);
    }
}

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

void add_words_to_dictionary_and_filter(char_node_t* head, rb_node_t** root, rb_node_t** word_root) {
    char word[MAX_WORD_LENGTH];
    read_input(word);
    while(word[0] != '+') { // until there is an +inserisci_fine
        *word_root = rb_insert(*word_root, word);
        if(filter(head, word) == true) {
            *root = rb_insert(*root, word);
        }
        read_input(word);
    }
}

void add_words_to_dictionary(rb_node_t** word_root) {
    char word[MAX_WORD_LENGTH];
    read_input(word);
    while(word[0] != '+') { // until there is an +inserisci_fine
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
    for(unsigned short i = 0; i < k; i++) {
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

    for(unsigned short i = 0; i < k; i++) {
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
            for(unsigned short j = 0; j < k; j++) {
                if((result[j] == '|' || result[j] == '+') && word_input[j] == word_input[i]) {
                    counter++;
                }
            }
            x->appears_exactly = counter;
        }
    }

    for(unsigned short i = 0; i < k; i++) {
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
            for(unsigned short j = 0; j < k; j++) {
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
    char_node_t* x = head;
    while(x != NULL) {
        counter = 0;
        pos_node_t* curr = x->appears_in;
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
            for(unsigned short i = 0; i < k; i++) {
                if(word[i] == x->c) counter++;
            }
            if(counter != x->appears_exactly) return false;
        } else if(x->appears_at_least != -1) {
            counter = 0;
            for(unsigned short i = 0; i < k; i++) {
                if(word[i] == x->c) counter++;
            }
            if(counter < x->appears_at_least) return false;
        }
        x = x->next;
    }
    return true;
}

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

rb_node_t* update_tree_first_time(rb_node_t* word_root, char_node_t* head, unsigned* filtered_counter) {
    rb_node_t* filtered_root = NULL;
    copy_filter_count_tree(word_root, head, &filtered_root, filtered_counter);
    return filtered_root;
}

void print_filtered(rb_node_t* root) {
    if(root != NULL) {
        print_filtered(root->left);
        puts(root->word);
        print_filtered(root->right);
    }
}

unsigned short game_init(rb_node_t** word_root) {
    char input[MAX_WORD_LENGTH];
    read_input(input);
    k = (unsigned short)atoi(input);
    read_input(input);
    while(input[0] != '+' && !feof(stdin)) {
        *word_root = rb_insert(*word_root, input);
        read_input(input);
    }
    if(input[1] == 'n') {
        return 0;
    } else if(input[1] == 'i'){
        return 1;
    } else {
        return 2;
    }
}

void main_game(rb_node_t** word_root) {
    bool first_time = true;
    unsigned attempts;
    rb_node_t* filtered_root = NULL;
    char_node_t* char_list_head = NULL;
    char_node_t* curr;

    char word_to_find[k+1];
    read_input(word_to_find);

    char input[MAX_WORD_LENGTH];
    read_input(input);
    attempts = (unsigned)atoi(input);

    while(attempts > 0) {
        read_input(input);
         if(input[0] == '+') {    // if there is a command in input
            if(input[1] == 'i') {   // in the command is +inserisci_inizio
                if(!first_time) add_words_to_dictionary_and_filter(char_list_head, &filtered_root, word_root);
                else add_words_to_dictionary(word_root);
            } else {    // if the command is +stampa_filtrate
                if(!first_time) print_filtered(filtered_root);
                else print_filtered(*word_root);
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
                 if(!first_time) {
                     filtered_root = update_tree(filtered_root, char_list_head, &filtered_counter);
                 } else {
                     filtered_root = update_tree_first_time(*word_root, char_list_head, &filtered_counter);
                     first_time = false;
                 }

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

    unsigned short new_game = game_init(&word_root);

    if(new_game == 0) {
        main_game(&word_root);
    } else if(new_game == 1) {
        add_words_to_dictionary(&word_root);
    } else {
        return 0;
    }
    while(fgets(command, MAX_WORD_LENGTH, stdin) != NULL) {
        if(command[0] == '+') { // if there is a command in input
            if(command[1] == 'n') { // if the command is +nuova_partita
                main_game(&word_root);
            } else if(command[1] == 'i') {  // if the command is +inserisci_inizio
                add_words_to_dictionary(&word_root);
            }
        }
    }

    rb_destroy(word_root);
    return 0;
}
