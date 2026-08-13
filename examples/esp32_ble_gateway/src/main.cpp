/**
 * @file main.cpp
 * @brief SensESP BLE gateway on a standard ESP32-WROOM-32 dev board.
 *
 * Uses NativeBLE (Bluedroid) since the classic ESP32 has its own
 * onboard BT/BLE controller — no esp_hosted companion chip, no
 * NimBLE fallback needed.
 */

#include "sensesp_ble_gateway/ble_signalk_gateway.h"
#include "sensesp_ble_gateway/native_bluedroid_ble.h"
#include "sensesp_app_builder.h"

using namespace sensesp;

static std::shared_ptr<NativeBLE> g_ble;
static std::shared_ptr<BLESignalKGateway> g_gateway;

void setup() {
  SetupLogging(ESP_LOG_INFO);

  SensESPAppBuilder builder;
  auto app = builder.set_hostname(GATEWAY_HOSTNAME)
                 ->set_wifi_client("YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD")
                 ->enable_ota("esp32-ble-gw-ota")
                 ->get_app();

  g_ble = std::make_shared<NativeBLE>();

  g_gateway =
      std::make_shared<BLESignalKGateway>(g_ble, app->get_ws_client());
  g_gateway->start();

  event_loop()->onRepeat(5000, []() {
    ESP_LOGI(
        "GW",
        "alive — uptime=%lus heap=%u ble_hits=%u ble_scan=%d gw_rx=%u "
        "gw_posted=%u gw_dropped=%u post_ok=%u post_fail=%u ws_up=%d",
        (unsigned long)(millis() / 1000), (unsigned)ESP.getFreeHeap(),
        (unsigned)(g_ble ? g_ble->scan_hit_count() : 0),
        (int)(g_ble ? g_ble->is_scanning() : false),
        (unsigned)(g_gateway ? g_gateway->advertisements_received() : 0),
        (unsigned)(g_gateway ? g_gateway->advertisements_posted() : 0),
        (unsigned)(g_gateway ? g_gateway->advertisements_dropped() : 0),
        (unsigned)(g_gateway ? g_gateway->http_post_success() : 0),
        (unsigned)(g_gateway ? g_gateway->http_post_fail() : 0),
        (int)(g_gateway ? g_gateway->control_ws_connected() : false));
  });
}

void loop() { event_loop()->tick(); }
