#include "dwin_services.h"
#include "parser.h"
#include "dwin_help.h"
#include "dwin_timer.h"

static char local_response_buffer[CLIENT_BUF_LEN];
static char url_buf[SIZE_URL_BUF];
static char backup_output_buffer[CLIENT_BUF_LEN];

static esp_err_t
http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer = NULL;
    static int output_len;
    switch (evt->event_id) {
    case HTTP_EVENT_ON_DATA:
        if (!esp_http_client_is_chunked_response(evt->client)) {
            int copy_len = 0;
            if (evt->user_data) {
                copy_len = MIN(evt->data_len, (CLIENT_BUF_LEN - output_len));
                if (copy_len) {
                    memcpy((char *)evt->user_data + output_len, evt->data, copy_len);
                }
            } else {
                const int buffer_len = esp_http_client_get_content_length(evt->client);
                if (output_buffer == NULL) {
                    output_buffer = backup_output_buffer;
                    output_len    = 0;
                }
                copy_len = MIN(evt->data_len, MIN(buffer_len - output_len, CLIENT_BUF_LEN - output_len));
                if (copy_len) {
                    memcpy(output_buffer + output_len, evt->data, copy_len);
                }
            }
            output_len += copy_len;
        }
        break;
    case HTTP_EVENT_ON_FINISH:
        output_buffer = NULL;
        output_len    = 0;
        break;
    case HTTP_EVENT_DISCONNECTED:
        output_buffer = NULL;
        output_len    = 0;
        break;
    default:
        break;
    }
    return ESP_OK;
}

