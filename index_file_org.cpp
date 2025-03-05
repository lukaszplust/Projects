#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
// uzywam w dla funkcji sin()
#include <cmath>
#include <vector>


using namespace std;

#define FILEINDEX "index"
#define TEMPDATA "temp"
#define FILEDATA "file"

#define INDEX_SIZE sizeof(Index)
#define RECORD_SIZE sizeof(Record)

#define ALPHA 1
// wspolczynnik blokowania
#define PAGESIZE 4


// struktura dla zmiennych globalnych
struct GlobalVariables {
    int recordsPrimaryArea;
    int recordsOverflowArea;
    int maxNumberRecordsPrimary;
    int maxNumberRecordsOverflow;
    // zmienne potrzebne do eksperymentu dodatek
    int totalReads;
    int totalWrites;
    int added;

    // konstruktor inicjalizujący
    GlobalVariables() {
        recordsPrimaryArea = 0;
        recordsOverflowArea = 0;
        maxNumberRecordsPrimary = 4;
        maxNumberRecordsOverflow = maxNumberRecordsPrimary / 2;
        totalReads = 0;
        totalWrites = 0;
    }
};
// tworzenie instancji globalnych zmiennych
GlobalVariables globalVariable;


// index as on lecture
// index contains a kay and number of page which represents where data is located
struct Index{
    int key;
    int page;
};


// record (row) in the file with data
struct Record {
    int key;
    float side1;
    float side2;
    float angle;
    int pointer;

    Record(){
        key = -1;
        side1 = 0;
        side2 =0;
        angle = 0;
        pointer = -1;
    };
};

// Przekazanie zmiennych przez referencję (&) pozwala na zmiane aktualnych wartości 
// tych zmiennych po zakończeniu funkcji. Zamiast zwracać te wartości z funkcji 
// (np. poprzez return), modyfikuje oryginalne zmienne, co jest bardziej efektywne 
// i wygodne, zwłaszcza,gdy chce, aby te wartości były dostępne poza funkcją

void init();
void addRecord(float side1, float side2, float angle, int& read, int& write);
void indexCreate(int& read, int& write);
int indexSearch(int key, int& read);
void indexShow();
void fileShow();
void find(int k);
void reorg(int& read, int& write);
void removeFromBuffer(Record* buffer, int records_counter);
void sort(Record buffer[], int size);

//void calculateBlockingFactor() {
//    size_t recordSize = sizeof(Record); // Rozmiar rekordu w bajtach
//    size_t pageSize = PAGESIZE * sizeof(Record); // Rozmiar bloku w bajtach

//    int blockingFactor = pageSize / recordSize; // Obliczenie współczynnika blokowania
//    cout << "Blocking factor: " << blockingFactor << endl;
//}

int main(){
    //cout << "WYNIK MOJ: " << ceil(0.7 * 4) <<endl;
    //calculateBlockingFactor();
    init();
    int x;
    int key;
    int read;
    int write;

    float side1;
    float side2;
    float angle;
    bool end = false;
    string choice;
    string file_name;
    cout << "Choice 'keyboard' or 'file' and input that:" << endl;
    cin >> choice;
    if (choice == "keyboard"){
        while (!end){
            read = 0;
            write = 0;
            cout << "\n";
            cout << "make a choice : " << endl;
            cout << "1 - add record:\n";
            cout << "2 - show index\n";
            cout << "3 - show file\n";
            cout << "4 - reorganize\n";
            cout << "5 - find\n";
            cout << "6 - end\n";
            cin >> x;
            cout << "Your choice is: " << x <<"\n";
            cout << "\n";
            if (x == 1){
                cout << "Input side1: "<< endl;
                cin>> side1;
                cout << "Input side2: "<< endl;
                cin>> side2;
                cout << "Input angle: "<< endl;;
                cin>> angle;
                addRecord(side1,side2,angle,read,write);
                cout << "Added new record" << endl;
                
                cout << "reads: " << read << " writes" << write << endl;
            }
            else if (x ==2){
                indexShow();
            }
            else if (x == 3){
                fileShow();
            }
            else if (x == 6){
                end = true;
                cout << "End of program\n";
            }
            else if (x == 5){
                cout << "Input key to find it and values: "<< endl;
                cin>> key;
                find(key);
            }
            else if (x == 4){
                cout << "REORGANIZING: "<< endl;
                reorg(read, write);
                cout << "reads: " << read << " writes: "<< write << endl;
            }

            else{
                cout << "Input correct number\n";
            }
        }
    }
    if (choice == "file"){
        std::ifstream file_choice;
        cout << "Input file name: " << "\n";
        cin >> file_name;
        file_choice.open(file_name);

        while (!end){
            read = 0;
            write = 0;
            file_choice >> file_name;

            if (file_name == "add"){
                file_choice >> side1 >> side2 >> angle;
                addRecord(side1,side2,angle,read,write);
                //cout << "ADDED NEW RECORD OF KEY: " << (int)(float)(side1 * side2 * sin(angle * M_PI / 180.0)) <<endl;
                
                cout << "reads: " << read << " writes:  " << write << endl;
            }
            else if (file_name == "show"){
                file_choice >> file_name;
                if (file_name == "file"){
                    fileShow();
                }
                else {
                    indexShow();
                }
            }
            else if (file_name == "reorganize"){
                reorg(read,write);
                cout << "REORGANIZING" << endl;
                cout << "reads: " << read << " writes: "<< write << endl;
            }
            else if (file_name == "find"){
                file_choice >> key;
                find(key);
            }
            else if (file_name == "exit"){
                end = true;
                cout << "toal reads: " << globalVariable.totalReads / globalVariable.added << endl;
                cout << "toal writes: " << globalVariable.totalWrites/ globalVariable.added << endl;
            }
        }
        file_choice.close();
    }
    return 0;
};


