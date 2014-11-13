#ifndef _RBC_MESH_H__
#define _RBC_MESH_H__
#include <stdint.h>
#include <stdbool.h>
#include "nrf51.h"
#include "ble.h"

#define RBC_MESH_ACCESS_ADDRESS_BLE_ADV  (0x8E89BED6)
#define RBC_MESH_ADV_INT_MIN             (5)
#define RBC_MESH_ADV_INT_MAX             (60000)
/** 
* @brief Rebroadcast value handle type 
*
* @detailed Handle type used to identify a value in the mesh, is consistent
*   throughout the network
*/
typedef uint16_t rbc_mesh_value_handle_t;
    
/** @brief Event type enum. Identifies framework generated events */
typedef enum
{
    RBC_MESH_EVENT_TYPE_UPDATE_VAL,      /** Another node has updated the value */
    RBC_MESH_EVENT_TYPE_CONFLICTING_VAL, /** Another node has a conflicting version of the value */
    RBC_MESH_EVENT_TYPE_NEW_VAL,         /** A previously unallocated value has been received and allocated */
} rbc_mesh_event_type_t;

/** 
* @brief Rebroadcast framework generated event. Carries information about a
*   change to one database value.
*/
typedef struct
{
    rbc_mesh_event_type_t event_type;        /** See @ref rbc_mesh_event_type_t */
    rbc_mesh_value_handle_t value_handle;    /** Handle of the value the event is generated for */
    uint8_t* data;                      /** Current data array contained at the event handle location */
    uint8_t data_len;                   /** Length of data array */
    ble_gap_addr_t originator_address; /** GAP address of node where this version of the message appeared */
} rbc_mesh_event_t;


/*****************************************************************************
     Interface Functions 
*****************************************************************************/

/**
* @brief Initialize Rebroadcast module, must be called before any other 
*   rebroadcast function. 
*
* @note The nRF51 Softdevice must be initialized by the application before
*    the mesh framework intialization is called, otherwise, the function will
*    return NRF_ERROR_SOFTDEVICE_NOT_ENABLED.
* 
* @param[in] access_addr The access address the mesh will work on. This must be the 
*    same for all nodes in the mesh. RBC_MESH_ACCESS_ADDRESS_BLE_ADV gives the mesh
*    the same access address as regular BLE advertisements, which makes the
*    traffic visible to external BLE devices (Note that other access addresses 
*    does not provide any data security, the traffic is merely ignored by 
*    regular BLE radios). Multiple meshes may in theory work concurrently in 
*    the same area with different access addresses, but will be prone to 
*    on-air collisions, and it is recommended to use separate channels for this
* @param[in] channel The BLE channel the mesh works on. It is strongly recommended 
*    to use one of the three adv channels 37, 38 or 39, as others may be prone
*    to on-air collisions with WiFi channels. Separate meshes may work 
*    concurrently without packet collision if they are assigned to different 
*    channels. Must be between 1 and 39.
* @param[in] handle_count The maximum number of handle-value pairs available to the
*    application. May not be higher than 155 due to BLE namespace requirements
* @param[in] adv_int_ms The minimum adv_interval for nodes in the network in 
*    millis. Must be between 5 and 60000.
* 
* @return NRF_SUCCESS the initialization is successful 
* @return NRF_ERROR_INVALID_PARAM a parameter does not meet its requiremented range.
* @return NRF_ERROR_INVALID_STATE the framework has already been initialized.
* @return NRF_ERROR_SOFTDEVICE_NOT_ENABLED the Softdevice has not been enabled.
*/
uint32_t rbc_mesh_init(
    uint32_t access_addr, 
    uint8_t channel, 
    uint8_t handle_count, 
    uint8_t adv_int_ms);

/**
* @brief Broadcast a request for an update on the value connected to the 
*   handle indicated.
*
* @note The value request is sent asynchronously to the function call, and 
*   a response is likely to be received after several milliseconds (depending
*   on the adv_int_ms value set in @ref rbc_mesh_init
*
* @param[in] handle Handle to request a value for 
*
* @return NRF_SUCCESS A request was successfully scheduled for broadcast
* @return NRF_ERROR_INVALID_ADDR the handle is outside the range provided 
*   in @ref rbc_mesh_init.
* @return NRF_ERROR_INVALID_STATE The framework has not been initiated
*/
uint32_t rbc_mesh_value_req(uint8_t handle);

