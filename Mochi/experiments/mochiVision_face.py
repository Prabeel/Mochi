import cv2

face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + "haarcascade_frontalface_default.xml")

url = "http://192.168.29.71:81/stream"

cap = cv2.VideoCapture(url)

if not cap.isOpened():
    print("❌ Could not connect to ESP32-CAM.")
    exit()

print("✅ Connected to ESP32-CAM!")


while True:
    ret, frame = cap.read()

    if not ret:
        print("❌ Frame not received.")
        break

    Gray_image = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    faces = face_cascade.detectMultiScale(Gray_image,scaleFactor=1.1,minNeighbors=5,minSize=(30, 30))
    print(faces)

    for (x, y, w, h) in faces:
        cv2.rectangle(Gray_image, (x, y), (x + w, y + h), (255, 0, 0), 2)

    cv2.imshow('Detected Faces', frame)

    if cv2.waitKey(1) & 0xFF == 27: 
        break 

cap.release()
cv2.destroyAllWindows()