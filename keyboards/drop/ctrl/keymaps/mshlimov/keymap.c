#include QMK_KEYBOARD_H
#include "keymap.h" 
#include "matrix.h"


// Static variables used to track keyboard idle time and shut-off LEDs
static uint16_t idle_timer;             // Idle LED timeout timer
static uint8_t idle_second_counter;     // Idle LED seconds counter, counts seconds not milliseconds
static uint8_t key_event_counter;       // This counter is used to check if any keys are being held

enum ctrl_keycodes {
    DBG_TOG = SAFE_RANGE,  // DEBUG Toggle On / Off
    DBG_MTRX,              // DEBUG Toggle Matrix Prints
    DBG_KBD,               // DEBUG Toggle Keyboard Prints
    DBG_MOU,               // DEBUG Toggle Mouse Prints
    MD_BOOT,               // Restart into bootloader after hold timeout
    ROUT_TG,               // Timeout Toggle. Toggle idle LED time out on or off
    ROUT_VI,               // Timeout Value Increase. Increase idle time out before LED disabled
    ROUT_VD,               // Timeout Value Decrease. Decrease idle time out before LED disabled
    ROUT_FM,               // RGB timeout fast mode toggle
    LD_MAC,                // Set leader key OS mode to Mac
    LD_PC,                 // Set leader key OS mode to PC
};



// Combo Key Definitions
enum combos {
	LK_BSPC, // L and K at the same time send backspace
};


const uint16_t PROGMEM lk_combo[] = {KC_L, KC_K, COMBO_END};

combo_t key_combos[COMBO_COUNT] = {
  [LK_BSPC] = COMBO(lk_combo, KC_BSPC)
};

// Tap Dance definitions

enum td_keycodes {
	TD_CAPS, // single tap is LCTRL, double tap toggles CAPS
	TD_LCTL, //double tap sends CTRL+T to open firefox, single tap is LCTL
	TD_FN, //single tap is leader, double tap enables layer 1, single hold momentarily enables layer 1
};

typedef enum {
	TD_NONE,
	TD_UNKNOWN,
	TD_SINGLE_TAP,
	TD_SINGLE_HOLD,
	TD_DOUBLE_TAP,
} td_state_t;

typedef struct {
	bool is_press_action;
	td_state_t state;
} td_tap_t;

// Function to determine the current tapdance state;
td_state_t cur_dance(tap_dance_state_t *state) {
	if (state->count == 1) {
		if (state->interrupted || !state->pressed) {
			return TD_SINGLE_TAP;
		} else if (state->pressed){
			return TD_SINGLE_HOLD;
		}
	} else if (state->count == 2) {
			return TD_DOUBLE_TAP;
	}
	return TD_UNKNOWN;
}


static td_tap_t ql_tap_state = {
	.is_press_action = true,
	.state = TD_NONE
};

void td_fn_finished(tap_dance_state_t *state, void *user_data) {
	ql_tap_state.state = cur_dance(state);
	switch (ql_tap_state.state) {
		case TD_SINGLE_TAP:
			leader_start(); //single tap of the Fn key is leader
			break;
		case TD_SINGLE_HOLD:
			layer_on(1);
			break;
		case TD_DOUBLE_TAP:
			//check to see if the layer is already set
			if(layer_state_is(1)){
				layer_off(1);
			} else {
				layer_on(1);
			}
			break;
		default:
			break;
	}
}

void td_fn_reset(tap_dance_state_t *state, void *user_data) {
	if (ql_tap_state.state == TD_SINGLE_HOLD) {
		layer_off(1);
	}
	ql_tap_state.state = TD_NONE;
}

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        // Assign a shorter time (150ms) for a fast tap dance key
        case TD(TD_FN):
            return 150;
            
        // Return your default TAPPING_TERM for all other keys
        default:
            return TAPPING_TERM;
    }
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_CAPS] = ACTION_TAP_DANCE_DOUBLE(KC_LCTL, KC_CAPS),
    [TD_LCTL] = ACTION_TAP_DANCE_DOUBLE(KC_LCTL, LCA(KC_T)),
    [TD_FN] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_fn_finished, td_fn_reset) //150ms is the default doubletap timeout
};
	
