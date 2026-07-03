import cv2

mouseX,mouseY = -1,-1

# ESP32-CAM stream URL
url = "http://192.168.29.71:81/stream"

cap = cv2.VideoCapture(url)

if not cap.isOpened():
    print("❌ Could not connect to ESP32-CAM.")
    exit()

print("✅ Connected to ESP32-CAM!")

def draw_crosshair(event,x,y,flags,param):
    global mouseX,mouseY

    if event == cv2.EVENT_LBUTTONDOWN:

        
        mouseX,mouseY = x,y
        img = frame 
        b,g,r = img[y,x]

        print(f"clicked coodinates x={x} , y = {y}")
        print(f"R={r} B={b} G={g}")

cv2.namedWindow('Crosshair Image')
cv2.setMouseCallback('Crosshair Image', draw_crosshair)

while True:
    ret, frame = cap.read()

    if not ret:
        print("❌ Frame not received.")
        break

    height, width = frame.shape[:2]

    cv2.putText(
        frame,
        f"{width} x {height}",
        (20, 40),
        cv2.FONT_HERSHEY_SIMPLEX,
        1,
        (0, 255, 0),
        2,
    )


        
    cv2.line(frame,(mouseX,0),(mouseX,frame.shape[0]),(0,255,0),2)
    cv2.line(frame,(0,mouseY),(frame.shape[1],mouseY),(0,255,0),2)

    cv2.imshow('Crosshair Image',frame)

    if cv2.waitKey(1) & 0xFF == 27: 
        break 

    # height,width = frame.shape[:2]

    # center_x = width//2
    # center_y = height//2

    # cv2.line(frame,(center_x,0),(center_x,height),(0,255,0),thickness=3,lineType=cv2.LINE_AA)
    # cv2.line(frame,(0,center_y),(width,center_y),(0,255,0),thickness=3,lineType=cv2.LINE_AA)

    # cv2.circle(frame,(160,120),5,(0,255,0),thickness=cv2.FILLED,lineType=cv2.LINE_AA)

    # cv2.imshow("Outline Circle", frame)


    

cap.release()
cv2.destroyAllWindows()