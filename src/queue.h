/** @file
 * Interfejs klasy obsługującej kolejkę.
 *
 * @author Mateusz Jastrzębski <m.jastrzebsk28@student.uw.edu.pl>
 * @date 16.04.2020
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stdint.h>

///Struktura, przechowująca elementy kolejki. 
typedef struct T_queue TQueue;

/**
 * @brief Tworzy wskaźnik do nowej kolejki.
 * Alokuje pamięć dla nowej kolejki.
 * @return wskaźnik do utworzonej kolejki.
 */
extern TQueue *init();

/**
 * @brief Odpowiada na pytanie, czy kolejka jest pusta.
 * @param[in] q - wskaźnik na kolejkę, której zawartość funkcja bada.
 * @return @p true, gdy kolejka jest pusta, @p false w przeciwnym przypadku.
 */
extern bool empty_q(TQueue *q);

/**
 * @brief Dodaje kolejny element na koniec kolejki.
 * Elementy są postaci pary (@p x1, @p y) i zawierają współrzędne pola. 
 * @param[in, out] q - wskaźnik na kolejkę, do której dodawany jest nowy
 *                     element.
 * @param[in] x1 - pierwsza współrzędna dodawanego do kolejki elementu.
 * @param[in] y1 - druga współrzędna dodawanego do kolejki elementu.
 * @return @p true jeśli udało się zaalokować pamięć, @p false w przeciwnym
 *         przypadku
 */
extern bool push_q(TQueue *q, uint32_t x1, uint32_t y1);

/**
 * @brief Usuwa element z początku kolejki.
 * Współrzędne pola usuwanego z kolejki zapisywane są kolejno w zmiennych,
 * na które wskaźniki znajdują się wśród parametrów: @p x1, @p y1. 
 * @param[in, out] q - wskaźnik na kolejkę, z której usuwany jest element. 
 * @param[out] x1 - wskaźnik na zmienną, do której trafi pierwsza współrzędna
 *                  elementu usuwanego z kolejki.
 * @param[out] y1 - wskaźnik na zmienną, do której trafi druga współrzędna
 *                  elementu usuwanego z kolejki.
 */
extern void pop_q(TQueue *q, uint32_t *x1, uint32_t *y1);

/**
 * @brief Funkcja podaje współrzędne pierwszego elementu w kolejce. 
 * @param[in] q - wskaźnik na kolejkę, której pierwszy element jest badany.
 * @param[out] x1 - wskaźnik na zmienną, do której trafi pierwsza współrzędna
 *                  elementu usuwanego z kolejki.
 * @param[out] y1 - wskaźnik na zmienną, do której trafi druga współrzędna
 *                  elementu usuwanego z kolejki.
 */
extern void first_q(TQueue *q, uint32_t *x1, uint32_t *y1);

/**
 * @brief Usuwa wszystkie elementy kolejki @p q oraz zwalnia pamięć 
 *        przeznaczoną na tę kolejkę
 * @param[in, out] q - kolejka, która ma zostać przez funkcję usunięta.
 */
extern void del_queue(TQueue *q);

#endif /* QUEUE_H */
