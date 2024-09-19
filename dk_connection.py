import socket
import cv2
import numpy as np

class SocketServer:
    def __init__(self, udp_ip="0.0.0.0", udp_port=12345):
        self.udp_ip = udp_ip
        self.udp_port = udp_port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind((self.udp_ip, self.udp_port))
        self.conected_adress = None
        print("Listening on {}:{}".format(self.udp_ip, self.udp_port))

    def receive_data(self):
        data_buffer = b''
        while True:
            try:
                packet, addr = self.sock.recvfrom(65536)  # Maksimum UDP paket boyutu
                self.conected_adress = addr
                data_buffer += packet

                # Veri tamamlandığında, görüntüyü işleme
                if len(packet) < 1400:  # Son paket daha küçük olacaktır
                    np_arr = np.frombuffer(data_buffer, np.uint8)
                    img = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)
                    data_buffer = b''  # Bufferı sıfırlayın
                    return img  # Görüntüyü döndür

            except Exception as e:
                print("Error in receiving data:", e)
                break
    def send_data(self, message):
        try:
            ip = self.conected_adress[0]
            port = self.conected_adress[1]
            # Mesajı byte'lara dönüştür
            message_bytes = message.encode('utf-8')

            # Mesajı belirtilen IP ve porta gönder
            self.sock.sendto(message_bytes, (ip, port))
            print(f"Sent message to {ip}:{port}")

        except Exception as e:
            print("Error in sending data:", e)
    

    def close(self):
        self.sock.close()


class ImageReceiver:
    def __init__(self, socket_server):
        self.socket_server = socket_server

    def get_image(self):
        img = self.socket_server.receive_data()
        if img is not None:
            return img
        return None


def main():
    socket_server = SocketServer()
    image_receiver = ImageReceiver(socket_server)

    while True:
        img = image_receiver.get_image()
        if img is not None:
            # Görüntüyü istenen boyuta ölçeklendirin
            resized_img = cv2.resize(img, (1920, 1080))
            cv2.imshow("Received Image", resized_img)
            if cv2.waitKey(1) & 0xFF == 27:  # ESC tuşuna basıldığında çık
                break

    socket_server.close()
    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