keymap_config_t keymap_config;

#undef _______
#define _______ KC_TRNS

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_tkl_ansi(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,             KC_PSCR, KC_SCRL,KC_PAUS,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,   KC_INS,  KC_HOME, KC_PGUP,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,   KC_DEL,  KC_END,  KC_PGDN,
        TD(TD_CAPS), KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,                              KC_UP,
        TD(TD_LCTL), KC_LGUI, KC_LALT,                   KC_SPC,                         KC_RALT, TD(TD_FN),   KC_APP,  KC_RCTL,    KC_LEFT, KC_DOWN, KC_RGHT
    ),

    [1] = LAYOUT_tkl_ansi(
        _______, KC_F13,  KC_F14,  KC_F15,  KC_F16,  KC_F17,  KC_F18,  KC_F19,  KC_F20,  KC_F21,  KC_F22,  KC_F23,  KC_F24,             KC_MUTE, AS_TOGG, _______, \
        _______, TG(1), TG(2), TG(3), _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   KC_MPLY, KC_MSTP, KC_VOLU, \
        _______, RM_NEXT, RM_SPDU, RM_HUEU, RM_SATU, RM_VALU, _______, _______,_______,_______, LD_PC, _______, _______, _______,   KC_MPRV, KC_MNXT, KC_VOLD, \
        _______, RM_PREV, RM_SPDD, RM_HUED, RM_SATD, RM_VALD, _______, _______, _______, _______, _______, _______, _______, \
        _______, RM_TOGG, ROUT_TG, ROUT_FM, _______, MD_BOOT, NK_TOGG, LD_MAC, _______, _______, _______, _______,                              DBG_TOG, \
        _______, _______, _______,                   _______,                            _______, TD(TD_FN), _______, _______,            DBG_MTRX, DBG_KBD, DBG_MOU \
    ),

    [2] = LAYOUT_tkl_ansi(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______,
        _______, _______, KC_UP,   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______,
        _______, KC_LEFT, KC_DOWN, KC_RIGHT, _______, _______, KC_LEFT, KC_DOWN, KC_UP, KC_RIGHT, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                              _______,
        _______, _______, _______,                   _______,                            _______, TG(2), _______, _______,            _______, _______, _______
    ),
    [3] = LAYOUT_tkl_ansi(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, KC_P7,   KC_P8,   KC_P9,   KC_PMNS, KC_PSLS, KC_PAST, _______,   _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, KC_P4,   KC_P5,   KC_P6,   KC_PPLS, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, KC_P1,   KC_P2,   KC_P3,    _______, _______,                              _______,
        _______, _______, _______,                   KC_0,                               KC_DOT,  TG(3), _______, _______,            _______, _______, _______
    )
    /*
    [X] = LAYOUT_tkl_ansi(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, NK_TOGG, _______, _______, _______, _______, _______,                              _______,
        _______, _______, _______,                   _______,                            _______, _______, _______, _______,            _______, _______, _______
    ),
    */
};



#ifdef _______
#undef _______
#define _______ {0, 0, 0}

