# HandTrackingModule.py

import cv2
import mediapipe as mp
import time


class handDetector():
    def __init__(self, mode=False, maxHands=2, detectionCon=0.5, trackCon=0.5):
        self.mode = mode
        self.maxHands = maxHands
        self.detectionCon = detectionCon  # Convert to int
        self.trackCon = trackCon  # Convert to int

        self.mpHands = mp.solutions.hands
        self.hands = self.mpHands.Hands(
            static_image_mode=False,
            max_num_hands=2,
            model_complexity=0,
            min_detection_confidence=0.5,
            min_tracking_confidence=0.5)
        # tespit edilen eklemlerin görünümü için tanımlanan değer
        self.mpDraw = mp.solutions.drawing_utils

    def findHands(self, img, draw=True):
        # gelen resmi el tanıma için BGR (Blue, Green, Red) renk alanındaki bir görüntüyü
        # RGB (Red, Green, Blue) renk alanına dönüştürür.
        imgRGB = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        # model içinde işlemeye başlar
        self.results = self.hands.process(imgRGB)

        # multi_hand_landmarks tespit edilen el verilerinin çıktılarını verir
        if self.results.multi_hand_landmarks:
            for handLms in self.results.multi_hand_landmarks:
                if draw:
                    # mpHands.HAND_CONNECTIONS -> eklemler arasını birelşirmeyi göstermek için
                    self.mpDraw.draw_landmarks(img, handLms,
                                               self.mpHands.HAND_CONNECTIONS)
        return img

    def findPosition(self, img, handNo=0, draw=True):
        """
        geriye id,  canvas_x, canvas_y içeren eklem listesini döndürür
        """
        lmList = []
        if self.results.multi_hand_landmarks:
            myHand = self.results.multi_hand_landmarks[handNo]
            for id, lm in enumerate(myHand.landmark):
                h, w, c = img.shape
                cx, cy = int(lm.x * w), int(lm.y * h)
                lmList.append([id, cx, cy])
                if draw:
                    cv2.circle(img, (cx, cy), 1, (255, 0, 0), cv2.FILLED)
                    cv2.putText(img, str(id) + " " + str(cx) + " " + str(cy) + " " +  str(int(lm.z*100)),
                                (cx, cy), cv2.FONT_HERSHEY_PLAIN, 0.5, (0, 0, 0), 1)

        return lmList


def main():
    pTime = 0
    cTime = 0
    cap = cv2.VideoCapture(1)
    detector = handDetector()
    while True:
        success, img = cap.read()
        img = detector.findHands(img)
        lmList = detector.findPosition(img)
        if len(lmList) != 0:
            print(lmList[4])

        # fps hesaplama
        cTime = time.time()
        fps = 1 / (cTime - pTime)
        pTime = cTime

        cv2.putText(img, str(int(fps)), (10, 70), cv2.FONT_HERSHEY_PLAIN, 3,
                    (255, 0, 255), 3)

        cv2.imshow("Image", img)
        cv2.waitKey(1)


if __name__ == "__main__":
    main()
