/** @file
 * Implementacja funkcji pomocniczych do wykonania ruchów na planszy.
 *
 * @author Mateusz Jastrzębski <m.jastrzebsk28@student.uw.edu.pl>
 * @date 16.04.2020
 */

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "moves.h"

uint32_t **create_new_arr(gamma_t *g) {
    uint32_t **check_array;
    check_array = malloc(g->b_width * sizeof(uint32_t *));
    if (check_array == NULL)
        return NULL;
    
    for (uint32_t i = 0; i < g->b_width; i++) {
        check_array[i] = calloc(g->b_height, sizeof(uint32_t));

        if (check_array[i] == NULL) {
            for (uint32_t j = 0; j < i; j++)
                free(check_array[i]);
                
            free(check_array);

            return NULL;
        }
    }

    return check_array;
}

void free_arr(gamma_t *g, uint32_t **b_arr) {
    for (uint32_t i = 0; i < g->b_width; i++) {
        free(b_arr[i]);
    }
    free(b_arr);
}

bool make_help_arr(gamma_t *g, uint32_t player_arr[][2], uint32_t player_arr_2[][2],
                    uint32_t ***check_arr, uint32_t x, uint32_t y) {
    for (int i = 0 ; i < ADJACENT_FIELDS; i++) {
        player_arr[i][0] = 0;
        player_arr[i][1] = 0;
        player_arr_2[i][0] = 0;
        player_arr_2[i][1] = 0;
    }
    if (x > 0 && g->board[x - 1][y] > 0) {
        player_arr[0][0] = g->board[x - 1][y];
        player_arr_2[0][0] = g->board[x - 1][y];
    }
    if (x < g->b_width - 1 && g->board[x + 1][y] > 0) {
        player_arr[1][0] = g->board[x + 1][y];
        player_arr_2[1][0] = g->board[x + 1][y];
        // sprawdzanie, czy numer gracza nie został wcześniej pobrany
        if (player_arr[0][0] == player_arr[1][0]) {
            player_arr[1][0] = 0;
            player_arr_2[1][0] = 0;
        }
    }
    if (y > 0 && g->board[x][y - 1] > 0) {
        player_arr[2][0] = g->board[x][y - 1];
        player_arr_2[2][0] = g->board[x][y - 1];
       // sprawdzanie, czy numer gracza nie został wcześniej pobrany 
        if (player_arr[0][0] == player_arr[2][0] ||
            player_arr[1][0] == player_arr[2][0]) {
            player_arr[2][0] = 0;
            player_arr_2[2][0] = 0;
        }

    }
    if (y < g->b_height - 1 && g->board[x][y + 1] > 0) {
        player_arr[3][0] = g->board[x][y + 1];
        player_arr_2[3][0] = g->board[x][y + 1];
        // sprawdzanie, czy numer gracza nie został wcześniej pobrany
        if (player_arr[0][0] == player_arr[3][0] ||
            player_arr[1][0] == player_arr[3][0] ||
            player_arr[2][0] == player_arr[3][0]) {
            player_arr[3][0] = 0;
            player_arr_2[3][0] = 0;
        }
    }

    *check_arr = create_new_arr(g);
    if ((*check_arr) == NULL) {
        return false;
    }

    return true;
}

bool check_move_parameters(gamma_t *g, uint32_t player, uint32_t x,
                            uint32_t y) {
    if (g == NULL) {
        return false;
    }
    if (player < 1 || player > g->b_players) {
        return false;
    }
    if (x > g->b_width - 1 || y > g->b_height - 1) {
        return false;
    }
    if (g->board[x][y] != 0) {
        return false;
    }

    return true;
    
}

/**
 * @brief Oznacza zbiór sąsiadujących ze sobą pól danego gracza.
 * Podstawą funkcji jest algorytm BFS.
 * @param[in] g - wskaźnik na strukturę, przechowującą stan gry.
 * @param[in] x - pierwsza współrzędna pola startowego. 
 * @param[in] y - druga współrzędna pola startowego.
 * @param[in] check_arr - tablica pól odwiedzonych.
 * @return @p True jeśli udało się bezproblemowo zaalokować pamięć na kolejkę,
 *         @p false w przeciwnym wypadku.
 */
