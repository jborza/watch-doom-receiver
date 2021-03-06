from PIL import Image, ImageDraw
import serial
import pdb
import sys
import datetime

# image size 
imgx = 240
imgy = 240
image = Image.new("1", (imgx, imgy))
draw = ImageDraw.Draw(image)
VSYNC = b'V'
HSYNC = b'H'

def get_row_bytes(image, row):
    y = row
    row_bytes = []
    for x in range(0, imgx, 8):
        #get 8 bits at a time
        byte = 0
        for x_offset in range(0,8):
            #pdb.set_trace()
            if(x_offset > 0):
                byte = byte << 1
            px = 0 if image.getpixel((x+x_offset,y)) == 0 else 1
            byte = byte | px
        row_bytes.append(byte)
    return bytearray(row_bytes)

def draw_frame(number):
    #wrap around frames
    if(number > 120):
        number = number % 120
    frame_txt = f'Frame {number:03}'
    fps_txt = f'{last_fps:02} FPS'

    draw.rectangle((0, 0, imgx, imgy), fill=(0))            #clear screen
    draw.line((0,0,number,imgy), fill=(255))                #line converging on 120,120
    draw.line((0,0,imgx,number), fill=(255))                #line converging on 120,120
    draw.line((number,0,imgx,imgy/2), fill=(255))           #line converging on 120,60
    draw.line((number,imgy,imgx,imgy/2), fill=(255))        #line converging on 120,120
    draw.line((10,number,imgx-10,imgy-number), fill=(255))      #spinner part 1
    draw.line((imgx-number,10,number,imgy-10), fill=(255))      #spinner part 2
    draw.text((10,10), frame_txt, fill=(255))
    draw.text((30,80), fps_txt, fill=(255))
    draw.text((number/4,number/2), "boo", fill=(255))
    if(number == 30):
        image.save('boo.png')

ser = serial.Serial('COM8', 500000)
ser.timeout = None
print(f'printing to {ser.name}, timeout={ser.timeout}')
last_second = 0
current_fps = 0
last_fps = 0

frame = 0
while True:
    #send a reset sequence
    print(f'drawing frame {frame}')
    draw_frame(frame)
    #send out rows over the serial port
    for row in range(0, imgy):
        ser.write(get_row_bytes(image, row))
        if ser.in_waiting:
            response = ser.read()
            if(response == VSYNC): 
                print('.', end='')
                break
    frame = frame + 1
    current_fps = current_fps + 1
    #recalculate fps
    if(datetime.datetime.now().second != last_second):
        last_second = datetime.datetime.now().second
        last_fps = current_fps
        current_fps = 0