void
weather_client_get_weather(dwin_data_t *main_data, uint8_t key)
{
    EventBits_t xEventGroup = xEventGroupWaitBits(dwin_event_group, BIT_PROCESS, false, false, WAIT_PROCEES);
    weather_PIC             = NO_WEATHER_PIC;
    xEventGroupClearBits(dwin_event_group, BIT_WEATHER_OK | BIT_RESPONSE_400_SERVER);
    DWIN_IF_FALSE_GOTO(!(xEventGroup & BIT_DENIED_STA), st_1);
    if (strnlen(api_KEY, SIZE_BUF) != SIZE_API) {
        if (!(xEventGroup & BIT_WRONG_API_KEY)) {
            xEventGroupSetBits(dwin_event_group, BIT_WRONG_API_KEY);
        }
        goto st_1;
    } else if (xEventGroup & BIT_WRONG_API_KEY) {
        xEventGroupClearBits(dwin_event_group, BIT_WRONG_API_KEY);
    }
    if (strnlen(name_CITY, SIZE_BUF) == 0) {
        goto st_1;
    }
    if (!(xEventGroup & BIT_CON_STA_OK)) {
        dwin_timer_set_new_command(START_STA);
        vTaskDelay(100);
        xEventGroup = xEventGroupWaitBits(dwin_event_group, BIT_PROCESS, false, false, WAIT_PROCEES);
    }
    DWIN_IF_FALSE_GOTO(xEventGroup & BIT_CON_STA_OK, st_1);
    memset(url_buf, 0, SIZE_URL_BUF);
    memset(local_response_buffer, 0, CLIENT_BUF_LEN);
    snprintf(url_buf, SIZE_URL_BUF, "%s%s%s%s", FIRST_URL, name_CITY, SECOND_URL, api_KEY);
    esp_http_client_config_t config = {.url           = url_buf,
                                       .event_handler = http_event_handler,
                                       .user_data     = (void *)local_response_buffer,
                                       .method        = HTTP_METHOD_GET,
                                       .buffer_size   = CLIENT_BUF_LEN,
                                       .auth_type     = HTTP_AUTH_TYPE_NONE};
    esp_http_client_handle_t client = esp_http_client_init(&config);
    DWIN_CHECK_AND_GO(esp_http_client_perform(client), st_3);
    const size_t data_len = esp_http_client_get_content_length(client);
    DWIN_IF_FALSE_GOTO(data_len != 0, st_3);
    cJSON *json = cJSON_Parse(local_response_buffer);
    if (json == NULL) {
        xEventGroupSetBits(dwin_event_group, BIT_RESPONSE_400_SERVER);
        goto st_3;
    }

    cJSON *city = cJSON_GetObjectItemCaseSensitive(json, "city");
    if (city) {
        cJSON *sunrise = cJSON_GetObjectItemCaseSensitive(city, "sunrise");
        cJSON *sunset  = cJSON_GetObjectItemCaseSensitive(city, "sunset");
        if (cJSON_IsNumber(sunrise)) {
            time_t time_now = (time_t)sunrise->valuedouble;
            struct tm timeinfo;
            localtime_r(&time_now, &timeinfo);
            sunrise_HOUR = timeinfo.tm_hour;
            sunrise_MIN  = timeinfo.tm_min;
        }
        if (cJSON_IsNumber(sunset)) {
            time_t time_now = (time_t)sunset->valuedouble;
            struct tm timeinfo;
            localtime_r(&time_now, &timeinfo);
            sunset_HOUR = timeinfo.tm_hour;
            sunset_MIN  = timeinfo.tm_min;
        }
    }

    cJSON *list = cJSON_GetObjectItemCaseSensitive(json, "list");
    if (cJSON_IsArray(list)) {
        int list_count    = cJSON_GetArraySize(list);
        cJSON *first_item = cJSON_GetArrayItem(list, 0);
        if (first_item) {
            cJSON *dt_txt = cJSON_GetObjectItemCaseSensitive(first_item, "dt_txt");
            if (cJSON_IsString(dt_txt) && dt_txt->valuestring) {
                dt_TX = atoi(dt_txt->valuestring + SHIFT_DT_TX);
            }

            cJSON *weather_arr = cJSON_GetObjectItemCaseSensitive(first_item, "weather");
            cJSON *weather     = cJSON_GetArrayItem(weather_arr, 0);
            if (weather) {
                cJSON *id   = cJSON_GetObjectItemCaseSensitive(weather, "id");
                cJSON *desc = cJSON_GetObjectItemCaseSensitive(weather, "description");
                cJSON *sys  = cJSON_GetObjectItemCaseSensitive(first_item, "sys");
                cJSON *pod  = NULL;
                if (sys)
                    pod = cJSON_GetObjectItemCaseSensitive(sys, "pod");

                bool is_night = false;
                if (cJSON_IsString(pod) && pod->valuestring && pod->valuestring[0] == 'n') {
                    is_night = true;
                }
                if (cJSON_IsNumber(id)) {
                    weather_PIC = get_pic((int)id->valuedouble, is_night);
                }
                if (cJSON_IsString(desc) && desc->valuestring) {
                    strncpy(description_WEATHER, desc->valuestring, MAX_LEN_DESCRIPTION);
                }
            }

            cJSON *main = cJSON_GetObjectItemCaseSensitive(first_item, "main");
            if (main) {
                cJSON *temp = cJSON_GetObjectItemCaseSensitive(main, "temp");
                if (cJSON_IsNumber(temp))
                    temp_OUTDOOR = temp->valuedouble;
            }
        }

        for (int i = 0; i < NUMBER_DATA_WEATHER && i < list_count; i++) {
            cJSON *item = cJSON_GetArrayItem(list, i);
            if (!item)
                continue;

            cJSON *pop = cJSON_GetObjectItemCaseSensitive(item, "pop");
            if (cJSON_IsNumber(pop)) {
                PoP[i] = pop->valuedouble * 100;
            }

            cJSON *main = cJSON_GetObjectItemCaseSensitive(item, "main");
            if (main) {
                cJSON *feels_like = cJSON_GetObjectItemCaseSensitive(main, "feels_like");
                if (cJSON_IsNumber(feels_like)) {
                    temp_FEELS_LIKE[i] = feels_like->valuedouble;
                }
            }
        }
    }

    xEventGroupSetBits(dwin_event_group, BIT_WEATHER_OK);
    cJSON_Delete(json);

st_3:
    esp_http_client_cleanup(client);
st_1:
    dwin_timer_set_new_command(UPDATE_WEATHER_COMPLETE);
}

