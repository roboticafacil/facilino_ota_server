curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
mkdir -p arduino-cli
mv bin/arduino-cli arduino-cli/
rmdir bin

cd arduino-cli
arduinocli=./arduino-cli
$arduinocli config init --overwrite
$arduinocli config set directories.data $PWD/Arduino15
$arduinocli config set directories.downloads $PWD/Arduino15/staging
$arduinocli config set library.enable_unsafe_install true
$arduinocli core update-index --additional-urls http://arduino.esp8266.com/stable/package_esp8266com_index.json --additional-urls https://dl.espressif.com/dl/package_esp32_index.json
$arduinocli core update-index
$arduinocli core install arduino:avr
$arduinocli core install arduino:megaavr
$arduinocli core install esp8266:esp8266
$arduinocli core install esp32:esp32
$arduinocli lib update-index
$arduinocli lib install Servo
$arduinocli lib install LiquidCrystal
$arduinocli lib install 'LiquidCrystal I2C'
$arduinocli lib install 'Adafruit NeoPixel'
$arduinocli lib install 'Adafruit SSD1306'
$arduinocli lib install 'Adafruit GFX Library'
$arduinocli lib install --zip-path $PWD/ESP8266.zip
$arduinocli lib install IRremote
$arduinocli lib install 'Dimmable Light for Arduino'
$arduinocli lib install DFRobotDFPlayerMini
$arduinocli lib install 'DHT sensor library'
$arduinocli lib install 'DHT sensor library for ESPx'
$arduinocli lib install DallasTemperature
$arduinocli lib install 'Adafruit BMP085 Library'
$arduinocli lib install --zip-path $PWD/ESP32_Servo.zip
$arduinocli lib install --zip-path $PWD/AsyncTCP.zip
$arduinocli lib install --zip-path $PWD/ESPAsyncTCP.zip
$arduinocli lib install --zip-path $PWD/ESPAsyncWebServer.zip
$arduinocli lib install ThingsBoard
$arduinocli lib install PubSubClient
$arduinocli lib install ArduinoJson
$arduinocli lib install FauxmoESP
$arduinocli lib install ESPUI
$arduinocli lib install LittleFS_esp32
cd ..
