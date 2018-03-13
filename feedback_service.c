
#include <stdint.h>
#include <string.h>
#include "nrf_gpio.h"
#include "feedback_service.h"
#include "bsp.h"
#include "bsp_btn_ble.h"
#include "ble_srv_common.h"
#include "app_error.h"

// ALREADY_DONE_FOR_YOU: Declaration of a function that will take care of some housekeeping of ble connections related to our service and characteristic
void ble_feedback_service_on_ble_evt(ble_fbs_t * p_feedback_service, ble_evt_t * p_ble_evt)
{
    // OUR_JOB: Step 3.D Implement switch case handling BLE events related to our service. 
    uint32_t err_code;
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            p_feedback_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            p_feedback_service->conn_handle = BLE_CONN_HANDLE_INVALID;
            break;
        case BLE_GATTS_EVT_WRITE:
            err_code = feedback_char_handle_write(p_feedback_service);
            APP_ERROR_CHECK(err_code);
        default:
            // No implementation needed.
            break;
    }
}

uint32_t feedback_char_handle_write(ble_fbs_t *p_feedback_service){
    uint32_t err_code;
    
    uint8_t input[2];
    ble_gatts_value_t temp;


    temp.p_value  = &input[0];
    temp.len      = 2;
    temp.offset   = 0;
    err_code = sd_ble_gatts_value_set(p_feedback_service->conn_handle, p_feedback_service->feedback_char_handles.value_handle, &temp);
    APP_ERROR_CHECK(err_code);


    //check input command check if null (0) 
    if(input[0] != 0){
        //start buzzer and then kick off a timer to stop it
        nrf_gpio_pin_toggle(LED_3);
    }

    return NRF_SUCCESS;
}


/**@brief Function for adding our new characterstic to "Our service" that we initiated in the previous tutorial. 
 *
 * @param[in]   p_feedback_service        Feedback Service structure.
 *
 */
static uint32_t feedback_char_add(ble_fbs_t * p_feedback_service)
{
    // Add a custom characteristic UUID
    uint32_t            err_code;
    ble_uuid_t          char_uuid;
    ble_uuid128_t       base_uuid = BLE_UUID_FEEDBACK_BASE_UUID;
    char_uuid.uuid      = BLE_UUID_FEEDBACK_CHARACTERISTC_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
    APP_ERROR_CHECK(err_code); 
    
    // Add read/write properties to our characteristic
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 0;
    char_md.char_props.write = 1;

    
    // Configuring Client Characteristic Configuration Descriptor metadata and add to char_md structure
    ble_gatts_attr_md_t cccd_md;
    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;    
    char_md.p_cccd_md           = &cccd_md;
    char_md.char_props.notify   = 0;
   
    
    // Configure the attribute metadata
    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md));
    attr_md.vloc        = BLE_GATTS_VLOC_STACK;
    
    
    // Set read/write security levels to our characteristic
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm); 
    
    
    // Configure the characteristic value attribute
    ble_gatts_attr_t    attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));    
    attr_char_value.p_uuid      = &char_uuid;
    attr_char_value.p_attr_md   = &attr_md;
    
    // Set characteristic length in number of bytes
    attr_char_value.max_len     = 2;
    attr_char_value.init_len    = 2;
    uint8_t value[2]           = {0x00, 0x00};
    attr_char_value.p_value     = value;

    // OUR_JOB: Step 2.E, Add our new characteristic to the service
    err_code = sd_ble_gatts_characteristic_add(p_feedback_service->service_handle,
                                              &char_md,
                                              &attr_char_value,
                                              &p_feedback_service->feedback_char_handles);
    APP_ERROR_CHECK(err_code);

    return NRF_SUCCESS;
}

/**@brief Function for initiating our new service.
 *
 * @param[in]   p_feedback_service        Feedback Service structure.
 *
 */
void feedback_service_init(ble_fbs_t * p_feedback_service)
{
    uint32_t   err_code; // Variable to hold return codes from library and softdevice functions

    // FROM_SERVICE_TUTORIAL: Declare 16-bit service and 128-bit base UUIDs and add them to the BLE stack
    ble_uuid_t        service_uuid;
    ble_uuid128_t     base_uuid = BLE_UUID_FEEDBACK_BASE_UUID;
    service_uuid.uuid = BLE_UUID_FEEDBACK_SERVICE_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
    APP_ERROR_CHECK(err_code);    
    
    // OUR_JOB: Step 3.B, Set our service connection handle to default value. I.e. an invalid handle since we are not yet in a connection.
    p_feedback_service->conn_handle = BLE_CONN_HANDLE_INVALID;

    // FROM_SERVICE_TUTORIAL: Add our service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &service_uuid,
                                        &p_feedback_service->service_handle);
    
    APP_ERROR_CHECK(err_code);
    
    // OUR_JOB: Call the function our_char_add() to add our new characteristic to the service. 
    feedback_char_add(p_feedback_service);
}

// ALREADY_DONE_FOR_YOU: Function to be called when updating characteristic value
void feedback_characteristic_update(ble_fbs_t *p_feedback_service, uint8_t *feedbackValue)
{
    // OUR_JOB: Step 3.E, Update characteristic value
    if (p_feedback_service->conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        uint16_t               len = 2;
        ble_gatts_hvx_params_t hvx_params;
        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_feedback_service->feedback_char_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0;
        hvx_params.p_len  = &len;
        hvx_params.p_data = (uint8_t*)feedbackValue;  

        sd_ble_gatts_hvx(p_feedback_service->conn_handle, &hvx_params);
    }

}


