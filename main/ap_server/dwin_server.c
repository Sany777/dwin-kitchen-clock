#include "dwin_server.h"
#include "stdlib.h"

#define LEN_DATA_SEND_NOTIF (SIZE_BUF_NOTIFICATION*2+1)
#define BASE_RESPONSE_SAVE "Save image to position"
#define LEN_RESPONSE_SAVE (sizeof(BASE_RESPONSE_SAVE))
#define BASE_PATH_SAVE_PIC "/sevepic"
#define BASE_PATH_IMG "/img"


static esp_err_t index_redirect_handler(httpd_req_t *req)
{
    httpd_resp_set_status(req, "307 Temporary Redirect");
    httpd_resp_set_hdr(req, "Location", "/index.html");
    httpd_resp_send(req, NULL, 0);  // Response body can be empty
    return ESP_OK;
}

/*index*/
static esp_err_t get_index_handler(httpd_req_t *req)
{
    extern const unsigned char index_html_start[] asm( "_binary_index_html_start" );
    extern const unsigned char index_html_end[] asm( "_binary_index_html_end" );
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start); 
    return ESP_OK;
}

static esp_err_t get_css_handler(httpd_req_t *req)
{
    extern const unsigned char style_css_start[] asm( "_binary_style_css_start" );
    extern const unsigned char style_css_end[] asm( "_binary_style_css_end" );
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)style_css_start, style_css_end - style_css_start ); 
    return ESP_OK;
}

static esp_err_t get_js_handler(httpd_req_t *req)
{
    extern const unsigned char script_js_start[] asm( "_binary_script_js_start" );
    extern const unsigned char script_js_end[] asm( "_binary_script_js_end" );
    httpd_resp_set_type(req, "text/javascript");
    httpd_resp_send(req, (const char *)script_js_start, script_js_end - script_js_start ); 
    return ESP_OK;
}

/*aqua*/
static esp_err_t get_aqua_handler(httpd_req_t *req)
{
    extern const unsigned char aqua_html_start[] asm( "_binary_aqua_html_start" );
    extern const unsigned char aqua_html_end[] asm( "_binary_aqua_html_end" );
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)aqua_html_start, aqua_html_end - aqua_html_start ); 
    return ESP_OK;
}

static esp_err_t get_aqua_css_handler(httpd_req_t *req)
{
    extern const unsigned char aqua_css_start[] asm( "_binary_aqua_css_start" );
    extern const unsigned char aqua_css_end[] asm( "_binary_aqua_css_end" );
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)aqua_css_start, aqua_css_end - aqua_css_start); 
    return ESP_OK;
}

static esp_err_t get_aqua_js_handler(httpd_req_t *req)
{
    extern const unsigned char aqua_js_start[] asm( "_binary_aqua_js_start" );
    extern const unsigned char aqua_js_end[] asm( "_binary_aqua_js_end" );
    httpd_resp_set_type(req, "text/javascript");
    httpd_resp_send(req, (const char *)aqua_js_start, aqua_js_end - aqua_js_start ); 
    return ESP_OK;
}

/*setting*/
static esp_err_t get_setting_css_handler(httpd_req_t *req)
{
    extern const unsigned char setting_css_start[] asm( "_binary_setting_css_start" );
    extern const unsigned char setting_css_end[] asm( "_binary_setting_css_end" );
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)setting_css_start, setting_css_end - setting_css_start ); 
    return ESP_OK;
}

static esp_err_t get_setting_js_handler(httpd_req_t *req)
{
    extern const unsigned char setting_js_start[] asm( "_binary_setting_js_start" );
    extern const unsigned char setting_js_end[] asm( "_binary_setting_js_end" );
    httpd_resp_set_type(req, "text/javascript");
    httpd_resp_send(req, (const char *)setting_js_start, setting_js_end - setting_js_start ); 
    return ESP_OK;
}

/*dwin portal*/
static esp_err_t get_dwin_handler(httpd_req_t *req)
{
    extern const unsigned char portal_html_start[] asm( "_binary_dwin_html_start" );
    extern const unsigned char portal_html_end[] asm( "_binary_dwin_html_end" );
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)portal_html_start, portal_html_end - portal_html_start); 
    return ESP_OK;
}

