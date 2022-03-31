#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_err.h"
#include "esp_log.h"
#include "drv_nvs.h"

static const char *TAG = "DRV NVS";
static bool nvs_initialized = false;

esp_err_t nvs_init(void)
{
    esp_err_t ret = ESP_OK;
    if(nvs_initialized == true)
    {
        ESP_LOGE(TAG, "nvs driver has been Initialized");
        return ret;
    }
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    nvs_initialized = true;
    ESP_LOGE(TAG, "nvs driver is ok");
    return ret;
}

bool is_nvs_initialized(void)
{
    return nvs_initialized;
}

esp_err_t nvs_write_string(const char* namespace, const char* key, char* write_string)
{
    esp_err_t ret = ESP_OK;
    nvs_handle_t my_handle;
    size_t len = NVS_STRING_LENGTH_MAX;
    char check_buffer[NVS_STRING_LENGTH_MAX];
    memset(check_buffer, 0, NVS_STRING_LENGTH_MAX);
    if(!is_nvs_initialized())
    {
        nvs_init();
    }
    //Open   
    ret = nvs_open(namespace, NVS_READWRITE, &my_handle);
    if (ret != ESP_OK) 
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(ret));
        goto err_done;
    }
    else
    {
        ESP_LOGE(TAG, "nv open successed");
        // Write
        len = NVS_STRING_LENGTH_MAX;
        ret = nvs_get_str(my_handle, key, check_buffer, &len);

        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "read key failed!");
        }
        if(strcmp(write_string, check_buffer) != 0)
        {
            ret = nvs_set_str(my_handle, key, write_string);
        }
        else
        {
            ESP_LOGE(TAG, "string is same");
            ret = ESP_OK;
        }
        
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Error (%s) write to nv flash!\n", esp_err_to_name(ret));
            goto err_done;
        }
        ret = nvs_commit(my_handle);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "commit failed");
            goto err_done;
        }
    }
err_done:

    // Close
    nvs_close(my_handle);
    return ret;
}

esp_err_t nvs_read_string(const char* namespace, const char* key, char* read_string)
{
    esp_err_t ret = ESP_OK;
    nvs_handle my_handle;
    ret = nvs_open(namespace, NVS_READONLY, &my_handle);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(ret));
        return ret;
    } 
    else 
    {
        ESP_LOGE(TAG, "nv open successed");
        size_t len = NVS_STRING_LENGTH_MAX;
        ret = nvs_get_str(my_handle, key, read_string, &len);
        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Error (%s) read to nv flash!\n", esp_err_to_name(ret));
            nvs_close(my_handle);
            return ret;
        }
        else
        {
            nvs_close(my_handle);
            return ret;
        }
    }
}

esp_err_t nvs_write_i32(const char* namespace, const char* key, int32_t value)
{
    esp_err_t ret = ESP_OK;
    nvs_handle_t my_handle;
    int32_t check_value = -1;

    if(!is_nvs_initialized())
    {
        nvs_init();
    }
    //Open   
    ret = nvs_open(namespace, NVS_READWRITE, &my_handle);
    if (ret != ESP_OK) 
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(ret));
        goto err_done;
    }
    else
    {
        // Write
        ret = nvs_get_i32(my_handle, key, &check_value);

        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "read key failed!");
        }
        if(!(check_value == value))
        {
            ret = nvs_set_i32(my_handle, key, value);
        }
        else
        {
            ESP_LOGE(TAG, "nvs value is same");
            ret = ESP_OK;
        }
        
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Error (%s) write to nv flash!\n", esp_err_to_name(ret));
            goto err_done;
        }
        ret = nvs_commit(my_handle);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "commit failed");
            goto err_done;
        }
    }
err_done:

    // Close
    nvs_close(my_handle);
    return ret;
}

esp_err_t nvs_read_i32(const char* namespace, const char* key, int32_t* value)
{
    esp_err_t ret = ESP_OK;
    nvs_handle my_handle;
    ret = nvs_open(namespace, NVS_READONLY, &my_handle);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(ret));
        return ret;
    } 
    else 
    {
        size_t len = NVS_STRING_LENGTH_MAX;
        ret = nvs_get_i32(my_handle, key, value);
        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Error (%s) read to nv flash!\n", esp_err_to_name(ret));
            nvs_close(my_handle);
            return ret;
        }
        else
        {
            nvs_close(my_handle);
            return ret;
        }
    }
}
