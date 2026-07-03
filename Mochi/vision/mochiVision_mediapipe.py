import cv2
import mediapipe as mp

mp_face_detection = mp.solutions.face_detection
mp_drawing = mp.solutions.drawing_utils

url = "http://192.168.29.71:81/stream"

cap = cv2.VideoCapture(url)

if not cap.isOpened():
    print("❌ Could not connect to ESP32-CAM.")
    exit()

print("✅ Connected to ESP32-CAM!")

with mp_face_detection.FaceDetection(model_selection=0,min_detection_confidence=0.5) as face_detection:

    while True:
        ret, frame = cap.read()

        if not ret:
            print("❌ Frame not received.")
            break

        h,w = frame.shape[:2]
        
        image_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)  
        results = face_detection.process(image_rgb)

        if results.detections:
            for detection in results.detections:
                mp_drawing.draw_detection(frame, detection)
        
                bbox = detection.location_data.relative_bounding_box

            xmin = int(bbox.xmin*w)  
            ymin = int(bbox.ymin*h)
            b_width = int(bbox.width*w)
            b_height = int(bbox.height*h) 

            facex = xmin + b_width//2
            facey = ymin + b_height//2   

            x = w//2
            y = h//2

            cv2.circle(frame,(x,y),5,(0,0,255),-1)

            cv2.circle(frame,(facex,facey),5,(0,255,0),-1)
            print(facex,facey)

        cv2.imshow('MediaPipe Face Detection', frame)

        if cv2.waitKey(1) & 0xFF == 27: 
            break 

cap.release()
cv2.destroyAllWindows()