void init() {
    
    Record* buff = new Record[PAGESIZE];
    FILE* f = fopen(FILEDATA, "wb");

    int read = 0;
    int write = 0;

    // W pętli zapisuje puste rekordy do pliku tak, by pokryły obszar
    // globalVariable.maxNumberRecordsPrimary + globalVariable.maxNumberRecordsOverflow. 
    // Zmienna write zlicza operacje zapisu

    // globalVariable.maxNumberRecordsPrimary i globalVariable.maxNumberRecordsOverflow 
    // określają maksymalną liczbę rekordów, jakie można przechowywać w 
    // obszarze podstawowym i nadmiarowym. 
    // Suma tych wartości dzielona przez PAGESIZE (liczba rekordów na stronę)
    // daje przybliżoną liczbę stron potrzebnych do zapisania tych rekordów

    // ceil() zaokrągla wynik w górę, aby w przypadku części dziesiętnej zawsze mieć pełną stronę
    // i - liczba stron do zapisania
    int pages = (int)ceil(((double)(globalVariable.maxNumberRecordsPrimary + globalVariable.maxNumberRecordsOverflow)) / (double)PAGESIZE);
    for (int i =0; i < pages; i++){
        // size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream);
        fwrite(buff, RECORD_SIZE, PAGESIZE, f);
        write++;
    }
    
    delete[] buff;
    fclose(f);
    indexCreate(read,write);

    cout << "Init values: " << endl;
    cout<< "reads:  "<< read << " " << "writes: " << write << endl;
    
    // najmniejszy mozliwy klucz to 1 
    // sin z 90 stopni to 1  
    addRecord(1.0, 1.0, 90.0, read,write);    
};

