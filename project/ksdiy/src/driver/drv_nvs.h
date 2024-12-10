
#ifndef __DRV_NVS_H__
#define __DRV_NVS_H__

#define NVS_STRING_LENGTH_MAX  (128)

#define START_PY_NAMESPACE "START_32"
#define START_PY_KEY "VALUE"

esp_err_t nvs_init(void);
esp_err_t nvs_write_string(const char* namespace, const char* key, char* write_string);
esp_err_t nvs_read_string(const char* namespace, const char* key, char* read_string);
esp_err_t nvs_write_i32(const char* namespace, const char* key, int32_t value);
esp_err_t nvs_read_i32(const char* namespace, const char* key, int32_t* value);
#endif
