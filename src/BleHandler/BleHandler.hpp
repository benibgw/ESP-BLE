#include <string>
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_log.h"

class BLEManager {
public:
    BLEManager(const std::string& deviceName,
               const std::string& serviceUUID,
               const std::string& charUUID);

    void init();
    void startAdvertising();
    void stopAdvertising();
    bool isDeviceConnected() const;

    void setReceiveCallback(void (*cb)(const std::string&));
    bool send(const std::string& data);

private:
    std::string deviceName;
    std::string serviceUUID;
    std::string charUUID;

    uint16_t gatts_if;
    uint16_t conn_id;
    uint16_t char_handle;
    bool deviceConnected;

    void (*receiveCallback)(const std::string&);

    // Callbacks internos
    static void gattsEventHandler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
    static void gapEventHandler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
};
