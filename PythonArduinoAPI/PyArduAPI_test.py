from PyArduAPI import Arduino
from PyArduAPI import m328p as uK
import time

Nano = Arduino()
print("TCCR0A = " + str(Nano.readRegister(uK.TCCR0A))) # is 3 -> FastPWM
print("TCCR0B = " + str(Nano.readRegister(uK.TCCR0B))) # is 3 -> clk/64 = 250 kHz
print("PINB = " + str(Nano.readRegisterBit(uK.PINB,4)))     # 24 = b0001_0000

print("TIMSK0 = " + str(Nano.readRegisterBit(uK.TIMSK0,0)))
print("TOVF0 = " + str(Nano.readRegisterBit(uK.TIFR0,0)))

Nano.writeRegisterBit(uK.TIMSK0,0,0)
print("TIMSK0 = " + str(Nano.readRegisterBit(uK.TIMSK0,0)))
for i in range(10):
    print("TOVF0 = " + str(Nano.readRegisterBit(uK.TIFR0,0)))
time.sleep(5)