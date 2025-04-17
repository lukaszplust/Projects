import os
import face_recognition

# KURWA TO NIE DZIALA WZYSTKO , DO POPRAWY WYKRYWANIE TWARZY KONKRETNEJ
KNOWN_FACES_DIR = "known_faces"


def load_known_faces():
    known_faces = []
    known_names = []
    
    for filename in os.listdir(KNOWN_FACES_DIR):
        if filename.endswith((".png", ".jpg", ".jpeg")):  # Obsługuje różne formaty
            image_path = os.path.join(KNOWN_FACES_DIR, filename)
            
            try:
                image = face_recognition.load_image_file(image_path)
                encoding = face_recognition.face_encodings(image)
                
                if encoding:
                    known_faces.append(encoding[0])
                    known_names.append(filename.split('.')[0])  # Używamy nazwy pliku jako podpisu
            except Exception as e:
                print(f"Nie udało się wczytać obrazu {filename}: {e}")
    
    return known_faces, known_names