# AetherMod 8.0 — Pass/Age 2 / 7

Pass 2 adds **PHONE LINK**, designed around an iPhone 15 first while retaining an Android-capable protocol surface.

## Phone Link
- Dedicated home application on the 3-page / 24-slot architecture.
- Hotspot state, band/profile state, RSSI, TX/RX counters, ping and packet telemetry.
- iPhone 15 profile is the primary target; Android is retained as a compatible secondary profile.
- Wi-Fi hotspot is the primary transport because the stock DSi radio is 2.4 GHz 802.11b/g/n-class and cannot directly become a 5G modem.
- The phone supplies the cellular backhaul; AetherMod treats the phone as the network gateway rather than pretending the DSi itself has 5G.
- Future passes can add a companion web endpoint for commands, file transfer, telemetry streaming and Aether Bot interaction.

Apple documents that iPhone Personal Hotspot can share cellular connectivity over Wi-Fi, Bluetooth or USB. Nintendo documents that DSi wireless operation is 2.4 GHz and supports WPA/WPA2 modes through the DSi's advanced connection settings. Therefore this build deliberately uses the compatible 2.4 GHz hotspot path rather than claiming unsupported 5 GHz/5G radio capability.

## 8.0 expansion direction
Phone Link becomes the bridge for later live TinySA telemetry, Codex transfer, Animal AI input/output, Aether Bot control, Network Gateway services and SD file synchronization. No cellular modem is fabricated in software.
