import cv2
import numpy as np

import tcp_streamer

import threading

image = np.zeros((100, 100, 4), dtype=np.uint8)
actorinfo = ""


def img_thread():
    streamer = tcp_streamer.TCPStreamer()
    server_address = ('localhost', 8999)
    streamer.listen(server_address)

    while True:
        message = streamer.recvMessage()
        global image
        if message:
            image = cv2.imdecode(np.frombuffer(message, dtype=np.uint8), cv2.IMREAD_UNCHANGED)


def actorinfo_thread():
    streamer = tcp_streamer.TCPStreamer()
    server_address = ('localhost', 8998)
    streamer.listen(server_address)

    while True:
        message = streamer.recvMessage()
        global actorinfo
        if message:
            actorinfo = message.decode('utf-8')


if __name__ == '__main__':
    threading.Thread(target=img_thread).start()
    threading.Thread(target=actorinfo_thread).start()

while True:
    lines = actorinfo.splitlines()
    for line in lines:
        if line == '':
            continue
        split = line.split(' ', 1)
        rectangleStr = split[1]
        rectangleNumbers = rectangleStr.split(' ')
        height, width, channels = image.shape

        #rectWidth = int(float(rectangleNumbers[2]) * width)
        #rectHeight = int(float(rectangleNumbers[3]) * height)

        #rectX = int(float(rectangleNumbers[0]))
        #rectY = int(float(rectangleNumbers[1]))

        rectWidth = int(float(rectangleNumbers[2]) * width)
        rectHeight = int(float(rectangleNumbers[3]) * height)

        rectX = int(float(rectangleNumbers[0]) * width - rectWidth / 2)
        rectY = int(float(rectangleNumbers[1]) * height - rectHeight / 2)

        cv2.rectangle(image, (rectX, rectY), (rectX + rectWidth, rectY + rectHeight), (255, 0, 0), 1)

    cv2.imshow('frame', image)
    cv2.waitKey(1)
