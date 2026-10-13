#include "dwin_services.h"
#include "parser.h"
#include "dwin_help.h"
#include "dwin_timer.h"
#include "math_utils.h"
#include "string_utils.h"
#include "esp_timer.h"
#include "esp_crt_bundle.h"

static char local_response_buffer[CLIENT_BUF_LEN];
static char url_buf[SIZE_URL_BUF];
static char backup_output_buffer[CLIENT_BUF_LEN];

typedef struct {
    int32_t day_key;
    float usd_open;
    float eur_open;
} currency_day_baseline_t;

static currency_day_baseline_t currency_day_baseline;
static bool currency_day_baseline_loaded;

static bool get_currency_rate(const cJSON *item, const char *key, float *value)
{
    const cJSON *rate = cJSON_GetObjectItemCaseSensitive(item, key);
    if (!cJSON_IsString(rate) || rate->valuestring == NULL ||
        !parse_float_string(rate->valuestring, value)) {
        return false;
    }
    return *value > 0.0f;
}

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
    xEventGroupClearBits(dwin_event_group, BIT_RESPONSE_400_SERVER);
    if (xEventGroup & BIT_DENIED_STA) {
        ESP_LOGW("Weather", "Skipping request: station mode is disabled");
        goto st_1;
    }
    if (strnlen(api_KEY, SIZE_BUF) != SIZE_API) {
        ESP_LOGW("Weather", "API key is missing or has an invalid length");
        if (!(xEventGroup & BIT_WRONG_API_KEY)) {
            xEventGroupSetBits(dwin_event_group, BIT_WRONG_API_KEY);
        }
        goto st_1;
    } else if (xEventGroup & BIT_WRONG_API_KEY) {
        xEventGroupClearBits(dwin_event_group, BIT_WRONG_API_KEY);
    }
    if (strnlen(name_CITY, SIZE_BUF) == 0) {
        ESP_LOGW("Weather", "City is not configured");
        goto st_1;
    }
    if (!(xEventGroup & BIT_CON_STA_OK)) {
        dwin_timer_set_new_command(START_STA);
        vTaskDelay(100);
        xEventGroup = xEventGroupWaitBits(dwin_event_group, BIT_PROCESS, false, false, WAIT_PROCEES);
    }
    if (!(xEventGroup & BIT_CON_STA_OK)) {
        ESP_LOGW("Weather", "Skipping request: Wi-Fi is not connected");
        goto st_1;
    }
    memset(url_buf, 0, SIZE_URL_BUF);
    memset(local_response_buffer, 0, CLIENT_BUF_LEN);
    snprintf(url_buf, SIZE_URL_BUF, "%s%s%s%s", FIRST_URL, name_CITY, SECOND_URL, api_KEY);
    esp_http_client_config_t config = {.url           = url_buf,
                                       .event_handler = http_event_handler,
                                       .user_data     = (void *)local_response_buffer,
                                       .method        = HTTP_METHOD_GET,
                                       .buffer_size   = CLIENT_BUF_LEN,
                                       .auth_type     = HTTP_AUTH_TYPE_NONE,
                                       .crt_bundle_attach = esp_crt_bundle_attach};
    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE("Weather", "Could not create HTTP client");
        goto st_1;
    }
    esp_err_t request_result = esp_http_client_perform(client);
    if (request_result != ESP_OK) {
        ESP_LOGW("Weather", "Request failed: %s", esp_err_to_name(request_result));
        goto st_3;
    }
    const int status_code = esp_http_client_get_status_code(client);
    if (status_code != 200) {
        ESP_LOGW("Weather", "Server returned HTTP %d", status_code);
        goto st_3;
    }
    const size_t data_len = esp_http_client_get_content_length(client);
    if (data_len == 0) {
        ESP_LOGW("Weather", "Server returned an empty response");
        goto st_3;
    }
    cJSON *json = cJSON_Parse(local_response_buffer);
    if (json == NULL) {
        ESP_LOGW("Weather", "Could not parse server response as JSON");
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

    bool has_weather_data = false;
    cJSON *list = cJSON_GetObjectItemCaseSensitive(json, "list");
    if (cJSON_IsArray(list)) {
        int list_count    = cJSON_GetArraySize(list);
        cJSON *first_item = cJSON_GetArrayItem(list, 0);
        if (first_item) {
            has_weather_data = true;
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
                    copy_bounded_string(description_WEATHER,
                                        sizeof(description_WEATHER),
                                        desc->valuestring,
                                        strlen(desc->valuestring) + 1);
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

    if (has_weather_data) {
        main_data->weather_data.updated_at_us = esp_timer_get_time();
        xEventGroupSetBits(dwin_event_group, BIT_WEATHER_OK);
    } else {
        ESP_LOGW("Weather", "Response did not contain forecast data");
    }
    cJSON_Delete(json);

st_3:
    esp_http_client_cleanup(client);
st_1:
    dwin_timer_set_new_command(UPDATE_WEATHER_COMPLETE);
}

static void
set_currency_state(dwin_data_t *main_data, float newUsd, float newEur)
{
    const EventBits_t bits = xEventGroupGetBits(dwin_event_group);
    struct tm *local_time = wifi_dwin_get_time_tm();
    if (!(bits & BIT_IS_TIME) || local_time->tm_year < 120) {
        usd_State = NO_CHANGE;
        eur_State = NO_CHANGE;
        ESP_LOGI("Currency", "Valid local time unavailable; trend pending");
        return;
    }

    const int32_t day_key = (local_time->tm_year + 1900) * 1000 + local_time->tm_yday;
    if (!currency_day_baseline_loaded) {
        currency_day_baseline_loaded = true;
        nvs_handle_t handle;
        if (nvs_open("nvs", NVS_READONLY, &handle) == ESP_OK) {
            size_t size = sizeof(currency_day_baseline);
            if (nvs_get_blob(handle, "fxday", &currency_day_baseline, &size) != ESP_OK ||
                size != sizeof(currency_day_baseline)) {
                memset(&currency_day_baseline, 0, sizeof(currency_day_baseline));
            }
            nvs_close(handle);
        }
    }

    if (currency_day_baseline.day_key != day_key ||
        currency_day_baseline.usd_open <= 0.0f ||
        currency_day_baseline.eur_open <= 0.0f) {
        currency_day_baseline = (currency_day_baseline_t){
            .day_key = day_key,
            .usd_open = newUsd,
            .eur_open = newEur,
        };
        nvs_handle_t handle;
        if (nvs_open("nvs", NVS_READWRITE, &handle) == ESP_OK) {
            if (nvs_set_blob(handle, "fxday", &currency_day_baseline,
                             sizeof(currency_day_baseline)) == ESP_OK) {
                nvs_commit(handle);
            }
            nvs_close(handle);
        }
        usd_State = NO_CHANGE;
        eur_State = NO_CHANGE;
        ESP_LOGI("Currency", "Daily baseline set: USD %.4f EUR %.4f",
                 newUsd, newEur);
        return;
    }

    usd_State = newUsd > currency_day_baseline.usd_open ? GO_UP :
                newUsd < currency_day_baseline.usd_open ? GO_DOWN : NO_CHANGE;
    eur_State = newEur > currency_day_baseline.eur_open ? GO_UP :
                newEur < currency_day_baseline.eur_open ? GO_DOWN : NO_CHANGE;
    ESP_LOGI("Currency", "USD %.4f/%s EUR %.4f/%s (daily open %.4f/%.4f)",
             newUsd, usd_State == GO_UP ? "up" : usd_State == GO_DOWN ? "down" : "same",
             newEur, eur_State == GO_UP ? "up" : eur_State == GO_DOWN ? "down" : "same",
             currency_day_baseline.usd_open, currency_day_baseline.eur_open);
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
                                       .auth_type     = HTTP_AUTH_TYPE_NONE,
                                       .crt_bundle_attach = esp_crt_bundle_attach};

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
                            found_usd = get_currency_rate(item, "buy", &new_usd_bay) &&
                                        get_currency_rate(item, "sale", &new_usd_sale);
                        } else if (strcmp(ccy->valuestring, "EUR") == 0) {
                            found_eur = get_currency_rate(item, "buy", &new_eur_bay) &&
                                        get_currency_rate(item, "sale", &new_eur_sale);
                        }
                    }
                }
                if (found_usd && found_eur) {
                    set_currency_state(main_data, new_usd_sale, new_eur_sale);
                    usd_Bay  = new_usd_bay;
                    usd_Sale = new_usd_sale;
                    eur_Bay  = new_eur_bay;
                    eur_Sale = new_eur_sale;
                    main_data->cur_value.updated_at_us = esp_timer_get_time();
                    dwin_timer_set_periodic_event(UPDATE_CURRENCY, 60 * 60,
                                                  RELOAD_COUNT);
                }
            }
            cJSON_Delete(json);
        }
    }

    esp_http_client_cleanup(client);
    dwin_timer_set_new_command(UPDATE_DATA_COMPLETE);
    return;
_end:
    return;
}
