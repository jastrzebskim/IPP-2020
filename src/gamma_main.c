/**
 * @file 
 * Plik z funkcją main, która wywołuje funkcję, rozpoczynającą wczytywanie
 * i analizę tekstu z wejścia.
 * @author Mateusz Jastrzębski <m.jastrzebsk28@student.uw.edu.pl>
 * @date 17.05.2020
 */

#include <stdio.h>
#include "parser.h"

/**
 * @brief Funkcja główna.
 * 
 * @return Zero w przypadku, gdy program nie zakończył swojego działania
 *         wcześniej na skutek problemów lub jeden, gdy w czasie działania
 *         programu doszło do komplikacji.
 */
int main() {
    if (read_initial_line() == false)
        return 1;
    return 0;
}