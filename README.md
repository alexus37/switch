# Switch

This project controls the switch of the attic.
The first chip is an esp32 firebettle https://arduino-esp8266.readthedocs.io/en/latest/installing.html
and the sound chip is a DY-SV5W

## Flashing

Use the ESP32 firebettle board to flash the chip and use windows if possible 

## Importanten notes

1. Check if the IP of the lught computer is correct (on light computer use the command ipconfig window+r=> CMD)
    - If not change in code and flash again

2. ensure that both (chip and light computer are the in same wifi (fischnetz))
    - If not change in code and flash again
2. On light computer ensure the following settings are made in magicQ.
    - Go to network > set your ip address
    - Go to setup > dmx/IO enable Universe 7 (Artnet out 6)
    - Go tot the marco window > automation > insert a new automation
3. Make the following automtion settings
    - Input: DMX_input
    - set channel and universe: 7-4
    - mode: toggle




