import requests
import socket


deneyap_ip = '192.168.*.*'
kamera_port = 80


url = f'http://{deneyap_ip}:{kamera_port}/capture'


def get_img():
    resp = requests.get(url)
    if resp.status_code == 200:
        return True, resp.content


# tcp part
client_socket: socket


def start_socket():
    c = 0
    global client_socket
    while (c < 10):
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_address = (deneyap_ip, 65000)
        try:
            client_socket.connect(server_address)
            return
        except Exception as e:
            c += 1
            print(e)


def send_data(msg: str):
    msg = msg + '\n'
    client_socket.sendall(msg.encode())