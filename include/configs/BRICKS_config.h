#pragma once
#define BRICKS_config_h

// -- EXAMPLE BRICKS LOGGER HANDLER
const String NAME_ACCEL = "ACCEL";           // Log something to do with the accelerometer
const String NAME_SD = "SD";                 // '' SD
const String NAME_CACHER = "CACHER";         // '' File upload cacher
const String NAME_RTC = "RTC";               // '' RTC
const String NAME_UPLOAD = "UPLOAD";         // '' Uploading (inc. InfluxDB && rest POST/GETs)
const String NAME_BUFFER = "BUFFER";         // '' the Upload Buffer
const String NAME_ESP32 = "ESP32";           // '' specifically with the ESP32
const String NAME_SUBSAMPLER = "SUBSAMPLER"; // '' the subsampling system
