#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "config.h"

// This is the Default APP_ID to work with old versions of httpebble
//#define MY_UUID { 0x91, 0x41, 0xB6, 0x28, 0xBC, 0x89, 0x49, 0x8E, 0xB1, 0x47, 0x04, 0x9F, 0x49, 0xC0, 0x99, 0xAD }

//#define MY_UUID { 0x91, 0x41, 0xB6, 0x28, 0xBC, 0x89, 0x49, 0x8E, 0xB1, 0x47, 0x29, 0x08, 0xF1, 0x7C, 0x3F, 0xAC}
// Vic's
#define MY_UUID { 0x40, 0xCE, 0x5C, 0x4A, 0xB7, 0x53, 0x40, 0xAF, 0x9F, 0xCF, 0x0E, 0x57, 0xDC, 0xE1, 0x9A, 0x0E }

#define DATE_Y 83
#define TIME_Y 107
	
#define TIMEZONE_ONE_Y 11
#define TIMEZONE_TWO_Y 45
	
#define HOUR_VIBRATION_START 8
#define HOUR_VIBRATION_END 21
	
#define TIMEZONE_LOCAL_OFFSET (-7)

#define TIMEZONE_ONE_NAME "CHN"
#define TIMEZONE_ONE_OFFSET (+8)

#define TIMEZONE_TWO_NAME "GER" 
#define TIMEZONE_TWO_OFFSET (+2)

	
PBL_APP_INFO(MY_UUID,
	     "Simple Vic 91", "rfrcarvalho & Vic",
	     1, 0, /* App major/minor version */
	     RESOURCE_ID_IMAGE_MENU_ICON,
	     APP_INFO_WATCH_FACE);

Window window;
TextLayer DayOfWeekLayer;
BmpContainer background_image;
BmpContainer time_format_image;

TextLayer timezone_one;
TextLayer timezone_two;
TextLayer tzOneLayer;
TextLayer tzTwoLayer;

const int DATENUM_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_DATENUM_0,
  RESOURCE_ID_IMAGE_DATENUM_1,
  RESOURCE_ID_IMAGE_DATENUM_2,
  RESOURCE_ID_IMAGE_DATENUM_3,
  RESOURCE_ID_IMAGE_DATENUM_4,
  RESOURCE_ID_IMAGE_DATENUM_5,
  RESOURCE_ID_IMAGE_DATENUM_6,
  RESOURCE_ID_IMAGE_DATENUM_7,
  RESOURCE_ID_IMAGE_DATENUM_8,
  RESOURCE_ID_IMAGE_DATENUM_9
};

#define TOTAL_DATE_DIGITS 8
BmpContainer date_digits_images[TOTAL_DATE_DIGITS];

const int BIG_DIGIT_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_NUM_0,
  RESOURCE_ID_IMAGE_NUM_1,
  RESOURCE_ID_IMAGE_NUM_2,
  RESOURCE_ID_IMAGE_NUM_3,
  RESOURCE_ID_IMAGE_NUM_4,
  RESOURCE_ID_IMAGE_NUM_5,
  RESOURCE_ID_IMAGE_NUM_6,
  RESOURCE_ID_IMAGE_NUM_7,
  RESOURCE_ID_IMAGE_NUM_8,
  RESOURCE_ID_IMAGE_NUM_9
};

#define TOTAL_TIME_DIGITS 4
BmpContainer time_digits_images[TOTAL_TIME_DIGITS];

// Vic's
BmpContainer tzOne_digits_images[TOTAL_TIME_DIGITS];
BmpContainer tzTwo_digits_images[TOTAL_TIME_DIGITS];
BmpContainer tzMore[2];

void set_container_image(BmpContainer *bmp_container, const int resource_id, GPoint origin) {
  layer_remove_from_parent(&bmp_container->layer.layer);
  bmp_deinit_container(bmp_container);

  bmp_init_container(resource_id, bmp_container);

  GRect frame = layer_get_frame(&bmp_container->layer.layer);
  frame.origin.x = origin.x;
  frame.origin.y = origin.y;
  layer_set_frame(&bmp_container->layer.layer, frame);

  layer_add_child(&window.layer, &bmp_container->layer.layer);
}

unsigned short get_display_hour(unsigned short hour) {
  if (clock_is_24h_style()) {
    return hour;
  }
	
  if (hour == 12) 
	  return hour;

  unsigned short display_hour = hour % 12;
  return display_hour;

}

unsigned short the_last_hour = 25;

