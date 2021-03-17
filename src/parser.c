/**
 * @file
 * Implementacja funkcji odpowiedzialnych za przetwarzanie danych z wejścia, 
 * wczytanie linii rozpoczynającej rozgrywkę i wywołanie funkcji 
 * odpowiedzialnych za przeprowadzenie rozgrywki w podanym trybie.
 * 
 * @author Mateusz Jastrzębski
 * @date 16.05.2020 
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <termio.h>
#include <unistd.h>
#include <inttypes.h>
#include "gamma.h"
#include "batch.h"
#include "interactive.h"
#include "parser.h"

/**
 * Tablica zawierająca białe znaki.
 */
static const char delimiters[7] = {' ', '\t', '\v', '\f', '\r', '\n', '\0'};

/**
 * Liczba ciągów znaków oddzielonych białymi znakami w poprawnej linijce,
 * wywołującej przejście do jednego z dwóch trybów gry. 
 */
static const int VALID_NUMBER_OF_PARAMETERS = 5;

/**
 * @brief Tworzy tablicę pięciu wskaźników.
 * @param[in, out] words - wskaźnik na tablicę wskaźników.
 * @return @p true, gdy udało się zaalokować pamięć, @p false w przeciwnym
 * przypadku.
 */
static bool initiate_arr_of_words(char ***words) {
    *words = malloc(VALID_NUMBER_OF_PARAMETERS * sizeof(char *));

    if ((*words) == NULL)
        return false;

    return true;
}

bool check_num_parameter(char *word) {
    char *pointer_to_end;
    unsigned long long parameter_num;

    if ((int)word[0] < (int)'0' || (int)word[0] > (int)'9')
            return false;
        
    parameter_num = strtoull(word, &pointer_to_end, 10);
        
    if (strcmp(pointer_to_end, "") != 0) {
        return false;
    }

    if (parameter_num > UINT32_MAX) {
        errno = 0;
        return false;
    }

    return true;
}

/**
 * @brief Sprawdza poprawność linii wpisanych przed wejściem do jednego z
 *        trybów gry.
 * Sprawdza: liczbę łańcuchów znaków oddzielonych białymi znakami w linii,
 * pierwszy łańcuch znaków oraz poprawność łańcuchów, które docelowo mają 
 * składać się na liczby.
 * @param[in] words - tablica wskaźników do pierwszych znaków z kolejnych
 *                    łańcuchów znaków z linii.
 * @param[in] number_of_words_in_line - liczba łańcuchów znaków, oddzielonych
 *                                  białymi znakami w linii.
 * @return @p true, gdy linijka może być poprawną linią wprowadzającą w jeden
 *         z trybów gry, @p false w przeciwnym przypadku.
 */
static bool check_initial_line(char **words, int number_of_words_in_line) {
    if (number_of_words_in_line > VALID_NUMBER_OF_PARAMETERS ||
        number_of_words_in_line < VALID_NUMBER_OF_PARAMETERS) {
        return false;
        }
    // sprawdzanie poprawności całego pierwszego słowa (strcmp) 
    if (strcmp(words[0], "I") != 0 && strcmp(words[0], "B") != 0) {
        return false;
    }
    
    for (int i = 1; i < VALID_NUMBER_OF_PARAMETERS; i++) {
        if (!check_num_parameter(words[i])) 
            return false;
    }

    for (int i = 1; i < 5; i++) {
        if (strcmp(words[i], "0") == 0) {
            return false;
        }
    }

    return true;
}

int separate_words(char *buff, char **words) {
    int number_of_words_in_line = 0;
    char *word_from_line;

    word_from_line = strtok(buff, delimiters);
    if (word_from_line != NULL) {
        words[0] = word_from_line;
        number_of_words_in_line++;
    }

    while (word_from_line != NULL) {
        word_from_line = strtok(NULL, delimiters);

        if (word_from_line != NULL) {
            if (number_of_words_in_line < VALID_NUMBER_OF_PARAMETERS)
                words[number_of_words_in_line] = word_from_line;
            number_of_words_in_line++;
        }
    }

    return number_of_words_in_line;
}

/**
 * @brief Zmienia argumenty z wejścia na liczby i tworzy nową grę.
 * 
 * @param[in] words - tablica wskaźników na kolejne ciągi znaków z wejścia.
 * @param[in] line_number - numer linii, na której wystąpiło obecnie wykonywane
 * polecenie.
 * @return Wskaźnik na utworzoną strukturę, przechowującą stan gry. 
 */