const uint8_t PROGMEM ledmap[][RGB_MATRIX_LED_COUNT][3] = {
    [0] = {
        _______, CORAL,   CORAL,   _______, _______, CORAL,   CORAL,   _______, _______, CORAL,   _______, YELLOW,  YELLOW,           TEAL,    GOLD,   GOLD,
        _______, _______, PINK,    PINK,    PINK,    PINK,    _______, _______, _______, GREEN,   GREEN,   GREEN,   GREEN,   _______, TEAL,    TEAL,   TEAL,
        ORANGE,  ORANGE,  ORANGE,  ORANGE,  ORANGE,  ORANGE,  _______, AZURE,   AZURE,   _______, _______, _______, _______, _______, TEAL,    TEAL,   TEAL,
        _______, ORANGE,  ORANGE,  ORANGE,  ORANGE,  ORANGE,  _______, _______, _______, _______, _______, _______, _______,
        _______, ORANGE,  _______, CORAL,   _______, AZURE,   AZURE,   _______, _______, _______, _______, AZURE,                              SPRING,
        _______, _______, _______,                   _______,                            _______, PINK,    _______, AZURE,            _______, SPRING, _______
    },
    [1] = {
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,             BLUE, RED, GREEN, \
        _______, RED, RED, RED, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   BLUE, BLUE, BLUE, \
        _______, TEAL, TEAL, TEAL, TEAL, TEAL, _______, CORAL,CORAL,_______, GREEN, _______, _______, _______,   BLUE, BLUE, BLUE, \
        _______, TEAL, TEAL, TEAL,  TEAL, TEAL,  _______, _______, _______, _______, _______, _______, _______, \
        _______, GREEN,GREEN, GREEN, _______, CORAL, CORAL, GREEN, _______, _______, _______, _______,                             PINK, \
        _______, _______, _______, _______, _______, RED, _______, _______,            PINK, PINK, PINK \
    },
    [2] = {
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, \
   GREEN, _______, GREEN,   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, \
        _______, GREEN, GREEN, GREEN, _______, _______, GREEN, GREEN, GREEN, GREEN, _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                              _______, \
        _______, _______, _______,                   _______,                            _______, RED, _______, _______,            _______, _______, _______ \
    },
    [3] = {
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, GREEN,   GREEN,   GREEN,   TEAL, TEAL, TEAL, _______,   _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, GREEN,   GREEN,   GREEN,   TEAL, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, GREEN,   GREEN,   GREEN,    _______, _______,                              _______, \
        _______, _______, _______,                   GREEN,                               GREEN,  RED, _______, _______,            _______, _______, _______ \
    },

};

#undef _______
#define _______ KC_TRNS
#endif

bool did_leader_succeed; //150ms is the default doubletap timeout
uint16_t blink_timer;
uint16_t blinking_timer;
bool isLeaderLedOn;

// Runs just one time when the keyboard initializes.
void matrix_init_user(void) {
    idle_second_counter = 0;                            // Counter for number of seconds keyboard has been idle.
    key_event_counter = 0;                              // Counter to determine if keys are being held, neutral at 0.
    rgb_time_out_seconds = RGB_DEFAULT_TIME_OUT;        // RGB timeout initialized to its default configure in keymap.h
    rgb_time_out_enable = true;                        // Enable RGB timeout by default. Enable using toggle key.
    rgb_time_out_user_value = true;                    // Has to have the same initial value as rgb_time_out_enable.
    rgb_enabled_flag = true;                            // Initially, keyboard RGB is enabled. Change to false config.h initializes RGB disabled.
    rgb_time_out_fast_mode_enabled = false;             // RGB timeout fast mode disabled initially.
    rgb_time_out_saved_flag = rgb_matrix_get_flags();   // Save RGB matrix state for when keyboard comes back from ide.
    ld_mac = LD_MAC_DEFAULT;                            // Leader k ey default OS initialized to its default configure in keymap.h
    leader_success_blink = false;
    leader_fail_blink = false;
    blink_timer = 0;
    blinking_timer = 0;
    isLeaderLedOn = false;
};

void keyboard_post_init_user(void) {
}




