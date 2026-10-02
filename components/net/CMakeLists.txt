idf_component_register(SRCS "dwin_server.c" "dwin_services.c"
                       INCLUDE_DIRS "includes"
                       REQUIRES core wifi dwin_screen esp_timer esp_http_client esp_http_server json
                                app_update esp_partition driver esp_driver_uart esp_hw_support
                       EMBED_FILES
                           "embedded/aqua.css"
                           "embedded/aqua.js"
                           "embedded/aqua.html"
                           "embedded/setting.js"
                           "embedded/setting.css"
                           "embedded/index.html"
                           "embedded/style.css"
                           "embedded/script.js"
                           "embedded/dwin.html"
                           "embedded/dwin.css"
                           "embedded/dwin.js"
                           "embedded/favicon.ico")
