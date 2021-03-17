/**
 * @file
 * Interfejs modułu zawierającego funkcje odpowiedzialne za obsługę trybu 
 * wsadowego.
 * 
 * @author Mateusz Jastrzębski <m.jastrzebsk28@student.uw.edu.pl>
 * @date 16.05.2020
 */

#ifndef BATCH_H
#define BATCH_H

#include <stdbool.h>
#include <stdint.h>
#include "gamma.h"

/**
 * @brief Pobiera polecenia po wejściu w tryb wsadowy, sprawdza ich poprawność
 *        i je interpretuje.
 * @param[in, out] g - wskaźnik na strukturę, przechowującą stan gry.
 * @param[in] line - numer linii, na której zostało wywołane poprawne polecenie
 *               wejścia do trybu wsadowego.
 * @return @p true, gdy w czasie wykonywania ruchów nie wystąpiły problemy z
 *         brakiem pamięci, @p false w przeciwnym przypadku.
 */
extern bool batch_mode(gamma_t *g, unsigned long long line);

#endif /*BATCH_H*/