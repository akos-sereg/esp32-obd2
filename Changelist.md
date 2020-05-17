# Changelog

Below versions are tags in the repo.

## v1.1 - Next Release
- Fuel Pressure - in kPa - (3rd page on LCD) instead of Engine oil temperature
- Led strip to display Engine Load instead of RPM
- Toggle LCD backlight by long pressing navigation button
- Store and restore backlight state upon launch
- OBD2 protocol initialization (eg. ask device to not send unnecessary load)
- Auto connect to known devices based on MAC address (to avoid configuration overhead)
- Avoid flickering on LCD update when updating with the same text

- Bugs: Fuel Pressure is not displayed
- To be tested: Engine Oil temperature data, Fuel Pressure data from OBD2

## v1.0 - First working prototype
 - Led strip displays current RPM real-time
 - LCD displaying: 
   - Distance to empty (km)
   - Coolant temp (C)
   - Engine oil temp: this feature is broken, because my OBD2 device (or the car) is not sending oil temp data
   - Battery Voltage (V)