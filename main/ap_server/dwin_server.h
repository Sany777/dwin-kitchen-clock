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



/* GET */

#ifdef __cplusplus
extern "C" {
#endif


#define MAX_LEN_COMMAND_DWIN 249


#define DWIN_CHECK_FALSE_AND_GO(a, goto_tag)                                                                                             \
                                            do{                                                                                    \
                                                if (!(a)) {                                                                                              \
                                                    goto goto_tag;                                                                  \
                                                }                                                                                   \
                                            } while(0)

#define DWIN_CHECK_NULL_AND_GO(ptr, str, goto_tag)                                                                                  \
                                            do{                                                                                     \
                                                if (ptr == NULL) {                                                                  \
                                                    ESP_LOGI(TAG, "%s", str);                                                       \
                                                    goto goto_tag;                                                                  \
                                                }                                                                                   \
                                            } while(0)


#define DWIN_CHECK_AND_GO(a, goto_tag)                                                                                             \
                                            do{                                                                                    \
                                                if (ESP_OK != (a)) {                                                               \
                                                    ESP_LOGI(TAG, "%s", esp_err_to_name(a));                                        \
                                                    goto goto_tag;                                                                  \
                                                }                                                                                   \
                                            } while(0)


#define DWIN_SERVER_CHECK(a, req, goto_tag)                                                                                         \
                                             do{                                                                                    \
                                                if (ESP_OK != (a)) {                                                                \
                                                    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, esp_err_to_name(a));  \
                                                    goto goto_tag;                                                                  \
                                                }                                                                                   \
                                            } while(0)

#define DWIN_RESP_ERR(req, str, goto_tag)                                                                                           \
                                            do {                                                                                    \
                                                    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, str);                 \
                                                    goto goto_tag;                                                                  \
                                                } while(0)

/* if !main_data : stop server */
esp_err_t set_run_webserver(main_data_t *main_data);


#ifdef __cplusplus
}
#endif






