import cv2
import numpy as np

import threading

from flask import Flask, request

app = Flask(__name__)

image = np.zeros((100, 100, 4), dtype=np.uint8)

actorinfo = ""


@app.route('/image', methods=['POST'])
def imageRoute():
    global image
    image = cv2.imdecode(np.frombuffer(request.data, dtype=np.uint8), cv2.IMREAD_UNCHANGED)

    return ""


@app.route('/actorinfo', methods=['POST'])
def actorinfoRoute():
    global actorinfo
    actorinfo = request.data.decode("utf-8") 

    return ""


def flask_thread():
    app.run(threaded=True, debug=True, use_reloader=False)


if __name__ == '__main__':
    threading.Thread(target=flask_thread).start()


while True:
    lines = actorinfo.splitlines()
    for line in lines:
        if line == '':
            continue
        split = line.split(' ', 1)
        rectangleStr = split[1]
        rectangleNumbers = rectangleStr.split(' ')
        width, height, channels = image.shape

        rectWidth = int(float(rectangleNumbers[2]) * width)
        rectHeight = int(float(rectangleNumbers[3]) * height)

        rectX = int(float(rectangleNumbers[0]) * width - rectWidth / 2)
        rectY = int(float(rectangleNumbers[1]) * height - rectHeight / 2)

        cv2.rectangle(image, (rectX, rectY), (rectX + rectWidth, rectY + rectHeight), (255, 0, 0), 1)
 
    cv2.imshow('frame', image)
    cv2.waitKey(1)