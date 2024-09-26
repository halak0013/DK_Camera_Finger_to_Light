# Bismillahirrahmanirrahim

import cv2
import cvzone.HandTrackingModule as htm
from dk_connection import ImageReceiver, SocketServer


class FingerCount:
    def __init__(self, target_width=1800, target_height=1000, detection_confidence=0.65, max_hands=1, source="camera"):
        self.target_width = target_width
        self.target_height = target_height
        self.source = source

        if source == "camera":
            self.cap = cv2.VideoCapture(0)
            self.cap.set(3, self.target_width)
            self.cap.set(4, self.target_height)
        else:
            self.sock_server = SocketServer()
            self.image_receiver = ImageReceiver(self.sock_server)

        self.detector = htm.HandDetector(
            detectionCon=detection_confidence, maxHands=max_hands)

        self.fingers = []

    def get_img(self):
        img = None
        succes = None
        if self.source == "sock":
            img = self.image_receiver.get_image()
            succes = img is not None
        else:
            succes, img = self.cap.read()
        return succes, img

    def send_data(self):
        result = ','.join(map(str, self.fingers))
        self.sock_server.send_data(f"{result}")

    def run(self):
        while True:
            success, img = self.get_img()
            if not success:
                continue
            img = cv2.flip(img, 1)
            img = cv2.resize(img, (self.target_width, self.target_height))
            # Detect hands
            hands, img = self.detector.findHands(img)

            if hands:
                self.fingers = self.detector.fingersUp(hands[0])
                if self.fingers == [0, 0, 1, 0, 0]:
                    self.fingers = [0, 0, 0, 0, 0]
                print(self.fingers)
                if self.source == "sock":
                    self.send_data()

            cv2.imshow("Finger Count", img)
            if cv2.waitKey(5) & 0xFF == 27:  # Press 'ESC' to exit
                print("Çık")
                break


if __name__ == "__main__":
    fc = FingerCount(source="camera")
    fc.run()