static gamma_t *new_gamma(char **words, unsigned long long line_number) {
    char *pointer_to_end;
    uint32_t param1 = strtoul(words[1], &pointer_to_end, 10);
    uint32_t param2 = strtoul(words[2], &pointer_to_end, 10);
    uint32_t param3 = strtoul(words[3], &pointer_to_end, 10);
    uint32_t param4 = strtoul(words[4], &pointer_to_end, 10);
    gamma_t *g = gamma_new(param1, param2, param3, param4);

    if (g == NULL) {
        fprintf(stderr, "ERROR %lld\n", line_number);
    }

    return g;  
}

/**
 * @brief Przetwarza poprawną linię, odsyłającą do jednego z dwóch trybów
 *        gry.
 * Przygotowuje terminal do przejścia w tryb interaktywny.
 * @param[in, out] g - wskaźnik na strukturę, przechowującą stan gry.
 * @param[in] words - tablica łańcuchów znaków z wejścia, oddzielonych białymi
 *                znakami.
 * @param[in] line_number - numer linii, na której wystąpiło poprawne polecenie
 *                      przejścia do jednego z dwóch trybów gry.
 * @return @p true, gdy w czasie gry nie wystąpił błąd krytyczny,
 *         @p false w przeciwnym przypadku.
 */
static bool start_game(gamma_t *g, char **words,
                        unsigned long long line_number) {
    bool success = true;
    char *pointer_to_end;
    uint32_t param1 = strtoul(words[1], &pointer_to_end, 10);
    uint32_t param2 = strtoul(words[2], &pointer_to_end, 10);
    uint32_t param3 = strtoul(words[3], &pointer_to_end, 10);

    if (strcmp(words[0], "I") == 0 && g != NULL) {
        struct winsize window_size;
        ioctl(0, TIOCGWINSZ, &window_size);
        unsigned int width = window_size.ws_col;
        unsigned int height = window_size.ws_row;
        if ((param3 < 10 && width < param1 + 1) ||
            (param3 > 9 && width < param1 * (num_len(param3) + 1) + 1) ||
            height < param2 + 2) {
            printf("Zbyt duże wymiary planszy!\n");
            return false;
        }
        struct termios prevsett, currsett;
        // zapis ustawień terminala
        tcgetattr(STDIN_FILENO, &prevsett);
        currsett = prevsett;
        // włącza tryb kanoniczny i blokuje wypisywanie znaków z klawiatury
        currsett.c_lflag &= ~(ICANON | ECHO);
        // nowe ustawienia
        tcsetattr(STDIN_FILENO, TCSANOW, &currsett);
        clear_console();
        success = interactive_mode(g, words);
        // powrót do starych ustawień
        tcsetattr(STDIN_FILENO, TCSANOW, &prevsett);
    }
    else if (strcmp(words[0], "B") == 0) {
        success = batch_mode(g, line_number);
    }
    else {
        return false;
    }
    return success;
}

bool read_initial_line() {
    size_t buffersize = 4;
    char **words;
    if(!initiate_arr_of_words(&words))
        return false;
    char *buff = NULL;
    unsigned long long line_number = 0;
    int getline_ret_code = 0;

    while (!feof(stdin)) {
        errno = 0;
        line_number++;
        getline_ret_code = getline(&buff, &buffersize, stdin);
        if (errno == ENOMEM) {
            free(buff);
            free(words);
            return false;
        }
        if (feof(stdin)) {
            if (getline_ret_code != GETLINE_EOF && buff[0] != '#')
                fprintf(stderr, "ERROR %lld\n", line_number);
            break;
        }
        if (buff[0] == '#' || strcmp(buff, "\n") == 0)
            continue;
        if (buff[0] != 'B' && buff[0] != 'I') {
            fprintf(stderr, "ERROR %lld\n", line_number);
            continue;
        }

        int number_of_words_in_line = separate_words(buff, words);
        
        if (!check_initial_line(words, number_of_words_in_line)) {
            fprintf(stderr, "ERROR %lld\n", line_number);
        }
        else {
            gamma_t *g = new_gamma(words, line_number);
            if (g == NULL)
                continue;
            if (strcmp(words[0], "B") == 0)
                printf("OK %lld\n", line_number);
            bool success = start_game(g, words, line_number);
            if (!success) {
                gamma_delete(g);
                free(buff);
                free(words);
                return false;
            }
            if (g != NULL)
                gamma_delete(g);
            break;
        }
    }
    free(buff);
    free(words);
    return true;
}