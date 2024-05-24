## SFMU Reverse Vending Machine

This Arduino project rewards users for recycling plastic and metal bottles with QR codes for redemption. Users capture a picture of the QR code on their phone for later redemption. The machine features a 1.8-inch TFT display, three buttons for user interaction, a servo motor for opening the appropriate recycling hole, and a bottle counting mechanism.

**Hardware:**

* Arduino Uno R4 (or compatible)
* Servo motor (for opening recycling holes)
* Bottle detection sensors (two sensors for plastic and metal detection)
* 1.8-inch TFT display module (SPI interface)
* Three buttons (green, red, blue)
* (optional: LEDs)

**Software:**

* Arduino IDE ([https://www.arduino.cc/](https://www.arduino.cc/))
* TFT library (likely included in Arduino IDE)

**Setup:**

1. Install Arduino IDE.
2. Clone/download this repo.
3. Open `SFMU_Reverse_Vending_Machine.ino` in Arduino IDE.
4. Connect your Arduino and upload the code.

**Usage:**

1. Power on the machine.
2. Choose the appropriate button:
    * Green: For plastic bottle recycling
    * Red: For metal bottle recycling
3. The servo motor will open the corresponding recycling hole based on your button selection.
4. Deposit as many bottles as you want in the opened hole.
    * The bottle detection sensors will count each deposited bottle.
    * The current bottle count will be displayed on the TFT screen.
5. **To complete the recycling process:**
    * Press the blue button.
6. **Upon pressing the blue button:**
    * The total number of deposited bottles will be displayed on the TFT screen.
    * The reward dispensing mechanism will be activated (depending on code).
    * A QR code for your reward will be displayed on the TFT screen.
7. Use your smartphone camera to capture a picture of the QR code for later redemption.

**Customization:**

Modify the code for different bottle types & rewards. Utilize the TFT display for:

* Instructions
* Animations
* Bottle type recognition (connected to button selection)
* Reward information/system
* Bottle count display

(Optional: Add sound effects and LEDs for feedback)

**License:** MIT ([https://choosealicense.com/](https://choosealicense.com/))

**Contact:** [Faysal Al Mahmud/ faysalalmahmud78@gmail.com] (or create issues on this repo)


**Additional Notes:**

* You might need to install a specific TFT library depending on the display model. 
* Refer to the library documentation for proper initialization and usage with the Arduino.
