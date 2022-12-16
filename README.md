# WordChecker
WordChecker - A Wordle like Guessing Game written in C.

WordChecker is the final project of Algorithms and Data Structures course at Polytechnic University of Milan.

---

# Specification

The goal of this project is to build a system that, at its heart, checks the correspondence between the letters of 2 words of equallength.

Words are intended as sequences of symbols which can be lowercase (az) or uppercase (AZ) alphabetic characters, numerical digits (0-9), or the symbols - (hyphen) and _ ("underscore"). An example of a 20 symbols word can be: djHD1af9fj7g__l-ssOP.

The system reads a sequence of information and instructions from standard input, and produces output strings depending on the case. More precisely, the system
reads: 

- a value k, which indicates the length of the words, 
- a sequence (of arbitrary length) of words  each of length k, which constitutes the set of admissible words

Also assume that the sequence of words does not contain duplicates.

At that point, a sequence of "games" is read from standard input, where the start of each new game is marked by the command (always read from input) `+nuova_partita`

The sequences of input strings for each match (following the command `+nuova_partita`) are done in the following way:

- reference word (of k characters lenght), assume that the reference word belongs to the set of eligible words;
- Maximum n number of words to compare with the reference word;
- Sequence of words (each of k characters) to be compared with the reference word;
- Occasionally, in the sequence of input strings, may appear the command: `+stampa_filtrate`, the effect of this command is explained below;
- In addition, both during a game and between games, the `+inserisci_inizio` and `+inserisci_fine` commands may appear that enclose between them a sequence of new words to be added to the set of eligible words;
- added words are also of length k, and it is always assumed that there are no duplicate words (not even with respect to words already in the set of eligible words).

For each word read (which we denote in the following as p), to be compared with the reference word (which we denote in the following as r), the program writes to standard output a sequence of k characters done in the following manner. <br />
We denote by p[1], p[2], ... p[k] the characters of the word p, by r[1], r[2], ... r[k] those of the word r, and by res[1], res[2], ... res[k] those of the printed sequence.

For any 1 ≤ i ≤ k, we have that:
- res[i] is the `+` character if the i-th character of p is equal to the i-th character of r (if it is true that p[i] = r[i], then p[i] is "in the correct position");
- res[i] is the character `/` if the i-th character of p appears nowhere in r;
- res[i] is the `|` character if the i-th character of p (denoted, in the following, as p[i]) appears in r, but not in the i-th position; however, if n_i instances of p[i] appear in r, if c_i is the number of instances of the character p[i] that are in the correct position (clearly it must hold that c_i ≤ n_i) and if there are before the i-th character in p at least ni-th characters equal to p[i] that are in the incorrect position, then res[i] must be `/` instead of `|`.

Examples of comparisons (where the first line is the reference word r, the second is p, and the third is the output res):

- `djPDi939-s__e-s` <br />
`gioSON-we2_w234` <br />
`/|////|/|/+//|/` 
- `djPDi939-s__e-s` <br />
`kiidsa92KFaa94-` <br />
`/|/||/|/////|/|`
- `djPDi939-s__e-s` <br />
`ewi-n4wp-sesr-v` <br />
`|/|/////++/|/+/`
- `DIk834k249kaoe_` <br />
`48kDkkkf-saancd` <br />
`||+||/+////+///`

If a word is read from standard input that does not belong to the set of eligible words, the program writes to standard output the string `not_exists`.

If, on the other hand, the word r is read (i.e., if p = r), then the program writes `ok` (without printing the detailed result of the comparison) and the match ends.

If, after reading n eligible words (with n, remember, being the maximum number of words to compare with r), none of them were equal to r, the program writes `ko` (after printing the result of the last word comparison), and the game ends.

After the game is over:
- There may be no other words to compare (but there may be the inclusion of new eligible words)
- If there is the command `+nuova_partita` in input, a new game starts

When, during a match, the +stampa_filtrate command is read from input, the program should output, in lexicographical order, the set of allowable words that are compatible with the constraints learned so far in the match, written one per line. <br />
Note that the learned constraints concern, for each symbol:

1. If the symbol does not belong to r;
2. places where that symbol must appear in r;
3. Places where that symbol cannot appear in r;
4. Minimum number of times the symbol appears in r;
5. Exact number of times the symbol appears in r (note that constraint 5 is stronger than constraint 4).

Symbol order (used to establish the lexicographic order of words) is as specified by the ASCII standard.

In addition, after each comparison, the program must output the number of eligible words still compatible with the learned constraints except in the case of a comparison with a `not_exists` outcome.

