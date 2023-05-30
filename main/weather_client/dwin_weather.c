#include "dwin_weather.h"

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer; 
    static int output_len;      
    switch(evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            if (!esp_http_client_is_chunked_response(evt->client)) {
                int copy_len = 0;
                if (evt->user_data) {
                    copy_len = MIN(evt->data_len, (CLIENT_BUF_LEN - output_len));
                    if (copy_len) {
                        memcpy(evt->user_data + output_len, evt->data, copy_len);
                    }
                } else {
                    const int buffer_len = esp_http_client_get_content_length(evt->client);
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc(buffer_len);
                        output_len = 0;
                        if (output_buffer == NULL) {
                            return ESP_FAIL;
                        }
                    }
                    copy_len = MIN(evt->data_len, (buffer_len - output_len));
                    if (copy_len) {
                        memcpy(output_buffer + output_len, evt->data, copy_len);
                    }
                }
                output_len += copy_len;
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        default:break;
    }
    return ESP_OK;
}

void get_weather(void *pv)
{
    main_data_t *main_data = (main_data_t *)pv;
    if(main_data->weather_data == NULL){
        main_data->weather_data = calloc(1, sizeof(weather_data_t));
        assert(main_data->weather_data);
    }
    char *local_response_buffer = (char *)malloc(CLIENT_BUF_LEN);
    esp_http_client_config_t config = {
        .url = "https://api.openweathermap.org/data/2.5/forecast?q=Kyiv&units=metric&cnt=5&appid=54404f71fe4cbd50e2d1068353f013e6",
        .host = "https://api.openweathermap.org",
        .event_handler = _http_event_handler,
        .user_data = (void*)local_response_buffer,        // Pass address of local buffer to get response
        .method = HTTP_METHOD_GET,
        .buffer_size = CLIENT_BUF_LEN,
        .auth_type = HTTP_AUTH_TYPE_NONE

    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %"PRIu64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    const size_t data_len = esp_http_client_get_content_length(client);
    if(data_len){
        char **pop = find_str_key(local_response_buffer, data_len, "\"pop\":");
        char **temp = find_str_key(local_response_buffer, data_len, "\"temp\":");
        char **temp_feel = find_str_key(local_response_buffer, data_len, "\"feels_like\":");
        char **description = find_str_key(local_response_buffer, data_len, "\"description\":");
        char **clouds = find_str_key(local_response_buffer, data_len, "\"clouds\":{\"all\":");
        char **id = find_str_key(local_response_buffer, data_len, "\"id\":");
        char **sunrise = find_str_key(local_response_buffer, data_len, "\"sunrise\":");
        char **sunset = find_str_key(local_response_buffer, data_len, "\"sunset\":");
        for(int i=0; i<data_len; i++){
            if(local_response_buffer[i] == ',' 
                    || local_response_buffer[i] == '}')
            {
                local_response_buffer[i] = 0;
            }
        }
        // assert(sunset && sunrise && id && clouds && description && temp_feel && temp);
        struct tm timeinfo;
        time_t time_now = atol(sunrise[0]);
        localtime_r(&time_now, &timeinfo);
        sunrise_HOUR = timeinfo.tm_hour;
        sunrise_MIN = timeinfo.tm_min;
        time_now = atol(sunset[0]);
        localtime_r(&time_now, &timeinfo);
        sunset_HOUR = timeinfo.tm_hour;
        sunset_MIN = timeinfo.tm_min;

        ESP_LOGI(TAG, "sunset  %d:%d",sunset_HOUR, sunset_MIN);

        for(int i=0; temp && temp[i]; i++){
            temp_OUTDOOR[i] = atoi(temp[i]);
            ESP_LOGI(TAG, "temp %d", temp_OUTDOOR[i]);
            temp_FEELS_LIKE[i] = atoi(temp_feel[i]);
            ESP_LOGI(TAG, "temp_feel %d", temp_FEELS_LIKE[i]);
            PoP[i] = (uint8_t) atoi(pop[i]);
        }
        // free(sunrise);
        // free(sunset);


        free(id);
        free(clouds);
        free(temp);
        free(description);
        free(temp_feel);
    }
    esp_http_client_cleanup(client);
}


#define SIZE_LIST_KEYS 10
char ** find_str_key(char *buf, const size_t buf_len, const char *key)
{
    size_t keys_len = 0;
    int key_numb = 0;
    char *ptr = buf;
    char **list = NULL;
    const size_t size_key = strlen(key);
    do {
        ptr = strstr(ptr, key);
        if(ptr){
            if(key_numb+2 >= keys_len){
                const size_t new_keys_len = keys_len + SIZE_LIST_KEYS;
                char **new_list = calloc(sizeof(char *), new_keys_len);
                assert(new_list);
                if(keys_len){
                    memcpy(new_list, list, keys_len);
                    free(list);
                }
                list = new_list;
                keys_len = new_keys_len;
            }
            ptr += size_key;
            list[key_numb++] = ptr; 
            if(buf_len+buf <= ptr) break;
        }
    } while(ptr);
    return list;
}