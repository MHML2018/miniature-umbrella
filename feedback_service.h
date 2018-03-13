
#ifndef FEEDBACK_SERVICE_H__
#define FEEDBACK_SERVICE_H__

#include <stdint.h>
#include "ble.h"
#include "ble_srv_common.h"

// Defining 16-bit service and 128-bit base UUIDs
// BASE UUID = c76279e0-8599-40f7-b6c6-f2336b53a164
#define BLE_UUID_FEEDBACK_BASE_UUID              {{0xC7, 0x62, 0x79, 0xE0, 0x85, 0x99, 0x40, 0xF7, 0xB6, 0xC6, 0xF2, 0x33, 0x6B, 0x53, 0xA1, 0x64}} // 128-bit base UUID
#define BLE_UUID_FEEDBACK_SERVICE_UUID                0x5175 // Just a random, but recognizable value

// ALREADY_DONE_FOR_YOU: Defining 16-bit characteristic UUID
#define BLE_UUID_FEEDBACK_CHARACTERISTC_UUID          0x5176 // Just a random, but recognizable value

// This structure contains various status information for our service. 
// The name is based on the naming convention used in Nordics SDKs. 
// 'ble’ indicates that it is a Bluetooth Low Energy relevant structure and 
// ‘os’ is short for Our Service). 
typedef struct
{
    uint16_t                    conn_handle;    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection).*/
    uint16_t                    service_handle; /**< Handle of Our Service (as provided by the BLE stack). */
    // Add handles for the characteristic attributes to our struct
    ble_gatts_char_handles_t    feedback_char_handles; 
}ble_fbs_t;

/**@brief Function for handling BLE Stack events related to feedback service and characteristic.
 *
 * @details Handles all events from the BLE stack of interest to FeedBack Service.
 *
 * @param[in]   p_feedback_service       Feedback Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_feedback_service_on_ble_evt(ble_fbs_t * p_feedback_service, ble_evt_t * p_ble_evt);

/**@brief Function for initializing our new service.
 *
 * @param[in]   p_feedback_service       Pointer to Feedback Service structure.
 */
void feedback_service_init(ble_fbs_t * p_feedback_service);

/**@brief Function for updating and sending new characteristic values
 *
 * @details The application calls this function whenever our timer_timeout_handler triggers
 *
 * @param[in]   p_feedback_service        Feedback Service structure.
 * @param[in]   characteristic_value     New characteristic value.
 */
void feedback_characteristic_update(ble_fbs_t *p_feedback_service, uint8_t *feedbackValue);

uint32_t feedback_char_handle_write(ble_fbs_t *p_feedback_service);

#endif  /* _ POSTURE_SERVICE_H__ */
