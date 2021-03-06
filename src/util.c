#include "util.h"
#include "defs.h"
#include <ctype.h>
#include <libtcod.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int is_alpha(int c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }

int is_num(int c) { return c >= '0' && c <= '9'; }

/* isalnum() does not work properly with ncurses keys */
int is_alphanum(int c) { return is_alpha(c) || is_num(c); }

/* Example: printf("You see here %s %s!\n", a_or_an(name), name);
 */
int _a_or_an(const char *s) {
    switch (s[0]) {
    case 'a':
    case 'e':
    case 'i':
    case 'o':
    case 'u':
        return 0;

    default:
        return 1;
    }
}

void capitalize(char *s) { s[0] = toupper(s[0]); }

/* Input: arg count, strings to be concatenated.
 * Returns: pointer to string (should be freed)
 */
char *string_create(uint argc, ...) {
    va_list args;
    uint j;
    uint len = 0;
    char *buf;

    va_start(args, argc);
    for (j = 0; j < argc; j++) {
        len += strlen(va_arg(args, char *));
    }
    va_end(args);

    buf = malloc(len + 1);

    va_start(args, argc);
    strcpy(buf, va_arg(args, char *));
    for (j = 1; j < argc; j++) {
        strcat(buf, va_arg(args, char *));
    }
    va_end(args);

    return buf;
}

char *subject_form(char *article, uint num, char *subject) {
    char *art;
    char *plur;
    char *ret;

    if (strcmp(article, "you") == 0) {
        return string_create(1, "you"); /* return a malloc'd string */
    }

    if (num == 1) {
        art = article;
    } else {
        art = malloc(16);
        inttostr(art, num, 10);
    }

    plur = (num > 1) ? "s" : "";

    if (strcmp(art, "") == 0) {
        ret = string_create(2, subject, plur);
    } else {
        ret = string_create(4, art, " ", subject, plur);
    }

    if (num != 1) {
        free(art);
    }

    return ret;
}

char *sentence_form(char *article1, uint num1, char *subject, char *verb_sing,
                    char *verb_plur, char *article2, uint num2, char *object) {
    char *art1;
    char *art2;
    char *ret;

    art1 = subject_form(article1, num1, subject);
    art2 = subject_form(article2, num2, object);

    if (strcmp(article1, "you") == 0 || num1 > 1) {
        ret = string_create(5, art1, " ", verb_plur, " ", art2);
    } else {
        ret = string_create(5, art1, " ", verb_sing, " ", art2);
    }

    free(art1);
    free(art2);
    capitalize(ret);
    return ret;
}

/* Got this from stack overflow */
char *str_copy(const char *s) {
    char *d = malloc(strlen(s) + 1); /* Space for length plus nul */

    if (d == NULL) {
        return NULL; /* No memory */
    }

    strcpy(d, s); /* Copy the characters */
    return d;     /* Return the new string */
}

/* Returns random integer in [a, b] */
int rand_int(int a, int b) {
    return TCOD_random_get_int(NULL, MIN(a, b), MAX(a, b));
}

uint rand_unsigned_int(uint a, uint b) {
    return (uint) TCOD_random_get_int(NULL, (int) MIN(a, b), (int) MAX(a, b));
}

double rand_float(double a, double b) {
    return TCOD_random_get_double(NULL, MIN(a, b), MAX(a, b));
}

/* Converts an integer input to its representation in the provided base. Assumes
 * non-negative input and base <= 16.
 */
uint inttostr(char *buffer, unsigned long i, uint base) {
    char *hex_chars = "0123456789abcdef";
    char result[16];
    char result2[16] = "";

    if (i == 0) {
        strcpy(buffer, "0");
        return 0;
    }

    while (i > 0) {
        strncpy(result, hex_chars + (i % base), 1);
        result[1] = '\0';
        strcat(result, result2);
        strcpy(result2, result);
        i /= base;
    }

    strcpy(buffer, result);
    return 0;
}

/* Length of a non-negative integer */
uint intlen(uint a) {
    uint i = 1;

    a /= 10;
    while (a) {
        a /= 10, i++;
    }

    return i;
}

/* Builds up a name from pre-defined sequences of letters. Highly
   configurable in terms of which sequences are possible. Remember to
   FREE the string after you're done with it.
 */