/**
* @brief Set the contents of the data array pointed to by the provided handle
*
* @param[in] handle The handle of the value we want to update. Is mesh-global.
* @param[in] data Databuffer to be copied into the value slot
* @param[in] len Length of the provided data. Must not exceed RBC_VALUE_MAX_LEN.
* 
* @return NRF_SUCCESS if the value has been successfully updated.
* @return NRF_ERROR_INVALID_STATE if the framework has not been initialized.
* @return NRF_ERROR_INVALID_ADDR if the handle is outside the range provided
*    in @ref rbc_mesh_init.
* @return NRF_ERROR_INVALID_LENGTH if len exceeds RBC_VALUE_MAX_LEN.
*/
uint32_t rbc_mesh_value_set(uint8_t handle, uint8_t* data, uint16_t len);

/**
 * @brief Get the contents of the data array pointed to by the provided handle
*
* @param[in] handle The handle of the value we want to update. Is mesh-global.
* @param[out] data Databuffer to be copied into the value slot. Must be at least
*    RBC_VALUE_MAX_LEN long
* @param[out] len Length of the copied data. Will not exceed RBC_VALUE_MAX_LEN.
* 
* @return NRF_SUCCESS the value has been successfully fetched.
* @return NRF_ERROR_INVALID_STATE the framework has not been initialized.
* @return NRF_ERROR_INVALID_ADDR the handle is outside the range provided
*    in @ref rbc_mesh_init.
*/
uint32_t rbc_mesh_value_get(uint8_t handle, uint8_t* data, uint16_t* len);

/**
* @brief Get current mesh access address
* 
* @param[out] access_addr Pointer location to put access address in
* 
* @return NRF_SUCCESS the value was fetched successfully
* @return NRF_ERROR_INVALID_STATE the framework has not been initialized 
*/
uint32_t rbc_mesh_access_address_get(uint32_t* access_address);

/**
* @brief Get current mesh channel
* 
* @param[out] ch Pointer location to put mesh channel in 
*
* @return NRF_SUCCESS the value was fetched successfully
* @return NRF_ERROR_INVALID_STATE the framework has not been initialized 
*/
uint32_t rbc_mesh_channel_get(uint8_t* ch);

/**
* @brief Get the amount of allocated handle-value pairs 
* 
* @param[out] handle_count Pointer location to put handle count in 
*
* @return NRF_SUCCESS the value was fetched successfully
* @return NRF_ERROR_INVALID_STATE the framework has not been initialized 
*/
uint32_t rbc_mesh_handle_count_get(uint8_t* handle_count);

/**
* @brief Get the mesh minimum advertise interval in ms
*
* @param[out] adv_int_ms Pointer location to put adv int in
*
* @return NRF_SUCCESS the value was fetched successfully
* @return NRF_ERROR_INVALID_STATE the framework has not been initialized
*/
uint32_t rbc_mesh_adv_int_get(uint32_t* adv_int_ms);

/**
* @brief Event handler to be called upon external BLE event arrival.
*   Only handles GATTS write events, all other types are ignored.
*   Has a similar effect as @ref rbc_mesh_value_set, by refreshing version
*   numbers and timing parameters related to the indicated characteristic.
*
* @note This event may be called regardless of if the indicated characteristic
*   belongs to the mesh or not, the framework will filter out uninteresting 
*   events and return NRF_SUCCESS. However, if the incoming event points at 
*   the mesh service, but the characteristic handle is out of range, the 
*   function returns NRF_ERROR_INVALID_ADDR. 
*
* @note This function will also trigger any update/new events in the application
*   space 
*   
* @param[in] evt BLE event received from softdevice.
*
* @return NRF_SUCCESS Event successfully handled.
* @return NRF_ERROR_INVALID_ADDR Handle is part of service, but does not belong
*   any valid characteristics.
* @return NRF_ERROR_INVALID_STATE the framework has not been initialized.
*/
uint32_t rbc_mesh_ble_evt_handler(ble_evt_t* evt);

/**
* @brief Softdevice interrupt handler, checking if there are any 
*   incomming events related to the framework. 
*
* @note Should be called from the SD_IRQHandler function. Will poll the 
*   softdevice for new sd_evt.
*/
void rbc_mesh_sd_irq_handler(void);

/**
 * @brief Application space event handler. TO BE IMPLEMENTED IN APPLICATION 
*   SPACE.
*
* @note Does not have an implementation within the framework, but acts as a 
*   feedback channel for the framework to notify the application of any 
*   changes in values.
*
* @param evt Framework generated event presented to the application. 
*/
void rbc_mesh_event_handler(rbc_mesh_event_t* evt);

#endif /* _RBC_MESH_H__ */
