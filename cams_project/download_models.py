import tensorflow_hub as hub
import cv2
import requests
import os
import tensorflow as tf

MODEL_DIR = "saved_model_ssd_mobilenet_v2"

if not os.path.exists(MODEL_DIR):
    print("Pobieranie modelu SSD MobileNetV2 z TF Hub")
    model = hub.load("https://tfhub.dev/tensorflow/ssd_mobilenet_v2/2")
    tf.saved_model.save(model, MODEL_DIR)
    print("Zapisano model do:", MODEL_DIR)
else:
    print("Model już istnieje — ładowanie z dysku")

# wczytuje model z dysku
model = tf.saved_model.load(MODEL_DIR)


CASCADE_PATH = 'haarcascade_frontalface_default.xml'

if not os.path.exists(CASCADE_PATH):
    print("Pobieranie Haar Cascade...")
    url = "https://raw.githubusercontent.com/opencv/opencv/master/data/haarcascades/haarcascade_frontalface_default.xml"
    response = requests.get(url)
    with open(CASCADE_PATH, 'wb') as f:
        f.write(response.content)
    print("Zapisano Haar Cascade.")
else:
    print("Haar Cascade już istnieje.")
    
face_cascade = cv2.CascadeClassifier(CASCADE_PATH)