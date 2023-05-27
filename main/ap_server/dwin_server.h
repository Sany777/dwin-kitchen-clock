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

#define MY_IP "http://192.168.4.1";


/* GET */

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t set_run_webserver(const bool start);


#ifdef __cplusplus
}
#endif