void addRecord(float side1, float side2, float angle, int& read, int& write){
    // jesli overfloow jest caly zapelniony, wykonuje reorganizacje
    if (globalVariable.recordsOverflowArea == globalVariable.maxNumberRecordsOverflow){
        reorg(read, write);
    }

    
    int key = (int)(float)(side1 * side2 * sin(angle * M_PI / 180.0));
    if (key <= 0){
        // sprawdzam czy nie dodaje pola, ktore jest ujemne
        // innymi slowa nie dodaje rekordu mniejszego od 1
        cout << "nie mozna dodac rekordu mniejszego od 1" << endl;
        return;
        }

    int pointer;
    int page;
    int j;
    int i = 0;
    bool changeIndex = false;
    int k;
    int indexPoint;
    bool isOverflow = false;
    // buffer for data file

    // tworze dynamicznie alokowaną tablicę rekordów o rozmiarze PAGESIZE
    // każdy element tablicy jest obiektem typu Record
    Record* buffer = new Record[PAGESIZE];
    // Data file
    // rb - read binary

    /*otwieram plik FILEDATA w trybie binarnym i wskaźnik fileRead umożliwia czytanie danych binarnych z tego pliku.*/

    FILE* fileRead = fopen(FILEDATA, "rb");
    // Temporary file
    // wb - write binary

    /*otwieram plik tymczasowy (TEMPDATA) w trybie binarnych, wskaźnik fileWrite umożliwia zapis zmodyfikowanych danych do pliku tymczasowego.*/
    
    FILE* fileWrite = fopen(TEMPDATA, "wb");
    // look for page with key
    // page - okresla na ktorej stronie mam wpisac klucz, jesli dopisuje do overflow,
    // nadal wskazuje na te, z ktorej dopisuje
    page = indexSearch(key, read);
    
    // iteruje przez wszystkie strony, az do tej przed page
    // tu nie przechodze jesli stron jest mniej niz 1
    for (; i < page; i++){
        
        fread(buffer, RECORD_SIZE, PAGESIZE, fileRead);
        read++;
        fwrite(buffer, RECORD_SIZE, PAGESIZE, fileWrite);
        write++;
    }
    
    // laduje strone z ktorej chce zaladowac rekord
    fread(buffer, RECORD_SIZE, PAGESIZE, fileRead);
    read++;

    // sprawdzam czy jest wolne miejsce na stronie
    // Sprawdzam, czy ostatni rekord na stronie (indeks PAGESIZE - 1) ma klucz -1.
    // -1 wskazuje na pusty wpis. Jeśli warunek jest spełniony, oznacza to, że
    // na stronie jest wolne miejsce
    if (buffer[PAGESIZE - 1].key == -1){
        //jesli jest miejsce dodaje nowy rekord
        // przechodze przez wszystkie rekordy na stronie
        for (k=0; k < PAGESIZE; k++){
            if (buffer[k].key == key){
                cout << "Already exist" <<endl;
                fclose(fileRead);
                fclose(fileWrite);
                delete[] buffer;

                remove(TEMPDATA);
                return;
            }
            // jesli nie mam takiego rekordu to dodaje go
            // buffer[i].key == -1 - oznacza, ze jest to pusta strona
            else if (buffer[k].key == -1){
                // jesli jest to pierwszy rekord na stronie musze zaktualizowac indexy
                if (i == 0){
                    changeIndex = true;
                }
                buffer[k].key = key;
                buffer[k].side1 = side1;
                buffer[k].side2 = side2;
                buffer[k].angle = angle;
                // daje -1, bo nie ustawiam tego w overflow
                buffer[k].pointer = -1;
                globalVariable.recordsPrimaryArea++;

                sort(buffer,k);
    
                // zapisuje strone, ktora zostala zmieniona
                fwrite(buffer, RECORD_SIZE, PAGESIZE, fileWrite);
                write++;

                // przepisuje reszte z primary area
                // page + 1 -> bo przepisuje kolejna po tej ktora przetworzylem
                for (k = page + 1; k < globalVariable.maxNumberRecordsPrimary / PAGESIZE; k++){
                    fread(buffer, RECORD_SIZE, PAGESIZE, fileRead);
                    read++;
                    fwrite(buffer, RECORD_SIZE, PAGESIZE, fileWrite);
                    write++;
                }


                // przepisuje overflow
                fread(buffer, RECORD_SIZE, globalVariable.maxNumberRecordsOverflow,fileRead);
                read++;
                fwrite(buffer,RECORD_SIZE, globalVariable.maxNumberRecordsOverflow, fileWrite);
                write++;

                fclose(fileRead);
                fclose(fileWrite);

                delete[] buffer;

                remove(FILEDATA);
                rename(TEMPDATA, FILEDATA);

                break;
            }
        }
        // jesli zapisalem na nowa strone tu flaga ustawila mi sie na true,
        // wiec dodaje nowy index

        if (changeIndex != false){
            indexCreate(read, write);
        }
    }
    // brak wolnego miejsca, dlatego dodaje do overflow
    else {
        // przechodze przez wszystkie rekordy na stronie
        
        for (i = 0; i < PAGESIZE; i++){
            // sprawdzam czy ten klucz istnieje
            if (buffer[i].key == key){
                cout << "Already exist with key: "<< key << endl;

                fclose(fileRead);
                fclose(fileWrite);

                delete[] buffer;

                remove(TEMPDATA);
                return;
            }
            // jezeli znalazlem na stronie klucz, ktory jest wiekszy niz ten moj
            // oznacza to, ze musi byc tuz przed nim
            else if (buffer[i].key > key){
                //isOverflow = true;
                //cout << "USTAWIAM FLAGE" << endl;
                //isOverflow = true;
                // wychodze z fora
                break;
            }
            
        }
        //isOverflow = true;
        //cout << "DODAJE DO OVERFLOW!!!" << endl;
        // nowy rekord, ktory chce wstawic jest wiekszy niz juz te ktore sa
        // decrementuje i, aby wskazywało na rekord, ktory jest tuz przed tym, ktory chce wpisac
        i--;
        
        // przed zmniejszeniem i wskazywalo na rekord, ktory jest wiekszy od mojego ktory
        // chce wstawic. Zatem po zmniejszeniu bedzie wskazywal idealnie na ten, z ktorego
        // bede pointowal na overflow
        
        // to jest ten ktory wskazuje z PRIMARY/ albo ten, ktory wskazuje na
        // rekord w overflow

        pointer = buffer[i].pointer;
        //cout << "POINTER Z PRIMARY: " << pointer << endl;
        // ten warunek dziala
        /* int x =1; 
        if (isOverflow == true){
            if (buffer[i-x].pointer == -1){
                
                pointer = buffer[i].pointer;
            }
            while (buffer[i-x].pointer != -1){
                x--;
                
            }
            pointer = buffer[i-x+1].pointer;
            
            
        }*/
        
        // buffer[i].pointer == -1 - wskaznik dla danego rekordu jest pusty
        // buffer[i].pointer > key - sprawdzam, czy klucz nowo dodanego rekordu jest
        // mniejszy niz obecnie ustawiony wskaznik

        // innymi slowy -1 oznacza, ze nie ma kolejnego rekordu
        // buffer[i].pointer > key oznacza - jesli wskaźnik jest większy niż nowy klucz
        // oznacza to, że nowy rekord ma klucz, który powinien być umieszczony przed 
        // aktualnie wskazywanym rekordem

        // jesli ten moj pointer,ktory od teraz bedzie wskazywal na overflow na nic
        //  nie wskazuje to musze ustawic odpowiednio pointer jako klucz np. jak wstawiam
        // rekord o kluczu 9 do overflow to buffer[i]. pointer bedzie rowny 9.

        // musze natomiast rozaptrzec jeszcze jeden przypadek, gdy powiedzmy mam w primary
        // 1 2 3 10, w overflow 9, i chce wstawic 8. Pointer z 3 z primary wskazuje na 9
        // I jak chce wstawic 8 to sie okazuje ze ten pointer wskazuje na klucz wiekszy
        // nize ten ktory chce wstawic. A ja chce przeciez wstawic 8 przed 9, dlatego
        // pointer z 3 musialby wskazywac na 8
        
        
        // buffer[i].pointer - pointer z primary, wiec gdy wskazuje on na klucz
        // wiekszy i wstawiam mniejszy, to musze ustawic wskaznik z primary na ten
        // mniejszy (czyli na key) - to ten przypadek buffer[i].pointer > key
        if (buffer[i].pointer == -1 || buffer[i].pointer > key){
        
            // ustawiam wskaznik na nowo dodany rekord o kluczu key
            // np. ustawiam go w prypadku gdy dodalem 8 i sie okazalo, ze jest przed
            // 9. Zatem wskaznik bedzie rowny kluczowi, czyli 8
            buffer[i].pointer = key;
            
            //cout << "POINTER PO PRZYPISANIU: " << buffer[i].pointer << endl;
        }
        /*if (saved == -1 && savedNext > key){
            if (saved == 0){
                pointer =-1;
            }*/
            // ustawiam wskaznik na nowo dodany rekord o kluczu key
            // np. ustawiam go w prypadku gdy dodalem 8 i sie okazalo, ze jest przed
            // 9. Zatem wskaznik bedzie rowny kluczowi, czyli 8

            /*saved = savedNext;
            pointer = saved;
            
            //buffer[i].pointer = key;
            buffer[i].pointer = saved;
            buffer[i+1].pointer = savedNext;
        
        }*/
        // ide dalej jesli rekord, ktory chce wstawic jest wiekszy od tego ktory juz
        // jest w overflow

        // zapis strony, ktora zostala zmieniona
        fwrite(buffer, RECORD_SIZE, PAGESIZE, fileWrite);
        write++;

        // zapisuje pozostala czesc strony glownej
        // musze zaczac od kolejnej strony, dlatego page + 1

        for (k = page +1; k < globalVariable.maxNumberRecordsPrimary / PAGESIZE; k++){
            fread(buffer, RECORD_SIZE, PAGESIZE, fileRead);
            read++;
            fwrite(buffer, RECORD_SIZE, PAGESIZE, fileWrite);
            write++;
        } 
        delete[] buffer;

        // zapisuje overflow

        buffer = new Record[globalVariable.maxNumberRecordsOverflow];
        
        fread(buffer, RECORD_SIZE, globalVariable.maxNumberRecordsOverflow, fileRead);
        read++;

        // teraz wstawiam rekord do overflow, przechodzac przez maksymalna liczbe
        // rekordow w overflow
        for (j = 0; j < globalVariable.maxNumberRecordsOverflow; j++) {
            if (buffer[j].key == key) {
                // jeśli klucz już istnieje, nie dodajem go ponownie
                cout << "Already exist with key: " << key << " in overflow!" << endl;
                return;  
            }
            // jeśli znalazłem pierwsze wolne miejsce w overflow (key == -1) lub miejsce na mniejszy klucz

            // j - pozycja na ktora nalezy wstawic rekord
            
            if (buffer[j].key == -1 || key < buffer[j].key) {
                // przesuwam rekordy w prawo, aby zrobić miejsce dla nowego rekordu
                for (int k = globalVariable.maxNumberRecordsOverflow - 1; k > j; k--) {
                    // robie miejsce dla rekordu o pozycji j, przesuwajac wszystkie
                    // ktore sa od niego wieksze w gore w overflow (albo w prawo :)
                    // jak kto woli nazywac :)) )
                    buffer[k] = buffer[k - 1];
                }

                // wstawiam nowy rekord na właściwe miejsce
                buffer[j].key = key;
                buffer[j].side1 = side1;
                buffer[j].side2 = side2;
                buffer[j].angle = angle;

                // aktualizacja wskaźników
                if (j > 0) {
                    // poprzedni rekord wskazuje na nowy np. gdy w overflow mam 5 i 15
                    // i wstawiam 6 to 5 bedzie wskazywala na key (czyli na 6)
                    buffer[j - 1].pointer = key; 
                }

                // teraz musze ustawic wskaznik na ten kolejny
                // jesli warunek jest spelniony to wartosc po ? zostaje wybrana
                // jesli nie to : 

                // czyli jesli indeks na ktory wpisuje jest jeszcze mniejszy niz maks
                // liczba w overflow -1 (bo indeksuje od 0) i, ze kolejny
                // rekord w ogole istnieje (klucz rozny od -1)
                buffer[j].pointer = (j < globalVariable.maxNumberRecordsOverflow - 1 && buffer[j + 1].key != -1)
                                        // jesli warunek jest spelniony to wskaznik
                                        // jest rowny kolejnemu kluczowi
                                        ? buffer[j + 1].key
                                        // jesli nie to ustawiam wskaznik na -1
                                        : -1; // wskaźnik nowego rekordu na następny lub -1, jeśli brak
                // rekord został wstawiony, kończe pętlę
                break; 
            }
        }



        // zwiekszam liczbe rekordow w Overflow
        globalVariable.recordsOverflowArea++;
        fwrite(buffer, RECORD_SIZE, globalVariable.maxNumberRecordsOverflow, fileWrite);
        write++;

        fclose(fileRead);
        fclose(fileWrite);

        delete[] buffer;
        remove(FILEDATA);
        rename(TEMPDATA, FILEDATA);
    }
    cout << "ADDED NEW RECORD OF KEY: " << (int)(float)(side1 * side2 * sin(angle * M_PI / 180.0)) <<endl;
    globalVariable.totalReads += read;
    globalVariable.totalWrites += write;
    globalVariable.added++;
};


