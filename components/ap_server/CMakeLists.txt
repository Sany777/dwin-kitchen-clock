idf_component_register(SRC_DIRS "."
                       INCLUDE_DIRS "include"
                       REQUIRES common esp_http_server json esp_wifi esp_netif app_update esp_partition driver esp_driver_uart esp_hw_support dwin_drivers wifi
                       EMBED_FILES 
                           "./embedded/aqua.css"
                           "./embedded/aqua.js"
                           "./embedded/aqua.html"
                           "./embedded/setting.js"
                           "./embedded/setting.css"
                           "./embedded/index.html"
                           "./embedded/style.css"
                           "./embedded/script.js"
                           "./embedded/dwin.html"
                           "./embedded/dwin.css"
                           "./embedded/dwin.js"
                           "./embedded/favicon.ico")
