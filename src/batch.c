/**
 * @file
 * Implementacja funkcji odpowiedzialnych za obsługę trybu wsadowego.
 * 
 * @author Mateusz Jastrzębski <m.jastrzebsk28@student.uw.edu.pl>
 * @date 16.05.2020
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <inttypes.h>
#include "parser.h"
#include "batch.h"

/**
 * Liczba parametrów, których do wywołania wymagają funkcje:
 * gamma_golden_move(...) oraz gamma_move(...).
 */
static const int G_M_NUMBER_OF_PARAMETERS = 4;

/**
 * Liczba parametrów, których do wywołania wymagają funkcje:
 * gamma_busy_fields(...), gamma_free_fields(...) oraz
 * gamma_golden_possible(...).
 */
static const int B_F_Q_NUMBER_OF_PARAMETERS = 2;

/**Liczba parametrów wymagana do poprawnego wywołania funkcji
 * gamma_board(...).
 * 
 */
static const int P_NUMBER_OF_PARAMETERS = 1;

/**
 * @brief Sprawdza poprawność ciągów znaków, składających się na linię.
 * @param[in] words - tablica wskaźników do kolejnych ciągów znaków.
 * @param[in] number_of_words_in_line - liczba ciągów znaków w linii.
 * @return @p true, gdy ciągi znaków w linii odpowiadają poprawnemu poleceniu
 *         wykonania funkcji z silnika, @p false w przeciwnym przypadku.
 */
static bool batch_check_line(char **words, int number_of_words_in_line) {
    bool check = true;
    // sprawdzanie poprawności całego pierwszego słowa (strcmp)
    if ((strcmp(words[0], "m") == 0 || strcmp(words[0], "g") == 0)
        && number_of_words_in_line == G_M_NUMBER_OF_PARAMETERS) {
        for (int i = 1; i < number_of_words_in_line; i++) {
            if (!check_num_parameter(words[i])) {
                check = false;
            }
        }
        if (check) {
            return true;
        }
    }
    else if ((strcmp(words[0], "b") == 0 || strcmp(words[0], "f") == 0
            || strcmp(words[0], "q") == 0)
            && number_of_words_in_line == B_F_Q_NUMBER_OF_PARAMETERS) {
        if (check_num_parameter(words[1])) {
            return true;
        }
    }
    else if (strcmp(words[0], "p") == 0 &&
            number_of_words_in_line == P_NUMBER_OF_PARAMETERS) {
        return true;
    }

    return false;
}

/**
 * @brief Sprawdza, czy pierwszy znak w linii z wejścia odpowiada jednej z
 *        funkcji z silnika, które można za pomocą trybu wsadowego wywołać.
 * 
 * @param[in] buff - wskaźnik na początek rozpatrywanego ciągu znaków.
 * @return @p true w przypadku, gdy pierwszy znak w rozpatrywanym ciągu znaków
 *         odpowiada poprawnej dla trybu wsadowego linii, @p false w przeciwnym
 *         przypadku. 
 */
static bool check_first_char(char *buff) {
    if (buff[0] != 'm' && buff[0] != 'g' && buff[0] != 'b' && buff[0] != 'f'
        && buff[0] != 'q' && buff[0] != 'p') {
            return false;
        }
    return true;
}

/**
 * @brief Konwertuje ciągi znaków z poprawnej linii na liczby i wykonuje
 *        zadane w linijce wywołanie odpowiedniej funkcji z silnika, która
 *        wymaga czterech parametrów. 
 * @param[in, out] g - wskaźnik na strukturę, przechowującą obecny stan gry.
 * @param[in] words - tablica wskaźników do pierwszych znaków kolejnych ciągów
 *                znaków z polecenia.
 * @return @p true, gdy wywołana funkcja zwróci @p true, @p false, w przypadku,
 *         kiedy wywołana funkcja zwróci @p false.
 */
static bool four_param_functions(gamma_t *g, char **words) {
    char *pointer_to_end;
    bool check;
    unsigned long param1 = strtoul(words[1], &pointer_to_end, 10);
    unsigned long param2 = strtoul(words[2], &pointer_to_end, 10);
    unsigned long param3 = strtoul(words[3], &pointer_to_end, 10);

    if (strcmp(words[0], "m") == 0) {
        check = gamma_move(g, (uint32_t)param1, (uint32_t)param2,
                            (uint32_t)param3);
    }
    else {
        check = gamma_golden_move(g, (uint32_t)param1, (uint32_t)param2,
                                    (uint32_t)param3);
    }

    return check;
}

