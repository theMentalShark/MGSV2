#include <pebble.h>
#define KEY_CONDITIONS 0
#define KEY_TEMPERATURE_C 1
#define KEY_TEMPERATURE_F 2
#define KEY_POWERSAVING 3
#define KEY_IMPERIAL 4
// #define GColorGreen ((uint8_t)0b11001100
// #define GColorSpringBud (GColor8){.argb=GColorSpringBudARGB8}
// #define GColorYellow (GColor8){.argb=GColorYellowARGB8}
// #define GColorOrange (GColor8){.argb=GColorOrangeARGB8}
// #define GColorRed ((uint8_t)0b11110000)
  
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_time_sec_layer;
static TextLayer *s_am_pm_layer;
static TextLayer *s_date_layer;
static TextLayer *s_weekday_text_layer;
static Layer *s_hands_layer;
static Layer *s_battery_layer;
static Layer *s_bt_layer;
static Layer *s_setting_layer;
static bool s_bt_connection;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
static BitmapLayer *s_just_diamond_layer;
static GBitmap *s_just_diamond_bitmap;

static char p_day[1];

static GBitmap *s_diamond_white_bitmap, *s_diamond_black_bitmap;
static Layer *s_diamond_layer;

static BitmapLayer *s_day_layer;
static GBitmap *s_day_mo_bitmap;
static GBitmap *s_day_tu_bitmap;
static GBitmap *s_day_we_bitmap;
static GBitmap *s_day_th_bitmap;
static GBitmap *s_day_fr_bitmap;
static GBitmap *s_day_sa_bitmap;
static GBitmap *s_day_su_bitmap;

static GFont s_time_font;
static GFont s_letter_font;
static GFont s_time_mid_font;
static GFont s_time_small_font;

static int s_battery_level;
static TextLayer *s_weather_layer;
static bool s_power_saving = false;
static bool s_temp_format_f = false;


static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char temperature_f_buffer[8];
  static char temperature_c_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];
  
//   Tuple *power_saving_t = dict_find(iterator, KEY_POWERSAVING);
//   Tuple *temp_formate_f_t = dict_find(iterator, KEY_IMPERIAL);
  
//   s_power_saving = power_saving_t->value->int8;
//   s_temp_format_f = temp_formate_f_t->value->int8;
  
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_TEMPERATURE_F:
      snprintf(temperature_f_buffer, sizeof(temperature_f_buffer), "%dF", (int)t->value->int32);
      break;
    case KEY_TEMPERATURE_C:
      snprintf(temperature_c_buffer, sizeof(temperature_c_buffer), "%dC", (int)t->value->int32);
      break;
    case KEY_CONDITIONS:
      snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
      break;
    case KEY_POWERSAVING:
      if((int)t->value->int8 < 109){
        s_power_saving = false;
        persist_write_bool(KEY_POWERSAVING, false);
      }else{
        s_power_saving = true;
        persist_write_bool(KEY_POWERSAVING, true);
      }
      break;
    case KEY_IMPERIAL:      
      if((int)t->value->int8 < 109){
        s_temp_format_f = false;
        persist_write_bool(KEY_IMPERIAL, false);
      }else{
        s_temp_format_f = true;
        persist_write_bool(KEY_IMPERIAL, true);
      }
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }
  
  // Assemble full string and display
  if(s_temp_format_f){
    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s,%s", temperature_f_buffer, conditions_buffer);
  }else{
    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s,%s", temperature_c_buffer, conditions_buffer);
  }
  text_layer_set_text(s_weather_layer, weather_layer_buffer);
}



static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}


static void bluetooth_callback(bool connected) {
   s_bt_connection = connected;
}

static void battery_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  // Find the width of the bar
  int width = (int)(float)(((float)s_battery_level / 100.0F) * bounds.size.w);

  // Draw the background
//   graphics_context_set_fill_color(ctx, GColorWhite);
//   graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  // Draw the bar
  
