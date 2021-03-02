from PyArduAPI import Arduino
from PyArduAPI import m328p as uK
import time

Nano = Arduino()

Nano.pinMode(13, 'OUTPUT')
i=0
while i<10:
    Nano.digitalWrite(13, 'HIGH')
    time.sleep(1)
    Nano.digitalWrite(13, 'LOW')
    time.sleep(1)
    i += 1