void indexCreate(int& read, int& write) {
    const int pages = globalVariable.maxNumberRecordsPrimary / PAGESIZE;

    // bufory dla rekordów i indeksów
    Record* bufferRecord = new Record[PAGESIZE];
    Index* bufferIndex = new Index[PAGESIZE];

    FILE* fileData = fopen(FILEDATA, "rb");
    FILE* fileIndex = fopen(FILEINDEX, "wb");

    // licznik rekordow zapisanych do bufferIndex
    int j = 0;

    // przechodze przez kazda strone
    for (int i = 0; i < pages; i++) {
        
        if (fread(bufferRecord, RECORD_SIZE, PAGESIZE, fileData) > 0) {
            read++;

            // po odczytaniu rekordu, funkcja przypisuje klucz pierwszego rekordu na
            // stronie (bufferRecord[0].key) do indeksu w INDEX, a także zapisuje numer strony 'i' w bufferIndex[j].page
            bufferIndex[j].key = bufferRecord[0].key;
            bufferIndex[j].page = i;
            // zwiekszam odczytane rekordy
            j++;

            // jeśli bufor indeksu jest pełny, zapisuje go do pliku i resetuje licznik
            if (j == PAGESIZE) {
                fwrite(bufferIndex, INDEX_SIZE, PAGESIZE, fileIndex);
                write++;
                j = 0;
            }
        }
    }

    // zapisuje pozoste rekordy, jeśli istnieją
    if (j > 0) {
        fwrite(bufferIndex, INDEX_SIZE, j, fileIndex);
        write++;
    }

    fclose(fileData);
    fclose(fileIndex);

    // Zwolniam pamięc
    delete[] bufferRecord;
    delete[] bufferIndex;
};

int indexSearch(int key, int& read) {
    
    Index* buff = new Index[PAGESIZE];

    // otwieram plik indeksu w trybie odczytu binarnego
    FILE* f = fopen(FILEINDEX, "rb");
    
    // wskaźnik pliku f zostaje ustawiony na koniec pliku
    fseek(f, 0, SEEK_END);
    // ftell(f) - zwraca aktualną pozycję wskaźnika pliku, która odpowiada 
    // całkowitemu rozmiarowi pliku w bajtach
    long fileSize = ftell(f);
    // rewind(f) - ustawia wskaźnik pliku z powrotem na początek, aby można
    // było odczytywać dane od początku
    rewind(f);

    // liczba rekordow/kluczy w index
    int totalRecords = fileSize / INDEX_SIZE;
    //cout << "LICZBA KLUCZY W INDEX: " << totalRecords << endl;

    // zaokrąglam do pełnych stron. Powiedzmy, ze mam 11 kluczy w INDEX, laduje
    // PAGESIZE stron, ja ustalilem PAGESIZE na 4, zatem mam (11+4-1)/4 = 3
    int pages = (totalRecords + PAGESIZE - 1) / PAGESIZE;
    //cout << "STRONY: " << pages<<endl;
    // na poczatku nie odnaleziono jeszcze strony
    int page = -1;
    // liczba odczytanych rekordów z pliku
    int records = 0; 

    // iteruje po stronach
    for (int i = 0; i < pages && page == -1; ++i) {
        // odczytuje dane dla bieżącej strony
        records = fread(buff, INDEX_SIZE, PAGESIZE, f);
        read++;

        //cout << "CZYTAM KOLEJNE ZALADOWANIE: " << i << ", odczytano rekordy: " << records << endl;

        // Iteracja po rekordach w bieżącym buforze
        // Dla każdego rekordu sprawdzane jest, czy jego klucz (buff[k].key) jest
        // większy od szukanego klucza (key). Jeśli tak, oznacza to, że rekord,
        // którego szukam, znajduje się na stronie, a wskaźnik page ustawiam 
        // na numer strony (buff[k].page -1), -1 bo indeksuje od 0
        for (int k = 0; k < records; ++k) {
            
            // sprawdzamm, czy klucz w rekordzie jest większy niż poszukiwany
            if (buff[k].key > key) {
                // Jeśli wskaźnik na Overflow już został ustawiony, pomijam dalsze operacje
                // jeśli tak to przypisuje numer strony
                // buff[k].page mimo tego, ze przetwarzam nowa strone (nowy odczyt )
                // nie bedzie iterowalny od zera, tylko przez caly czas sie zwieksza.
                // Dlatego tez page bedzie wskazywalo na indeks klucza w INDEX liczac
                // od 0 (bez zerowania po nowym odczycie, tak jak opisalem wyzej)
                // Z tego tez powodu aby dostac sie do odpowiedniego indeksu musze odjac 1
                page = buff[k].page - 1;
                //cout << "Znaleziono stronę o kluczu: " << buff[k].page << endl;
            
                //cout << "Znaleziono stronę: " << page << endl;
                break; // Przerywamy iterację po rekordach
            }
        }

        // jeśli strona została znaleziona, przerywam iterację po stronach
        if (page != -1){
            //cout << "ZNALEZIONO" << endl;
            break;
        } 
    }

    // jeśli nie znalazlem strony, i sa jeszcze jakies rekordy to oznacza to ze jest
    // to ostatnia strona (records - 1, bo indeksuje od 0)
    if (page == -1 && records > 0) {
        // ostatni rekord w buforze
        page = buff[records - 1].page;
        //cout << "Znaleziono ostatnią stronę: " << page << endl;
    }

    // zamykam plik
    fclose(f);
    delete[] buff;

    // zwracam numer strony lub -1, jeśli nie znaleziono
    return page;
};

