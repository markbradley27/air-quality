# air-quality

### pin layout

| ESP | PM2.5 | OLED | DHT22 | Buttons |
| --- | ----- | ---- | ----- | ------- |
| Vin | VCC   |      |       |         |
| G   | G     |      |       |         |
| G   |       | G    |       |         |
| 3V3 |       | VCC  |       |         |
| D1  |       | SCL  |       |         |
| D2  |       | SDA  |       |         |
| D3  |       |      | DATA  |         |
| D4  | TXD   |      |       |         |
| D5  |       |      |       | right   |
| D6  |       |      |       | middle  |
| D7  |       |      |       | left    |
