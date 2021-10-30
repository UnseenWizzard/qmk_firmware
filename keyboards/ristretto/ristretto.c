/* Copyright 2021 Brandon Lewis
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 2 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
  */

#include "ristretto.h"

enum layers {
	_BASE,
	_RAISE,
	_LOWER,
	_ADJUST
};

bool encoder_update_kb(uint8_t index, bool clockwise) {
    if (!encoder_update_user(index, clockwise)) { return false; }
	if (index != 0) { return true; }
	switch (get_highest_layer(layer_state)) {
		case _LOWER: //screen ctrl; click is prtn screen
			if (clockwise) {
				tap_code(KC_BRID);
			} else {
				tap_code(KC_BRIU);
			}
			break;
		case _RAISE: //media ctrl; click is play
			if (clockwise) {
				tap_code(KC_MPRV);
			} else {
				tap_code(KC_MNXT);
			}
			break;
		default:  //default behaviour volume; click is mute
			if (clockwise) {
				tap_code(KC_VOLD);
			} else {
				tap_code(KC_VOLU);
			}
			break;
	}	
	return true;
}

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
	return OLED_ROTATION_270;
}

static void render_logo(void) {
    static const char PROGMEM raw_logo[] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,192,160,  0,  0,192,160,  0,  0,192, 96,  0,128,192,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,240,248, 56, 30, 29, 24, 24, 30, 27, 24, 24, 29, 31, 24, 31, 60,248,240, 48, 16, 48,240,224,  0,  0,  0,  0,  0,  0,  0,  0,128,207, 63,120, 96,224,192,192,192,192,192,192,192,224, 96,112,124,191,135, 12, 12, 14,  7,  3,  0,  0,  0,  0,  0,  0,  0,  0,  3,  7,  6, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,  7,  3,  0,  0,  0,  0,  0,  0,  0,  0,
    };
    oled_write_raw_P(raw_logo, sizeof(raw_logo));
    oled_set_cursor(0,5);
	}

void render_mod_state(uint8_t modifiers) {
    oled_write_ln_P(PSTR("shft"), (modifiers & MOD_MASK_SHIFT));
	oled_write_ln_P(PSTR("ctrl"), (modifiers & MOD_MASK_CTRL));
    oled_write_ln_P(PSTR("supr"), (modifiers & MOD_MASK_GUI));
	oled_write_ln_P(PSTR("alt"), (modifiers & MOD_MASK_ALT));
}

void render_keylock_state(led_t led_state) {
	oled_write_ln_P(PSTR("caps"), led_state.caps_lock);
    oled_write_ln_P(PSTR("num"), led_state.num_lock);
    oled_write_ln_P(PSTR("scrl"), led_state.scroll_lock);
}

static void render_layers(void) {
	switch (get_highest_layer(layer_state)) {
		case _BASE:
			oled_write_P(PSTR("BASE\n"), false);
			break;
		case _RAISE:
			oled_write_P(PSTR("RAISE\n"), false);
			break;
		case _LOWER:
			oled_write_P(PSTR("LOWER\n"), false);
			break;
		case _ADJUST:
			oled_write_P(PSTR("ADJ\n"), false);
			break;
	}
}

__attribute__((weak)) void oled_task_user(void) {
	render_logo();
	render_mod_state(get_mods()|get_oneshot_mods());
	render_keylock_state(host_keyboard_led_state());
	oled_write_ln_P(PSTR(""), false);
	render_layers();
}

#endif