// Runs constantly in the background, in a loop.
void matrix_scan_user(void) {
    if(rgb_time_out_enable && rgb_enabled_flag) {
        // If the key event counter is not zero then some key was pressed down but not released, thus reset the timeout counter.
        if (key_event_counter) {
            idle_second_counter = 0;
        } else if (timer_elapsed(idle_timer) > MILLISECONDS_IN_SECOND) {
            idle_second_counter++;
	    if (idle_second_counter % 10 == 0) dprintf("idle seconds = %d \r\n", idle_second_counter);
            idle_timer = timer_read();
        }

        if (idle_second_counter >= rgb_time_out_seconds) {
            rgb_time_out_saved_flag = rgb_matrix_get_flags();
            rgb_matrix_set_flags(LED_FLAG_NONE);
            rgb_matrix_disable_noeeprom();
            rgb_enabled_flag = false;
            idle_second_counter = 0;
        }
    }

    // LEADER_DICTIONARY() {
    //     did_leader_succeed = leading = false;

    //     SEQ_ONE_KEY(KC_Q) {
    //         if (!ld_mac) { // PC
    //             SEND_STRING(SS_LALT(SS_TAP(X_F4)));
    //         } else { // MAC
    //             SEND_STRING(SS_LGUI(SS_TAP(X_Q)));
    //         }
    //         did_leader_succeed = true;
    //     }

    //     SEQ_ONE_KEY(KC_W) {
    //         if (!ld_mac) { // PC
    //             SEND_STRING(SS_LCTRL(SS_TAP(X_F4)));
    //         } else { // MAC
    //             SEND_STRING(SS_LGUI(SS_TAP(X_W)));
    //         }
    //         did_leader_succeed = true;
    //     }

    //     SEQ_ONE_KEY(KC_T) {
    //         if (!ld_mac) { // PC
    //             SEND_STRING(SS_LCTRL(SS_TAP(X_T)));
    //         } else { // MAC
    //             SEND_STRING(SS_LGUI(SS_TAP(X_T)));
    //         }
    //         did_leader_succeed = true;
    //     }

    //     SEQ_ONE_KEY(KC_F) {
    //         if (!ld_mac) { // PC
    //             SEND_STRING(SS_TAP(X_LGUI) "firefox" SS_TAP(X_ENTER));
    //         } else { // MAC
    //             // TODO SEND_STRING(SS_LCTRL(SS_LGUI(SS_TAP(X_Q))));
    //         }
    //         did_leader_succeed = true;
    //     }

    //     SEQ_ONE_KEY(KC_LEAD) {
    //         if (layer_state_is(1)) {
    //             layer_off(1);
    //         } else {
    //             layer_on(1);
    //         }
    //     }

    //     SEQ_ONE_KEY(KC_L) {
    //         if (!ld_mac) { // PC
    //             SEND_STRING(SS_LGUI(SS_TAP(X_L)));
    //         } else { // MAC
    //             SEND_STRING(SS_LCTRL(SS_LGUI(SS_TAP(X_Q))));
    //         }
    //         did_leader_succeed = true;
    //     }

    //     SEQ_TWO_KEYS(KC_P, KC_M) {
    //         if (!ld_mac) {                                                 // PC
    //             SEND_STRING(SS_TAP(X_LGUI) "YouTube Music" SS_TAP(X_ENT)); // Run youtube music from start menu
    //             wait_ms(3000);
    //             SEND_STRING(SS_TAP(X_TAB)); // Select first playlist
    //             wait_ms(300);
    //             SEND_STRING(SS_TAP(X_SPC)); // Select first playlist
    //             wait_ms(3000);
    //             SEND_STRING(SS_LGUI(SS_TAP(X_DOWN))); // Shortcut to mimimize
    //         } else {                                  // MAC
    //             // TODO SEND_STRING(SS_LCTRL(SS_LGUI(SS_TAP(X_Q))));
    //         }
    //         did_leader_succeed = true;
    //     }

    //     leader_end();
    // }
};


// void leader_end(void) {
// 	if(did_leader_succeed) {
// 		leader_success_blink = true;
// 		blinking_timer = timer_read();
// 		isLeaderLedOn = true;
// 	} else {
// 		leader_fail_blink = true;
// 		blinking_timer = timer_read();
// 		isLeaderLedOn = true;
// 	}
// }


// void leader_end_user(void) {
//     if (leader_sequence_started) {
//         // For a single-key sequence (e.g., Lead -> A)
//         SEQ_ONE_KEY(KC_A, {
//             SEND_STRING("Hello World");
//         });

