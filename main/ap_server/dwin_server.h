#pragma once 

#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include "cJSON.h"
#include "esp_http_server.h"

#include "driver/uart.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "esp_chip_info.h"
#include "dwin_common.h"

#ifdef __cplusplus
extern "C" {
#endif


#define MAX_LEN_COMMAND_DWIN 249

#define DWIN_SERVER_CHECK(a, req, goto_tag)                                                 \
        do{                                                                                 \
        if (ESP_OK != (a)) {                                                                \
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, esp_err_to_name(a));  \
            goto goto_tag;                                                                  \
        }                                                                                   \
    } while(0)

#define DWIN_RESP_ERR(req, str, goto_tag)                                                   \
    do {                                                                                    \
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, str);                 \
            goto goto_tag;                                                                  \
        } while(0)

/* if !main_data : stop server */
esp_err_t start_server(dwin_data_t *main_data);
esp_err_t stop_server();

#ifdef __cplusplus
}
#endif