static bool mark_set(gamma_t *g, uint32_t x, uint32_t y, uint32_t **check_arr) {
    TQueue *field_queue = init();
    if (field_queue == NULL) {
        return false;
    }

    push_q(field_queue, x, y);

    uint32_t x_tmp, y_tmp;

    while (!empty_q(field_queue)) {
        pop_q(field_queue, &x_tmp, &y_tmp);
        check_arr[x_tmp][y_tmp] = 1;

        if (x_tmp > 0 &&
            g->board[x_tmp - 1][y_tmp] == g->board[x_tmp][y_tmp] &&
            check_arr[x_tmp - 1][y_tmp] == 0) {
            if (!push_q(field_queue, x_tmp - 1, y_tmp)) {
                del_queue(field_queue);
                return false;
            }
        }
        if (x_tmp < g->b_width - 1 &&
            g->board[x_tmp + 1][y_tmp] == g->board[x_tmp][y_tmp] &&
            check_arr[x_tmp + 1][y_tmp] == 0) {
            if (!push_q(field_queue, x_tmp + 1, y_tmp)) {
                del_queue(field_queue);
                return false;
            }
        }
        if (y_tmp > 0 &&
            g->board[x_tmp][y_tmp - 1] == g->board[x_tmp][y_tmp] &&
            check_arr[x_tmp][y_tmp - 1] == 0) {
            if (!push_q(field_queue, x_tmp, y_tmp - 1)) {
                del_queue(field_queue);
                return false;
            }
        }
        if (y_tmp < g->b_height - 1 &&
            g->board[x_tmp][y_tmp + 1] == g->board[x_tmp][y_tmp] &&
            check_arr[x_tmp][y_tmp + 1] == 0) {
            if (!push_q(field_queue, x_tmp, y_tmp + 1)) {
                del_queue(field_queue);
                return false;
            }
        }
    }

    del_queue(field_queue);
    return true;
}

/**
 * @brief Szuka liczby spójnych obszarów danego gracza wokół pola (@p x, @p y).
 * @param[in] g - wskaźnik na strukturę, przechowującą stan gry.
 * @param[in] x - pierwsza współrzędna pola, wokół którego szukane są liczby
 *            spójnych obszarów.
 * @param[in] y - druga współrzędna pola, wokół którego szukane są liczby
 *            spójnych obszarów.
 * @param[in] check_arr - tablica, trzymająca stan pól na planszy
 *                        (odwiedzony, nieodwiedzony).
 * @param[out] player_arr - tablica trzymająca liczbę rożnych obszarów zajętych
 *                      przez danego gracza wokół pola (@p x, @p y).
 *                     
 * @return @p True gdy udało się zaalokować pamięć w funkcji mark_set(...),
 *         @p false w przeciwnym przypadku.
 */
static bool find_sets(gamma_t *g, uint32_t x, uint32_t y, uint32_t **check_arr,
                uint32_t player_arr[][2]) {
    if (x > 0 && g->board[x - 1][y] > 0) {
        if (!mark_set(g, x - 1, y, check_arr)) {
            return false;
        }
        for (int i = 0; i < ADJACENT_FIELDS; i++) {
            if (player_arr[i][0] == g->board[x - 1][y])
                player_arr[i][1]++;
        }
    }
    if (x < g->b_width - 1 && g->board[x + 1][y] > 0 &&
        check_arr[x + 1][y] == 0) {
        if (!mark_set(g, x + 1, y, check_arr)) {
            return false;
        }
        for (int i = 0; i < ADJACENT_FIELDS; i++) {
            if (player_arr[i][0] == g->board[x + 1][y])
                player_arr[i][1]++;
        }
    }
    if (y > 0 && g->board[x][y - 1] > 0 &&
        check_arr[x][y - 1] == 0) {
        if (!mark_set(g, x, y - 1, check_arr)) {
            return false;
        }
        for (int i = 0; i < ADJACENT_FIELDS; i++) {
            if (player_arr[i][0] == g->board[x][y - 1])
                player_arr[i][1]++;
        }
    }
    if (y < g->b_height - 1 && g->board[x][y + 1] > 0 &&
        check_arr[x][y + 1] == 0) {
        for (int i = 0; i < ADJACENT_FIELDS; i++) {
            if (player_arr[i][0] == g->board[x][y + 1])
                player_arr[i][1]++;
        }
    }
    
    return true;
}

