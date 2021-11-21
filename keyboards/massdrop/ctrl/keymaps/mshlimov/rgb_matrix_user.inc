// !!! DO NOT ADD #pragma once !!! //

// Step 1.
// Declare custom effects using the RGB_MATRIX_EFFECT macro
// (note the lack of semicolon after the macro!)
RGB_MATRIX_EFFECT(my_cool_effect)
RGB_MATRIX_EFFECT(my_cool_effect2)

// Step 2.
// Define effects inside the `RGB_MATRIX_CUSTOM_EFFECT_IMPLS` ifdef block
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

// e.g: A simple effect, self-contained within a single method
static bool my_cool_effect(effect_params_t* params) {
  RGB_MATRIX_USE_LIMITS(led_min, led_max);
  for (uint8_t i = led_min; i < led_max; i++) {
    rgb_matrix_set_color(i, 0xff, 0xff, 0x00);
  }
  return led_max < DRIVER_LED_TOTAL;
}





// e.g: A more complex effect, relying on external methods and state, with
// dedicated init and run methods
static uint8_t counter;
static void my_cool_effect2_complex_init(effect_params_t* params) {
  counter = 0;
}

static bool my_cool_effect2_complex_run(effect_params_t* params) {
  RGB_MATRIX_USE_LIMITS(led_min, led_max);
  RGB rgb = rgb_matrix_hsv_to_rgb(rgb_matrix_config.hsv);



  if(scale16by8(g_rgb_timer, rgb_matrix_config.speed)) {
	  for (uint8_t i = led_min; i < led_max; i++) {
	    RGB_MATRIX_TEST_LED_FLAGS();
	    if (i == counter) {
	    	rgb_matrix_set_color(i, 0, 0, 0);
	    } else {
	    	rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
	    }
	  }

	  counter += 1;
	  counter %= DRIVER_LED_TOTAL;
  }
  return led_max < DRIVER_LED_TOTAL;
}

static bool my_cool_effect2(effect_params_t* params) {
  if (params->init) my_cool_effect2_complex_init(params);
  return my_cool_effect2_complex_run(params);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