// #define GColorGreen (GColor8){.argb=GColorGreenARGB8}
// #define GColorSpringBud (GColor8){.argb=GColorSpringBudARGB8}
// #define GColorYellow (GColor8){.argb=GColorYellowARGB8}
// #define GColorOrange (GColor8){.argb=GColorOrangeARGB8}
// #define GColorRed (GColor8){.argb=GColorRedARGB8}
//   if(s_battery_level > 80){
//     graphics_context_set_fill_color(ctx, GColorGreen);
//   }else if(s_battery_level > 60){
//     graphics_context_set_fill_color(ctx, GColorSpringBud);
//   }else if(s_battery_level > 40){
//     graphics_context_set_fill_color(ctx, GColorYellow);
//   }else if(s_battery_level > 20){
//     graphics_context_set_fill_color(ctx, GColorOrange);
//   }else{
//     graphics_context_set_fill_color(ctx, GColorRed);
//   }
  
    graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(0, 0, width, bounds.size.h), 0, GCornerNone);
}



static void setting_update_proc(Layer *layer, GContext *ctx) {

  // Find the width of the bar

  // Draw the background
  if(s_power_saving){
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, GRect(0, 9, 16, 2), 0, GCornerNone);
  }
  if(s_temp_format_f){
    
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, GRect(0, 0, 16, 2), 0, GCornerNone);
  }
  // Draw the bar
//   graphics_context_set_fill_color(ctx, GColorBlack);
//   graphics_fill_rect(ctx, GRect(0, 0, width, bounds.size.h), 0, GCornerNone);
}

static void bt_update_proc(Layer *layer, GContext *ctx) {

  // Find the width of the bar

  // Draw the background
  if(s_bt_connection){
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, GRect(0, 0, 16, 2), 0, GCornerNone);
  }
  // Draw the bar
//   graphics_context_set_fill_color(ctx, GColorBlack);
//   graphics_fill_rect(ctx, GRect(0, 0, width, bounds.size.h), 0, GCornerNone);
}

static void battery_callback(BatteryChargeState state) {
  // Record the new battery level
  s_battery_level = state.charge_percent;
  // Update meter
  layer_mark_dirty(s_battery_layer);
}