void update_display(PblTm *current_time) {
  
  unsigned short display_hour = get_display_hour(current_time->tm_hour);
  short tzOne_hour = current_time->tm_hour + (TIMEZONE_ONE_OFFSET - TIMEZONE_LOCAL_OFFSET);
  short tzTwo_hour = current_time->tm_hour + (TIMEZONE_TWO_OFFSET - TIMEZONE_LOCAL_OFFSET);
	
	if (tzOne_hour >= 24)	tzOne_hour -= 24;
	if (tzOne_hour <   0)	tzOne_hour += 24;
	if (tzTwo_hour >= 24)	tzTwo_hour -= 24;
	if (tzTwo_hour <   0)	tzTwo_hour += 24;	
  
  //Hour
  if (display_hour/10 != 0) {
  	set_container_image(&time_digits_images[0], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour/10], GPoint(4, TIME_Y));
  }
  set_container_image(&time_digits_images[1], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour%10], GPoint(37, TIME_Y));
	
  if(current_time->tm_min == 0
      && current_time->tm_hour >= HOUR_VIBRATION_START
      && current_time->tm_hour <= HOUR_VIBRATION_END)
  {
      vibes_double_pulse();
  }
	
  if(current_time->tm_min == 0 && current_time->tm_hour == 0)
  {
      vibes_double_pulse();
  }
	
  
  //Minute
  set_container_image(&time_digits_images[2], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min/10], GPoint(80, TIME_Y));
  set_container_image(&time_digits_images[3], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min%10], GPoint(111, TIME_Y));
	
	
	// Vic's
	if (tzOne_hour/10 != 0) {
		set_container_image(&tzOne_digits_images[0], DATENUM_IMAGE_RESOURCE_IDS[tzOne_hour/10], GPoint(71, TIMEZONE_ONE_Y));
	} else {
		layer_remove_from_parent(&tzOne_digits_images[0].layer.layer);
		bmp_deinit_container(&tzOne_digits_images[0]);
	}
	set_container_image(&tzOne_digits_images[1], DATENUM_IMAGE_RESOURCE_IDS[tzOne_hour%10], GPoint(83, TIMEZONE_ONE_Y));
	
	set_container_image(&tzOne_digits_images[2], DATENUM_IMAGE_RESOURCE_IDS[current_time->tm_min/10], GPoint(103, TIMEZONE_ONE_Y));
	set_container_image(&tzOne_digits_images[3], DATENUM_IMAGE_RESOURCE_IDS[current_time->tm_min%10], GPoint(115, TIMEZONE_ONE_Y));
		
	
	if (tzTwo_hour/10 != 0) {
		//set_container_image(&tzTwo_digits_images[0], DATENUM_IMAGE_RESOURCE_IDS[1], GPoint(71, TIMEZONE_TWO_Y));
		set_container_image(&tzTwo_digits_images[2], DATENUM_IMAGE_RESOURCE_IDS[tzTwo_hour/10], GPoint(71, TIMEZONE_TWO_Y));
	} else {
		layer_remove_from_parent(&tzTwo_digits_images[2].layer.layer);
		bmp_deinit_container(&tzTwo_digits_images[2]);
	}
	//set_container_image(&tzTwo_digits_images[1], DATENUM_IMAGE_RESOURCE_IDS[1], GPoint(83, TIMEZONE_TWO_Y));
	set_container_image(&tzTwo_digits_images[3], DATENUM_IMAGE_RESOURCE_IDS[tzTwo_hour%10], GPoint(83, TIMEZONE_TWO_Y));
	
	//text_layer_set_text(&tzTwoLayer, VIC_HOURS[tzTwo_hour]);
	
	set_container_image(&tzMore[0], DATENUM_IMAGE_RESOURCE_IDS[current_time->tm_min/10], GPoint(103, TIMEZONE_TWO_Y));
	set_container_image(&tzMore[1], DATENUM_IMAGE_RESOURCE_IDS[current_time->tm_min%10], GPoint(115, TIMEZONE_TWO_Y));
	
   
  if (the_last_hour != display_hour){
	  
	  // Day of week
	  text_layer_set_text(&DayOfWeekLayer, DAY_NAME_LANGUAGE[current_time->tm_wday]); 
	  
	  // Day
	  if (current_time->tm_mday/10 != 0) {
	  	set_container_image(&date_digits_images[0], DATENUM_IMAGE_RESOURCE_IDS[current_time->tm_mday/10], GPoint(day_month_x[0], DATE_Y));
	  }
	  set_container_image(&date_digits_images[1], DATENUM_IMAGE_RESOURCE_IDS[current_time->tm_mday%10], GPoint(day_month_x[0] + 13, DATE_Y));
	  
	  // Month
	  if ((current_time->tm_mon+1)/10 != 0) {
	    set_container_image(&date_digits_images[2], DATENUM_IMAGE_RESOURCE_IDS[(current_time->tm_mon+1)/10], GPoint(day_month_x[1], DATE_Y));
	  }
	  set_container_image(&date_digits_images[3], DATENUM_IMAGE_RESOURCE_IDS[(current_time->tm_mon+1)%10], GPoint(day_month_x[1] + 13, DATE_Y));
	  
	  // Year
	  set_container_image(&date_digits_images[4], DATENUM_IMAGE_RESOURCE_IDS[((1900+current_time->tm_year)%1000)/10], GPoint(day_month_x[2], DATE_Y));
	  set_container_image(&date_digits_images[5], DATENUM_IMAGE_RESOURCE_IDS[((1900+current_time->tm_year)%1000)%10], GPoint(day_month_x[2] + 13, DATE_Y));
		
	  
	  if (!clock_is_24h_style()) {
		if (current_time->tm_hour >= 12) {
		  set_container_image(&time_format_image, RESOURCE_ID_IMAGE_PM_MODE, GPoint(120, 154));
		} else {
		  set_container_image(&time_format_image, RESOURCE_ID_IMAGE_AM_MODE, GPoint(120, 154));
		}

		if (display_hour/10 == 0) {
		  layer_remove_from_parent(&time_digits_images[0].layer.layer);
		  bmp_deinit_container(&time_digits_images[0]);
		}
	  }
	  
	  the_last_hour = display_hour;
  }
	
}


