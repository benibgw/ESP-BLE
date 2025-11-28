#include "BleHandler/BleHandler.hpp"
#include <stdio.h>

extern "C" void app_main() {
    // Cria o servidor BLE com nome e UUIDs
    BLEManager ble("ESP32-Servidor",
                   "12a4523c-ba3c-4b36-9817-5716b5c031d3",
                   "a28a3026-6b22-4822-a9c3-100222167e42");

    // Inicializa o BLE
    ble.init();

    // Inicia publicidade para que o celular encontre o ESP32
    ble.startAdvertising();

    printf("Servidor BLE pronto. Aguardando conexão do celular...\n");

    // Loop principal vazio, apenas mantém o ESP32 ligado
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