//         // For a two-key sequence (e.g., Lead -> D -> D)
//         SEQ_TWO_KEYS(KC_D, KC_D, {
//             tap_code16(LALT(KC_F4));
//         });

//         // For a three-key sequence (e.g., Lead -> J -> K -> L)
//         SEQ_THREE_KEYS(KC_J, KC_K, KC_L, {
//             // Your custom macro actions here
//         });
//     }
// }

#define MODS_SHIFT  (get_mods() & MOD_MASK_SHIFT)
#define MODS_CTRL   (get_mods() & MOD_MASK_CTRL)
#define MODS_ALT    (get_mods() & MOD_MASK_ALT)

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint32_t key_timer;

    // Increment key event counter for every press and decrement for every release.
    if (record->event.pressed) {
        key_event_counter++;
    } else {
        key_event_counter--;
    }

    if (rgb_time_out_enable) {
        idle_timer = timer_read();
        // Reset the seconds counter. Without this, something like press> leave x seconds> press, would be x seconds on the effective counter not 0 as it should.
        idle_second_counter = 0;
        if (!rgb_enabled_flag) {
            rgb_matrix_enable_noeeprom();
            rgb_matrix_set_flags(rgb_time_out_saved_flag);
            rgb_enabled_flag = true;
        }
    }

    switch (keycode) {
        case DBG_TOG:
            if (record->event.pressed) {
                TOGGLE_FLAG_AND_PRINT(debug_enable, "Debug mode");
            }
            return false;
        case DBG_MTRX:
            if (record->event.pressed) {
                TOGGLE_FLAG_AND_PRINT(debug_matrix, "Debug matrix");
            }
            return false;
        case DBG_KBD:
            if (record->event.pressed) {
                TOGGLE_FLAG_AND_PRINT(debug_keyboard, "Debug keyboard");
            }
            return false;
        case DBG_MOU:
            if (record->event.pressed) {
                TOGGLE_FLAG_AND_PRINT(debug_mouse, "Debug mouse");
            }
            return false;
        case MD_BOOT:
            if (record->event.pressed) {
                key_timer = timer_read32();
            } else {
                if (timer_elapsed32(key_timer) >= 500) {
                    reset_keyboard();
                }
            }
            return false;
        case RM_TOGG:
	    rgb_time_out_enable = rgb_time_out_user_value;
            if (record->event.pressed) {
              switch (rgb_matrix_get_flags()) {
                case LED_FLAG_ALL: {
                    rgb_matrix_set_flags(LED_FLAG_KEYLIGHT | LED_FLAG_MODIFIER | LED_FLAG_INDICATOR);
                    rgb_matrix_set_color_all(0, 0, 0);
		    dprint("RGB Toggle: Underglow LEDs off, Keycap LEDs on\r\n");
                  }
                  break;
                case (LED_FLAG_KEYLIGHT | LED_FLAG_MODIFIER | LED_FLAG_INDICATOR): {
                    rgb_matrix_set_flags(LED_FLAG_UNDERGLOW);
                    rgb_matrix_set_color_all(0, 0, 0);
		    dprint("RGB Toggle: Underglow LEDs on, Keycap LEDs off\r\n");
                  }
                  break;
                case LED_FLAG_UNDERGLOW: {
                    rgb_matrix_set_flags(LED_FLAG_NONE);
                    rgb_matrix_disable_noeeprom();
		    dprint("RGB Toggle: No LEDs on\r\n");
                  }
                  break;
                default: {
                    rgb_matrix_set_flags(LED_FLAG_ALL);
                    rgb_matrix_enable_noeeprom();
		    dprint("RGB Toggle: All LEDs on\r\n");
                  }
                  break;
              }
            }
            return false;
        case ROUT_TG:
	    if (record->event.pressed){
            // Toggle idle LED timeout on or off
	            TOGGLE_FLAG_AND_PRINT(rgb_time_out_enable, "RGB Timeout");
        	    rgb_time_out_user_value = rgb_time_out_enable;
	    }
            return false;
        case ROUT_VI:
            // Increase idle LED timeout value in seconds
            // Only increase if current value is lower than RGB_TIME_OUT_MAX. Don't care what value the result will be
            // Modity RGB_TIME_OUT_STEP for bigger or smaller increments
            if (!rgb_time_out_fast_mode_enabled && rgb_time_out_seconds <= RGB_TIME_OUT_MAX) {
                rgb_time_out_seconds += RGB_TIME_OUT_STEP;
            }
            return false;
        case ROUT_VD:
            // Decrease idle LED timeout value in seconds
            // Only decrease if current value is higher than minimum value and the result is larger than zero
            // Modity RGB_TIME_OUT_STEP for bigger or smaller decrements
            if (!rgb_time_out_fast_mode_enabled && rgb_time_out_seconds > RGB_TIME_OUT_MIN) {
                rgb_time_out_seconds -= RGB_TIME_OUT_STEP;
            }
            return false;
        case ROUT_FM:
	    if (record->event.pressed){
                if (rgb_time_out_fast_mode_enabled) {
                    rgb_time_out_seconds = rgb_time_out_saved_seconds;
                } else {
                    rgb_time_out_saved_seconds = rgb_time_out_seconds;
                    rgb_time_out_seconds = RGB_FAST_MODE_TIME_OUT;
                }
                TOGGLE_FLAG_AND_PRINT(rgb_time_out_fast_mode_enabled, "RGB Timeout Fast Mode");
	    }
            return false;
	case LD_MAC:
	    if(record->event.pressed){
		ld_mac = true;
	    }
	    return false;
	case LD_PC:
	    if(record->event.pressed){
		ld_mac = false;
	    }
	    return false;
        default:
            return true; //Process all other keycodes normally
    }
}