static esp_err_t get_dwin_css_handler(httpd_req_t *req)
{
    extern const unsigned char dwin_css_start[] asm( "_binary_dwin_css_start" );
    extern const unsigned char dwin_css_end[] asm( "_binary_dwin_css_end" );
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)dwin_css_start, dwin_css_end - dwin_css_start); 
    return ESP_OK;
}

static esp_err_t get_dwin_js_handler(httpd_req_t *req)
{
    extern const unsigned char dwin_js_start[] asm( "_binary_dwin_js_start" );
    extern const unsigned char dwin_js_end[] asm( "_binary_dwin_js_end" );
    httpd_resp_set_type(req, "text/javascript");
    httpd_resp_send(req, (const char *)dwin_js_start, dwin_js_end - dwin_js_start); 
    return ESP_OK;
}

static esp_err_t get_ico_handler(httpd_req_t *req)
{
    extern const unsigned char favicon_ico_start[] asm( "_binary_favicon_ico_start" );
    extern const unsigned char favicon_ico_end[] asm( "_binary_favicon_ico_end" );
    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_send(req, (const char *)favicon_ico_start, favicon_ico_end - favicon_ico_start); 
    return ESP_OK;
}

static esp_err_t handler_update_dwin(httpd_req_t *req)
{
    int total_len = req->content_len;
    if (total_len >= SCRATCH_SIZE) {
        DWIN_RESP_ERR(req, "Content too long", err);
    }
    init_update_dwin();
    /* check response */
    char * const server_buf = (char *)req->user_ctx;
    const int received = httpd_req_recv(req, server_buf, SCRATCH_SIZE);
    if (received != total_len) {
        DWIN_RESP_ERR(req, "Failed to post control value", err);
    }
    uart_write_bytes(UART_DWIN, server_buf, total_len);
    return ESP_OK;
err:
    return ESP_FAIL;
}



static esp_err_t handler_set_raw_data(httpd_req_t *req)
{
    const int total_len = req->content_len;
    if (total_len >= MAX_LEN_COMMAND_DWIN) {
        DWIN_RESP_ERR(req, "Content too long", err);
    }
    char * const server_buf = (char *)req->user_ctx;
    const int received = httpd_req_recv(req, server_buf, SCRATCH_SIZE);
    if (received != total_len) {
        DWIN_RESP_ERR(req, "Failed to post control value", err);
    }
    send_char(FRAME_HEADER);
    uart_write_bytes(UART_NUM_0, server_buf, total_len);
    print_end();
    httpd_resp_sendstr(req, "Send ok");
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t handler_update_esp(httpd_req_t *req)
{
    const esp_partition_t *update_partition = NULL;
    esp_ota_handle_t update_handle = 0 ;
    const int total_len = req->content_len;
    char * const server_buf = (char *)req->user_ctx;
    int cur_len = 0;
    int received = 0;
    if (total_len >= MAX_OTA_SIZE) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    };
    update_partition = esp_ota_get_next_update_partition(NULL);
    DWIN_SERVER_CHECK(esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle), req, _err_start);
    while (cur_len < total_len) {
        received = httpd_req_recv(req, server_buf, SCRATCH_SIZE-1);
        if(received <= 0 ){
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR,"Failed to post control value");
            goto _err;
        }
        cur_len += received;
        DWIN_SERVER_CHECK(esp_ota_write(update_handle, server_buf, received), req, _err);
    }
    DWIN_SERVER_CHECK(esp_ota_end(update_handle), req, _err);
    DWIN_SERVER_CHECK(esp_ota_set_boot_partition(update_partition), req, _err);
    httpd_resp_sendstr(req, "Update seccessful");
    vTaskDelay(100);
    esp_restart();
    return ESP_OK;
_err:
    esp_ota_abort(update_handle);
_err_start:
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Fail update");
    return ESP_FAIL;
}



