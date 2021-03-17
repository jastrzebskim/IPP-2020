/** @file
 * Interfejs modułu zawierającego funkcje pomocnicze do wykonania ruchów 
 * na planszy.
 *
 * @author Mateusz Jastrzębski <m.jastrzebsk28@student.uw.edu.pl>
 * @date 16.04.2020
 */

#ifndef MOVES_H
#define MOVES_H

/**
 * Maksymalna liczba pól sąsiadujących.
 */
#define ADJACENT_FIELDS 4

#include <stdint.h>
#include <stdbool.h>
#include "gamma.h"

/**
 * @brief Tworzy nową tablicę o wymiarach @p g->b_width na @p g->b_height.
 * Wpisuje w każde pole w tej tablicy 0.
 * @param[in] g - wskaźnik na strukturę, przechowującą stan gry.
 * @return Wyzerowany wskaźnik na tablicę o wymiarach 
 *         @p g->b_width na @p g->b_height.
 */
extern uint32_t **create_new_arr(gamma_t* g);

/**
 * @brief Usuwa tablicę @p b_arr o dowolnej wielkości pierwszego wymiaru oraz o
 *        wielkości drugiego wymiaru równej @p g->b_height.
 * @param[in] g - wskaźnik na strukturę, przechowującą stan gry.
 * @param[in] b_arr - tablica, którą usuwa funkcja.
 */
extern void free_arr(gamma_t *g, uint32_t **b_arr);

/**
 * @brief Tworzy tablice pomocnicze dla funkcji gamma_golden_move(...).
 * 
 * @param[in] g - wskaźnik na strukturę, przechowującą stan gry.
 * @param[in, out] player_arr - tablica, która będzie zawierać numer gracza
 *                              oraz liczbę obszarów przez niego zajętych,
 *                              sąsiadujących z polem (@p x, @p y) przed 
 *                              wykonaniem złotego ruchu.
 * @param[in, out] player_arr_2 - tablica, która będzie zawierać numer gracza
 *                                oraz liczbę obszarów przez niego zajętych,
 *                                sąsiadujących z polem (@p x, @p y) po
 *                                wykonaniu złotego ruchu.
 * @param[in, out] check_arr - tablica, która będzie zawierać informację o tym,
 *                             czy dane pole było odwiedzone podczas oznaczanie
 *                             obszarów.
 * @param[in] x - pierwsza współrzędna pola, na którym ma być zrobiony złoty
 *                ruch.
 * @param[in] y - druga współrzędna pola, na którym ma być zrobiony złoty ruch.
 * @return @p True, gdy alokacja pamięci dla check_arr się powiedzie, @p false
 *         w przeciwnym przypadku.
 */
extern bool make_help_arr(gamma_t *g, uint32_t player_arr[][2],
            uint32_t player_arr_2[][2], uint32_t ***check_arr,
            uint32_t x, uint32_t y);

/**
 * @brief Sprawdza, czy podane parametry ruchu są poprawne.
 * @param[in] g - wskaźnik na strukturę, przechowującą stan gry.
 * @param[in] player - numer gracza, wykonującego ruch.
 * @param[in] x - pierwsza współrzędna pola, na które ma zostać postawiony pionek.
 * @param[in] y - druga współrzędna pola, na które ma zostać postawiony pionek.
 * @return @p True jeśli parametry są poprawne, @p false jeśli nie są poprawne. 
 */
extern bool check_move_parameters(gamma_t *g, uint32_t player, uint32_t x,
                            uint32_t y);

/**
 * @brief Sprawdza, czy pole (@p x, @p y) sąsiaduje z polem, na którym
 *        stoi pionek gracza @p player.
 * @param[in] g - wskaźnik na strukturę, przechowującą stan gry.
 * @param[in] player - numer gracza, którego pionków funkcja szuka na polach
 *                 sąsiadujących z polem (@p x, @p y).
 * @param[in] x - pierwsza współrzędna pola, którego pola sąsiadujące
 *            są sprawdzane przez funkcje.
 * @param[in] y - druga współrzędna pola, którego pola sąsiadujące
 *            są sprawdzane przez funkcje.
 * @return @p True, gdy z polem (@p x, @p y) sąsiaduje pole, na którym
 *         znajduje się pionek gracza @p player, @p false w przeciwnym
 *         przypadku.
 */
extern bool check_neighbors(gamma_t *g, uint32_t player, uint32_t x,
                            uint32_t y);

/**
 * @brief Sprawdza liczbę spójnych obszarów gracza @p player, sąsiadujących
 *        z polem (@p x, @p y).
 * @param[in] g - wskaźnik na strukturę, przechowującą stan gry.
 * @param[in] player - numer gracza, którego spójnych obszarów funkcja szuka
 *                 sąsiadujących z polem (@p x, @p y).
 * @param[in] x - pierwsza współrzędna pola, z którym sąsiądujących spójnych
 *                obszarów gracza @p player szuka funkcja.
 * @param[in] y - druga współrzędna pola, z którym sąsiądujących spójnych
 *                obszarów gracza @p player szuka funkcja.
 * @param[in] check_arr - tablica, trzymająca stan pól na planszy
 *                        (odwiedzony, nieodwiedzony).
 * @param[out] counter - wskaźnik na licznik spójnych obszarów gracza
 *                       @p player, sąsiadujących z polem (@p x, @p y).
 * @return @p True gdy udało się zaalokować pamięć w funkcji mark_set(...),
 *         @p false w przeciwnym przypadku.
 */
