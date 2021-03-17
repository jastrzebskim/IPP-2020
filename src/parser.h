/**
 * @file
 * Interfejs modułu zawierającego funkcje odpowiedzialne za przetwarzanie 
 * danych z wejścia, wczytanie linii rozpoczynającej rozgrywkę i wywołanie 
 * funkcji odpowiedzialnych za przeprowadzenie rozgrywki w podanym trybie.
 * 
 * @author Mateusz Jastrzębski
 * @date 16.05.2020 
 */

#ifndef PARSER_H
#define PARSER_H

/**
 * Wartość zwracana przez funkcję getline(...), gdy ta napotka koniec pliku.
 */
#define GETLINE_EOF -1

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Sprawdza poprawność numerycznych parametrów.
 * @param[in] word - pobrany z wejścia ciąg znaków.
 * @return @p true, gdy da się skonwertować ciąg znaków @p word na liczbę
 *         z zakresu od zera do UINT32_MAX, @p false w przeciwnym przypadku.
 */
extern bool check_num_parameter(char *word);

/**
 * @brief Liczy, jak wiele ciągów znaków rozdzielonych przez białe znaki
 *        wystąpiło w pobranej linii. Separuje te ciągi znaków od siebie
 *        nawzajem. 
 * @param[in, out] buff - wskaźnik do tablicy znaków. Tablica zawiera znaki
 *                        podane kolejno na wejściu.
 * @param[out] words - tablica wskaźników, w której funkcja zapisuje wskaźniki
 *                     do pierwszych znaków kolejnych odseparowanych ciągów
 *                     znaków.
 * @return Liczba odseparowanych ciągów znaków.
 */
extern int separate_words(char *buff, char **words);

/**
 * @brief Wczytuje i analizuje kolejne linie z wejścia, poszukując poprawnej
 *        linii, odsyłającej do jednego z dwóch trybów gry.
 * @return @p true, gdy gra przebiegła przez komplikacji lub do gry nie doszło,
 *         @p false w przeciwnym przypadku.
 */
extern bool read_initial_line();

#endif /*PARSER_H*/