static esp_err_t handler_set_img(httpd_req_t *req)
{
    const int total_len = req->content_len;
    if (total_len > MAX_LEN_CHUNC_IMG || !total_len) {
        DWIN_RESP_ERR(req, "Data length too long", err);
    }
    char * const server_buf = (char *)req->user_ctx;
    int cur_len = 0;
    int received = 0;
    while (cur_len < total_len) {
        received = httpd_req_recv(req, server_buf, total_len);
        if (received <= 0) {
            DWIN_RESP_ERR(req, "Failed to post control value", err);
        }
        cur_len += received;
    }
    send_chunc(server_buf, total_len);
    if(total_len < MAX_LEN_CHUNC_IMG){
        char *pos_pic_str = get_data_from_uri(req->uri, BASE_PATH_SAVE_PIC);
        if(!pos_pic_str){
            DWIN_RESP_ERR(req, "Format data position pic is wrong", err);
        }
        int pos_pic = atoi(pos_pic_str);
        if(pos_pic > 255 || pos_pic < 0){
            DWIN_RESP_ERR(req, "Format data position pic is wrong", err);
        }
        save_pic(pos_pic);
    }
    httpd_resp_sendstr(req, "ok");
    return ESP_OK;
err:
    send_chunc(NULL, 0);
    return ESP_FAIL;
}


static esp_err_t handler_set_color(httpd_req_t *req)
{
    const int total_len = req->content_len;
    char * const server_buf = (char *)req->user_ctx;
    if(total_len > SCRATCH_SIZE){
        DWIN_RESP_ERR(req, "Data length too long", err);
    }
    const int received = httpd_req_recv(req, server_buf, total_len);
    if (received != total_len) {
        DWIN_RESP_ERR(req, "Data not read", err);
    }
    server_buf[total_len] = 0;
    cJSON *root = cJSON_Parse(server_buf);
    const cJSON *bcolor_j = cJSON_GetObjectItemCaseSensitive(root, "b");
    const cJSON *fcolor_j = cJSON_GetObjectItemCaseSensitive(root, "f");
    uint16_t bcolor = 0, fcolor = 0;
    bool is_f = false, is_b = false;
    if(cJSON_IsNumber(bcolor_j)){
        bcolor = (uint16_t )bcolor_j->valueint;
        is_b = true;
    }
    if(cJSON_IsNumber(fcolor_j)){
        fcolor = (uint16_t )fcolor_j->valueint;
        is_f = true;
    }
    if(!is_b || !is_f){
        DWIN_RESP_ERR(req, "Data not read", _err);
    }
    set_color(fcolor, bcolor);
    httpd_resp_sendstr(req, "Success");
    cJSON_Delete(root);
    return ESP_OK;
_err:
    cJSON_Delete(root);
err:
    return ESP_FAIL;
}

