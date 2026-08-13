"""Pre-build script: create dummy certificate .S files needed by
ESP Insights/RainMaker managed components. These components get
pulled in by Arduino-ESP32 even when CONFIG_ESP_INSIGHTS_ENABLED=n."""
Import("env")

import os

build_dir = env.subst("$BUILD_DIR")
os.makedirs(build_dir, exist_ok=True)

for name in ["https_server.crt", "rmaker_mqtt_server.crt",
             "rmaker_claim_service_server.crt", "rmaker_ota_server.crt"]:
    path = os.path.join(build_dir, name + ".S")
    if not os.path.exists(path):
        sym = name.replace(".", "_")
        with open(path, "w") as f:
            f.write(f".section .rodata.embedded\n")
            f.write(f".global {sym}_start\n")
            f.write(f".global {sym}_end\n")
            f.write(f".align 4\n")
            f.write(f"{sym}_start:\n")
            f.write(f".byte 0\n")
            f.write(f"{sym}_end:\n")
