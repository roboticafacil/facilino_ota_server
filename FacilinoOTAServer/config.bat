@echo on
set arduino-cli-cmd=C:\FacilinoOTAServer\arduino-cli\arduino-cli.exe
cmd /C %arduino-cli-cmd% config init --overwrite
cmd /C %arduino-cli-cmd% config set directories.data C:\FacilinoOTAServer\arduino-cli\Arduino15
cmd /C %arduino-cli-cmd% config set directories.downloads C:\FacilinoOTAServer\arduino-cli\Arduino15\staging
cmd /C %arduino-cli-cmd% config set library.enable_unsafe_install true
;cmd /C %arduino-cli-cmd% core update-index --additional-urls http://arduino.esp8266.com/stable/package_esp8266com_index.json --additional-urls https://dl.espressif.com/dl/package_esp32_index.json
;cmd /C %arduino-cli-cmd% core update-index
;cmd /C %arduino-cli-cmd% core install arduino:avr
;cmd /C %arduino-cli-cmd% core install arduino:megaavr
;cmd /C %arduino-cli-cmd% core install esp8266:esp8266
;cmd /C %arduino-cli-cmd% core install esp32:esp32
;cmd /C %arduino-cli-cmd% lib update-index
;cmd /C %arduino-cli-cmd% lib install Servo
;cmd /C %arduino-cli-cmd% lib install LiquidCrystal
;cmd /C %arduino-cli-cmd% lib install "LiquidCrystal I2C"
;cmd /C %arduino-cli-cmd% lib install "Adafruit NeoPixel"
;cmd /C %arduino-cli-cmd% lib install "Adafruit SSD1306"
;cmd /C %arduino-cli-cmd% lib install "Adafruit GFX Library"
;cmd /C %arduino-cli-cmd% lib install --zip-path C:\FacilinoOTAServer\arduino-cli\ESP8266.zip"
;cmd /C %arduino-cli-cmd% lib install IRremote
;cmd /C %arduino-cli-cmd% lib install "Dimmable Light for Arduino"
;cmd /C %arduino-cli-cmd% lib install DFRobotDFPlayerMini
;cmd /C %arduino-cli-cmd% lib install "DHT sensor library"
;cmd /C %arduino-cli-cmd% lib install "DHT sensor library for ESPx"
;cmd /C %arduino-cli-cmd% lib install DallasTemperature
;cmd /C %arduino-cli-cmd% lib install "Adafruit BMP085 Library"
;cmd /C %arduino-cli-cmd% lib install --zip-path C:\FacilinoOTAServer\arduino-cli\ESP32_Servo.zip
;cmd /C %arduino-cli-cmd% lib install --zip-path C:\FacilinoOTAServer\arduino-cli\AsyncTCP.zip"
;cmd /C %arduino-cli-cmd% lib install --zip-path C:\FacilinoOTAServer\arduino-cli\ESPAsyncTCP.zip"
;cmd /C %arduino-cli-cmd% lib install --zip-path C:\FacilinoOTAServer\arduino-cli\ESPAsyncWebServer.zip"
;cmd /C %arduino-cli-cmd% lib install ThingsBoard
;cmd /C %arduino-cli-cmd% lib install PubSubClient
;cmd /C %arduino-cli-cmd% lib install ArduinoJson
;cmd /C %arduino-cli-cmd% lib install FauxmoESP
;cmd /C %arduino-cli-cmd% lib install ESPUI
;cmd /C %arduino-cli-cmd% lib install LittleFS_esp32