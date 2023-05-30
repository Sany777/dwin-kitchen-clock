#include "dwin_weather.h"

#define MAX_HTTP_RECV_BUFFER 2048
#define MAX_HTTP_OUTPUT_BUFFER 4000

static const char *TAG = "HTTP CLIENT dwin";

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;  // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                int copy_len = 0;
                if (evt->user_data) {
                    copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
                    if (copy_len) {
                        memcpy(evt->user_data + output_len, evt->data, copy_len);
                    }
                } else {
                    const int buffer_len = esp_http_client_get_content_length(evt->client);
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc(buffer_len);
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
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
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
            break;
    }
    return ESP_OK;
}

void get_weather(void)
{
    char *local_response_buffer = (char *)calloc(1, MAX_HTTP_OUTPUT_BUFFER);
    esp_http_client_config_t config = {
        .url = "https://api.openweathermap.org/data/2.5/forecast?q=Kyiv&units=metric&cnt=5&appid=54404f71fe4cbd50e2d1068353f013e6",
        .host = "https://api.openweathermap.org",
        .event_handler = _http_event_handler,
        .user_data = (void*)local_response_buffer,        // Pass address of local buffer to get response
        .method = HTTP_METHOD_GET,
        .buffer_size = MAX_HTTP_OUTPUT_BUFFER,
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
        local_response_buffer[data_len] = 0;
        int number_keys = 0;
        char **temp_key = find_str_key(local_response_buffer, data_len, "\"temp\":");
        char **temp_feel = find_str_key(local_response_buffer, data_len, "\"feels_like\":");
        char **description = find_str_key(local_response_buffer, data_len, "\"description\":");
        for(int i=0; i<data_len; i++){
            if(local_response_buffer[i] == ',')local_response_buffer[i] = 0;
        }
        for(int i=0; temp_key && temp_key[i]; i++){
            ESP_LOGI(TAG, "temp %s", temp_key[i]);
        }
        for(int i=0; temp_feel && temp_feel[i]; i++){
            ESP_LOGI(TAG, "temp feel  %s", temp_feel[i]);
        }
        for(int i=0; description && description[i]; i++){
            ESP_LOGI(TAG, "description - %s", description[i]);
        }
        free(temp_key);
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