static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  int16_t second_hand_length = bounds.size.w / 1.5;

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  if(s_power_saving){
    for(int i = 0; i < 60; i++) {
      int32_t second_angle = TRIG_MAX_ANGLE * i / 60;
      GPoint second_hand = {
        .x = (int16_t)(sin_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.x,
        .y = (int16_t)(-cos_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.y,
      };
    
      // second hand
//       graphics_context_set_stroke_width(ctx, 4);
      graphics_context_set_stroke_color(ctx, GColorBlack);
      graphics_draw_line(ctx, second_hand, center);
    }
  }else{
    for(int i = 0; i < t->tm_sec + 1; i++) {
      int32_t second_angle = TRIG_MAX_ANGLE * i / 60;
      GPoint second_hand = {
        .x = (int16_t)(sin_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.x,
        .y = (int16_t)(-cos_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.y,
      };
    
      // second hand
      graphics_context_set_stroke_color(ctx, GColorBlack);
      graphics_draw_line(ctx, second_hand, center);
    }
  }
}


static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  
  
  //second displaying
  if(s_power_saving){
    //create another buffer
    static char sec_buffer[] = "  ";
    text_layer_set_text(s_time_sec_layer, sec_buffer);
    //display second
  }else{
    //create another buffer
    static char sec_buffer[] = "00";
    //write current sec to buffer
    strftime(sec_buffer, sizeof("00"), "%S", tick_time);
    text_layer_set_text(s_time_sec_layer, sec_buffer);
    //display second
  }
  
  //display am pm
  if(clock_is_24h_style() == true) {
    
    static char am_buffer[] = "  ";
    text_layer_set_text(s_am_pm_layer, am_buffer);
    // Use 24 hour format
  } else {
    
    // Use 12 hour format
    //create another buffer
    static char am_buffer[] = "AM";
    strftime(am_buffer, sizeof("AM"), "%p", tick_time);
    text_layer_set_text(s_am_pm_layer, am_buffer);
  // Write the current hours and minutes into the buffer
  }
  //display second
  
  //create another buffer
  static char date_buffer[] = "0000-000-00";
  //write current date to buffer
  strftime(date_buffer, sizeof("0000-000-00"), "%Y %h %d", tick_time);
  //display second
  text_layer_set_text(s_date_layer, date_buffer);
  
  //create another buffer
//   static char day_buffer[] = "wed";
  //write current date to buffer
//   strftime(day_buffer, sizeof("wed"), "%a", tick_time);
//   day_buffer[2] = (char);
//   strcpy(day_buffer[2], " ");
  //display second
  static char day_buffer[3];
  static char weekday_buffer[] = "1";
  //write current sec to buffer
  strftime(weekday_buffer, sizeof("1"), "%u", tick_time);
  if(weekday_buffer[0] != p_day[0]){
    if(weekday_buffer[0] == '1'){
      bitmap_layer_set_bitmap(s_day_layer, s_day_mo_bitmap);
//       day_buffer = "MO";
      strcpy(day_buffer,"mo");
    }else if(weekday_buffer[0] == '2'){
      bitmap_layer_set_bitmap(s_day_layer, s_day_tu_bitmap);
      strcpy(day_buffer,"tu");
    }else if(weekday_buffer[0] == '3'){
      bitmap_layer_set_bitmap(s_day_layer, s_day_we_bitmap);
      strcpy(day_buffer,"we");
    }else if(weekday_buffer[0] == '4'){
      bitmap_layer_set_bitmap(s_day_layer, s_day_th_bitmap);
      strcpy(day_buffer,"th");
    }else if(weekday_buffer[0] == '5'){
      bitmap_layer_set_bitmap(s_day_layer, s_day_fr_bitmap);
      strcpy(day_buffer,"fr");
    }else if(weekday_buffer[0] == '6'){
      bitmap_layer_set_bitmap(s_day_layer, s_day_sa_bitmap);
      strcpy(day_buffer,"sa");
    }else if(weekday_buffer[0] == '7'){
      bitmap_layer_set_bitmap(s_day_layer, s_day_su_bitmap);
      strcpy(day_buffer,"su");
    }
    p_day[0] = weekday_buffer[0];
  }
  text_layer_set_text(s_weekday_text_layer, day_buffer);
}

static void layer_update_proc(Layer *layer, GContext *ctx) {
  // Draw white
  graphics_context_set_compositing_mode(ctx, GCompOpOr);
  graphics_draw_bitmap_in_rect(ctx, s_diamond_white_bitmap, gbitmap_get_bounds(s_diamond_white_bitmap));

  // Draw black
//   graphics_context_set_compositing_mode(ctx, GCompOpClear);
//   graphics_draw_bitmap_in_rect(ctx, s_diamond_black_bitmap, gbitmap_get_bounds(s_diamond_black_bitmap));
}

static void main_window_load(Window *window){
  // Create canvas Layer
  s_diamond_layer = layer_create(GRect(11, 16, 95, 95));
  layer_set_update_proc(s_diamond_layer, layer_update_proc);
  // Create GBitmap, then set to created BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND_MGSV);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  // Create GBitmaps
  s_diamond_white_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIAMOND_WHITE);
  s_diamond_black_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIAMOND_BLACK);
  
  s_hands_layer = layer_create(GRect(11, 16, 95, 95));
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(window_get_root_layer(window), s_hands_layer);
  layer_add_child(window_get_root_layer(window), s_diamond_layer);
  
  
  
  s_just_diamond_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_JUST_THE_DIAMOND);
  s_just_diamond_layer = bitmap_layer_create(GRect(46, 51, 25, 25));
  bitmap_layer_set_bitmap(s_just_diamond_layer, s_just_diamond_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_just_diamond_layer));
  
  
  //day layer, load all bitmap
