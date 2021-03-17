/** @file
 * Interfejs modułu zawierającego funkcje obsługujące terminal podczas wejścia 
 * do trybu interaktywnego oraz grę w tym trybie.
 * 
 * @author Mateusz Jastrzębski <m.jastrzebsk28@student.uw.edu.pl>
 * @date 15.05.2020
 */

#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#include <stdbool.h>
#include <stdint.h>
#include "gamma.h"

/**
 * @brief Przygotowuje konsolę do wyświetlenia planszy w trybie interaktywnym
 *        poprzez wyczyszczenie jej i ustawienie kursora na początku
 *        konsoli. 
 */
extern void clear_console();

/**
 * @brief Oblicza, z ilu znaków składa się liczba @p number w systemie
 *        dziesiętnym.
 * @param[in] number - liczba, której długość funkcja liczy.
 * @return Liczba znaków, z których składa się liczba @p number w systemie
 *         dziesiętnym.
 */
extern unsigned int num_len(uint32_t number);

/**
 * @brief Funkcja przygotowująca terminal do wykonania ruchu oraz pobierająca
 *        parametry gry z tablicy @p words.
 * 
 * @param[in, out] g - wskaźnik na strukturę, przechowującą obecny stan gry.
 * @param[in] words - tablica, zawierająca wskaźniki do kolejnych parametrów
 *                    gry, podanych na wejściu.
 * @return @p true w przypadku, gdy udało się bezproblemowo obsłużyć grę,
 *         @p false w przeciwnym przypadku.
 */
extern bool interactive_mode(gamma_t *g, char **words);

#endif /*INTERACTIVE_H*/