// tworzenie Indexu na podstawie rekordu w primary area
void indexShow() {
    // licznik operacji odczytu
    int readCount = 0;
    // liczba odczytanych rekordów
    int recordsRead;

    // bufor na odczytane dane
    Index* buff = new Index[PAGESIZE];

    // otwieram plik w trybie binarnym
    FILE* f = fopen(FILEINDEX, "rb");

    if (!f) {
        cout << "Error opening file!" << endl;
        delete[] buff;
        // zakończenie funkcji, jeśli plik nie został otwarty
        return;
    }

    cout << "INDEX AREA" << endl;
    cout << "\n";

    // odczytuje dane z pliku do bufora
    while ((recordsRead = fread(buff, INDEX_SIZE, PAGESIZE, f)) > 0) {
        readCount++;

        // wyświetlam dane odczytane z pliku
        // iteruje po odczytanych rekordach
        for (int i = 0; i < recordsRead; i++) {
            cout << "Key: " << buff[i].key << " Page: " << buff[i].page << endl;
        }
    }
    // zamykam plik
    fclose(f);  
    // usuwam alokowany bufor
    delete[] buff;  

    // wyświetlam liczbę operacji odczytu
    cout << "\n";
    cout << "reads: " << readCount << endl;
    cout << "\n";
};


void fileShow() {
    // licznik operacji odczytu
    int readCount = 0;
    // liczba stron w PRIMARY AREA
    int pagesCount = globalVariable.maxNumberRecordsPrimary / PAGESIZE;
    int pointer;

    // bufor dla stron PRIMARY AREA
    Record* bufferPrimary = new Record[PAGESIZE];
    // bufor dla stron OVERFLOW AREA
    Record* bufferOverflow = new Record[globalVariable.maxNumberRecordsOverflow];

    FILE* f = fopen(FILEDATA, "rb");
    
    // przesuwam wskaznik na sekcje OVERFLOW AREA
    fseek(f, globalVariable.maxNumberRecordsPrimary * RECORD_SIZE, SEEK_SET);
    fread(bufferOverflow, RECORD_SIZE, globalVariable.maxNumberRecordsOverflow, f);
    readCount++;

    // przechodze do początku pliku
    fseek(f, 0, SEEK_SET);

    // wyświetlanie danych o PRIMARY AREA
    cout << "PRIMARY AREA" << endl;
    cout << "\n";
    for (int i = 0; i < pagesCount; i++) {
        cout << "\nPAGE " << i  << endl << "\n";
        fread(bufferPrimary, RECORD_SIZE, PAGESIZE, f);
        readCount++;

        // wyświetlam rekordy z PRIMARY AREA
        for (int j = 0; j < PAGESIZE; j++) {
            cout << bufferPrimary[j].key << " " 
                 << bufferPrimary[j].side1 << " "
                 << bufferPrimary[j].side2 << " "
                 << bufferPrimary[j].angle;
            pointer = bufferPrimary[j].pointer;

            if (pointer != -1) {
                cout << " pointer: " << pointer << endl;
            } else {
                cout << "\n";
            }
        }
    }

    // wyświetlam dane z Overflow Area
    cout << "\nOVERFLOW AREA" << endl << "\n";
    for (int k = 0; k < globalVariable.maxNumberRecordsOverflow; k++) {
        cout << bufferOverflow[k].key << " "
             << bufferOverflow[k].side1 << " "
             << bufferOverflow[k].side2 << " "
             << bufferOverflow[k].angle;

        pointer = bufferOverflow[k].pointer;
        if (pointer != -1) {
            cout << " pointer: " << pointer << "\n";
        } else {
            cout << "\n";
        }
    }
    // zamykam plik
    fclose(f);  

    // usuwam alokowane bufory
    delete[] bufferPrimary;
    delete[] bufferOverflow;
    cout << "reads: " << readCount << endl << "\n";
};

// ODCZYT REKORDU
// odczytanie rekordu na podstawie klucza
void find(int k){
    int pointer;
    int read = 0;
    // potrzebne do wskazania na ktorej stronie jest klucz. 
    // Strony sa numerowane od 0
    int pageNumber = 0;
    bool find = false;

    // szukam strony na odpowiedniej stronie w tablicy indeksow
    int p = indexSearch(k, read);
    
    // bufor na dane
    Record* buffer = new Record[PAGESIZE];
    // plik z ktorego bede czytal
    FILE* file = fopen(FILEDATA, "rb");

    // odczytuje p stron, dzieki temu szukam strony docelowej
    for (int i = 0; i < p; i++){
        //cout << "Odczytuje: "<< i << endl;
        fread(buffer, RECORD_SIZE, PAGESIZE, file);
        read++;
        pageNumber++;
    }
    // zaladowanie konkretnej strony
    fread(buffer, RECORD_SIZE, PAGESIZE, file);

    // iteruje po znalezionej stronie, czyli po tym wczytanym buforze
    for (int j = 0; j < PAGESIZE; j++){
        
        if (buffer[j].key == k){
            cout << "FOUND RECORD ON PAGE: " << pageNumber << " WITH KEY: " << buffer[j].key << " side1: " << buffer[j].side1 << " side2: " << buffer[j].side2 << " angle: " << buffer[j].angle << endl;
            find = true;
            break;
        }
    }

    // jezeli nie znalazlem w PRIMARY AREA musze przeskoczyc do OVERFLOW AREA
    if (find == false){
        // przesuwam wskaznik w file o liczbe rekordow w primary area pomnozona przez
        // ich rozmiar czy faktycznie wskaze na 1 element z overflow
        // SEEK_SET - zaczynam od poczatku
        fseek(file, globalVariable.maxNumberRecordsPrimary * RECORD_SIZE, SEEK_SET);

        // odczytuje czesc nadmiarowa do bufora
        fread(buffer, RECORD_SIZE, globalVariable.maxNumberRecordsOverflow,file);
        read++;

        for (int l = 0; l < globalVariable.maxNumberRecordsOverflow; l++){
            if (buffer[l].key == k){
                cout << "FOUND RECORD IN OVERFLOV AREA WITH KEY: " << buffer[l].key << " side1: " << buffer[l].side1 << " side2: " << buffer[l].side2 << " angle: " << buffer[l].angle << endl;
                find = true;
                break;
            }
        }
    }
    // jesli nawet w overflow area nie bylo
    if (find == false){
        cout <<"KEY: "<< k << " DOESN'T EXIST" << endl;
    }

    fclose(file);
    delete[] buffer;

    cout << "reads: " << read << endl;
};


