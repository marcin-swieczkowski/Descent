#include <stdlib.h>
#include <libtcod.h>
#include "defs.h"
#include "util.h"
#include "system.h"
#include "dungeon.h"
#include "actor.h"
#include "player.h"
#include "algorithms.h"
#include "io.h"
#include "menu.h"
#include "init.h"
#include "ui.h"

int main() {
    int             next_turn;
    TCOD_key_t      key;
    TCOD_event_t    ev;
    TCOD_mouse_t    mouse;

    /* Initialize game */
    init_all(); /* Initialize console and colors */

start:

    /* Main menu */
    if (handle_menu() == 1) {
        exit(1);
    }

    /* Generate dungeon */
    dungeon_gen(DUNGEON_CAVE);

    calc_fov();
    draw_game();

    /* MAIN GAME LOOP */
    do
    {
        ev = TCOD_sys_wait_for_event(TCOD_EVENT_KEY | TCOD_EVENT_MOUSE_PRESS, &key, &mouse, 0);
        if (TCOD_console_is_window_closed()) {
            game_end();
        }

        next_turn = handle_input(ev, key.vk, key.c, key.lctrl || key.rctrl, (unsigned int) mouse.cx, (unsigned int) mouse.cy);
        if (next_turn == 1) {

            /* if player used up a turn */
            TURN_COUNT++;
            advance_turn();
            calc_fov();
        }

        draw_game();
    } while (next_turn >= 0);

    if (next_turn == -1) {

        /* Escape was pressed, return to menu */
        goto start;
    }

    exit(1);
}