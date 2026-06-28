# Leslie MIDI Speed Controller
True MIDI speed control for your Leslie.

Use in conjunction with Leslie Foootswitch controller https://github.com/jazzmonger/Leslie-Dual-Motor-Footswitch-Controller

MIDI interpreter takes output from the Nord (any MIDI KB really) and watches for CC108=45(slow) and CC1008=137 (fast) then pulses a relay to control the speed.  See my other repository for the footswitch speed controller.  It could just as easily toggle on/off instead of pulse to directly control a 147.

Basic Operation
https://www.facebook.com/61550956205984/videos/1311657740688039/?idorvanity=467940163416921

Recording into Logic Pro and then playing back using MIDI to control the speed
https://www.facebook.com/61550956205984/videos/pcb.3182860038591573/1530799352055109

I normally use ESPHome to create these types of controllers, but the MIDI Arduino libraries are redily available so thats what I used this time. And I just told Claude to build it and debug it.  For this I used an ESP8266 D1 Mini.

Basic operation:
NS4-->MIDI Interpreter--> 1/4" jack that triggers the Leslie speed.

To record and playback the MIDI signals I use a CME Pro U6 MIDI router:
<img width="1299" height="659" alt="image" src="https://github.com/user-attachments/assets/ff54b3e4-25d6-4e1e-9c6d-389647b17cd4" />


The controller inside the Leslie sends a signal to an external LED when in fast mode.  I use this signal to synchronize the slow/fast operation when using MIDI control (it goes to D5 on the ESP). You have to know what your state is before changing state!

I used this cool MIDI breakout board - cheap too!
<img width="968" height="640" alt="image" src="https://github.com/user-attachments/assets/c93ddbc0-d140-4e63-809b-54d59ff2c9c3" />
D0 on that board (MIDI RXD) goes to D4 on the ESP - it could not be more simple.

