import cv2

# link RTSP
rtsp_url = "rtsp://admin:Plust01030609!@192.168.100.100:554/cam/realmonitor?channel=1&subtype=0"

# otwieram strumień RTSP
cap = cv2.VideoCapture(rtsp_url, cv2.CAP_FFMPEG)

if not cap.isOpened():
    print("Nie udało się połączyć z kamerą.")
else:
    print("Połączono z kamerą. Rozpoczynamy odczyt strumienia.")

while True:
    ret, frame = cap.read()
    if not ret:
        print("Błąd odczytu strumienia.")
        break

    cv2.imshow("Stream", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