static void
set_currency_state(dwin_data_t *main_data, float newUsd, float newEur)
{
    if (usd_Sale != DWIN_NO_DATA) {
        if (newUsd > usd_Sale) {
            usd_State = GO_UP;
        } else if (newUsd < usd_Sale) {
            usd_State = GO_DOWN;
        } else {
            usd_State = NO_CHANGE;
        }
    }
    if (eur_Sale != DWIN_NO_DATA) {
        if (newEur > eur_Sale) {
            eur_State = GO_UP;
        } else if (newEur < eur_Sale) {
            eur_State = GO_DOWN;
        } else {
            eur_State = NO_CHANGE;
        }
    }
}

void
weather_client_get_currency(dwin_data_t *main_data)
{
    EventBits_t xEventGroup = xEventGroupWaitBits(dwin_event_group, BIT_PROCESS, false, false, WAIT_PROCEES);

    DWIN_IF_FALSE_GOTO(xEventGroup & BIT_CON_STA_OK, _end);
    memset(local_response_buffer, 0, CLIENT_BUF_LEN);
    esp_http_client_config_t config = {.url           = SIMPLE_PRIVAT_API,
                                       .event_handler = http_event_handler,
                                       .user_data     = (void *)local_response_buffer,
                                       .method        = HTTP_METHOD_GET,
                                       .buffer_size   = CLIENT_BUF_LEN,
                                       .auth_type     = HTTP_AUTH_TYPE_NONE};

    esp_http_client_handle_t client = esp_http_client_init(&config);

    if (esp_http_client_perform(client) == ESP_OK) {

        const size_t data_len = esp_http_client_get_content_length(client);
        if (data_len) {
            cJSON *json = cJSON_Parse(local_response_buffer);
            if (cJSON_IsArray(json)) {
                int array_size    = cJSON_GetArraySize(json);
                float new_usd_bay = 0, new_usd_sale = 0, new_eur_bay = 0, new_eur_sale = 0;
                bool found_usd = false, found_eur = false;
                for (int i = 0; i < array_size; i++) {
                    cJSON *item = cJSON_GetArrayItem(json, i);
                    cJSON *ccy  = cJSON_GetObjectItemCaseSensitive(item, "ccy");
                    if (cJSON_IsString(ccy) && ccy->valuestring != NULL) {
                        if (strcmp(ccy->valuestring, "USD") == 0) {
                            cJSON *buy  = cJSON_GetObjectItemCaseSensitive(item, "buy");
                            cJSON *sale = cJSON_GetObjectItemCaseSensitive(item, "sale");
                            if (cJSON_IsString(buy))
                                new_usd_bay = atof(buy->valuestring);
                            if (cJSON_IsString(sale))
                                new_usd_sale = atof(sale->valuestring);
                            found_usd = true;
                        } else if (strcmp(ccy->valuestring, "EUR") == 0) {
                            cJSON *buy  = cJSON_GetObjectItemCaseSensitive(item, "buy");
                            cJSON *sale = cJSON_GetObjectItemCaseSensitive(item, "sale");
                            if (cJSON_IsString(buy))
                                new_eur_bay = atof(buy->valuestring);
                            if (cJSON_IsString(sale))
                                new_eur_sale = atof(sale->valuestring);
                            found_eur = true;
                        }
                    }
                }
                if (found_usd && found_eur) {
                    set_currency_state(main_data, new_usd_sale, new_eur_sale);
                    usd_Bay  = new_usd_bay;
                    usd_Sale = new_usd_sale;
                    eur_Bay  = new_eur_bay;
                    eur_Sale = new_eur_sale;
                }
            }
            cJSON_Delete(json);
        }
    }

    esp_http_client_cleanup(client);
    dwin_timer_set_new_command(UPDATE_DATA_COMPLETE);
    return;
_end:
    dwin_help_init_currency_val(main_data);
}
