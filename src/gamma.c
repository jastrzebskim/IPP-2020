/**
 * @file
 * Implementacja funkcji odpowiedzialnych za przeprowadzenie gry gamma.
 * 
 * @author Mateusz Jastrzębski <m.jastrzebsk28@student.uw.edu.pl>
 * @date 17.05.2020
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "moves.h"
#include "chboard.h"
#include "gamma.h"

/**
 * @brief Funkcja alokuje pamięć potrzebną do przeprowadzenia rozgrywki.
 * Alokuje pamięć dla: @p g->busy_areas, @p g->golden_move_available,
 * @p g->board.
 * @param[in, out] g - wskaźnik na strukturę, która ma za zadanie przechowywać
 *                     stan gry.
 * @return Jeśli alokacja pamięci się powiodła, wskaźnik na strukturę @p g,
 *         w przeciwnym przypadku @p NULL.
 */
static gamma_t* allocate_memory(gamma_t *g) {
    g->busy_areas = calloc(g->b_players, sizeof(uint32_t));

    if (g->busy_areas == NULL) {
        free(g);
        return NULL;
    }

    g->golden_move_available = malloc(g->b_players * sizeof(bool));

    if (g->golden_move_available == NULL) {
        free(g->busy_areas);
        free(g);
        return NULL;
    }

    g->board = malloc(g->b_width * sizeof(uint32_t *));
    if (g->board == NULL) {
        free(g->golden_move_available);
        free(g->busy_areas);
        free(g);
        return NULL;
    }
    
    for (uint32_t i = 0; i < g->b_width; i++) {
        g->board[i] = calloc(g->b_height, sizeof(uint32_t));

        if (g->board[i] == NULL) {
            for (uint32_t j = 0; j <= i; j++) {
                free(g->board[j]);
            }
            free(g->board);
            free(g->golden_move_available);
            free(g->busy_areas);
            free(g);
            return NULL;
        }
    }

    return g;    
}

/**
 * @brief Funkcja nadaje początkową wartość polom tablicy 
 * @p g->golden_move_available. 
 * @param[in, out] g - wskaźnik na strukturę, przechowująca stan gry.
 * @return Wskaźnik na strukturę @p g.
 */
static gamma_t* initial_value(gamma_t *g) {

    for (uint32_t i = 0; i < g->b_players; i++) {
        g->golden_move_available[i] = true;
    }

    return g;
}

gamma_t* gamma_new(uint32_t width, uint32_t height,
                   uint32_t players, uint32_t areas) {
    if (width < 1 || height < 1 || players < 1 || areas < 1) {
        return NULL;
    }

    gamma_t *g = malloc(sizeof(gamma_t));

    if (g == NULL) {
        return NULL;
    }

    g->b_players = players;
    g->b_width = width;
    g->b_height = height;
    g->areas_limit = areas;

    g = allocate_memory(g);

    if (g == NULL) {
        return NULL;
    }

    initial_value(g);

    return g;
}

void gamma_delete(gamma_t *g) {
    if (g != NULL) {
        for (uint32_t i = 0; i < g->b_width; i++) {
            free(g->board[i]);
        }
        
        free(g->golden_move_available);
        free(g->busy_areas);
        free(g->board);
        free(g);
    }
}

bool gamma_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if (!check_move_parameters(g, player, x, y))
        return false;
    
    uint32_t **check_arr = create_new_arr(g);

    if (check_arr == NULL) {
        errno = ENOMEM;
        return false;
    }

    uint32_t counter = 0;
    uint32_t counter2 = 0;
    
    if (!find_sets_of_player(g, player, x, y, check_arr, &counter)) {
        errno = ENOMEM;
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

    if (!find_sets_of_player(g, player, x, y, check_arr, &counter2)) {
        errno = ENOMEM;
        free_arr(g, check_arr);
        return false;
    }

    g->board[x][y] = prev_player;

    if (!check_neighbors(g, player, x, y)) {
        if (g->busy_areas[player - 1] + 1 > g->areas_limit) {
            free_arr(g, check_arr);
            return false;
        }
        g->busy_areas[player - 1]++;
    }

    g->busy_areas[player - 1] -= counter - counter2;
    g->board[x][y] = player;

    free_arr(g, check_arr);
    return true;
}

bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if (!check_golden_parameters(g, player, x, y))
        return false;
    uint32_t prev_player = g->board[x][y];
    uint32_t player_arr[ADJACENT_FIELDS][2], player_arr_2[ADJACENT_FIELDS][2];
    uint32_t **check_arr;
    if (!make_help_arr(g, player_arr, player_arr_2, &check_arr, x , y)) {
        errno = ENOMEM;
        return false;
    }
    
    if (!find_and_mark_sets(g, player, x, y, player_arr, player_arr_2,
        check_arr)) {
        errno = ENOMEM;
        return false;
    }

    if (!check_busy_areas(g, player, x, y, player_arr, player_arr_2,
        prev_player, check_arr)) {
        return false;
    }

    if (!check_neighbors(g, player, x, y)) {
        if (g->busy_areas[player - 1] + 1 > g->areas_limit) {
            g->board[x][y] = prev_player;
            free_arr(g, check_arr);
            return false;
        }

        g->busy_areas[player - 1] += 1;
    }

    if (!check_neighbors(g, prev_player, x, y)) {
        g->busy_areas[prev_player - 1] -= 1;
    }

    change_busy_areas(g, player, player_arr);

    uint32_t a = 0, b = 0;
    for (int i = 0; i < ADJACENT_FIELDS; i++) {
        if (player_arr[i][0] == player) {
            a = player_arr[i][1];
            b = player_arr_2[i][1];
        }
    }
    g->golden_move_available[player - 1] = false;
    g->busy_areas[player - 1] -= a - b;

    free_arr(g, check_arr);
    return true;
}

