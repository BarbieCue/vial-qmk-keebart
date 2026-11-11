// Copyright 2026 Keebart
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum layer_names {
    _BASE,
    _LOWER,
    _RAISE,
    _ADJUST,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT_split_3x5_3(
        KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,       KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
        KC_A,    KC_S,    KC_D,    KC_F,    KC_G,       KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN,
        KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,       KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH,
                          KC_LGUI, MO(_LOWER), KC_SPC,   KC_ENT, MO(_RAISE), KC_RALT
    ),

    [_LOWER] = LAYOUT_split_3x5_3(
        QK_BOOT, KC_F1,   KC_F2,   KC_F3,   KC_F4,      KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,
        KC_TAB,  KC_1,    KC_2,    KC_3,    KC_4,       KC_5,    KC_6,    KC_7,    KC_8,    KC_9,
        KC_LSFT, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,     KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_0,
                          _______, _______, _______,    _______, MO(_ADJUST), _______
    ),

    [_RAISE] = LAYOUT_split_3x5_3(
        KC_ESC,  KC_INS,  KC_PSCR, KC_APP,  KC_CAPS,    KC_PGUP, KC_HOME, KC_UP,   KC_END,  KC_BSPC,
        KC_LCTL, KC_LALT, KC_LGUI, KC_LSFT, XXXXXXX,    KC_PGDN, KC_LEFT, KC_DOWN, KC_RGHT, KC_DEL,
        _______, KC_UNDO, KC_CUT,  KC_COPY, KC_PSTE,    XXXXXXX, KC_MPRV, KC_VOLD, KC_VOLU, KC_MNXT,
                          _______, MO(_ADJUST), _______, _______, _______, _______
    ),

    [_ADJUST] = LAYOUT_split_3x5_3(
        QK_BOOT, RGB_TOG, RGB_MOD, RGB_HUI, RGB_SAI,    RGB_VAI, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        EE_CLR,  RGB_RMOD,RGB_HUD, RGB_SAD, RGB_VAD,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                          _______, _______, _______,    _______, _______, _______
    ),
};

/* ------------------------------------------------------------------------
 * Maus-Layer an den Host melden (GNOME-Extension "Mouse Layer Indicator",
 * siehe gnome-extensions/mouse-layer-indicator).
 *
 * Beim Betreten von Layer 3 wird Strg+Alt+Shift+Super+M getippt, beim Verlassen
 * Strg+Alt+Shift+Super+N. Gerade gehaltene Modifier (auch One-Shot) werden
 * dafür kurz entfernt und danach wiederhergestellt, damit am Host exakt diese
 * Kombination ankommt.
 * ------------------------------------------------------------------------ */
#define MOUSE_LAYER      3
#define MOUSE_LAYER_ON   C(A(S(G(KC_M))))
#define MOUSE_LAYER_OFF  C(A(S(G(KC_N))))

static void tap_chord_alone(uint16_t chord) {
    uint8_t mods = get_mods();
    uint8_t osm  = get_oneshot_mods();
    uint8_t weak = get_weak_mods();
    clear_mods();
    clear_oneshot_mods();
    clear_weak_mods();
    tap_code16(chord);
    set_mods(mods);
    set_oneshot_mods(osm);
    set_weak_mods(weak);
    send_keyboard_report();
}

/* Nicht in layer_state_set_user senden (läuft mitten in der Tastenverarbeitung),
 * sondern danach in der Hauptschleife. Der Vergleich mit dem zuletzt gemeldeten
 * Zustand fängt auch schnelles Hin- und Herschalten ab. */
void housekeeping_task_user(void) {
    /* Layerzustand wird auf die andere Hälfte gespiegelt (SPLIT_LAYER_STATE_ENABLE);
     * senden soll nur die Hälfte, die am USB hängt. */
    if (!is_keyboard_master()) return;

    static bool reported_on = false;
    bool        now_on      = layer_state_cmp(layer_state, MOUSE_LAYER);
    if (now_on != reported_on) {
        reported_on = now_on;
        tap_chord_alone(now_on ? MOUSE_LAYER_ON : MOUSE_LAYER_OFF);
    }
}
