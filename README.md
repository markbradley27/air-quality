> Note: Further development is happening over in [github.com/markbradley27/henrietta](https://github.com/markbradley27/henrietta/tree/main/src/boards/enviro_micky).

# air-quality

### pin layout

| ESP | PM2.5 | OLED | DHT22 | Buttons |
| --- | ----- | ---- | ----- | ------- |
| Vin | VCC   |      |       |         |
| 3V3 |       | VCC  | VCC   |         |
| D1  |       | SCL  |       |         |
| D2  |       | SDA  |       |         |
| D3  |       |      | DATA  |         |
| D4  | TXD   |      |       |         |
| D5  |       |      |       | right   |
| D6  |       |      |       | middle  |
| D7  |       |      |       | left    |
