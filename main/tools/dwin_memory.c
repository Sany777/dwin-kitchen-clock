#include "dwin_memory.h"




void write_memory(main_data_t *main_data, const int data_identificator) 
{
	nvs_handle_t nvs_handle_dwin;
    DWIN_SHOW_ERR(nvs_open("nvs", NVS_READWRITE, &nvs_handle_dwin));
	switch(data_identificator) {
		case DATA_PWD :
		{
			nvs_set_str(nvs_handle_dwin, "password", pwd_WIFI); 
			break;
		}
		case DATA_API :
		{
			nvs_set_str(nvs_handle_dwin, "api", api_KEY); 
			break;
		}
		case DATA_CITY :
		{
			nvs_set_str(nvs_handle_dwin, "city", name_CITY); 
			break;
		} 	
		case DATA_SSID :
		{
			nvs_set_str(nvs_handle_dwin, "ssid", name_SSID); 
			break;
		}
		case DATA_FLAGS :
		{
			EventBits_t uxBits = xEventGroupGetBits(dwin_event_group);
			uint32_t flags = (uint32_t)uxBits&STORED_FLAGS;
			nvs_set_i32(nvs_handle_dwin, "flag", flags); 
			break;
		}
		case DATA_COLOUR :
		{
			nvs_set_blob(nvs_handle_dwin, "color", colors_INTERFACE, SIZE_COLOURS_INTERFACE); 
			break;
		}	
		case DATA_NOTIF :
		{
			nvs_set_blob(nvs_handle_dwin, "notif", notification_DATA, SIZE_NOTIFICATION);							  	
			break;
		}
		
		default : break;
	}
	DWIN_SHOW_ERR(nvs_commit(nvs_handle_dwin));
	nvs_close(nvs_handle_dwin);
}


void read_memory(main_data_t *main_data, const int data_identificator) 
{
	nvs_handle_t nvs_handle_dwin;
	DWIN_SHOW_ERR(nvs_open("nvs", NVS_READWRITE, &nvs_handle_dwin));
	size_t len;
	switch(data_identificator) {
		case DATA_PWD : 
		{
			len = SIZE_BUF;
			nvs_get_str(nvs_handle_dwin, "password", pwd_WIFI, &len); 
			break;
		}
		case DATA_API :
		{
			len = SIZE_API;
			nvs_get_str(nvs_handle_dwin, "api", api_KEY, &len); 
			break;
		}
		case DATA_COLOUR :
		{
			len = SIZE_COLOURS_INTERFACE;
			nvs_get_blob(nvs_handle_dwin, "color", (uint8_t *)colors_INTERFACE, &len); 
			break;
		}
		case DATA_CITY :
		{
			len = SIZE_BUF;
			nvs_get_str(nvs_handle_dwin, "city", name_CITY, &len); 
			break;
		}
		case DATA_SSID :
		{
			len = SIZE_BUF;
			nvs_get_str(nvs_handle_dwin, "ssid", name_SSID, &len); 
			break;
		}
		case DATA_FLAGS :
		{
			// xEventGroupClearBits(dwin_event_group, STORED_FLAGS);
			uint32_t flags = 0;
			nvs_get_u32(nvs_handle_dwin, "flag", &flags); 
			// xEventGroupSetBits(dwin_event_group, flags&STORED_FLAGS);
			break;
		}
		case DATA_NOTIF :
		{
			len = SIZE_NOTIFICATION;
			nvs_get_blob(nvs_handle_dwin, "notif", (uint8_t *)notification_DATA, &len); 						  	
			break;
		}
		
		default : break;
	}
	nvs_close(nvs_handle_dwin);
}

void read_all_memory(main_data_t *main_data) 
{
	for(uint8_t i=0; i<END_DATA_IDENTEFIER; i++) {
		read_memory(main_data, i);
	}
}




