import ipaddress
import socket
import subprocess
from concurrent.futures import ThreadPoolExecutor

# funkcja do pingowania hostów
def ping_host(ip):
    try:
        result = subprocess.run(["ping", "-c", "1", "-W", "1", ip],
                                 stdout=subprocess.DEVNULL)
        if result.returncode == 0:
            print(f"Znaleziono aktywne urządzenie: {ip}")
            return ip
    except Exception as e:
        pass
    return None

# czy port jest otwarty na danym urządzeniu
def check_port(ip, port):
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(1)
        # Próba połączenia
        result = sock.connect_ex((ip, port))
        sock.close()
        # zwracam True, jeśli port jest otwarty
        return result == 0  
    except socket.error:
        return False

# funkcja do skanowania sieci i wykrywania kamer
def ping_skan(subnet):
    active_hosts = []
    network = ipaddress.ip_network(subnet, strict=False)
    print(f"Rozpoczynam skanowanie sieci {subnet}...")

    # tworze pulę wątków
    with ThreadPoolExecutor(max_workers=20) as executor:
        results = executor.map(ping_host, (str(ip) for ip in network.hosts()))
    
    # dodaje aktywne hosty do listy
    active_hosts = [result for result in results if result is not None]
    
    # sprawdzam, które urządzenia to kamery
    cameras = []
    for ip in active_hosts:
        print(f"Sprawdzam urządzenie {ip} na portach 80 i 554...")
        # sprawdzam port 554 dla RTSP
        if check_port(ip, 554):
            print(f"Znaleziono kamerę RTSP na {ip}")
            cameras.append(ip)
            # sprawdzamm port 80 dla HTTP dla pewnosci
        elif check_port(ip, 80):
            print(f"Znaleziono kamerę HTTP na {ip}")
            cameras.append(ip)
    
    return cameras

# uruchamiam skanowanie w sieci
devices = ping_skan("192.168.100.0/24")
print("Znalezione kamery:", devices)