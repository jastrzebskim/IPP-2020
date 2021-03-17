/**
 * @file
 * Plik zawierający przykładowe testy silnika gry gamma.
 * @author Mateusz Jastrzębski <m.jastrzebsk28@student.uw.edu.pl>
 * @date 13.06.2020
 * 
 */

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "gamma.h"


/**
 * @brief Główna funkcja programu.
 * Zawiera przykładowe testy.
 * 
 * @return 0, gdy program nie napotkał komplikacji podczas wykonywania się,
 * w innym przypadku kod błędu.
 */
int main() {

gamma_t *test = gamma_new(20, 20, 2, 2);
gamma_delete(test);

return 0;
}

