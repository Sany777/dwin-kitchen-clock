#include "parser.h"

uint16_t get_color_temp(int8_t temp)
{
    if(temp <= 0){
        return VIOLET;
    } else if(temp > 24 ){
        return ORANGE;
    } else if(temp > 18){
        return YELLOW;
    } else {
        return WHITE;
    }
}

picture_t get_pic(const uint weather_id, const bool night) 
{
	switch (weather_id)	{
	case 800:
		if (night){
			return  WEAT_CLEAR_NIGHT_PIC;
		} else {
			return  WEAT_CLEAR_DAY_PIC;
		}
	case 801:
		if (night) {
			return  WEAT_CLOUD_SMALL_NIGHT_PIC;
		} else {
			return  WEAT_CLOUD_SMALL_DAY_PIC;
		}
	case 802:
		return  WEAT_CLOUD_MEDIUM_PIC;
		break;
	case 803:
	case 804:
		return  WEAT_CLOUD_HEAVY_PIC;
	case 200:
	case 201:
	case 202:
	case 210:
	case 211:
	case 212:
	case 221:
	case 230:
	case 231:
	case 232:
		return  WEAT_THUNDER_PIC;
	case 300:
	case 301:
	case 302:
	case 310:
	case 311:
	case 312:
	case 313:
	case 314:
	case 321:
		if (night) {
			return  WEAT_RAIN_MOON_PIC;
		} else {
			return  WEAT_RAIN_SUN_PIC;
		}
	case 500:
	case 501:
	case 502:
	case 503:
	case 504:
	case 511:
	case 520:
	case 521:
	case 522:
	case 531:
		return  WEAT_RAIN_PIC;
	case 600:
	case 601:
	case 602:
	case 611:
	case 612:
	case 615:
	case 616:
	case 620:
	case 621:
	case 622:
		return  WEAT_SNOW_PIC;
	case 701:
	case 711:
	case 721:
	case 731:
	case 741:
	case 751:
	case 761:
	case 762:
	case 771:
	case 781:
		return  WEAT_FOG_PIC;
	default: return NO_WEATHER_PIC;
	}
}

const char *get_auth_mode(int authmode) {
    switch (authmode) {
    case WIFI_AUTH_OPEN:
        return "\tOPEN";
    case WIFI_AUTH_OWE:
        return "\t\tOWE";
    case WIFI_AUTH_WEP:
        return "\t\tWEP";
    case WIFI_AUTH_WPA_PSK:
        return "\t\tWPA";
    case WIFI_AUTH_WPA2_PSK:
        return "\t\tWPA2";
    case WIFI_AUTH_WPA_WPA2_PSK:
        return "\t\tWPA2";
    case WIFI_AUTH_WPA2_ENTERPRISE:
        return "\tWPA2 ENT";
    case WIFI_AUTH_WPA3_PSK:
        return "\t\tWPA3";
    default:
        return "\tUNKNOWN";
    }
}

const char *get_rssi_description(uint8_t rssi) 
{
    if(rssi < 40)return "bad";
	else if(rssi < 70)return "good";
	else return "excellent";
}

