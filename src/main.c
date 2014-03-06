#include <pebble.h>
#include <stdlib.h>
#include <string.h>
	
Window *window;
TextLayer *time_layer;
TextLayer *date_layer;
TextLayer *wday_layer;
TextLayer *bat_layer;
TextLayer *ampm_layer;
char time_buffer[] = "00:00";
char date_buffer[] = "DD de MMMNNNNNN";
char bat_buffer[10];
InverterLayer *inv_layer;

GBitmap *bt_bitmap;
BitmapLayer *bt_layer;
GBitmap *btc_bitmap;
BitmapLayer *btc_layer;

const char* const WEEKDAYS[] = {
  "Domingo",
  "Segunda",
  "Terça",
  "Quarta",
  "Quinta",
  "Sexta",
  "Sábado"
};

const char* const MONTHS[] = {
	"Janeiro",
	"Fevereiro",
	"Março",
	"Abril",
	"Maio",
	"Junho",
	"Julho",
	"Agosto",
	"Setembro",
	"Outubro",
	"Novembro",
	"Dezembro"
};

#define INT_DIGITS 2
	
char *itoa(int i)
{
  /* Room for INT_DIGITS digits, - and '\0' */
  static char buf[INT_DIGITS + 2];
  char *p = buf + INT_DIGITS + 1;        /* points to terminating '\0' */
  if (i >= 0) {
    do {
      *--p = '0' + (i % 10);
      i /= 10;
    } while (i != 0);
    return p;
  }
  else {                        /* i < 0 */
    do {
      *--p = '0' - (i % 10);
      i /= 10;
    } while (i != 0);
    *--p = '-';
  }
  return p;
}

void bluetooth_handler(bool connected){
	layer_set_hidden((Layer*) btc_layer, connected);
}

void battery_handler(BatteryChargeState charge){
	strcpy(bat_buffer, "");
	if(charge.is_charging)
		strcat(bat_buffer, "A carr: ");
	strcat(bat_buffer, itoa(charge.charge_percent));
	strcat(bat_buffer, "%");
	text_layer_set_text(bat_layer, bat_buffer);
}

void minute_handler(struct tm *tick_time, TimeUnits units_changed){
	int hour = tick_time->tm_hour;
	if(!clock_is_24h_style()){
		hour = hour % 12;
		if(hour == 0) hour = 12;
		if(tick_time->tm_hour < 12)
			text_layer_set_text(ampm_layer, "AM");
		else text_layer_set_text(ampm_layer, "PM");	
	}
	strcpy(time_buffer, itoa(hour));
	strcat(time_buffer, ":");
	if(tick_time->tm_min < 10)
		strcat(time_buffer, "0");
	strcat(time_buffer, itoa(tick_time->tm_min));
    text_layer_set_text(time_layer, time_buffer);
	strcpy(date_buffer, itoa(tick_time->tm_mday));
	strcat(date_buffer, " de ");
	strcat(date_buffer, MONTHS[tick_time->tm_mon]);
	text_layer_set_text(date_layer, date_buffer);
	text_layer_set_text(wday_layer, WEEKDAYS[tick_time->tm_wday]);
}

void window_load(Window *window){
  //We will add the creation of the Window's elements here soon!
	
	time_layer = text_layer_create(GRect(0, 51, 144, 44));
	text_layer_set_background_color(time_layer, GColorClear);
	text_layer_set_text_color(time_layer, GColorBlack);
	text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
	text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	 
	layer_add_child(window_get_root_layer(window), (Layer*) time_layer);
	
	ampm_layer = text_layer_create(GRect(0, 96, 144, 15));
	text_layer_set_background_color(ampm_layer, GColorClear);
	text_layer_set_text_color(ampm_layer, GColorBlack);
	text_layer_set_text_alignment(ampm_layer, GTextAlignmentCenter);
	text_layer_set_font(ampm_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	
	layer_add_child(window_get_root_layer(window), (Layer*) ampm_layer);
	
	date_layer = text_layer_create(GRect(0, 108, 144, 28));
	text_layer_set_background_color(date_layer, GColorClear);
	text_layer_set_text_color(date_layer, GColorBlack);
	text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
	text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
	 
	layer_add_child(window_get_root_layer(window), (Layer*) date_layer);
	
	wday_layer = text_layer_create(GRect(0, 130, 144, 28));
	text_layer_set_background_color(wday_layer, GColorClear);
	text_layer_set_text_color(wday_layer, GColorBlack);
	text_layer_set_text_alignment(wday_layer, GTextAlignmentCenter);
	text_layer_set_font(wday_layer,fonts_get_system_font(FONT_KEY_GOTHIC_24));
	
	layer_add_child(window_get_root_layer(window), (Layer*) wday_layer);
	
	bat_layer = text_layer_create(GRect(72, 1, 69, 16));
	text_layer_set_background_color(bat_layer, GColorClear);
	text_layer_set_text_color(bat_layer, GColorBlack);
	text_layer_set_text_alignment(bat_layer, GTextAlignmentRight);
	text_layer_set_font(bat_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	
	layer_add_child(window_get_root_layer(window), (Layer*) bat_layer);
	
	bt_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH_ICON);
	bt_layer = bitmap_layer_create(GRect(3, 3, 7, 13));
	bitmap_layer_set_bitmap(bt_layer, bt_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bt_layer));
	
	btc_bitmap = gbitmap_create_with_resource(RESOURCE_ID_WARNING_ICON);
	btc_layer = bitmap_layer_create(GRect(13, 5, 9,9));
	bitmap_layer_set_bitmap(btc_layer, btc_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(btc_layer));
	layer_set_hidden((Layer*) btc_layer, true);
	
	inv_layer = inverter_layer_create(GRect(0, 00, 144, 168));
	layer_add_child(window_get_root_layer(window), (Layer*) inv_layer);
	
	//Get a time structure so that the face doesn't start blank
	struct tm *t;
	time_t temp;
	temp = time(NULL);
	t = localtime(&temp);
	 
	//Manually call the tick handler when the window is loading
	minute_handler(t, MINUTE_UNIT);
	
	BatteryChargeState charge = battery_state_service_peek();
	battery_handler(charge);
	
	layer_set_hidden((Layer*) btc_layer, bluetooth_connection_service_peek());
}
 
void window_unload(Window *window){
  //We will safely destroy the Window's elements here!
	text_layer_destroy(time_layer);
	text_layer_destroy(date_layer);
	text_layer_destroy(wday_layer);
	text_layer_destroy(bat_layer);
	text_layer_destroy(ampm_layer);
	inverter_layer_destroy(inv_layer);
	gbitmap_destroy(bt_bitmap);
	bitmap_layer_destroy(bt_layer);
	gbitmap_destroy(btc_bitmap);
	bitmap_layer_destroy(btc_layer);
}

void init(){
  //Initialize the app elements here!
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	window_stack_push(window, true);
	tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler) minute_handler);
	battery_state_service_subscribe((BatteryStateHandler) battery_handler);
	bluetooth_connection_service_subscribe((BluetoothConnectionHandler) bluetooth_handler);
}
 
void deinit(){
  //De-initialize elements here to save memory!
	window_destroy(window);
	tick_timer_service_unsubscribe();
	battery_state_service_unsubscribe();
	bluetooth_connection_service_unsubscribe();
}	

int main(void){
  init();
  app_event_loop();
  deinit();
}