/**
 * @brief Konwertuje ciągi znaków z poprawnej linii na liczby i wykonuje
 *        zadane w linijce wywołanie odpowiedniej funkcji z silnika, która
 *        wymaga dwóch parametrów.
 * @param[in, out] g - wskaźnik na strukturę, przechowującą obecny stan gry.
 * @param[in] words - tablica wskaźników do pierwszych znaków kolejnych ciągów
 *                znaków z polecenia.
 * @return Wartość zwracana przez wywołaną funkcję.
 */
static uint64_t two_param_functions(gamma_t *g, char **words) {
    char *pointer_to_end;
    uint64_t num = 0;
    unsigned long param1 = strtoul(words[1], &pointer_to_end, 10);

    if (strcmp(words[0], "b") == 0) {
        num = gamma_busy_fields(g, (uint32_t)param1);
    }
    else if (strcmp(words[0], "f") == 0) {
        num = gamma_free_fields(g, (uint32_t)param1);
    }

    return num;
}

/**
 * @brief Wywołuje polecenie z poprawnej linijki w batch mode.
 * Obsługuje problemy z pamięcią. Drukuje zinterpretowane wyniki wywołania
 * funkcji. 
 * @param[in, out] g - wskaźnik na strukturę, przechowującą obecny stan gry.
 * @param[in] words - tablica wskaźników na pierwsze znaki kolejnych ciągów
 *                    znaków, oddzielonych białymi znakami, z wejścia.
 * @param[in] line_number - numer linii, na której wystąpiło obecnie rozpatrywane
 *                      polecenie.
 * @return @p true, gdy nie wystąpiły problemy z pamięcią, @p false
 *         w przeciwnym przypadku. 
 */
static bool make_move(gamma_t *g, char **words, unsigned long long line_number) {
    if (words[0][0] == 'm' || words[0][0] == 'g') {
        bool check = four_param_functions(g, words);

        if (check == false && errno == ENOMEM)
            return false;

        if (check == true) {
            printf("1\n");
        }
        else {
            printf("0\n");
        }
    }
    else if (words[0][0] == 'b' || words[0][0] == 'f') {
        uint64_t num = two_param_functions(g, words);

        printf("%" PRIu64 "\n", num);
    }
    else if (words[0][0] == 'q') {
        bool check;
        char *pointer_to_end;
        unsigned long param1 = strtoul(words[1], &pointer_to_end, 10);

        check = gamma_golden_possible(g, (uint32_t)param1);
        if (!check && errno == ENOMEM) {
            return false;
        }
        
        if (check) {
            printf("1\n");
        }
        else {
            printf("0\n");
        }
    }
    else {
        char *string_out;

        string_out = gamma_board(g);

        if (string_out == NULL) {
            fprintf(stderr, "ERROR %lld\n", line_number);
        }
        else {
            printf("%s", string_out);
        }

        free(string_out);
    }
    return true;
}

bool batch_mode(gamma_t *g, unsigned long long line) {
    unsigned long long line_number = line;
    char **words = malloc((G_M_NUMBER_OF_PARAMETERS + 1) * sizeof(char *));
    if (words == NULL) 
        return false;
    size_t buffer_size = 0;
    char *buff = NULL;
    int getline_ret_code = 0;
    int number_of_words_in_line;

    while (!feof(stdin)) {
        errno = 0;
        line_number++;
        getline_ret_code = getline(&buff, &buffer_size, stdin);
        if (errno == ENOMEM) {
            free(buff);
            free(words);
            return false;
        }
        if (feof(stdin)) {
            if (getline_ret_code != GETLINE_EOF && buff[0] != '#') {
                fprintf(stderr, "ERROR %lld\n", line_number);
            }
            break;
        }
        if (buff[0] == '#' || strcmp(buff, "\n") == 0)
            continue;
        if (!check_first_char(buff)) {
            fprintf(stderr, "ERROR %lld\n", line_number);
            continue;
        }

        number_of_words_in_line = separate_words(buff, words);
        
        if (!batch_check_line(words, number_of_words_in_line)) {
            fprintf(stderr, "ERROR %lld\n", line_number);
        }
        else {
            errno = 0;
            if (!make_move(g, words, line_number) &&
                errno == ENOMEM) {
                    free(buff);
                    free(words);
                    return false;
                }
        }
    }
    free(buff);
    free(words);
    return true;
}