void set_layer_color(int layer) {
    if (layer == 0) { return; }
    for (int i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        HSV hsv = {
            .h = pgm_read_byte(&ledmap[layer][i][0]),
            .s = pgm_read_byte(&ledmap[layer][i][1]),
            .v = pgm_read_byte(&ledmap[layer][i][2]),
        };
        if (hsv.h || hsv.s || hsv.v) {
            RGB rgb = hsv_to_rgb(hsv);
            float f = (float)rgb_matrix_config.hsv.v / UINT8_MAX;
            rgb_matrix_set_color(i, f * rgb.r, f * rgb.g, f * rgb.b);
        } else {
            // If the values are all false then it's a transparent key and deactivate LED at this layer
            rgb_matrix_set_color(i, 0, 0, 0);
        }
    }
}

bool rgb_matrix_indicators_user(void) {
    if (rgb_matrix_get_suspend_state() || disable_layer_color ||
        rgb_matrix_get_flags() == LED_FLAG_NONE ||
        rgb_matrix_get_flags() == LED_FLAG_UNDERGLOW) {
            return false;
        }
    set_layer_color(get_highest_layer(layer_state));

    if (leader_success_blink || leader_fail_blink) {
        if(timer_elapsed(blink_timer) > 30) {
            isLeaderLedOn = !isLeaderLedOn;
            blink_timer = timer_read();
        }	
        if (isLeaderLedOn) {
            for (int i=0; i < RGB_MATRIX_LED_COUNT; i++) {
                if (leader_success_blink) rgb_matrix_set_color(i, 0x00, 0xFF, 0x00); 
                if (leader_fail_blink) rgb_matrix_set_color(i, 0xFF, 0xFF, 0xFF); 
            }
        } else {
            for (int i=0; i < RGB_MATRIX_LED_COUNT; i++) {
                rgb_matrix_set_color(i, 0x00, 0x00, 0x00); 
            }
	    }
    }
    
    if(timer_elapsed(blinking_timer) > 500){
	leader_success_blink = false;
	leader_fail_blink = false;
	isLeaderLedOn = false;
    }
    return false;
}
