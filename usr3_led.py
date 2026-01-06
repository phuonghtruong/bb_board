# ################
# Author: BrianT
# Date:
# ################

import Adafruit_BBIO.GPIO as BBIO
import time

LED = "USR3"

BBIO.setup(LED, BBIO.OUT)

try:
    print("USR3 LED blinking...")
    while True:
        BBIO.output(LED, BBIO.HIGH)
        time.sleep(1)
        BBIO.output(LED, BBIO.LOW)
        time.sleep(1)

finally:
    # --- Clean up Phase (Runs when the program exits) ---
    print("\ncleaning up pins...")
    BBIO.cleanup()
    print("Done.")