char *name_gen() {
    /* Define the list of consonant, double consonant, etc. sequences */
    const char *consonants = "bdfghklmnprsstv";
    const char *start_dconsonants = "brchcltrthdrslwhphblcrfrwrgrstqu";
    const char *dconsonants = "brchclcttrthghdrmmttllstqu";
    const char *end_dconsonants = "chctthghstrm";
    const char *tconsonants = "strthrchr";
    const char *vowels = "aaeeiioou";
    const char *dvowels = "ioiaai";
    char *word = calloc(MAX_NAME_LEN + 1, 1);
    uint c;
    uint m;
    uint cons_i = strlen(consonants);
    uint st_dcons_i = strlen(start_dconsonants) / 2;
    uint dcons_i = strlen(dconsonants) / 2;
    uint e_dcons_i = strlen(end_dconsonants) / 2;
    uint tcons_i = strlen(tconsonants) / 3;
    uint vow_i = strlen(vowels);
    uint dvow_i = strlen(dvowels) / 2;
    uint vowel = (rand_float(0, 1) < .33) ? 1 : 0;

    /* Choose initial sequence of letters */
    if (vowel) {
        strncpy(word, vowels + rand_unsigned_int(0, strlen(vowels) - 1), 1);
    } else {
        /* 'w', 'j', and 'z' can only appear at the beginning of a name */
        switch (rand_unsigned_int(1, 20)) {
        case 1:
            strcpy(word, "w");
            break;

        case 2:
            strcpy(word, "j");
            break;

        case 3:
            strcpy(word, "z");
            break;

        default:
            c = rand_unsigned_int(0, cons_i + st_dcons_i + tcons_i - 1);
            if (c < cons_i) {
                strncpy(word, consonants + c, 1);
            } else if (c < cons_i + st_dcons_i) {
                strncpy(word, start_dconsonants + 2 * (c - cons_i), 2);
            } else {
                strncpy(word, tconsonants + 3 * (c - cons_i - st_dcons_i), 3);
            }
        }
    }

    /* Alternate between choosing vowel and consonant sequences */
    m = rand_unsigned_int(2, 5);
    while (m-- > 0 && strlen(word) < MAX_NAME_LEN - 3) {
        if ((vowel = !vowel)) {
            c = rand_unsigned_int(0, vow_i + dvow_i - 1);
            if (c < vow_i) {
                strncat(word, vowels + c, 1);
            } else {
                strncat(word, dvowels + 2 * (c - vow_i), 2);
            }
        } else {
            if (m <= 0) {
                c = rand_unsigned_int(0, cons_i + e_dcons_i - 1);
            } else {
                c = rand_unsigned_int(0, cons_i + dcons_i + tcons_i - 1);
            }

            if (c < cons_i) {
                strncat(word, consonants + c, 1);
            } else if (c < cons_i + st_dcons_i) {
                if (m <= 0) {
                    strncat(word, end_dconsonants + 2 * (c - cons_i), 2);
                } else {
                    strncat(word, consonants + 2 * (c - cons_i), 2);
                }
            } else {
                strncat(word, tconsonants + 3 * (c - cons_i - dcons_i), 3);
            }
        }
    }

    capitalize(word);
    return word;
}

/* Moves the coordinates (y, x) in direction DIR by amount amt, and stores the
 * new coordinates in (new_y, new_x)
 */
void move_dir(int y, int x, int *new_y, int *new_x, DIRECTION dir, uint amt) {
    int temp_x, temp_y;

    switch (dir) {
    case DIR_N:
        temp_y = y - amt;
        temp_x = x;
        break;

    case DIR_E:
        temp_y = y;
        temp_x = x + amt;
        break;

    case DIR_S:
        temp_y = y + amt;
        temp_x = x;
        break;

    case DIR_W:
        temp_y = y;
        temp_x = x - amt;
        break;

    case DIR_NE:
        temp_y = y - amt;
        temp_x = x + amt;
        break;

    case DIR_SE:
        temp_y = y + amt;
        temp_x = x + amt;
        break;

    case DIR_SW:
        temp_y = y + amt;
        temp_x = x - amt;
        break;

    case DIR_NW:
        temp_y = y - amt;
        temp_x = x - amt;
        break;

    default:
        temp_y = y;
        temp_x = x;
        break;
    }

    if (new_y != NULL)
        *new_y = temp_y;
    if (new_x != NULL)
        *new_x = temp_x;
}

DIRECTION random_dir() { return rand_unsigned_int(DIR_NONE, DIR_END - 1); }