void reorg(int& read, int& write){
    
    // liczba stron w primary
    //cout << "MAX LICZBA REKORDOW: " << globalVariable.maxNumberRecordsPrimary << endl;
    int pages = globalVariable.maxNumberRecordsPrimary / PAGESIZE;
    // licznik rekordow umieszczonych w bufferMove
    // Gdy records_counter osiąga wartość ALPHA * PAGESIZE, 
    // bufor bufferMove jest uznawany za pełny i zapisywany do pliku
    int records_counter = 0;
    // licznik stron zapisanych po reorganizacji
    int pages_after_reorg =0;
    // indeks do poruszania sie po overflow
    int idx_ov = 0;
    int pointer;
    // wspolczynnik wypelnienia
    int filled = int(ceil((ALPHA * PAGESIZE)));

    //overflow visited
    int visit = 1;
    //cout << "WSPOLCZYNNIK WYPELNIENIA: " << filled << endl;
    // bufor na rekordy z obszaru primary
    Record* bufferPrimary = new Record[PAGESIZE];
    // bufor na rekordy z obszaru overflow
    Record* bufferOverflow = new Record[globalVariable.maxNumberRecordsOverflow];
    // bufor na rekordy, ktore beda przenoszone do pliku po reorganizacji 
    Record* bufferMove = new Record[PAGESIZE];

    FILE* fRead = fopen(FILEDATA, "rb");
    FILE* fWrite = fopen(TEMPDATA, "wb");


    // przeskakuje do obszaru overflow
    fseek(fRead, globalVariable.maxNumberRecordsPrimary * RECORD_SIZE, SEEK_SET);
    // gdy juz tam bede laduje cala zawartosc do bufferOverflow
    fread(bufferOverflow, RECORD_SIZE, globalVariable.maxNumberRecordsOverflow, fRead);
    read++;

    // skacze na sam poczatek odczytanych danych, czyli do obszaru primary, aby go odczytac
    fseek(fRead, 0, SEEK_SET);

    // przechodze przez strony w primary area
    for (int i = 0; i < pages; i++){
        // wczytuje strony z obszaru primary area
        fread(bufferPrimary, RECORD_SIZE, PAGESIZE, fRead);
        read++;

        // przechodze przez rekordy na kazdej stronie
        for (int j = 0; j < PAGESIZE; j++){
            // jesli liczba rekordow zaladowanych do bufferMove osiagnie ilosc zapelnienia
            // ALPHA * PAGESIZE określa limit liczby rekordów, którymi bufor bufferMove powinien zostać wypełniony
            //cout << "alpha * PAGESIZE" << int(ceil((ALPHA * PAGESIZE))) <<endl;
            //cout << "LICZNIK REKORDOW: " << records_counter << endl;
            if (records_counter == filled){
                //cout << "Wypelnilem sie !" << endl;
                // ustawiam wskaznik na -1 dla wszystkich rekordow w bufferMove, dzieki czemu
                // wszystkie rekordy nie będą miały powiązanych wskaźników do rekordów w obszarze przepełnienia
                for (int l = 0; l < records_counter; l++){
                    bufferMove[l].pointer = -1;
                }
                // zapisuje z bufforMove do pliku tymczasowego
                fwrite(bufferMove, RECORD_SIZE, PAGESIZE, fWrite);
                write++;

                // usuwam zapisane rekordy z bufferMove, aby zapobiec duplikatom przy kolejnym wypełnianiu bufora
                removeFromBuffer(bufferMove, records_counter);
                pages_after_reorg++;
                records_counter = 0;
            }

            // ZALADOWANIE Z PRIMARY

            // zapis z primary area do buffor move
            // sprawdzam, czy klucz w PRIME AREA wogole istnieje
            if (bufferPrimary[j].key != -1){
                //cout << "Wypelniam Move !" << endl;
                // rekord z primary buffer jest zapisywany na odpowiednim miesjcu
                // w bufferMove
                //cout << "KLUCZE Z PRIMARY PRZY REO: "<< bufferPrimary[j].key << endl;
                //cout << "POINTER Z PRIMARY PRZY REO: "<< bufferPrimary[j].pointer << endl;
                bufferMove[records_counter] = bufferPrimary[j];
                //cout<<  "BUFOR MOVE: " << bufferMove[records_counter].key << endl;
                // ustawiam wskaznik na wartosc z aktualnego rekordu, wazne, bo
                // gdy jest rozny od -1 to musi przejsc do overflow (bo pointuje na overflow :)) ) !!!!!
                pointer = bufferPrimary[j].pointer;

                records_counter++;
                //cout << "POINTER: " << pointer << endl;
            }

            // ZALADOWANIE Z OVERFLOW

            // idx_ov - indeks do poruszania sie po overflow
            // jesli rekord z obszaru primary area wskazuje na strone z obszaru overflow
            // oraz jesli indeks po overflow jest mniejszy niz wogole ich liczba

            // jesli jest to moje pierwsze zpointowanie do overflowa to pobieram wszystkie
            // rekordy z overflowa
            // dlaczego akurat tak?
            // Otoz dlatego ze jak juz pobiore je wszystkie to, gdyby kolejny pointer, z tej
            // samej strony wskzywalby ponownie na overflow to zamiast rekordu wskazalby -1
            // i doprowadziloby to do tego, ze -1 byloby traktowane jako rekord
            // dlatego wchodze tylko raz do overflow i pobieram wszystkie rekordy i dzieje sie
            // to dzieki zmniejszeniu visit-- i juz nie bedzie 1.

            if (visit == 1){
                while(pointer != -1 && idx_ov < globalVariable.maxNumberRecordsOverflow) {
                    
                    // musze sprawdzic czy bufor jest wypelniony wg alpha (rowniez po wypelnieniu
                    // go przez rekordy z primary area)

                    // sprawdzenie czy bufor jest alpha wypelniony, zapis bufora do pliku oraz
                    // wyczyszczenie go
                    
                    if (records_counter == filled){
                        for (int x =0; x < records_counter; x++){
                            // dla kazdego rekordu ustawiam wskaznik na -1, gdy zapisuje
                            // rekordy z bufora do pliku, ponieważ chce usunąć wszelkie
                            // odniesienia do innych rekordów
                            bufferMove[x].pointer = -1;
                        }
                        // wszystkie rekordy, które zostały załadowane do bufora, są zapisywane do pliku
                        fwrite(bufferMove, RECORD_SIZE, PAGESIZE, fWrite);
                        write++;
                        // opróżniam bufor, by przygotować go na przyjęcie nowych rekordów
                        removeFromBuffer(bufferMove, records_counter);
                        pages_after_reorg++;
                        // ustawiam records_counter na 0, ponieważ bufor jest pusty i gotowy na
                        //  kolejne rekordy do załadowania
                        records_counter = 0;
                    }
                    //cout << "licznik rekordu: " << records_counter << endl;
                    // jesli bufor nie jest pelny kopiuje rekord z overflow do bufferMove
                    // index w bufforMove bedzie wskazywal records_counter
                    // a index w Overflow idx_ov
                    
                    bufferMove[records_counter] = bufferOverflow[idx_ov];
                    //cout << "REO OVERFLOW: " << bufferMove[records_counter].key << endl;
                    // pointer jest aktualizowany na wartość wskaźnika z bufferOverflow[idx_ov]
                    // musze przepisac po prostu wskaznik rekordu, ktorego przepisalem do
                    // bufora Move, zeby zachowac ten wskaznik, zeby sprawdzic
                    // czy cos tam jeszcze zostalo w tym overflow (bo kazdy rekord
                    // w overflow wskazuje na kolejnego)
                    pointer = bufferOverflow[idx_ov].pointer;
                    // idx_ov ++ przesuwa do następnego rekordu w bufferOverflow
                    idx_ov++;
                    // zwiekszam rekordy zaladowane do bufferMove
                    records_counter++;
                    // jak juz raz odwiedzilem overflow area to juz wiecej razy tam
                    // nie chce wchodzic dlatego visit nie bedzie juz rowne 1
                    visit--;
                }    
            }
        }
    }
    // jesli w bufferMove jeszcze cos pozostalo, to trzeba to zapisac na koncu
    if (records_counter > 0){
        // iteruje przez rekordy w bufforMove
        for (int y = 0; y < records_counter; y++){
            // ustawiam na -1, zeby rekordy nie mialy zadnych powiazan w obszarze przepelnienia
            bufferMove[y].pointer = -1;
        }
        // zapisuje cała zawartość bufferMove do pliku
        fwrite(bufferMove, RECORD_SIZE, PAGESIZE, fWrite);
        write++;

        pages_after_reorg++;
    }

    //cout << "PAGES AFTER REORG: " << pages_after_reorg << endl;
    // ustawiam maksymalną liczbę rekordów, które mogą być przechowywane w 
    // primary area, w zależności od liczby stron oraz rozmiaru strony

    // ustawiam tak, ze ta maksymalna to bedzie suma dwa razy wieksza od wszystkich
    // mozliwych rekordow
    // juz z tymi nowo dodanymi stronami 
    globalVariable.maxNumberRecordsPrimary = pages_after_reorg * PAGESIZE;
    //cout << "MAX REC IN PRIMARY: " << globalVariable.maxNumberRecordsPrimary << endl;


    // ogolnie tutaj jest idea mam globalVariable.maxNumberRecordsPrimary jako 
    // wielokrotnosci 4, bo taki rozmiar ustawilem na poczatku
    // zatem sprawdzam tutaj czy globalVariable.maxNumberRecordsPrimary / 2 
    // bedzie wieksze od 4
    // przyklad: mam globalVariable.maxNumberRecordsPrimary = 4 / 2 = 2. 
    // W takim przypadku gdy mam 8 (czyli mam 2 strony) ustawiam wartosci overflow 
    // na 2.

    // ZATEM OGOLNIE zakladam, ze dla maksymalnej liczby rekordow w primary wiekszych 
    // od 8 (w moim przypadku wiecej niz 2 strony w primary area) maksymalna 
    // liczba rekordow w overflow bedzie rowna 4, natomiast, gdy ta liczba jest
    // mniejsza to juz wtedy ustawiam wartosc maxOverflow jako liczbe max rekordow 
    // z primary podzielona na 2
    if ((globalVariable.maxNumberRecordsPrimary / 2) > 4){
        globalVariable.maxNumberRecordsOverflow = 4;
        //cout << "MAX REC OVERFLOW: " << globalVariable.maxNumberRecordsOverflow << endl;
    }
    else {
        globalVariable.maxNumberRecordsOverflow = globalVariable.maxNumberRecordsPrimary / 2;
        //cout << "MAX REC OVERFLOW: " << globalVariable.maxNumberRecordsOverflow << endl;
    }

    // dodaje do ilosci rekordow primary area ilosc rekordow z overflow po reorganizacji
    globalVariable.recordsPrimaryArea += globalVariable.recordsOverflowArea;
    //cout << "PRIMARY AREA: " << globalVariable.recordsPrimaryArea<< endl;
    globalVariable.recordsOverflowArea = 0;

    // zapisuje wyczyszczony bufor overflow

    removeFromBuffer(bufferMove, globalVariable.maxNumberRecordsOverflow);
    fwrite(bufferMove, RECORD_SIZE, globalVariable.maxNumberRecordsOverflow,fWrite);
    write++;

    fclose(fRead);
    fclose(fWrite);

    delete[] bufferPrimary;
    delete[] bufferOverflow;
    delete[] bufferMove;
    
    // otwieram plik do odczytu
    FILE* fReadTemp = fopen(TEMPDATA, "rb");
    if (fReadTemp == nullptr) {
        cerr << "Nie można otworzyć pliku TEMPDATA do odczytu!" << endl;
        return;
    }

    // wczytuje cały plik do pamięci

    //przesuwam wskaźnik na koniec pliku
    fseek(fReadTemp, 0, SEEK_END);
    // rozmiar pliku
    size_t fileSize = ftell(fReadTemp); 
    // wracam na początku pliku
    fseek(fReadTemp, 0, SEEK_SET);

    // obliczam liczbę rekordów
    size_t recordCount = fileSize / RECORD_SIZE;

    // dynamiczna tablica przechowująca obiekty typu Record
    // wektor przechowuje readCount elementów typu Record
    vector<Record> records(recordCount);
    
    // wczytujee wszystkie rekordy do pamięci

    // &records[0] - wskaźnik na pierwszy element wektor, wskazuje na miejsce, w 
    // które fread ma zapisać dane z pliku

    // fread ma odczytać recordCount rekordow
    // &records[0] - oznacza wskaźnik na pierwszy element wektora records
    // size_t - typ bez znaku (ang. unsigned),przechowuje tylko liczby nieujemne

    // &records[0] wskazuje na początek bloku pamięci, w którym znajdują się elementy
    // wektora, dzięki czemu fread może wypełnić kolejne miejsca w pamięci
    // odczytanymi danymi

    // podsumowujac:
    // odczyta recordCount rekordow o rozmiarze RECORD_SIZE z fReadTemp i 
    // dane są zapisywane w pamięci, zaczynając od adresu &records[0]
    size_t readRecords = fread(&records[0], RECORD_SIZE, recordCount, fReadTemp);
    read++;
    // jesli zwrócona liczbę odczytanych rekordów przez fread jest rozna od readCont
    if (readRecords != recordCount) {
        cerr << "PROBLEM WITH READ TEMPDATA" << endl;
        fclose(fReadTemp);
        return;
    }
    fclose(fReadTemp);
    
    
    // tworze pusty wektor sortableRecords, który będzie przechowywał rekordy,
    // które bede mogl posortowac
    vector<Record> sortableRecords;
    
    // iteruje przez wszystkie rekordy z TEMPFILE
    // jesli klucz jest rozny od -1 (czyli ma klucz) to dodaje go do sortableRecords
    for (size_t i = 0; i < records.size(); ++i) {
        if (records[i].key != -1) {
            sortableRecords.push_back(records[i]);
        }
    }
    
    // sortuje babelkowo rekordy
    // zewnętrzna pętla wykonuje tyle iteracji, ile jest elementów w wektorze,
    // wewnętrzna pętla wykonuje odpowiednią liczbę porównań w zależności od tego,
    // ile elementów jeszcze nie zostało posortowanych.
    // iteruje przez wszystkie elementy wektora
    for (size_t j = 0; j < sortableRecords.size(); ++j) {
        // zakres tej pętli jest o jeden element krótszy. Dzięki temu ostatni element 
        // jest umieszczony na końcu wektora, a więc nie musi być już porównywany w kolejnych iteracjac
        for (size_t i = 0; i < sortableRecords.size() - 1 - j; ++i) {
            // jesli rekord poprzedni jest wiekszy niz kolejny to znaczy, ze nie sa posortowane
            if (sortableRecords[i].key > sortableRecords[i + 1].key) {
                // temp przechowuje tymczasowo wartość elementu sortableRecords[i]
                Record temp = sortableRecords[i];
                // tutaj zamieniam kolejny z poprzednim (ktory przechowuje w temp)
                sortableRecords[i] = sortableRecords[i + 1];
                sortableRecords[i + 1] = temp;
            }
        }
    }

    // wstawiam posortowane rekordy z powrotem do głównego wektora
    // sortIndex używam do śledzenia indeksu elementu w wektorze sortableRecords
    size_t sortIndex = 0;
    // tu records.size()-1?? juz raczej nie bo to zmienialem jak chcialem usunac ten rekord co mi nie pasowal
    for (size_t i = 0; i < records.size(); ++i) {
        // jesli klucz jest rozny od -1, to nadpisuje go wartoscia z posortowanych
        if (records[i].key != -1) {
            // rekord w records[i] zostaje nadpisany wartością z 
            // wektora sortableRecords na pozycji sortIndex
            records[i] = sortableRecords[sortIndex++];
        }
    }
   
    //sortableRecords.insert(sortableRecords.end(), ignoredRecords.begin(), ignoredRecords.end());
    /*cout << "Rekordy po posortowaniu (wraz z key = -1):\n";
    for (size_t i = 0; i < records.size(); ++i) {
        cout << "Key = " << records[i].key << endl;
    }*/

    // otwieram plik do zapisu
    FILE* fWriteTemp = fopen(TEMPDATA, "wb");
    if (fWriteTemp == nullptr) {
        cerr << "PROBLEM WITH WRITE TEMPDATA" << endl;
        return;
    }

    // zapisuje wszystkie rekordy w jednym zapisie
    size_t writtenRecords = fwrite(&records[0], RECORD_SIZE, records.size(), fWriteTemp);
    write++;
    if (writtenRecords != records.size()) {
        cerr << "PROBLEM WITH WRITE TEMPDATA" << endl;
    }
    fclose(fWriteTemp);

    remove(FILEDATA);
    rename(TEMPDATA, FILEDATA);
    // bardzo wazne !!
    // wyowluje to z read i write, aby podczas dzialania funkcji read i write uległy zmianie
    indexCreate(read, write);
};


