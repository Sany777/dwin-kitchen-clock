#pragma once

#include "esp_err.h"
#include "esp_log.h"

#define DWIN_IF_FALSE_GOTO(condition, target)                                  \
  do {                                                                           \
    if (!(condition)) {                                                         \
      goto target;                                                              \
    }                                                                            \
  } while (0)

#define DWIN_SHOW_ERR(expression)                                                \
  do {                                                                           \
    const esp_err_t dwin_error__ = (expression);                                \
    if (dwin_error__ != ESP_OK) {                                               \
      ESP_LOGE("DWIN", "%s", esp_err_to_name(dwin_error__));                   \
    }                                                                            \
  } while (0)

#define DWIN_CHECK_NULL_AND_GO(pointer, message, target)                        \
  do {                                                                           \
    if ((pointer) == NULL) {                                                    \
      ESP_LOGE("DWIN", "%s", (message));                                       \
      goto target;                                                              \
    }                                                                            \
  } while (0)

#define DWIN_CHECK_AND_GO(expression, target)                                   \
  do {                                                                           \
    const esp_err_t dwin_error__ = (expression);                               \
    if (dwin_error__ != ESP_OK) {                                               \
      ESP_LOGE("DWIN", "%s", esp_err_to_name(dwin_error__));                   \
      goto target;                                                              \
    }                                                                            \
  } while (0)

#define DWIN_CHECK_AND_RETURN(expression)                                       \
  do {                                                                           \
    const esp_err_t dwin_error__ = (expression);                               \
    if (dwin_error__ != ESP_OK) {                                               \
      ESP_LOGE("DWIN", "%s", esp_err_to_name(dwin_error__));                   \
      return dwin_error__;                                                      \
    }                                                                            \
  } while (0)
