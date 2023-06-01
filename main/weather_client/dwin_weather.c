#include "dwin_weather.h"

#define FIRST_URL_LEN  (sizeof(FIRST_URL)-1)
#define SECOND_URL_LEN  (sizeof(SECOND_URL)-1)

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



void get_weather_handler(void* args, esp_event_base_t base, int32_t key, void* event_data)
{
    EventBits_t xEventGroup = xEventGroupWaitBits(dwin_event_group,                       
                                                          BIT_WIFI_STA, false, false,                               
                                                          FIRST_WAIT_WIFI_BIT);                                     
    if(!(xEventGroup&BIT_WIFI_STA)){                                                      
        start_sta();                                                                      
        xEventGroup = xEventGroupWaitBits(dwin_event_group, BIT_WIFI_STA, 
                                            false, false,                                                                     SECOND_WAIT_WIFI_BIT);                                                            
        if(!(xEventGroup&BIT_WIFI_STA))return;                                            
    }
    main_data_t *main_data = (main_data_t*)args;
    char *url_buf = (char*)calloc(1, SIZE_URL_BUF);
    assert(url_buf);
    char *local_response_buffer = (char*)malloc(CLIENT_BUF_LEN);
    assert(local_response_buffer);
    const size_t sity_len = strnlen(name_CITY, MAX_STR_LEN);
    memcpy(url_buf, FIRST_URL, FIRST_URL_LEN);
    memcpy(url_buf+FIRST_URL_LEN, name_CITY, sity_len);
    memcpy(url_buf+FIRST_URL_LEN+sity_len, SECOND_URL, SECOND_URL_LEN);
    memcpy(url_buf+FIRST_URL_LEN+sity_len+SECOND_URL_LEN, api_KEY, SIZE_API);
    esp_http_client_config_t config = {
        .url = url_buf,
        .event_handler = _http_event_handler,
        .user_data = (void*)local_response_buffer,    
        .method = HTTP_METHOD_GET,
        .buffer_size = CLIENT_BUF_LEN,
        .auth_type = HTTP_AUTH_TYPE_NONE
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);
    if(err == ESP_OK){
        const size_t data_len = esp_http_client_get_content_length(client);
        char **pop = find_str_key(local_response_buffer, data_len, "\"pop\":");
        char **temp = find_str_key(local_response_buffer, data_len, "\"temp\":");
        char **temp_feel = find_str_key(local_response_buffer, data_len, "\"feels_like\":");
        char **description = find_str_key(local_response_buffer, data_len, "\"main\":\"");
        char **id = find_str_key(local_response_buffer, data_len, "\"id\":");
        char **sunrise = find_str_key(local_response_buffer, data_len, "\"sunrise\":");
        char **sunset = find_str_key(local_response_buffer, data_len, "\"sunset\":");
        char **dt_txt = find_str_key(local_response_buffer, data_len, "\"dt_txt\":");
        if(pop && temp && temp_feel && description && id && sunrise && sunset && dt_txt){
            for(int i=0; i<data_len; i++){
                if(local_response_buffer[i] == ',' 
                        || local_response_buffer[i] == '}'
                        || local_response_buffer[i] == '"')
                {
                    local_response_buffer[i] = 0;
                }
            }
            struct tm timeinfo;
            time_t time_now = atol(sunrise[0]);
            localtime_r(&time_now, &timeinfo);
            sunrise_HOUR = timeinfo.tm_hour;
            sunrise_MIN = timeinfo.tm_min;
            time_now = atol(sunset[0]);
            localtime_r(&time_now, &timeinfo);
            sunset_HOUR = timeinfo.tm_hour;
            sunset_MIN = timeinfo.tm_min;
            dt_TX = atoi((dt_txt[0]+SHIFT_DT_TX));
            weather_PIC = get_pic(atoi(id[0]), dt_TX>sunset_HOUR-2);
            strncpy(description_WEATHER, description[0], LEN_BUF_DESCRIPTION);
            for(int i=0; temp && temp[i]; i++){
                temp_OUTDOOR[i] = (atof(temp[i]))*10;
                ESP_LOGI(TAG, "temp %d.%d %d:00", temp_OUTDOOR[i]/10, temp_OUTDOOR[i]%10, i*STEP_DT_TX+dt_TX);
                temp_FEELS_LIKE[i] = (atof(temp_feel[i]))*10;
                ESP_LOGI(TAG, "temp_feel %d.%d", temp_FEELS_LIKE[i]/10, temp_FEELS_LIKE[i]%10);
                PoP[i] = (uint8_t) atoi(pop[i]);
            }
            ESP_LOGI(TAG, "des : %s", description_WEATHER);
            if(id)free(id);
            if(temp)free(temp);
            if(description)free(description);
            if(temp_feel)free(temp_feel);
        } 
    } else{
        DWIN_SHOW_ERR(err);
    }
    esp_http_client_cleanup(client);
    free(url_buf);
    free(local_response_buffer);
}

char ** find_str_key(char *buf, const size_t buf_len, const char *key)
{
    size_t keys_len = 0;
    int key_numb = 0;
    char *ptr = buf;
    char **list = NULL;
    const size_t size_key = strlen(key);
    while(ptr = strstr(ptr, key), ptr) {
        if(key_numb+2 >= keys_len){
            const size_t new_keys_len = keys_len + INITIAL_SIZE_LIST_KEYS;
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
    return list;
}