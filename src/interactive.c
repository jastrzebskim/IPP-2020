/** @file
 * Implementacja funkcji obsługujących terminal podczas wejścia do trybu 
 * interaktywnego oraz grę w tym trybie.
 * 
 * @author Mateusz Jastrzębski <m.jastrzebsk28@student.uw.edu.pl>
 * @date 15.05.2020
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include "gamma.h"
#include "interactive.h"

/**
 * Numer znaku '0' w ASCII.
 */
static const int ASCII_ZERO = 48;

void clear_console() {
    printf("\033[2J");
    printf("\033[1;1H");
}

unsigned int num_len(uint32_t number) {
    int len = 0;
    uint32_t max_player = number;
    if (max_player == 0) {
        return 1;
    }
    while (max_player > 0) {
        len++;
        max_player /= 10;
    }
    return len;
}

/**
 * @brief Podaje lokalizację ostatniego na planszy znaku, składającego się
 *        na numer gracza, zajmującego pole (@p x, @p y).
 * @param[in] x - pierwsza współrzędna przeglądanego pola.
 * @param[in] y - druga współrzędna przeglądanego pola.
 * @param[in] players - liczba graczy w grze.
 * @param[in] height - wysokość planszy.
 * @param[in] width - szerokość planszy
 * @return Numer indeksu w tablicy, pod którym kryje się ostatni znak numeru
 *         gracza, zajmującego pole (@p x, @p y).
 */
static unsigned int find_on_board(uint32_t x, uint32_t y, uint32_t players,
                    uint32_t height, uint32_t width) {
    if (players < 10) {
       return (height - 1 - y) * (width + 1) +  x;
    }
    else {
        unsigned max_player_len = num_len(players);
        return (height - 1 - y) * (width * (max_player_len + 1) + 1) 
        + (max_player_len + 1) * x + max_player_len;
    }
}

/**
 * @brief Drukuje planszę na ekran, wyróżniając rewersem pole o współrzędnych
 *        (@p x, @p y).
 * @param[in] p - wskaźnik na tablicę znaków, która przechowuje obecny stan
 *                planszy.
 * @param[in] players - liczba graczy biorących udział w grze.
 * @param[in] x - pierwsza współrzędna pola, które funkcja wyróżnia.
 * @param[in] y - druga współrzędna pola, które funkcja wyróżnia.
 * @param[in] width - szerokość planszy.
 * @param[in] height wysokość planszy.
 */
static void print_board(char *p, uint32_t players, uint32_t x, uint32_t y,
                uint32_t width, uint32_t height) {
    unsigned int place;
    place = find_on_board(x, y, players, height, width);
    unsigned int start = place - num_len(players);
    unsigned int i = 0;

    while (p[i] != '\0') {
        if (players > 9) {
            if (i == start) {
                printf("\033[0;7m");
            }

            printf("%c", p[i]);
        
            if (i == place) {
                printf("\033[0;27m");
            }
        }
        else {
            if (i == place) {
                printf("\033[0;7m");
            }
            printf("%c", p[i]);
            if (i == place) {
                printf("\033[0;27m");
            }
        }
        i++;
    }
}

/**
 * @brief Drukuje sformatowaną przez funkcję print_board(...) planszę oraz
 *        pasek, zawierający: napis "PLAYER", liczbę zajętych przez gracza pól,
 *        liczbę pól, które gracz może zająć oraz "G" w przypadku, gdy funkcja
 *        gamma_golden_possible(...) zwraca dla podanego gracza @p true.
 * @param[in] p - wskaźnik na tablicę znaków, która przechowuje obecny stan
 *            planszy.
 * @param[in] players - liczba graczy, biorących udział w grze.
 * @param[in] x - pierwsza współrzędna pola, które zostanie wyróżnione przez
 *                funkcję print_board(...).
 * @param[in] y - druga współrzędna pola, które zostanie wyróżnione przez
 *            funkcję print_board(...).
 * @param[in] width - szerokość planszy.
 * @param[in] height - wysokość planszy.
 * @param[in, out] g - wskaźnik na strukturę, przechowującą obecny stan gry.
 * @param[in] current_player - gracz, który wykonuje w danym momencie ruch.
 */
static void print_all(char *p, uint32_t players, uint32_t x, uint32_t y,
                uint32_t width, uint32_t height, gamma_t *g,
                uint32_t current_player) {
    printf("\033[1;1H");
    print_board(p, players, x, y, width, height);
    printf("\x1b[2K");
    printf("PLAYER %d %" PRIu64 " %" PRIu64, current_player,
            gamma_busy_fields(g, current_player),
            gamma_free_fields(g, current_player));
    if (gamma_golden_possible(g, current_player)) {
        printf("\033[32m");
        printf(" G");
        printf("\033[0m");
    }
}