extern bool find_sets_of_player(gamma_t *g, uint32_t player, uint32_t x,
                        uint32_t y, uint32_t **check_arr, uint32_t *counter);

/**
 * @brief Sprawdza, czy podane parametry są poprawne.
 * Sprawdza, czy dla podanych parametrów można wykonać złoty ruch.
 * @param[in] g - wskaźnik na strukturę, przechowującą stan gry.
 * @param[in] player - numer gracza, który wykonuje ruch.
 * @param[in] x - pierwsza współrzędna pola, na którym ma zostać wykonany
 *            złoty ruch.
 * @param[in] y - druga współrzędna pola, na którym ma zostać wykonany
 *            złoty ruch.
 * @return @p True, gdy parametry są poprawne, @p false w przeciwnym przypadku.
 */
extern bool check_golden_parameters(gamma_t *g, uint32_t player, uint32_t x,
                            uint32_t y);

/**
 * @brief Uzupełnia tablice @p player_arr oraz @p player_arr_2 o
 *        liczby należących do kolejnych graczy spójnych obszarów, 
 *        sąsiadujących z polem (@p x, @p y).
 * @param[in] g - wskaźnik na strukturę, przechowującą stan gry.
 * @param[in] player - numer gracza, wykonującego złoty ruch.
 * @param[in] x - pierwsza współrzędna pola, na którym wykonywany jest
 *                złoty ruch.
 * @param[in] y - druga współrzędna pola, na którym wykonywany jest
 *                złoty ruch.
 * @param[out] player_arr - tablica, trzymająca liczbę należących do kolejnych
 *                          graczy spójnych obszarów przed wykonaniem
 *                          złotego ruchu.
 * @param[out] player_arr_2 - tablica, trzymająca liczbę należących do 
 *                            kolejnych graczy spójnych obszarów po 
 *                            wykonaniu złotgo ruchu.
 * @param[in] check_arr - tablica, trzymająca stan pól na planszy
 *                        (odwiedzony, nieodwiedzony).
 * @return @p true, jeśli udało się zaalokować pamięć w funkcji mark_set(...),
 *         @p false w przeciwnym przypadku. 
 */
extern bool find_and_mark_sets(gamma_t *g, uint32_t player, uint32_t x,
                            uint32_t y, uint32_t player_arr[][2],
                            uint32_t player_arr_2[][2], uint32_t **check_arr);

/**
 * @brief Zmienia liczbę zajętych obszarów przez graczy, których obszary
 *        sąsiadują z polem, na którym wykonywany jest złoty ruch.
 * @param[in, out] g - wskaźnik na strukturę, przechowującą stan gry.
 * @param[in] player - gracz, który wykonuje złoty ruch.
 * @param[in] player_arr - tablica liczby obszarów, które należy dodać do
 *                         obecnej liczby zajętych przez danego gracza
 *                         obszarów.
 */
extern void change_busy_areas(gamma_t *g, uint32_t player, 
                                uint32_t player_arr[][2]);

/**
 * @brief Sprawdza, czy złoty ruch nie sprawi, że gracze, którzy mają obszar
 *        sąsiadujący z polem (@p x, @p y), przekroczą limit obszarów.
 * @param[in, out] g - wskaźnik na strukturę, przechowującą stan gry.
 * @param[in] player - gracz, wykonujący złoty ruch.
 * @param x - pierwsza współrzędna pola, na którym wykonywany jest
 *            złoty ruch.
 * @param[in] y - druga współrzędna pola, na którym wykonywany jest
 *                złoty ruch.
 * @param[in, out] player_arr - tablica liczby obszarów danego gracza,
 *                              sąsiadujących z polem (@p x, @p y) przed
 *                              wykonaniem złotego ruchu.
 * @param[in] player_arr_2 - tablica liczby obszarów danego gracza,
 *                           sąsiadujących z polem (@p x, @p y) przed
 *                           wykonaniem złotego ruchu.
 * @param[in] tmp - numer gracza, który zajmował pole (@p x, @p y) przed
 *                  wykonaniem złotego ruchu.
 * @param[in] check_arr - tablica, trzymająca stan pól
 *                        (odwiedzone, nieodwiedzone).
 * @return @p True, gdy złoty ruch nie spowoduje przekroczenie limitu
 *         zajętych obszarów przez żadnego z graczy, z wyjątkiem gracza
 *         @p player, @p false w przeciwnym przypadku.
 */
extern bool check_busy_areas(gamma_t *g, uint32_t player, uint32_t x,
                            uint32_t y, uint32_t player_arr[][2],
                            uint32_t player_arr_2[][2],
                            uint32_t tmp, uint32_t **check_arr);

#endif /* MOVES_H */