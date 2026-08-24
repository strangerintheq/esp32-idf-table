
# ESP32 polar sand table firmware


- install vscode extension "ESP-IDF"

- run idf.py menuconfig

 - configures LittelFS in menuconfig:

   - Serial flasher config ---> Flash size ---> 4 MB
   - Partition Table ---> Partition Table Selector ---> Custom partition table CSV.Partition Table ---> Custom partition table CSV file ---> partitions.csv

 - configure webserver in menuconfig:
   - HTTP Server ---> WebSocket server support
   - HTTP Server ---> WebSocket post handshake callback support

 - press S to save and Q to exit.
