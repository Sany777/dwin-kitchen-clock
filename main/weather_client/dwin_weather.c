#include "dwin_weather.h"

esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer; 
    static int output_len;      
    switch(evt->event_id) {
    case HTTP_EVENT_ON_DATA:
        if(!esp_http_client_is_chunked_response(evt->client)){
            int copy_len = 0;
            if(evt->user_data){
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
        if(output_buffer != NULL){
            free(output_buffer);
            output_buffer = NULL;
        }
        output_len = 0;
        break;
    case HTTP_EVENT_DISCONNECTED:
        if (output_buffer != NULL){
            free(output_buffer);
            output_buffer = NULL;
        }
        output_len = 0;
        break;
    default:break;
    }
    return ESP_OK;
}


void get_weather(main_data_t *main_data, uint8_t key)
{
    EventBits_t xEventGroup = xEventGroupWaitBits( dwin_event_group, 
                                        BIT_PROCESS,   
                                        false, false, 
                                        WAIT_PROCEES);  
    weather_PIC = NO_WEATHER_PIC;
    xEventGroupClearBits(
        dwin_event_group,BIT_WEATHER_OK
        |BIT_RESPONSE_400_SERVER);                                                         
    DWIN_IF_FALSE_GOTO(!(xEventGroup&BIT_DENIED_STA), st_1);
    DWIN_IF_FALSE_GOTO((strnlen(api_KEY, SIZE_BUF) == MAX_STR_LEN) 
                                || (strnlen(name_CITY, SIZE_BUF) == 0),
                                st_1);
    if(!(xEventGroup&BIT_CON_STA_OK)){
        set_new_command(START_STA);
        vTaskDelay(100);
        xEventGroup = xEventGroupWaitBits(dwin_event_group, 
                                            BIT_PROCESS,   
                                            false, false, 
                                            WAIT_PROCEES); 
    }
    DWIN_IF_FALSE_GOTO(xEventGroup&BIT_CON_STA_OK, st_1);
    char *url_buf = (char*)calloc(1, SIZE_URL_BUF);
    DWIN_CHECK_NULL_AND_GO(url_buf, "", st_1);
    char *local_response_buffer = (char*)malloc(CLIENT_BUF_LEN);
    DWIN_CHECK_NULL_AND_GO(local_response_buffer, "", st_2);
    const size_t sity_len = strnlen(name_CITY, MAX_STR_LEN);
    memcpy(url_buf, FIRST_URL, FIRST_URL_LEN);
    memcpy(url_buf+FIRST_URL_LEN, name_CITY, sity_len);
    memcpy(url_buf+FIRST_URL_LEN+sity_len, SECOND_URL, SECOND_URL_LEN);
    memcpy(url_buf+FIRST_URL_LEN+sity_len+SECOND_URL_LEN, api_KEY, SIZE_API);
    esp_http_client_config_t config = {
        .url = url_buf,
        .event_handler = http_event_handler,
        .user_data = (void*)local_response_buffer,    
        .method = HTTP_METHOD_GET,
        .buffer_size = CLIENT_BUF_LEN,
        .auth_type = HTTP_AUTH_TYPE_NONE
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    DWIN_CHECK_AND_GO(esp_http_client_perform(client), st_3);
    const size_t data_len = esp_http_client_get_content_length(client);
    DWIN_IF_FALSE_GOTO(data_len != 0, st_3);
    const char **pop = find_str_key(local_response_buffer, data_len, "\"pop\":");
    const char **temp = find_str_key(local_response_buffer, data_len, "\"temp\":");
    const char **temp_feel = find_str_key(local_response_buffer, data_len, "\"feels_like\":");
    const char **description = find_str_key(local_response_buffer, data_len, "\"description\":\"");
    const char **id = find_str_key(local_response_buffer, data_len, "\"id\":");
    const char **sunrise = find_str_key(local_response_buffer, data_len, "\"sunrise\":");
    const char **sunset = find_str_key(local_response_buffer, data_len, "\"sunset\":");
    const char **dt_txt = find_str_key(local_response_buffer, data_len, "\"dt_txt\":");
    const char **pod = find_str_key(local_response_buffer, data_len, "\"pod\":\"");
    if(pop == NULL 
            || temp == NULL
            || temp_feel == NULL
            || description == NULL
            || id == NULL
            || sunrise == NULL
            || sunset == NULL
            || dt_txt == NULL
            || pod == NULL)
    {
        xEventGroupSetBits(dwin_event_group, BIT_RESPONSE_400_SERVER); 
        goto st_4;
    }
    xEventGroupSetBits(dwin_event_group, BIT_WEATHER_OK);
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
    weather_PIC = get_pic(atoi(id[0]), pod[0][0] == 'n');
    strncpy(description_WEATHER, description[0], MAX_LEN_DESCRIPTION);
    for(int i=0; pop[i] && temp_feel[i]; i++){
        temp_FEELS_LIKE[i] = atoi(temp_feel[i]);
        PoP[i] = atof(pop[i])*100;
    }
    temp_OUTDOOR = atoi(temp[0]);
st_4:
    if(dt_txt)free(dt_txt);
    if(pop)free(pop);
    if(sunrise)free(sunrise);
    if(sunset)free(sunset);
    if(id)free(id);
    if(temp)free(temp);
    if(description)free(description);
    if(temp_feel)free(temp_feel);
    if(pod)free(pod);
st_3:
    esp_http_client_cleanup(client);
    free(local_response_buffer);
st_2:
    free(url_buf);
st_1:
    set_new_command(UPDATE_WEATHER_COMPLETE);
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