uint64_t gamma_busy_fields(gamma_t *g, uint32_t player) {
    if (g == NULL || player > g->b_players || player < 1)
        return 0;

    uint64_t counter = 0;

    for (uint32_t i = 0; i < g->b_width; i++) {
        for (uint32_t j = 0; j < g->b_height; j++) {
            if (g->board[i][j] == player) {
                counter ++;
            }
        }
    }

    return counter;
}

uint64_t gamma_free_fields(gamma_t *g, uint32_t player) {
    if (g == NULL)
        return 0;
    if (player > g->b_players || player < 1)
        return 0;

    uint64_t counter = 0;
    bool in_area = false;

    for (uint32_t i = 0; i < g->b_width; i++) {
        for (uint32_t j = 0; j < g->b_height; j++) {
            in_area = false;

            if (g->busy_areas[player - 1] >= g->areas_limit &&
                g->board[i][j] == 0) {
                if (i > 0 && g->board[i - 1][j] == player)
                    in_area = true;
                if (i < g->b_width - 1 && g->board[i + 1][j] == player)
                    in_area = true;
                if (j > 0 && g->board[i][j - 1] == player)
                    in_area = true;
                if (j < g->b_height - 1 && g->board[i][j + 1] == player)
                    in_area = true;

                if (in_area)
                    counter++;
            }
            else {
                if (g->board[i][j] == 0)
                    counter++;
            }
        }
    }

    return counter;
}

/**
 * @brief Sprawdza poprawność parametrów funkcji oraz sprawdza, czy gracz
 * @p player nie wykonał wcześnej złotego ruchu. 
 * @param[in] g - wskaźnik na strukturę przechowującą stan gry.
 * @param[in] player - numer gracza.
 * @return @p true, gdy parametry są poprawne oraz gracz @p player nie wykonał
 * wcześniej złotego ruchu, @p false w przeciwnym przypadku.
 */
static bool golden_possible_parameters(gamma_t *g, uint32_t player) {
    if (g == NULL)
        return false;
    if (player < 1 || player > g->b_players)
        return false;
    if (g->golden_move_available[player - 1] == false)
        return false;
        
    return true;    
}

/**
 * @brief Sprawdza, czy gracz @p player stawiając pionek na polu (@p x, @p y)
 * nie przekroczy limitu obszarów.
 * @param[in] g - wskaźnik na strukturę przechowującą stan gry.
 * @param[in] player - numer gracza
 * @param[in] x - pierwsza wspórzędna pola, na którym ma być postawiony pionek.
 * @param[in] y - druga wspórzędna pola, na którym ma być postawiony pionek.
 * @return @p true, gdy gracz @p player stawiając pionek na polu (@p x, @p y)
 * nie przekroczy limitu obszarów, @p false w przeciwnym przypadku.
 */
static bool check_limit_of_areas(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if (g->busy_areas[player - 1] < g->areas_limit) {
        return true;
    }
    else {
        if (x > 0 && g->board[x - 1][y] == player) 
            return true;
        else if (x < g->b_width - 1 &&
                g->board[x + 1][y] == player)
            return true;
        else if (y > 0 && g->board[x][y - 1] == player) 
            return true;
        else if (y < g->b_height - 1 && 
                g->board[x][y + 1] == player)
            return true;
    }

    return false;
}

/**
 * @brief Wypełnia dwuwymiarową tablicę o wymiarach @p g->b_width na
 * @p g->b_height zerami. 
 * @param[in] g - wskaźnik na strukturę przechowującą stan gry.
 * @param[in, out] array - tablica, która będzie wypełniona zerami.
 */
static void fill_array_with_zeros(gamma_t *g, uint32_t **array) {
    for (uint32_t i = 0; i < g->b_width; i++) {
        for (uint32_t j = 0; j < g->b_height; j++) {
            array[i][j] = 0;
        }
    }
}

bool gamma_golden_possible(gamma_t *g, uint32_t player) {
    if (golden_possible_parameters(g, player) == false) 
        return false;

    uint32_t **visited_fields = create_new_arr(g);
    if (visited_fields == NULL) {
        errno = ENOMEM;
        return false;
    }

    uint32_t counter_before = 0, counter_after = 0;
    uint32_t player_on_field; 
    
    for (uint32_t i = 0; i < g->b_width; i++) {
        for (uint32_t j = 0; j < g->b_height; j++) {
            player_on_field = g->board[i][j];
            if (g->board[i][j] > 0 && g->board[i][j] != player) {
                counter_before = 0;
                counter_after = 0;
                fill_array_with_zeros(g, visited_fields);
                if (find_sets_of_player(g, player_on_field, i, j,
                                visited_fields, &counter_before) == false) {
                    free_arr(g, visited_fields);
                    return false;
                 }

                g->board[i][j] = player;
                fill_array_with_zeros(g, visited_fields);
                if (find_sets_of_player(g, player_on_field, i, j,
                                    visited_fields, &counter_after) == false) {
                    free_arr(g, visited_fields);
                    return false;
                 }

                g->board[i][j] = player_on_field;

                if (counter_after - counter_before + 
                    g->busy_areas[player_on_field - 1] <= g->areas_limit
                    && check_limit_of_areas(g, player, i, j)) {
                    free_arr(g, visited_fields);
                    return true;
                }
            }
        }
    }
    free_arr(g, visited_fields);
    return false;
}

char *gamma_board(gamma_t *g) {
    if (g == NULL)
        return NULL;
    
    if (g->b_players < 10) {
        return board_max_9(g);
    }
    else {
        return board_more_than_9(g);
    }
}