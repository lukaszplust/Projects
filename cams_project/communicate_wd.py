import ipaddress
import subprocess
from concurrent.futures import ThreadPoolExecutor

def ping(ip):
    """Wykonaj szybki ping do IP."""
    try:
        subprocess.check_output(['fping', '-c1', '-t500', str(ip)], stderr=subprocess.DEVNULL)
        return str(ip)
    except subprocess.CalledProcessError:
        return None

def parse_nmap_output(output):
    """Parsuj wynik nmapa i zwróć informacje o urządzeniach."""
    devices = []
    lines = output.splitlines()
    current_ip = None
    description = ""

    for line in lines:
        if line.startswith("Nmap scan report for"):
            if current_ip:
                devices.append((current_ip, description.strip() or "Nieznane urządzenie"))
            parts = line.split()
            current_ip = parts[-1]
            description = ""
        elif "MAC Address" in line:
            # np. MAC Address: 00:11:22:33:44:55 (Apple)
            description += line.strip()
        elif "OS details" in line or "Device type" in line or "Running" in line:
            description += " | " + line.strip()

    if current_ip:
        devices.append((current_ip, description.strip() or "Nieznane urządzenie"))

    return devices

def scan_network(network_range):
    """Skanuj sieć i opisz aktywne urządzenia."""
    active = []
    with ThreadPoolExecutor(max_workers=200) as executor:
        futures = [executor.submit(ping, ip) for ip in ipaddress.IPv4Network(network_range)]
        for future in futures:
            ip = future.result()
            if ip:
                active.append(ip)

    if active:
        print(f"\nSkanowanie aktywnych urządzeń: {', '.join(active)}\n")
        result = subprocess.run(
            ["sudo", "nmap", "-T4", "-O", "-n"] + active,
            capture_output=True,
            text=True
        )
        devices = parse_nmap_output(result.stdout)
        for ip, desc in devices:
            print(f"🔹 {ip} — {desc}")
    else:
        print("Brak aktywnych urządzeń.")

    return active

# Przykład użycia
network_range = "192.168.100.0/24"
ips = scan_network(network_range)
print("\nSkan zakończony. Aktywne urządzenia:", ips)