/**
 * @brief Wywołuje funcję gamma_move(...).
 * Jeśli funkcja zwraca @p true, funkcja wstawia numer gracza na pole
 * (@p x, @p y) wewnątrz tablicy, do której wskaźnik jest przechowywany w @p p.
 * @param[in, out] g - wskaźnik na strukturę, przechowującą obecny stan gry.
 * @param[in] players - liczba graczy, biorących udział w grze.
 * @param[in] width - szerokość planszy.
 * @param[in] height - wysokość planszy.
 * @param[in] current_player - gracz, który wykonuje w danym momencie ruch.
 * @param[in] x - pierwsza współrzędna pola, na którym stoi w danym momencie
 *            kursor.
 * @param[in] y - druga współrzędna pola, na którym stoi w danym momencie
 *                kursor.
 * @param[in, out] p - wskaźnik na tablicę znaków, która przechowuje obecny
 *                     stan planszy.
 * @return @p true, gdy funkcja gamma_move(...) zawraca @p true,
 *         @p false w przeciwnym przypadku.
 */
static bool interactive_move(gamma_t *g, uint32_t players, uint32_t width,
                    uint32_t height, uint32_t current_player,
                    uint32_t x, uint32_t y, char *p) {
    bool final_ans = gamma_move(g, current_player, x, y);
    unsigned int place = find_on_board(x, y, players, height, width);
    unsigned int start = place - num_len(players);
    if (final_ans == true) {
        while (current_player > 0 ) {
            p[place] = (char)((current_player % 10) + ASCII_ZERO);
            current_player /= 10;
            if (current_player > 0)
                place--;
        }
        if (players > 9) {
            while (place > start) {
                place--;
                p[place] = ' ';
            }
        }
    }
    return final_ans;
}

/**
 * @brief Wywołuje funcję gamma_golden_move(...).
 * Jeśli funkcja zwraca @p true, funkcja wstawia numer gracza na pole
 * (@p x, @p y) wewnątrz tablicy, do której wskaźnik jest przechowywany w @p p.
 * @param[in, out] g - wskaźnik na strukturę, przechowującą obecny stan gry.
 * @param[in] players - liczba graczy, biorących udział w grze.
 * @param[in] width - szerokość planszy.
 * @param[in] height - wysokość planszy.
 * @param[in] current_player - gracz, który wykonuje w danym momencie ruch.
 * @param[in] x - pierwsza współrzędna pola, na którym stoi w danym momencie
 *            kursor.
 * @param[in] y - druga współrzędna pola, na którym stoi w danym momencie
 *                kursor.
 * @param[in, out] p - wskaźnik na tablicę znaków, która przechowuje obecny
 *                     stan planszy.
 * @return @p true, gdy funkcja gamma_golden_move(...) zawraca @p true,
 *         @p false w przeciwnym przypadku.
 */
static bool interactive_g_move(gamma_t *g, uint32_t players, uint32_t width,
                        uint32_t height, uint32_t current_player,
                        uint32_t x, uint32_t y, char *p) {
    bool final_ans = gamma_golden_move(g, current_player, x, y);
    unsigned int place = find_on_board(x, y, players, height, width);
    unsigned int start = place - num_len(players);
    if (final_ans == true) {
        while (current_player > 0) {
            p[place] = (char)((current_player % 10) + 48);
            current_player /= 10;
            if (current_player > 0)
                place--;
        }
        if (players > 9) {
            while (place > start) {
                place--;
                p[place] = ' ';
            }
        }
    }
    return final_ans;
}

/**
 * @brief Sprawdza, czy wczytane zostały strzałki oraz, jeśli tak, obsługuje
 *        przesunięcie kursora.
 * @param[in] first - najstarszy spośród trzech wczytany znak.
 * @param[in] second - drugi w kolejności wczytany znak.
 * @param[in] third - ostatni wczytany znak.
 * @param[in, out] x - wskaźnik na miejsce w pamięci, gdzie przechowywana jest
 *                     pierwsza współrzędna pola, na którym stoi w danym
 *                     momencie kursor.
 * @param[in, out] y - wskaźnik na miejsce w pamięci, gdzie przechowywana jest
 *                     druga współrzędna pola, na którym stoi w danym momencie
 *                     kursor.
 * @param[in] width - szerokość planszy.
 * @param[in] height - wysokość planszy.
 * @return @p true w przypadku, gdy wczytane zostały strzałki, @p false
 *         w przeciwnym przypadku.
 */
