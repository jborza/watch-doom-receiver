# test data sender
#from serial import Serial
import serial
import random
from PIL import Image 

im = Image.open("lenna-dithered-120.png")

ser = serial.Serial('COM8', 115200)
print(f'printing to {ser.name}')
#write my pixel data

#prepare pixel data of the image
rows = []
for y in range(0, 120):
    row_bytes = []
    for x in range(0, 120, 8):
        #get 8 bits at a time
        byte = 0
        for x_offset in range(0,8):
            if(x_offset > 0):
                byte = byte << 1
            px = im.getpixel((x+x_offset,y))
            byte = byte | px
        row_bytes.append(byte)
    rows.append(bytearray(row_bytes))

#generate a simple pattern
counter = 0
frame = 0
#offset the rows by one
while True:
    for y in range(0, 120):
        #generate row data based on the counter
        ser.write(rows[y])
    print('.')
    frame = frame + 1

