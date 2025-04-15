| Key Features

-Node Registration & Heartbeat: The coordinator keeps track of active nodes and their heartbeat signals. When a node doesn't send a heartbeat, it is marked as offline.
-Interactive OLED Display: The OLED screen displays the list of nodes and their statuses, with the ability to scroll through and select nodes.
-Command Sending: The coordinator can send custom commands to individual nodes. For example, a "Command to Node" message can be sent to trigger specific actions on the node.
-Rotary Encoder Navigation: The rotary encoder is used for scrolling through the list of nodes and selecting the desired node for interaction.
-Push Button Action: Pressing the button sends the selected command to the node.

| Pinout
- oled sda gpio 21
- oled scl gpio 22
- rotary clk gpio 32
- rotary dt gpio 33
- push button 34
- led gpio 25
- buzzer/alarm gpio 26

| Components

-ESP32: Acts as the coordinator, managing the nodes and handling communication via ESP-NOW.
-OLED Display (SSD1306): Displays the list of nodes and their statuses.
-Rotary Encoder: Used to scroll through the nodes displayed on the OLED.
-Push Button: Used to select a node and send a command.
-ESP-NOW: A protocol used for communication between nodes and the coordinator.
