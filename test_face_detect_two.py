# import all the required modules
"""
with the use of pan_tilt_auto_three.ino, track according to web-cam (PC)
pan_tilt_auto_four.ino, track using web_cam (Cutiepie itself)
"""
from keras.preprocessing.image import img_to_array
from keras.models import load_model
import numpy as np
import serial
import time
import cv2


def recognize_face(face, img):
    x, y, w, h = face
    cv2.rectangle(img, (x, y), (x + w, y + h), (0, 255, 0), 5)
    arr = {y: y + h, x: x + w}
    print(arr)

    print('X :' + str(x))
    print('Y :' + str(y))
    print('x+w :' + str(x + w))
    print('y+h :' + str(y + h))

    # Center of roi (Rectangle)
    xx = int(x + (x + h)) / 2
    yy = int(y + (y + w)) / 2
    print(xx)
    print(yy)
    center = (xx, yy)
    xx = int(xx)
    yy = int(yy)
    cv2.circle(img, (xx, yy), 10, (255, 0, 0))
    # sending data to arduino
    print("Center of Rectangle is :", center)
    # the_string = "X{0:.0f}Y{0:.0f}"
    the_string = "X{}Y{}"
    data = the_string.format(xx, yy)
    print("output = '" + data + "'")
    arduino.write(bytes(data, 'utf-8'))


def send_default_message():
    init_x = 0
    init_y = 0
    the_string = "X{}Y{}"
    data = the_string.format(init_x, init_y)
    print("output = '" + data + "'")
    arduino.write(bytes(data, 'utf-8'))


emotion_model_path = '_mini_XCEPTION.02-0.45.hdf5'
emotion_classifier = load_model(emotion_model_path, compile=False)
EMOTIONS = ["angry", "disgust", "scared", "happy", "sad", "surprised", "neutral"]
sad_count = 0
happy_count = 0
emotion_index = 0


def detect_emotion(face, frame):
    global sad_count
    global happy_count
    global emotion_index

    (FX, FY, FW, FH) = face
    # extract region of interest from the grey-scale image
    roi = gray[FY:FY + FH, FX:FX + FW]
    roi = cv2.resize(roi, (48, 48))
    # normalize the gray color
    # intensity becomes between 0 to 1
    roi = roi.astype("float") / 255.0
    # convert image to numpy array
    roi = img_to_array(roi)
    # reminder: shape of the array is
    # the size of the array along each dimension
    """
    e.g.
    b = np.array([[1,2,3],[4,5,6]])    # Create a rank 2 array
    print(b.shape)                     # Prints "(2, 3)"

    """
    roi = np.expand_dims(roi, axis=0)
    preds = emotion_classifier.predict(roi)[0]
    # emotion_probability = np.max(preds)
    label = EMOTIONS[preds.argmax()]
    cv2.putText(frame, label, (FX, FY - 10), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
    if label != "sad":
        if label != "happy":
            emotion_index = 0

    if label == "sad":
        sad_count += 1
    else:
        if label == "happy":
            happy_count += 1
    if sad_count >= 100:
        emotion_index = 1
        sad_count = 0
    if happy_count >= 70:
        emotion_index = 2
        happy_count = 0

    emotion_string = "E{}"
    emotion_data = emotion_string.format(emotion_index)
    arduino.write(bytes(emotion_data, 'utf-8'))


# Setup Communication path for arduino (In place of 'COM5' put the port to which your arduino is connected)
arduino = serial.Serial('COM3', 9600)
time.sleep(2)
print("Connected to arduino...")

# importing the Haar_cascade for face detection
face_cascade = cv2.CascadeClassifier('haarcascade_frontalface_default.xml')

# To capture the video stream from webcam.
cap = cv2.VideoCapture(1)

# Read the captured image, convert it to Gray image and find faces
while 1:
    ret, img = cap.read()
    cv2.resizeWindow('img', 500, 500)
    cv2.line(img, (500, 250), (0, 250), (0, 255, 0), 1)
    cv2.line(img, (250, 0), (250, 500), (0, 255, 0), 1)
    cv2.circle(img, (250, 250), 5, (255, 255, 255), -1)
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    faces = face_cascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5, minSize=(100, 100))
    if len(faces) > 0:
        faces = sorted(faces, reverse=True, key=lambda x: (x[2] - x[0]) * (x[3] - x[1]))[0]
    # detect the face and make a rectangle around it.
        recognize_face(faces, img)
        detect_emotion(faces, img)
    else:
        if len(faces) == 0:
            send_default_message()

    # Display the stream.
    cv2.imshow('img', img)

    # Hit 'Esc' to terminate execution
    k = cv2.waitKey(30) & 0xff
    if k == 27:
        break
cap.release()
cv2.destroyAllWindows
