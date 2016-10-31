#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont s_time_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
/* Comment. */

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M": "%I:%M", tick_time);
  
  text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Create GBitmap
 s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CIRCLE_DIAL);

 // Create BitmapLayer to display the GBitmap
 s_background_layer = bitmap_layer_create(bounds);

 // Set the bitmap onto the layer and add to the window
 bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
 layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
  
  s_time_layer = text_layer_create(
  GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
  
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_RINGBEARER_42));
  
  
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
  
  fonts_unload_custom_font(s_time_font);
  text_layer_destroy(s_time_layer);
  
  // Destroy GBitmap
gbitmap_destroy(s_background_bitmap);

// Destroy BitmapLayer
bitmap_layer_destroy(s_background_layer);
}

static void init() {
  s_main_window = window_create();
  
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_stack_push(s_main_window, true);

    
  update_time();
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  //window_set_background_color(s_main_window, GColorBlack);

}

static void deinit() {
  window_destroy(s_main_window);
  
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
