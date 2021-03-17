/** @file
 * Implementacja interfejsu klasy odpowiedzialnej za obsługę kolejki.
 *
 * @author Mateusz Jastrzębski <m.jastrzebsk28@student.uw.edu.pl>
 * @date 16.04.2020
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "queue.h"

/**Struktura, przechowująca element kolejki. 
 */
struct pair {
    uint32_t x;///<pierwsza współrzędna.
    uint32_t y;///<druga współrzędna.
    struct pair *next;///<kolejny element kolejki.
};

///Struktura, przechowująca element listy.
typedef struct pair TList;

/**Struktura, przechowująca elementy kolejki.
 */
struct T_queue {
    TList *first;///<pierwszy element kolejki.
    TList *last;///<ostatani element kolejki.
};

TQueue *init() {
    TQueue *q = malloc(sizeof(TQueue));
    q->first = NULL;
    q->last = NULL;
    return q;
}

bool empty_q(TQueue *q) {
    return (q->first == NULL);
}

bool push_q(TQueue *q, uint32_t x1, uint32_t y1) {
    if (q->first == NULL) {
        q->first = malloc(sizeof(TList));
        q->first->x = x1;
        q->first->y = y1;
        q->last = q->first;
    }
    else {
        TList *q_tmp = malloc(sizeof(TList));
        if (q_tmp == NULL) {
            return NULL;
        }
        q_tmp->x = x1;
        q_tmp->y = y1;
        q->last->next = q_tmp;
        q->last = q->last->next;
    }
    return true;
}

void pop_q(TQueue *q, uint32_t *x1, uint32_t *y1) {
    *x1 = q->first->x;
    *y1 = q->first->y;
    
    TList *tmp = q->first;

    if (q->first == q->last) {
        q->first = NULL;
    }
    else {
        q->first = q->first->next;
    }
    
    free(tmp);
}

void first_q(TQueue *q, uint32_t *x1, uint32_t *y1) {
    *x1 = q->first->x;
    *y1 = q->first->y;
}

void del_queue(TQueue *q) {
    uint32_t tmp1, tmp2;

    while (!empty_q(q)) {
        pop_q(q, &tmp1, &tmp2);
    }

    free(q);
}