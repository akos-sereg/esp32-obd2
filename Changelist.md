# Changelog

Below versions are tags in the repo. All versions are representing a state which can be deployed to esp32 device 
without any extra configuration.

- Improvement: toggle lcd backlight after 1200 ms instead of falling edge

## v1.1.1 - Next
- LCD backlight UX fix: toggle backlight right after 1200 ms (eg. dont wait until falling edge)
- "Dist. to empty" was changing in every 13 kms only (eg. one liter). Fixing to display in 1km resolution

## v1.1.0
- Fuel Pressure - in kPa - (3rd page on LCD) instead of Engine oil temperature
- Led strip to display Engine Load instead of RPM
- Toggle LCD backlight by long pressing navigation button
- Store and restore backlight state upon launch
- OBD2 protocol initialization (eg. ask device to not send unnecessary load)
- Auto connect to known devices based on MAC address (to avoid configuration overhead)
- Avoid flickering on LCD update when updating with the same text

## v1.0 - First working prototype
 - Led strip displays current RPM real-time
 - LCD displaying: 
   - Distance to empty (km)
   - Coolant temp (C)
   - Engine oil temp: this feature is broken, because my OBD2 device (or the car) is not sending oil temp data
   - Battery Voltage (V)