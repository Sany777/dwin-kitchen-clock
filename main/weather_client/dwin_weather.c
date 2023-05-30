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
    const size_t len_data = esp_http_client_get_content_length(client);
    if(len_data){
        local_response_buffer[len_data] = 0;

        // for(size_t i=0; i<len_data;i++){
        //     if(local_response_buffer[i] == '[')local_response_buffer[i] = '{';
        //     if(local_response_buffer[i] == ']')local_response_buffer[i] = '}';
        // }
        cJSON *root = cJSON_Parse(local_response_buffer);

        const cJSON *sunrise_j = cJSON_GetObjectItemCaseSensitive(root, "cnt");
        const cJSON *sunset_j = cJSON_GetObjectItemCaseSensitive(root, "sunset");
        // if(cJSON_IsString(city_j) && (city_j->valuestring != NULL)){
        //     city_name = city_j->valuestring;
        //     city_len = strnlen(city_name, SIZE_BUF);
        // }

        if(cJSON_IsNumber(sunrise_j)){
            int sunrise = sunrise_j->valuedouble;
            ESP_LOGI(TAG, "GET !!! cnt %d", sunrise);
        }else{
            ESP_LOGI(TAG, "wrong");
        }
        cJSON_Delete(root);
    }

    // ESP_LOGI(TAG, "respnse %dl", sunrise);
    esp_http_client_cleanup(client);

}
