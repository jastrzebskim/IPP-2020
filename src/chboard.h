/** @file
 * Interfejs modułu zawierającego funkcje odpowiedzialne za tworzenie stringa, 
 * przechowującego obecny stan planszy.
 *
 * @author Mateusz Jastrzębski <m.jastrzebsk28@student.uw.edu.pl>
 * @date 16.04.2020
 */

#ifndef CHBOARD_H
#define CHBOARD_H

#include <stdint.h>
#include "gamma.h"

/**
 * @brief Podaje liczbę znaków, z których składa się liczba @p number.
 * Dzieli liczbę przez 10, dopóki nie będzie ona równa 0.
 * Każda operacja dzielenia oznacza zwiększenie długości o 1.
 * @param[in] number - liczba, której długość funkcja ma zwrócić.
 * @return Liczba znaków, z których składa się liczba.
 */
extern uint32_t int_len(uint32_t number);

/**
 * @brief Podaje, która spośród dwóch liczb: @p a, @p b, jest większa.
 * @param[in] a - pierwsza liczba do porównania.
 * @param[in] b - druga liczba do porównania.
 * @return Większa liczba spośród @p a i @p b.
 */
extern uint32_t max_f(uint32_t a, uint32_t b);

/**
 * @brief Tworzy finalną wersję stringa, w którym jest stan planszy.
 * Ma zastosowanie, gdy największym numerem gracza na planszy jest 9.
 * @param[in] g - struktura, przechowująca stan gry.
 * @return String zawierający znakową(char) reprezentację stanu gry.
 */
extern char *board_max_9(gamma_t *g);

/**
 * @brief Tworzy finalną wersję stringa, w którym jest stan planszy.
 * Ma zastosowanie, gdy na planszy jest pionek gracza o numerze co najmniej 10.
 * @param[in] g - wskaźnik na strukturę, przechowującą stan gry.
 * @return String zawierający znakową(char) reprezentację stanu gry.
 */
extern char *board_more_than_9(gamma_t *g);

#endif /* CHBOARD_H */