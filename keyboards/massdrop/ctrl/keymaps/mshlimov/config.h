#pragma once

#undef RGB_MATRIX_STARTUP_MODE
#define RGB_MATRIX_STARTUP_MODE RGB_MATRIX_ALPHAS_MODS // Sets the default mode, if none has been set
#define RGB_MATRIX_STARTUP_HUE 1 // Sets the default hue value, if none has been set
#define RGB_MATRIX_STARTUP_SAT 255 // Sets the default saturation value, if none has been set
#define RGB_MATRIX_STARTUP_VAL 255 // Sets the default brightness value, if none has been set
#define RGB_MATRIX_STARTUP_SPD 127 // Sets the default animation speed, if none has been set

#define LEADER_TIMEOUT 500 // Leader key processes for 300ms
#define LEADER_PET_KEY_TIMING // reset leaderkey timeout on  each key press

#define COMBO_COUNT 1