//   s_day_mo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MO);
//   s_day_tu_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TU);
//   s_day_we_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WE);
//   s_day_th_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TH);
//   s_day_fr_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FR);
//   s_day_sa_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SA);
//   s_day_su_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SU);
//   s_day_layer = bitmap_layer_create(GRect(98, 88, 35, 20));
//   bitmap_layer_set_bitmap(s_day_layer, s_day_mo_bitmap);
//   layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_day_layer));
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(34, 70, 150, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  
  s_time_sec_layer = text_layer_create(GRect(65, 22, 150, 40));
  text_layer_set_background_color(s_time_sec_layer, GColorClear);
  text_layer_set_text_color(s_time_sec_layer, GColorBlack);
  text_layer_set_text(s_time_sec_layer, "00");
  
  s_am_pm_layer = text_layer_create(GRect(21,70,20,15));
  text_layer_set_background_color(s_am_pm_layer, GColorClear);
  text_layer_set_text_color(s_am_pm_layer, GColorBlack);
  text_layer_set_text(s_am_pm_layer, "AM");
  
  s_date_layer = text_layer_create(GRect(7,120,140,30));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_text(s_date_layer, "2015 sep 16");
  
  // Create temperature Layer
  s_weather_layer = text_layer_create(GRect(77,-8,50,15));
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_color(s_weather_layer, GColorWhite);
  text_layer_set_text(s_weather_layer, "Loading...");
  
  s_weekday_text_layer = text_layer_create(GRect(115, 100, 50, 30));
  text_layer_set_background_color(s_weekday_text_layer, GColorClear);
  text_layer_set_text_color(s_weekday_text_layer, GColorBlack);
  text_layer_set_text(s_weekday_text_layer, "fr");
  
  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIGIT_SEVEN_30));
  s_time_mid_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIGIT_SEVEN_25));
  s_time_small_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIGIT_SEVEN_REG_15));
  s_letter_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SMALL_PIXEL_15));

  // Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_font(s_time_sec_layer, s_time_mid_font);
  text_layer_set_font(s_am_pm_layer, s_letter_font);
  text_layer_set_font(s_weekday_text_layer, s_time_mid_font);
  text_layer_set_font(s_date_layer, s_time_font);
  text_layer_set_font(s_weather_layer, s_letter_font);

  // Improve the layout to be more like a watchface
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
  text_layer_set_text_alignment(s_time_sec_layer, GTextAlignmentLeft);
  text_layer_set_text_alignment(s_am_pm_layer, GTextAlignmentLeft);
  text_layer_set_text_alignment(s_weekday_text_layer, GTextAlignmentLeft);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentLeft);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentRight);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_sec_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_am_pm_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
//   layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weekday_text_layer));
  
  
  // Create battery meter Layer
  s_battery_layer = layer_create(GRect(118, 76, 20, 11));
  layer_set_update_proc(s_battery_layer, battery_update_proc);

  s_bt_layer = layer_create(GRect(122, 30, 16, 2));
  layer_set_update_proc(s_bt_layer, bt_update_proc);
  
  s_setting_layer = layer_create(GRect(122, 39, 17, 12));
  layer_set_update_proc(s_setting_layer, setting_update_proc);
  
  
  // Add to Window
  layer_add_child(window_get_root_layer(window), s_battery_layer);
//   layer_add_child(window_get_root_layer(window), s_bt_layer);
//   layer_add_child(window_get_root_layer(window), s_setting_layer);
  
  s_power_saving = persist_read_bool(KEY_POWERSAVING);
  s_temp_format_f = persist_read_bool(KEY_IMPERIAL);
  
  
//   layer_mark_dirty(s_setting_layer);
  // Make sure the time is displayed from the start
  update_time();
}


static void main_window_unload(Window *window){
  
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_time_sec_layer);
  text_layer_destroy(s_am_pm_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_weather_layer);
  text_layer_destroy(s_weekday_text_layer);
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_time_mid_font);
  fonts_unload_custom_font(s_time_small_font);
  fonts_unload_custom_font(s_letter_font);
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);
  gbitmap_destroy(s_day_mo_bitmap);
  gbitmap_destroy(s_day_tu_bitmap);
  gbitmap_destroy(s_day_we_bitmap);
  gbitmap_destroy(s_day_th_bitmap);
  gbitmap_destroy(s_day_fr_bitmap);
  gbitmap_destroy(s_day_sa_bitmap);
  gbitmap_destroy(s_day_su_bitmap);
  gbitmap_destroy(s_diamond_white_bitmap);
  gbitmap_destroy(s_diamond_black_bitmap);
  gbitmap_destroy(s_just_diamond_bitmap);
  
  
  layer_destroy(s_hands_layer);
  layer_destroy(s_diamond_layer);
  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  bitmap_layer_destroy(s_day_layer);
  bitmap_layer_destroy(s_just_diamond_layer);
  layer_destroy(s_battery_layer);
  layer_destroy(s_bt_layer);
  layer_destroy(s_setting_layer);
  
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  // Get weather update every 30 minutes
  if(tick_time->tm_min % 30 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
  }
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  if(s_power_saving){
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  }else{
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  }
  battery_state_service_subscribe(battery_callback);
  
  // Ensure battery level is displayed from the start
  battery_callback(battery_state_service_peek());
  // Register for Bluetooth connection updates
  bluetooth_connection_service_subscribe(bluetooth_callback);
  // Show the correct state of the BT connection from the start
  bluetooth_callback(bluetooth_connection_service_peek());
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  

}

static void deinit() {
    // Destroy Window
    window_destroy(s_main_window);
}


int main(void) {
  init();
  app_event_loop();
  deinit();
}