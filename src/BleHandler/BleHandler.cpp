#include "BleHandler.hpp"
#include <cstring>

static const char* TAG = "BLEManager";

BLEManager* BLEManager::instance = nullptr;

BLEManager::BLEManager(const std::string& deviceName,
                       const std::string& serviceUUID,
                       const std::string& charUUID) :
{
    this->deviceName = deviceName;
    this->serviceUUID = serviceUUID;
    this->charUUID = charUUID;
    this->gatts_if = 0;
    this->conn_id = 0;
    this->char_handle = 0;
    this->deviceConnected = false;
    this->receiveCallback = nullpt;
}

void BLEManager::setReceiveCallback(void (*cb)(const std::string&)) {
    receiveCallback = cb;
}

bool BLEManager::isDeviceConnected() const {
    return deviceConnected;
}

void BLEManager::init() {
    esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);

    esp_bluedroid_init();
    esp_bluedroid_enable();

    esp_ble_gap_register_callback(gapEventHandler);
    esp_ble_gatts_register_callback(gattsEventHandler);
    esp_ble_gatts_app_register(0);

    ESP_LOGI(TAG, "BLE Initialized: %s", deviceName.c_str());
}

void BLEManager::startAdvertising() {
    esp_ble_gap_start_advertising(nullptr);
    ESP_LOGI(TAG, "Advertising started");
}

void BLEManager::stopAdvertising() {
    esp_ble_gap_stop_advertising(nullptr);
    ESP_LOGI(TAG, "Advertising stopped");
}

bool BLEManager::send(const std::string& data) {
    if (!deviceConnected || char_handle == 0) return false;

    esp_ble_gatts_send_indicate(
        gatts_if,
        conn_id,
        char_handle,
        data.length(),
        (uint8_t*)data.c_str(),
        false
    );

    ESP_LOGI(TAG, "Sent data: %s", data.c_str());
    return true;
}

/*==========================================================================
 * GAP CALLBACK
 *==========================================================================*/
void BLEManager::gapEventHandler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch(event) {
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            ESP_LOGI(TAG, "Advertising started");
            break;
        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            ESP_LOGI(TAG, "Advertising stopped");
            break;
        default:
            break;
    }
}

/*==========================================================================
 * GATTS CALLBACK
 *==========================================================================*/
void BLEManager::gattsEventHandler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
    if (!instance) return;

    switch(event) {
        case ESP_GATTS_REG_EVT:
            instance->gatts_if = gatts_if;
            break;

        case ESP_GATTS_CONNECT_EVT:
            instance->conn_id = param->connect.conn_id;
            instance->deviceConnected = true;
            ESP_LOGI(TAG, "Device connected");
            break;

        case ESP_GATTS_DISCONNECT_EVT:
            instance->deviceConnected = false;
            ESP_LOGI(TAG, "Device disconnected");
            instance->startAdvertising();
            break;

        case ESP_GATTS_WRITE_EVT:
            if (param->write.len > 0) {
                std::string data((char*)param->write.value, param->write.len);
                ESP_LOGI(TAG, "Received: %s", data.c_str());

                if (instance->receiveCallback) {
                    instance->receiveCallback(data);
                }
            }
            break;

        default:
            break;
    }
}