void handle_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)ctx;	
	
	if (t->tick_time->tm_sec == 0) {
		update_display(t->tick_time);
	}
	
}

void handle_init(AppContextRef ctx) {
  (void)ctx;

	window_init(&window, "Simple Vic 91");
	window_stack_push(&window, true /* Animated */);
  
	window_set_background_color(&window, GColorBlack);
  
	resource_init_current_app(&APP_RESOURCES);

	bmp_init_container(RESOURCE_ID_IMAGE_BACKGROUND, &background_image);
	layer_add_child(&window.layer, &background_image.layer.layer);
	
	if (clock_is_24h_style()) {
		bmp_init_container(RESOURCE_ID_IMAGE_24_HOUR_MODE, &time_format_image);

		time_format_image.layer.layer.frame.origin.x = 120;
		time_format_image.layer.layer.frame.origin.y = 154;

		layer_add_child(&window.layer, &time_format_image.layer.layer);
	}

	// Day of week text
	text_layer_init(&DayOfWeekLayer, GRect(5, 74, 130 /* width */, 30 /* height */));
	layer_add_child(&background_image.layer.layer, &DayOfWeekLayer.layer);
	text_layer_set_text_color(&DayOfWeekLayer, GColorWhite);
	text_layer_set_background_color(&DayOfWeekLayer, GColorClear);
	text_layer_set_font(&DayOfWeekLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	
	// Timezone one
	text_layer_init(&tzOneLayer, GRect(15, 0, 55 /* width */, 30 /* height */));
	layer_add_child(&background_image.layer.layer, &tzOneLayer.layer);
	text_layer_set_text_color(&tzOneLayer, GColorWhite);
	text_layer_set_background_color(&tzOneLayer, GColorClear);
	text_layer_set_font(&tzOneLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
	
	// Timezone two
	text_layer_init(&tzTwoLayer, GRect(15, 34, 55 /* width */, 30 /* height */));
	layer_add_child(&background_image.layer.layer, &tzTwoLayer.layer);
	text_layer_set_text_color(&tzTwoLayer, GColorWhite);
	text_layer_set_background_color(&tzTwoLayer, GColorClear);
	text_layer_set_font(&tzTwoLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
	

	
    // Avoids a blank screen on watch start.
    PblTm tick_time;

    get_time(&tick_time);
    update_display(&tick_time);
	
	text_layer_set_text(&tzOneLayer, TIMEZONE_ONE_NAME); 
	text_layer_set_text(&tzTwoLayer, TIMEZONE_TWO_NAME); 

}


void handle_deinit(AppContextRef ctx) {
	(void)ctx;

	bmp_deinit_container(&background_image);
	bmp_deinit_container(&time_format_image);

	for (int i = 0; i < TOTAL_DATE_DIGITS; i++) {
		bmp_deinit_container(&date_digits_images[i]);
	}
	
	for (int i = 0; i < TOTAL_TIME_DIGITS; i++) {
		bmp_deinit_container(&time_digits_images[i]);
		bmp_deinit_container(&tzOne_digits_images[i]);
	}
	
	bmp_deinit_container(&tzTwo_digits_images[2]);
	bmp_deinit_container(&tzTwo_digits_images[3]);
	
	bmp_deinit_container(&tzMore[0]);
	bmp_deinit_container(&tzMore[1]);	
	
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,
    .tick_info = {
      .tick_handler = &handle_tick,
      .tick_units = MINUTE_UNIT
    },
	.messaging_info = {
		.buffer_sizes = {
			.inbound = 124,
			.outbound = 256,
		}
	}
  };
  app_event_loop(params, &handlers);
}