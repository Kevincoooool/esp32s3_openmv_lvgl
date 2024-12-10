#include <nvs_flash.h>
#include <nvs.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "firmware_version.h"

// 初始化NVS
void initNVS() {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}
 
// 存储版本信息
void saveVersionInfo(const char* part, int version) {
    nvs_handle_t my_handle;
    ESP_ERROR_CHECK(nvs_open("nvs", NVS_READWRITE, &my_handle));
    ESP_ERROR_CHECK(nvs_set_i32(my_handle, part, version));
    ESP_ERROR_CHECK(nvs_commit(my_handle));
    nvs_close(my_handle);
}
 
// 读取版本信息
int loadVersionInfo(const char* part) {
    nvs_handle_t my_handle;
    ESP_ERROR_CHECK(nvs_open("nvs", NVS_READWRITE, &my_handle));
    int version = 0;
    nvs_get_i32(my_handle, part, &version);
    nvs_close(my_handle);
    return version;
}

void save_firmware_version(int version) {
    saveVersionInfo("S3_version", version);
}

int get_firmware_version(void) {
        // 读取版本信息
    return loadVersionInfo("S3_version");
}

int check_firmware_version(int version) {

	unsigned char version_major=version/100;
	unsigned char version_minor=((int)(version/10))%10;
	unsigned char version_patch=((int)(version%100))%10;
	printf("KSDIY_openmv_esp32S3 firmware version: %d.%d.%d\n",version_major ,version_minor,version_patch);
	return 0;
}