void removeFromBuffer(Record* buffer, int records_counter){
    for(int i = 0; i < records_counter; i++){
        buffer[i].key = -1;
        buffer[i].side1 = 0;
        buffer[i].side2 = 0;
        buffer[i].angle = 0;
        buffer[i].pointer = -1;
    }
};
//void removeFromBuffer(Record* buffer, int records_counter) {
//    for (int i = 0; i < records_counter; i++) {
//       // Resetujemy poszczególne składowe struktury do odpowiednich wartości
//        buffer[i] = { -1, 0, 0, 0, -1 };
//    }
//}

void sort(Record buff[], int size) {
    // tworzę zmienną tymczasową do przechowywania wartości podczas zamiany
    Record tmp;
    // przechodzę przez tablicę od końca w kierunku początku.
    for (int j = size; j >= 0; j--) {
        // iteruję po elementach od początku do aktualnej pozycji `size - j - 1`.
        for (int i = 0; i < j; i++) {
            // porównuję klucze sąsiednich elementów
            if (buff[i].key > buff[i + 1].key) {
                // zapisuję bieżący element do zmiennej tymczasowej
                tmp = buff[i];
                // przypisuję następny element do bieżącego
                buff[i] = buff[i + 1];
                // zmienną tymczasową zapisuję w miejscu następnego elementu
                buff[i + 1] = tmp;
            }
        }
    }
};