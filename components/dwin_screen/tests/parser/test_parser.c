#include <assert.h>
#include <string.h>

#include "parser.h"
#include "esp_wifi.h"

static void test_temperature_colors(void) {
  assert(dwin_help_get_color_temp(-0.1f) == VIOLET);
  assert(dwin_help_get_color_temp(0.0f) == VIOLET);
  assert(dwin_help_get_color_temp(0.1f) == WHITE);
  assert(dwin_help_get_color_temp(18.0f) == WHITE);
  assert(dwin_help_get_color_temp(18.01f) == YELLOW);
  assert(dwin_help_get_color_temp(24.0f) == YELLOW);
  assert(dwin_help_get_color_temp(24.01f) == ORANGE);
}

static void test_weather_picture_mapping(void) {
  static const unsigned int thunder[] = {200, 201, 202, 210, 211,
                                         212, 221, 230, 231, 232};
  static const unsigned int drizzle[] = {300, 301, 302, 310, 311,
                                         312, 313, 314, 321};
  static const unsigned int rain[] = {500, 501, 502, 503, 504,
                                      511, 520, 521, 522, 531};
  static const unsigned int snow[] = {600, 601, 602, 611, 612,
                                      615, 616, 620, 621, 622};
  static const unsigned int fog[] = {701, 711, 721, 731, 741,
                                     751, 761, 762, 771, 781};

  assert(get_pic(800, false) == WEAT_CLEAR_DAY_PIC);
  assert(get_pic(800, true) == WEAT_CLEAR_NIGHT_PIC);
  assert(get_pic(801, false) == WEAT_CLOUD_SMALL_DAY_PIC);
  assert(get_pic(801, true) == WEAT_CLOUD_SMALL_NIGHT_PIC);
  assert(get_pic(802, false) == WEAT_CLOUD_MEDIUM_PIC);
  assert(get_pic(802, true) == WEAT_CLOUD_MEDIUM_PIC);
  assert(get_pic(803, false) == WEAT_CLOUD_HEAVY_PIC);
  assert(get_pic(804, true) == WEAT_CLOUD_HEAVY_PIC);
  for (size_t i = 0; i < sizeof(thunder) / sizeof(thunder[0]); i++) {
    assert(get_pic(thunder[i], false) == WEAT_THUNDER_PIC);
  }
  for (size_t i = 0; i < sizeof(drizzle) / sizeof(drizzle[0]); i++) {
    assert(get_pic(drizzle[i], false) == WEAT_RAIN_SUN_PIC);
    assert(get_pic(drizzle[i], true) == WEAT_RAIN_MOON_PIC);
  }
  for (size_t i = 0; i < sizeof(rain) / sizeof(rain[0]); i++) {
    assert(get_pic(rain[i], false) == WEAT_RAIN_PIC);
  }
  for (size_t i = 0; i < sizeof(snow) / sizeof(snow[0]); i++) {
    assert(get_pic(snow[i], false) == WEAT_SNOW_PIC);
  }
  for (size_t i = 0; i < sizeof(fog) / sizeof(fog[0]); i++) {
    assert(get_pic(fog[i], false) == WEAT_FOG_PIC);
  }
  assert(get_pic(0, false) == NO_WEATHER_PIC);
  assert(get_pic(199, false) == NO_WEATHER_PIC);
  assert(get_pic(203, false) == NO_WEATHER_PIC);
  assert(get_pic(299, false) == NO_WEATHER_PIC);
  assert(get_pic(322, false) == NO_WEATHER_PIC);
  assert(get_pic(799, false) == NO_WEATHER_PIC);
  assert(get_pic(805, false) == NO_WEATHER_PIC);
  assert(get_pic(999, false) == NO_WEATHER_PIC);
}

static void test_wifi_auth_labels(void) {
  assert(strcmp(parser_get_auth_mode(WIFI_AUTH_OPEN), "OPEN") == 0);
  assert(strcmp(parser_get_auth_mode(WIFI_AUTH_OWE), "OWE") == 0);
  assert(strcmp(parser_get_auth_mode(WIFI_AUTH_WEP), "WEP") == 0);
  assert(strcmp(parser_get_auth_mode(WIFI_AUTH_WPA_PSK), "WPA") == 0);
  assert(strcmp(parser_get_auth_mode(WIFI_AUTH_WPA2_PSK), "WPA2") == 0);
  assert(strcmp(parser_get_auth_mode(WIFI_AUTH_WPA_WPA2_PSK), "WPA2") == 0);
  assert(strcmp(parser_get_auth_mode(WIFI_AUTH_WPA2_ENTERPRISE), "WPA2 E.") == 0);
  assert(strcmp(parser_get_auth_mode(WIFI_AUTH_WPA3_PSK), "WPA3") == 0);
  assert(strcmp(parser_get_auth_mode(-1), "UNK.") == 0);
  assert(strcmp(parser_get_auth_mode(WIFI_AUTH_WPA3_PSK + 1), "UNK.") == 0);
}

static void test_rssi_categories(void) {
  assert(strcmp(parser_get_rssi_description(0), "perf.") == 0);
  assert(strcmp(parser_get_rssi_description(-64), "perf.") == 0);
  assert(strcmp(parser_get_rssi_description(-65), "good") == 0);
  assert(strcmp(parser_get_rssi_description(-84), "good") == 0);
  assert(strcmp(parser_get_rssi_description(-85), "bad") == 0);
  assert(strcmp(parser_get_rssi_description(INT8_MIN), "bad") == 0);
}

int main(void) {
  test_temperature_colors();
  test_weather_picture_mapping();
  test_wifi_auth_labels();
  test_rssi_categories();
  return 0;
}