static bool check_arrows(char first, char second, char third, uint32_t *x,
                    uint32_t *y, uint32_t width, uint32_t height) {
    // strzałka w górę
    if (first == '\033' && second == '[' && third == 'A') {
        if (*y < height - 1) 
            (*y)++;
        return true;
    }
    // strzałka w dół
    else if (first == '\033' && second == '[' && third == 'B') {
        if (*y > 0) 
            (*y)--;
        return true;
    }
    // strzałka w lewo
    else if (first == '\033' && second == '[' && third == 'D') {
        if (*x > 0) 
            (*x)--;
        return true;
    }
    // strzałka w prawo
    else if (first == '\033' && second == '[' && third == 'C') {
        if (*x < width - 1) 
            (*x)++;
        return true;
    }
    return false;
}

/**
 * @brief Czyta znaki z klawiatury, rozpoznaje je i odpowiednio reaguje.
 * @param[in, out] g - wskaźnik na strukturę, przechowująca obecny stan gry.
 * @param[in] players - liczba graczy, biorących udział w grze.
 * @param[in] width - szerokość planszy.
 * @param[in] height - wysokość planszy.
 * @param[in] current_player - gracz, wykonujący w danym momencie ruch.
 * @param[in, out] p - wskaźnik na tablicę znaków, przechowującą obecny stan
 *                     planszy.
 * @param[in, out] x - wskaźnik na pierwszą współrzędną pola, na którym w danym
 *                     momencie ustawiony jest kursor.
 * @param[in, out] y - wskaźnik na drugą współrzędną pola, na którym w danym
 *                     momencie ustawiony jest kursor.
 * @param[out] end_game - wskaźnik na zmienną typu bool, która zawiera
 *                        informację, czy gra powinna być natychmiast
 *                        zakończona. Jeśli wartość zmiennej to 
 *                        @p true - należy natychmiast zakończyć program
 *                        kodem 0.
 * @return @p true w przypadku, kiedy nie wystąpiły podczas wykonywania ruchu
 *         problemy z pamięcią, @p false w przeciwnym przypadku.
 */
static bool read_and_move(gamma_t *g, uint32_t players, uint32_t width,
                    uint32_t height, uint32_t current_player, char *p,
                    uint32_t *x, uint32_t *y, bool *end_game) {
    char first = '\0';
    char second = '\0';
    char third = '\0';
    bool success = true;

    while (third != '\4') {
        print_all(p, players, *x, *y, width, height, g, current_player);
        if (errno == ENOMEM) {
            return false;
        }
        first = second;
        second = third;
        third = getchar();

        if (check_arrows(first, second, third, x, y, width, height)) {
        }
        else if (third == '\40') {
            success = interactive_move(g, players, width, height,
                                        current_player, *x, *y, p);
            if (success == false) {
                if (errno == ENOMEM)
                    return false;
            }
            else {
                return true;
            }
        }
        else if (third == 'g' || third == 'G') {
            success = interactive_g_move(g, players, width, height,
                                        current_player, *x, *y, p);
            if (success == false) {
                if (errno == ENOMEM)
                    return false;
            }
            else {
                return true;
            }
        }
        else if (third == 'c' || third == 'C') {
            return true;
        }
        else if (third == '\4') {
            *end_game = true;
            return true;
        }
    }
    return true;
}

bool interactive_mode(gamma_t *g, char **words) {
    printf("\033[?25l");// ukrywa kursor
    char *pointer_to_end;
    uint32_t width = (uint32_t)strtoul(words[1], &pointer_to_end, 10);
    uint32_t height = (uint32_t)strtoul(words[2], &pointer_to_end, 10);
    uint32_t players = (uint32_t)strtoul(words[3], &pointer_to_end, 10);
    uint32_t x = 0;
    uint32_t y = height - 1;
    char *p = gamma_board(g);
    if (p == NULL)
        return false;
    uint32_t current_player = 1;
    bool end_game = false;
    unsigned int players_who_play = 0;
    print_all(p, players, x, y, width, height, g, current_player);

    while (!end_game) {
        if (current_player == 1)
            players_who_play = 0;

        if (gamma_free_fields(g, current_player) > 0 || 
            gamma_golden_possible(g, current_player)) {
            if (errno == ENOMEM) {
                free(p);
                return false;
            }
            players_who_play++;
            errno = 0;
            if (!read_and_move(g, players, width, height, current_player,
                                p, &x, &y, &end_game)) {
                free(p);
                return false;
            }
        }
        if (errno == ENOMEM) {
            free(p);
            return false;
        }
        if (current_player == players && players_who_play == 0) {
            end_game = true;
        }
        else if (current_player == players) {
            current_player = 1;
        }
        else {
            current_player++;
        }
    }
    printf("\033[1;1H");
    printf("%s", p);
    printf("Summary:\n");
    for (uint32_t i = 1; i <= players; i++) {
        printf("\x1b[2K");
        printf("PLAYER %d %" PRIu64 "\n", i, gamma_busy_fields(g, i));
    }
    free(p);
    printf("\033[?25h");// pokazuje kursor
    return true;
}