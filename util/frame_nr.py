from PIL import Image, ImageDraw

# image size 
imgx = 120
imgy = 120
image = Image.new("RGB", (imgx, imgy))
draw = ImageDraw.Draw(image)

def draw_frame(number):
    #wrap around frames
    if(number > 120):
        number = number % 120

    frame_txt = f'Frame {number:03}'
    frame_txt = f'Frame {number:03}'
    print(frame_txt)
    draw.rectangle((0, 0, imgx, imgy), fill=(0, 0, 0, 0))
    draw.line((0,0,number,imgy), fill=(255,255,255,255))
    draw.line((0,0,imgx,number), fill=(255,255,255,255))
    draw.line((number,0,number,imgy), fill=(255,255,255,255))
    draw.text((10,10), frame_txt)
    draw.text((number/4,number/2), "boo")
    image.save('boo.png')

draw_frame(1)