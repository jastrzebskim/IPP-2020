/** @file
 * Implememntacja funkcji odpowiedzialnych za tworzenie stringa, 
 * przechowującego obecny stan planszy.
 *
 * @author Mateusz Jastrzębski <m.jastrzebsk28@student.uw.edu.pl>
 * @date 16.04.2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chboard.h"

uint32_t int_len(uint32_t number) {
    uint32_t counter = 0;
    if (number == 0)
        return 1;
    
    while (number > 0) {
        number /= 10;
        counter++;
    }

    return counter;
}

uint32_t max_f(uint32_t a, uint32_t b) {
    if (a > b)
        return a;
    else
        return b;
}

char *board_max_9(gamma_t *g) {
    uint32_t charac;
    uint32_t ptr = 0;
    uint64_t board_size = g->b_width * g->b_height + g->b_height + 1;

    char *word_ptr = malloc(board_size * sizeof(char));

    if (word_ptr == NULL)
        return NULL;

    
    for (long long i = g->b_height - 1; i >= 0; i--) {
        for (uint32_t j = 0; j < g->b_width; j++) {
            if (g->board[j][i] == 0) {
                word_ptr[ptr] = '.';
                ptr++;
            }
            else {
                charac = g->board[j][i];
                word_ptr[ptr] = (char)((charac % 10) + 48);
                ptr++;
            }

            if (j == g->b_width - 1) {
                word_ptr[ptr] = '\n';
                ptr++;
            }
        }
    }
    word_ptr[board_size - 1] = '\0';
    return word_ptr;
}

char *board_more_than_9(gamma_t *g) {
    uint32_t max_num_len = int_len(g->b_players);
    uint32_t ptr = 0;
    uint32_t previous_num = 0;

    uint32_t board_size = g->b_width * g->b_height * (max_num_len + 1) + 1
    + g->b_height;

    char *word_ptr = malloc(board_size * sizeof(char));
    if (word_ptr == NULL)
        return NULL;

    for (int i = g->b_height - 1; i >= 0; i--) {
        for (uint32_t j = 0; j < g->b_width; j++) {
            if (g->board[j][i] == 0) {
                for (uint32_t k = 0; k < max_num_len; k++) {
                    word_ptr[ptr] = ' ';
                    ptr++;
                }
                word_ptr[ptr] = '.';
                ptr++;
            }
            else if (g->board[j][i] > 0) {
                previous_num = g->board[j][i];
                word_ptr[ptr] = ' ';
                ptr += max_num_len;
                for (uint32_t k = 0; k < max_num_len; k++) {
                    if (previous_num > 0) {
                        word_ptr[ptr] = (char)((previous_num % 10) + 48);
                        ptr--;
                        previous_num /= 10;
                    }
                    else {
                        word_ptr[ptr] = ' ';
                        ptr--;
                    }
                }
                ptr += max_num_len + 1;                
            }

            if (j == g->b_width - 1) {
                    word_ptr[ptr] = '\n';
                    ptr++;
            }
        }
    }
    word_ptr[board_size - 1] = '\0';
    return word_ptr;
}