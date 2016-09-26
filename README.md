# Energy Monitor ATSAMD21

Arduino sketch for use with [Energy Monitor FeatherWing PCB for Adafruit Feather M0](https://github.com/ndoo/Energy-Monitor-FeatherWing)

## Features

* Power calculation for 3 current+voltage transformer pairs
* Publishes readings to configurable MQTT feeds
* Supports CT-only operation - configure voltage in `sEmon sensors[SENSORS]` to a non-zero value

## Prerequisites/Caveats

* Uses a [lightly modified EmonLib](https://github.com/ndoo/ArduinoDue_3phase) to support CT-only operation
* EmonLib tends to output NaN floats - [modify Adafruit_MQTT to not publish these](https://github.com/ndoo/Adafruit_MQTT_Library/commit/56f60f5ce34b8fd3c2e178a7f93b78ca994c454b)

## Setup

1. Configure `WLAN_SSID` and `WLAN_PASS`
2. Configure `AIO_USERNAME` and `AIO_KEY` to match to your [Adafruit IO](https://io.adafruit.com) account
3. Configure the 3 members of `sEmon sensors[SENSORS]` to match your current/voltage transformer components
4. Configure the various MQTT feeds in `sEmon sensors[SENSORS]` (leave blank if not publishing)
