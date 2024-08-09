#Bismillahirrahmanirrahim
import cv2
import time
import HandTrackingModule as htm
import numpy as np

from dk_connection import get_img, start_socket, send_data

p_time = 0
finger_top_id = [4, 8, 12, 16, 20]

start_socket()
detector = htm.handDetector(detectionCon=0.75)


while True:
    success, img_resp = get_img()
    
    if not success:
        break

    # eklem algılama
    img_data = np.frombuffer(img_resp, dtype=np.uint8)
    img = cv2.imdecode(img_data, cv2.IMREAD_COLOR)

    img = detector.findHands(img=img, draw=True)
    # algılanan eklemlerin konumlarını alma
    lm_list = detector.findPosition(img, draw=True)

    fingers_open_lst = []

    if len(lm_list) != 0:

        if lm_list[finger_top_id[0]][1] > lm_list[finger_top_id[0]-1][1]:
            fingers_open_lst.append(1)
        else:
            fingers_open_lst.append(0)
        for id in range(1, 5):
            if lm_list[finger_top_id[id]][2] < lm_list[finger_top_id[id]-2][2]:
                fingers_open_lst.append(1)
            else:
                fingers_open_lst.append(0)


    if len(fingers_open_lst) > 0:
        send_data(str(fingers_open_lst)[1:-1])
    c_time = time.time()
    fps = 1/(c_time-p_time)
    p_time = c_time

    cv2.putText(img=img, text=str(fps), org=(17, 35),
                fontFace=cv2.FONT_HERSHEY_PLAIN, fontScale=2,
                color=(255, 0, 0), thickness=3)

    cv2.imshow("Deneyap parmak sayac", img)
    if cv2.waitKey(5) & 0xFF == 27:
        break
