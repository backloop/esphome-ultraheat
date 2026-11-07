# esphome-ultraheat
Communicate with LandisGyr UltraHeat devices over the bidirectional IR interface

Works in progress...

## Building

### Requirements
- `make`
- `docker`. By default esphome 2022.12.8 runs from the `esphome/esphome`
  image, so no local esphome install is needed.
- `g++`, for the host-side check program

### Secrets
Create `secrets.yaml` (git ignores it) with these keys:

```yaml
wifi_ssid: "..."
wifi_password: "..."
wifi_ap_ssid: "..."        # fallback hotspot
wifi_ap_password: "..."
api_encryption: "..."      # base64 key for the Home Assistant API
ota_password: "..."
```

### Targets

| Command              | Description |
|----------------------|-------------|
| `make ultraheat`     | Build the firmware from `ultraheat.yaml`. Also writes a disassembly (`build/firmware.list`) and a symbol size list (`build/firmware.sizes`). |
| `make install`       | Build and upload the firmware |
| `make ultraheat_ota` | Build the minimal firmware from `ultraheat_ota.yaml` (WiFi, API and OTA only) |
| `make ota`           | Build and upload the minimal firmware |
| `make check`         | Build and run `ultraheat_check`, which runs the state machine and parser on the host, feeding them a recorded meter response in random-sized chunks |
| `make clean`         | Remove `build/` |

To use a locally installed `esphome` instead of docker, add `USE_DOCKER=0`, e.g.
`make USE_DOCKER=0 ultraheat`.

The first docker build downloads the PlatformIO toolchain into
`.esphome/platformio/`. `make clean` keeps it, so later builds don't download
it again.

### Layout
- `src/`: source code (headers included by the esphome configs, plus
  `ultraheat_check.cpp`)
- `ultraheat.yaml`, `ultraheat_ota.yaml`: esphome device configs
- `build/`: all build output. The esphome/PlatformIO projects are in
  `build/esphome/<config>/`, and the firmware image is
  `build/esphome/ultraheat/.pioenvs/ultraheat/firmware.bin`.