static esp_err_t handler_save_pic(httpd_req_t *req)
{
    const char *pos_pic_str = get_data_from_uri(req->uri, BASE_PATH_SAVE_PIC);
    if(!pos_pic_str){
        DWIN_RESP_ERR(req, "Data not read", err);
    }
    int pos_pic = atoi(pos_pic_str);
    if(pos_pic > 255 || pos_pic < 0){
        DWIN_RESP_ERR(req, "Wrong value position pic", err);
    }
    save_pic(pos_pic);
    char *buf_resp = calloc(1, LEN_RESPONSE_SAVE+5);
    if(buf_resp){
        sprintf(buf_resp,  "%s %d", BASE_RESPONSE_SAVE, pos_pic);
        httpd_resp_sendstr(req, buf_resp);
        free(buf_resp);
    }
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t handler_clear_screen(httpd_req_t *req)
{
    clear_screen();
    httpd_resp_sendstr(req, "Screen cleaning");
    return ESP_OK;
}

static esp_err_t handler_send_hello(httpd_req_t *req)
{
    send_hello();
    /* check response */
    httpd_resp_sendstr(req, "Dwin ok");
    return ESP_OK;
}

static esp_err_t handler_close(httpd_req_t *req)
{
    httpd_resp_sendstr(req, "Goodbay!");
    vTaskDelay(100);
    esp_wifi_stop();
    return ESP_OK;
}

static esp_err_t handler_set_network(httpd_req_t *req)
{
    const size_t total_len = req->content_len;
    main_data_t *main_data = (main_data_t *)req->user_ctx;
    if(total_len > SCRATCH_SIZE){
         DWIN_RESP_ERR(req, "content too long", err);
    }
    char *server_buf = malloc(total_len+1);
    if(server_buf == NULL){
        DWIN_RESP_ERR(req, "Not enough storage", err);
    }
    const int received = httpd_req_recv(req, server_buf, total_len);
    if (received != total_len) {
        DWIN_RESP_ERR(req, "Data not read", err_b);
    }
    server_buf[received] = 0;
    cJSON *root = cJSON_Parse(server_buf);
    const cJSON *ssid_name_j = cJSON_GetObjectItemCaseSensitive(root, "SSID");
    const cJSON *pwd_wifi_j = cJSON_GetObjectItemCaseSensitive(root, "PWD");
    const char *ssid_name, *pwd_wifi;
    size_t pwd_len = 0, ssid_len = 0;
    if(cJSON_IsString(ssid_name_j) && (ssid_name_j->valuestring != NULL)){
        ssid_name = ssid_name_j->valuestring;
        ssid_len = strnlen(ssid_name, SIZE_BUF);
    }
    if(cJSON_IsString(pwd_wifi_j) && (pwd_wifi_j->valuestring != NULL)){
        pwd_wifi = pwd_wifi_j->valuestring;
        pwd_len = strnlen(pwd_wifi, SIZE_BUF);
    }
    if((!pwd_len && !ssid_len) 
        || ssid_len >= MAX_STR_LEN 
        || pwd_len >= MAX_STR_LEN)
    {
        DWIN_RESP_ERR(req, "Wrong format data", err_j);
    }
    if(ssid_len){
        memcpy(name_SSID, ssid_name, ssid_len+1);
        write_memory(main_data, DATA_SSID);
    }
    if(pwd_len){
        memcpy(pwd_WIFI, pwd_wifi, pwd_len+1);
        write_memory(main_data, DATA_PWD);
    }
    free(server_buf);
    cJSON_Delete(root);
    httpd_resp_sendstr(req, "Successful");
    return ESP_OK;
err_j: 
    cJSON_Delete(root);
err_b:
    free(server_buf);
err:
    return ESP_FAIL;
}

static esp_err_t handler_set_api(httpd_req_t *req)
{
    const int total_len = req->content_len;
    main_data_t *main_data = (main_data_t *)req->user_ctx;
    if(total_len > SCRATCH_SIZE){
        DWIN_RESP_ERR(req, "content too long", err);
    }
    char *server_buf = malloc(total_len+1);
    if(server_buf == NULL){
        DWIN_RESP_ERR(req, "Not enough storage", err);
    }
    const int received = httpd_req_recv(req, server_buf, total_len);
    if (received != total_len) {
        DWIN_RESP_ERR(req, "Data not read", _err);
    }
    server_buf[received] = 0;
    cJSON *root = cJSON_Parse(server_buf);
    const cJSON *city_j = cJSON_GetObjectItemCaseSensitive(root, "City");
    const cJSON *key_j = cJSON_GetObjectItemCaseSensitive(root, "Key");
    const char *key = NULL, *city_name = NULL;
    size_t key_len = 0, city_len = 0;
    if(cJSON_IsString(city_j) && (city_j->valuestring != NULL)){
        city_name = city_j->valuestring;
        city_len = strnlen(city_name, SIZE_BUF);
    }
    if(cJSON_IsString(key_j) && (key_j->valuestring != NULL)){
        key = key_j->valuestring;
        key_len = strnlen(key, SIZE_BUF);
    }
    if((city_len == 0 && key_len == 0) 
        || city_len > MAX_STR_LEN)
    {
        DWIN_RESP_ERR(req, "Wrong format input", __err);
    }
    if(key_len == SIZE_API){
        memcpy(api_KEY, key, key_len+1);
        write_memory(main_data, DATA_API);
    }
    if(city_len){
        memcpy(name_CITY, city_name, city_len+1); 
        write_memory(main_data, DATA_CITY);
    }
    cJSON_Delete(root);
    free(server_buf);
    httpd_resp_sendstr(req, "Successful");
    return ESP_OK;
__err:
    cJSON_Delete(root);
_err:
    free(server_buf);
err:
    return ESP_FAIL;
}



static esp_err_t handler_get_info(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    cJSON_AddStringToObject(root, "version", IDF_VER);
    cJSON_AddStringToObject(root, "chip", get_chip(chip_info.model));
    cJSON_AddNumberToObject(root, "revision", chip_info.revision);
    const char *sys_info = cJSON_Print(root);
    if(sys_info){
        httpd_resp_sendstr(req, sys_info);
        free((void *)sys_info);
    }
    cJSON_Delete(root);
    return ESP_OK;
}

static esp_err_t handler_set_time(httpd_req_t *req)
{
    const int total_len = req->content_len;
    char * const server_buf = (char *)req->user_ctx;
    if(total_len > SCRATCH_SIZE){
        DWIN_RESP_ERR(req, "Content too long", err);
    }
    const int received = httpd_req_recv(req, server_buf, total_len);
    if (received != total_len) {
        DWIN_RESP_ERR(req, "Data not read", err);
    }
    server_buf[total_len] = 0;
    long long time = atoll(server_buf);
    if(!time){
        DWIN_RESP_ERR(req, "Value wrong", err);
    }
    struct timeval tv;
    tv.tv_sec = time/1000;
    tv.tv_usec = time%1000;
    set_time_tv(&tv);
    httpd_resp_sendstr(req, "Set time successfully");
    return ESP_OK;
err:
    return ESP_FAIL;
}


static esp_err_t handler_give_data(httpd_req_t *req)
{
    main_data_t * main_data = (main_data_t *)req->user_ctx;
    char *notif_send = malloc(LEN_DATA_SEND_NOTIF);
    if(notif_send == NULL){
        DWIN_RESP_ERR(req, "Not enough storage", err);
    }
    EventBits_t uxBits = xEventGroupGetBits(dwin_event_group);
    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "SSID", name_SSID);
    cJSON_AddStringToObject(root, "PWD", pwd_WIFI);
    cJSON_AddStringToObject(root, "Key", api_KEY);
    cJSON_AddStringToObject(root, "City", name_CITY);
    cJSON_AddNumberToObject(root, "Status", uxBits);
    for(uint32_t day=0, notif=0, i_s=0, hour=0, min=0; i_s<LEN_DATA_SEND_NOTIF; notif++){
        hour = VALUE_NOTIF_HOUR(notif, day);
        min = VALUE_NOTIF_MIN(notif, day);
        if(!IS_HOUR(hour)){
            hour = 10;
        }
        if(!IS_MIN_OR_SEC(min)){
            min = 0;
        }
        notif_send[i_s++] = hour/10 +'0';
        notif_send[i_s++] = hour%10 +'0';
        notif_send[i_s++] = min/10 +'0';
        notif_send[i_s++] = min%10 + '0';
        if(notif == NOTIF_PER_DAY){
            notif = 0;
            day++;
        }
    }
    notif_send[LEN_DATA_SEND_NOTIF-1] = 0;
    cJSON_AddStringToObject(root, "Notification", notif_send);
    const char *data_info = cJSON_Print(root);
    if(!data_info){
        free(notif_send);
        DWIN_RESP_ERR(req, "Not enough storage", err);
    }
    httpd_resp_sendstr(req, data_info);
    free((void *)data_info);
    free(notif_send);
    cJSON_Delete(root);
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t handler_set_flag(httpd_req_t *req)
{
    const int total_len = req->content_len;
    char * const server_buf = (char *)req->user_ctx;
    if(total_len > SCRATCH_SIZE){
        DWIN_RESP_ERR(req, "Content too long", err);
    }
    const int received = httpd_req_recv(req, server_buf, total_len);
    if (received != total_len) {
        DWIN_RESP_ERR(req, "Data not read", err);
    }
    server_buf[total_len] = 0;
    long flag = atoll(server_buf);
    for(int i=0; i<NUMBER_STORED_FLAGS; i++){
        if(flag&(1<<i)){
            xEventGroupSetBits(dwin_event_group, (1<<i));
        }else {
            xEventGroupClearBits(dwin_event_group, (1<<i));
        }
    }
    write_memory(NULL, DATA_FLAGS);
    httpd_resp_sendstr(req, "Set flags successfully");
    return ESP_OK;
err:
    return ESP_FAIL;
}


static esp_err_t set_notif_handler(httpd_req_t *req)
{
    const int total_len = req->content_len;
    if(total_len != LEN_DATA_SEND_NOTIF-1){
        DWIN_RESP_ERR(req, "Wrong data format", err);
    }
    char * const server_buf = malloc(LEN_DATA_SEND_NOTIF);
        if(server_buf == NULL){
        DWIN_RESP_ERR(req, "Not enough storage", err);
    }
    main_data_t * main_data = (main_data_t *)req->user_ctx;
    const int received = httpd_req_recv(req, server_buf, total_len);
    if (received != total_len) {
        DWIN_RESP_ERR(req, "Data not read", _err);
    }
    for(uint8_t i=0, num_notif = 0, day=0,val=0; total_len > i; num_notif++){
        val = GET_NUMBER(server_buf[i])*10 + GET_NUMBER(server_buf[i+1]);
        if(!IS_HOUR(val)){
           DWIN_RESP_ERR(req, "Value hour is wrong", _err); 
        }
        SET_NOTIF_HOUR(num_notif, day, val);
        i += 2;
        val = GET_NUMBER(server_buf[i])*10 + GET_NUMBER(server_buf[i+1]);
        if(!IS_MIN_OR_SEC(val)){
           DWIN_RESP_ERR(req, "Value minute is wrong", _err); 
        }
        SET_NOTIF_MIN(num_notif, day, val);
        i += 2;
        if(num_notif == NOTIF_PER_DAY){
            day++;
            num_notif = 0;
        }
    }
    write_memory(main_data, DATA_NOTIF);
    httpd_resp_sendstr(req, "Update notification");
    free(server_buf);
    return ESP_OK;
_err:
    free(server_buf);
err:
    return ESP_FAIL;
}


esp_err_t set_run_webserver(main_data_t *main_data)
{ 
    static char *server_buf;
    static httpd_handle_t server;
    if(main_data && server_buf == NULL){
        server_buf = malloc(SCRATCH_SIZE);
        assert(server_buf);
        httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        config.max_uri_handlers = 29;
        config.uri_match_fn = httpd_uri_match_wildcard;
        esp_err_t e = httpd_start(&server, &config);
        if (e != ESP_OK){
            return e;
        }
    } else if(server_buf){
        esp_err_t err = httpd_stop(server);
        free(server_buf); 
        server_buf = NULL;
        return err;
    } 
    httpd_uri_t send_flags= {
        .uri      = "/Status",
        .method   = HTTP_POST,
        .handler  = handler_set_flag,
        .user_ctx = server_buf
    };
    httpd_register_uri_handler(server, &send_flags);

    httpd_uri_t send_raw = {
        .uri      = "/sendraw",
        .method   = HTTP_POST,
        .handler  = handler_set_raw_data,
        .user_ctx = server_buf
    };
    httpd_register_uri_handler(server, &send_raw);
     
    httpd_uri_t get_info = {
        .uri      = "/getinfo",
        .method   = HTTP_GET,
        .handler  = handler_get_info,
        .user_ctx = server_buf
    };
    httpd_register_uri_handler(server, &get_info);
    httpd_uri_t savepic = {
        .uri      = "/savepic/*",
        .method   = HTTP_POST,
        .handler  = handler_save_pic,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &savepic);
    httpd_uri_t clear = {
        .uri      = "/clear",
        .method   = HTTP_POST,
        .handler  = handler_clear_screen,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &clear);
    httpd_uri_t get_setting = {
        .uri      = "/data?",
        .method   = HTTP_GET,
        .handler  = handler_give_data,
        .user_ctx = main_data
    };
    httpd_register_uri_handler(server, &get_setting);
     httpd_uri_t dwin_uri = {
        .uri      = "/updatedwin",
        .method   = HTTP_POST,
        .handler  = handler_update_dwin,
        .user_ctx = server_buf
    };
    httpd_register_uri_handler(server, &dwin_uri);
     httpd_uri_t img_uri = {
        .uri      = "/img/*",
        .method   = HTTP_POST,
        .handler  = handler_set_img,
        .user_ctx = server_buf
    };
    httpd_register_uri_handler(server, &img_uri);
     httpd_uri_t set_color_uri = {
        .uri      = "/color",
        .method   = HTTP_POST,
        .handler  = handler_set_color,
        .user_ctx = server_buf
    };
    httpd_register_uri_handler(server, &set_color_uri);
     httpd_uri_t hello_uri = {
        .uri      = "/hello",
        .method   = HTTP_POST,
        .handler  = handler_send_hello,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &hello_uri);
     httpd_uri_t close_uri = {
        .uri      = "/close",
        .method   = HTTP_POST,
        .handler  = handler_close,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &close_uri);
     httpd_uri_t net_uri = {
        .uri      = "/Network",
        .method   = HTTP_POST,
        .handler  = handler_set_network,
        .user_ctx = main_data
    };
    httpd_register_uri_handler(server, &net_uri);
     httpd_uri_t api_uri = {
        .uri      = "/API",
        .method   = HTTP_POST,
        .handler  = handler_set_api,
        .user_ctx = main_data
    };
    httpd_register_uri_handler(server, &api_uri);
     httpd_uri_t time_uri = {
        .uri      = "/time",
        .method   = HTTP_POST,
        .handler  = handler_set_time,
        .user_ctx = server_buf
    };
    httpd_register_uri_handler(server, &time_uri);
     httpd_uri_t update_uri = {
        .uri      = "/Update",
        .method   = HTTP_POST,
        .handler  = handler_update_esp,
        .user_ctx = server_buf
    };
    httpd_register_uri_handler(server, &update_uri);

     httpd_uri_t index_uri = {
        .uri      = "/index.html",
        .method   = HTTP_GET,
        .handler  = get_index_handler ,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &index_uri);


     httpd_uri_t aqua_uri = {
        .uri      = "/aqua.html",
        .method   = HTTP_GET,
        .handler  = get_aqua_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &aqua_uri);
     httpd_uri_t get_ico = {
        .uri      = "/favicon.ico",
        .method   = HTTP_GET,
        .handler  = get_ico_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &get_ico);

     httpd_uri_t get_style = {
        .uri      = "/style.css",
        .method   = HTTP_GET,
        .handler  = get_css_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &get_style);

     httpd_uri_t get_script = {
        .uri      = "/script.js",
        .method   = HTTP_GET,
        .handler  = get_js_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &get_script);
    httpd_uri_t get_aqua_css = {
        .uri      = "/aqua.css",
        .method   = HTTP_GET,
        .handler  = get_aqua_css_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &get_aqua_css);
    httpd_uri_t get_dwin_css = {
        .uri      = "/dwin.css",
        .method   = HTTP_GET,
        .handler  = get_dwin_css_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &get_dwin_css);

     httpd_uri_t get_aqua_js = {
        .uri      = "/aqua.js",
        .method   = HTTP_GET,
        .handler  = get_aqua_js_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &get_aqua_js);
     httpd_uri_t get_setting_style = {
        .uri      = "/setting.css",
        .method   = HTTP_GET,
        .handler  = get_setting_css_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &get_setting_style);
     httpd_uri_t get_setting_script = {
        .uri      = "/setting.js",
        .method   = HTTP_GET,
        .handler  = get_setting_js_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &get_setting_script);
     httpd_uri_t get_dwin_js = {
        .uri      = "/dwin.js",
        .method   = HTTP_GET,
        .handler  = get_dwin_js_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &get_dwin_js);
     httpd_uri_t get_portal_style = {
            .uri      = "/dwin.html",
            .method   = HTTP_GET,
            .handler  = get_dwin_handler,
            .user_ctx = NULL
        };
    httpd_register_uri_handler(server, &get_portal_style);

     httpd_uri_t notif_uri = {
        .uri      = "/Notification",
        .method   = HTTP_POST,
        .handler  = set_notif_handler,
        .user_ctx = main_data
    };
    httpd_register_uri_handler(server, &notif_uri);
     httpd_uri_t redir_uri = {
        .uri      = "/*",
        .method   = HTTP_GET,
        .handler  = index_redirect_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &redir_uri);
    return ESP_OK;
}