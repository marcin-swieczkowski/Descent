#ifndef ACTOR_H
#define ACTOR_H

#include "defs.h"

void advance_turn();

int can_move(ACTOR *a, int dy, int dx);
void actor_move(ACTOR *a, int dy, int dx);
void actor_pickup(ACTOR *a, int y, int x, int i);
void actor_drop(ACTOR *a, int y, int x, int i);

void actor_act(ACTOR *a);

#endif