bool check_neighbors(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if (x > 0 && g->board[x - 1][y] == player) {
        return true;
    }
    if (x < g->b_width - 1 && g->board[x + 1][y] == player) {
        return true;
    }
    if (y < g->b_height - 1 && g->board[x][y + 1] == player) {
        return true;
    }
    if (y > 0 && g->board[x][y - 1] == player) {
        return true;
    }

    return false;
}

bool find_sets_of_player(gamma_t *g, uint32_t player, uint32_t x, uint32_t y,
                            uint32_t **check_arr, uint32_t *counter) {
    if (x > 0 && g->board[x - 1][y] == player) {
        if (!mark_set(g, x - 1, y, check_arr)) {
            return false;
        }
        (*counter)++;
    }
    if (x < g->b_width - 1 && g->board[x + 1][y] == player &&
        check_arr[x + 1][y] == 0) {
        if (!mark_set(g, x + 1, y, check_arr)) {
            return false;
        }
        (*counter)++;
    }
    if (y > 0 && g->board[x][y - 1] == player &&
        check_arr[x][y - 1] == 0) {
        if (!mark_set(g, x, y - 1, check_arr)) {
            return false;
        }
        (*counter)++;
    }
    if (y < g->b_height - 1 && g->board[x][y + 1] == player &&
        check_arr[x][y + 1] == 0) {
        (*counter)++;
    }
    
    return true;
}

bool check_golden_parameters(gamma_t *g, uint32_t player, uint32_t x,
                            uint32_t y) {
    if (g == NULL)
        return false;
    if (player < 1 || player > g->b_players)
        return false;
    if (x > g->b_width - 1)
        return false;
    if (y > g->b_height - 1)
        return false;
    if (g->board[x][y] == 0)
        return false;
    if (g->golden_move_available[player - 1] == false)
        return false;
    if (g->board[x][y] == player)
        return false;

    return true;
}

bool find_and_mark_sets(gamma_t *g, uint32_t player, uint32_t x, uint32_t y,
                        uint32_t player_arr[][2], uint32_t player_arr_2[][2],
                        uint32_t **check_arr) {
    if (!find_sets(g, x, y, check_arr, player_arr)) { 
        free_arr(g, check_arr);
        return false;
    }

    for (uint32_t i = 0; i < g->b_width; i++) {
        for (uint32_t j = 0; j < g->b_height; j++) {
            check_arr[i][j] = 0;
        }
    }
    uint32_t prev_player = g->board[x][y];
    g->board[x][y] = player;

    if (!find_sets(g, x, y, check_arr, player_arr_2)) {
        g->board[x][y] = prev_player;
        free_arr(g, check_arr);
        return false;
    }

    return true;
}

void change_busy_areas(gamma_t *g, uint32_t player, uint32_t player_arr[][2]) {
    for (uint32_t i = 0; i < ADJACENT_FIELDS; i++) {
        if (player_arr[i][0] != player && player_arr[i][0] > 0) {
            g->busy_areas[player_arr[i][0] - 1] += player_arr[i][1];
        }
    }
}

bool check_busy_areas(gamma_t *g, uint32_t player, uint32_t x, uint32_t y,
                        uint32_t player_arr[][2], uint32_t player_arr_2[][2],
                        uint32_t original_player, uint32_t **check_arr) {
    for (uint32_t i = 0; i < ADJACENT_FIELDS; i++) {
        if (player_arr[i][0] != player)
            player_arr[i][1] = player_arr_2[i][1] - player_arr[i][1];
        if (player_arr[i][0] != player && player_arr[i][0] > 0 &&
            g->busy_areas[player_arr[i][0] - 1] + player_arr[i][1] 
            > g->areas_limit) {
            g->board[x][y] = original_player;

            free_arr(g, check_arr);
            return false;
        }
    }

    return true;
}

