Zadania - zestaw 2
Zapoznaj się ze składnią i operacjami wykonywanymi przez poniższe funkcje:

funkcje operujące na plikach i katalogach: open, close, read, write, fcntl, stat, fstat, lstat, mkdir, rmdir, opendir, closedir, readdir, rewinddir, nftw, fopen, fclose, getc, putc,
funkcje i zmienne do obsługi błędów: perror, errno.
Zadanie 1. Porównanie wydajności systemowych i bibliotecznych funkcji we/wy
Celem zadania jest napisanie programu porównującego wydajność systemowych i bibliotecznych funkcji wejścia/wyjścia. Program operował będzie na przechowywanej w pliku tablicy rekordów. Pojedynczy rekord będzie tablicą bajtów o stałej wielkości. Nazwa pliku, wielkość oraz ilość rekordów stanowić będą argumenty wywołania programu.

Program udostępniać powinien 3 operacje:

generate - tworzenie plik z rekordami wypełnionego losową zawartością wygenerowaną poprzez odczyt pliku /dev/random
shuffle - losowo permutuje rekordy w pliku (można wykorzystać np. algorytm Fishera-Yates'a)
sort - sortuje rekordy w pliku używając sortowania bąbelkowego. Kluczem do sortowania niech będzie wartość pierwszego bajtu rekordu (interpretowanego jako liczba bez znaku - unsigned char)
Sortowanie i permutacja powinny być zaimplementowane w dwóch wariantach:

sys - przy użyciu funkcji systemowych: read i write
lib - przy użyciu funkcji biblioteki C: fread i fwrite
(do generowania danych wystarczy jedna wersja).

Rodzaj operacji oraz sposób dostępu do plików ma być wybierany na podstawie argumentu wywołania - np. ./program sys shuffle datafile 100 512 powinno losowo permutować dane w pliku datafile przy użyciu funkcji systemowych, zakładając że zawiera on 100 rekordów wielkości 512 bajtów.

Program powinien być zaimplementowany tak, by podczas permutowania i sortowania w pamięci przechowywane były jednocześnie najwyżej dwa rekordy (obie operacje sprowadzają się do zamieniania miejscami i/lub porównywania dwóch rekordów).

Dla obu wariantów implementacji przeprowadź pomiary czasu użytkownika i czasu systemowego operacji sortowania i permutowania. Testy wykonaj dla następujących rozmiarów rekordu: 4, 512, 4096 i 8192 bajty. Dla każdego rozmiaru rekordu wykonaj dwa testy różniące się liczbą rekordów w sortowanym pliku. Liczby rekordów dobierz tak, by czas sortowania mieścił się w przedziale od kilku do kilkudziesięciu sekund. Porównując dwa warianty implementacji należy korzystać z identycznego pliku do sortowania (po wygenerowaniu, a przed sortowaniem, utwórz jego kopię). Zmierzone czasy sortowania zestaw w pliku wyniki.txt. Do pliku dodaj komentarz podsumowujący wnioski z testów.

Zadanie 2. Operacje na strukturze katalogów
Napisz program przyjmujący dwa argumenty wywołania: ścieżkę do katalogu w systemie plików, oraz liczbę całkowitą oznaczającą rozmiar pliku w bajtach. Po uruchomieniu program przeszukuje drzewo katalogów zakorzenione w ścieżce przekazanej w pierwszym argumencie i szuka w nim plików zwyczajnych o rozmiarze nie większym niż wielkość podana jako drugi argument. Dla każdego znalezionego pliku wypisywane są następujące informacje:

ścieżka bezwzględna pliku
rozmiar w bajtach
prawa dostępu do pliku (w formacie używanym przez ls -l, np. r--r--rw- oznacza prawa odczytu dla wszystkich i zapisu dla właściciela)
datę ostatniej modyfikacji
Ścieżka podana jako argument wywołania może być względna lub bezwzględna. Program powinien wyszukiwać tylko pliki zwyczajne (bez dowiązań, urządzeń blokowych, itp). Program nie powinien podążać za dowiązaniami symbolicznymi do katalogów.

Program należy zaimplementować w dwóch wariantach:

Korzystając z funkcji opendir, readdir oraz funkcji z rodziny stat
Korzystając z funkcji nftw
Zadanie 3. Ryglowanie plików
Napisz program umożliwiający w trybie interaktywnym (tekstowym) wykonanie następujących operacji dla pliku będącego jego argumentem:

ustawienie rygla do odczytu na wybrany znak pliku,
ustawienie rygla do zapisu na wybrany znak pliku,
wyświetlenie listy zaryglowanych znaków pliku (z informacją o numerze PID procesu będącego właścicielem rygla oraz jego typie - odczyt/zapis),
zwolnienie wybranego rygla,
odczyt (funkcją read) wybranego znaku pliku,
zmiana (funkcją write) wybranego znaku pliku.
Operacje 1 oraz 2 powinny być dostępne w dwóch wariantach, różniących się zachowaniem w przypadku, gdy ustawienie rygla jest niemożliwe ze względu na rygle założone przez inne procesy:

wersja nieblokująca - próba ustawienia rygla kończy się wypisaniem informacji o błędzie
wersja blokująca - program czeka, aż ustawienie rygla będzie możliwe
Wybór znaku we wszystkich przypadkach polega na podaniu numeru bajtu w pliku. Do ryglowania należy wykorzystać funkcję fcntl i strukturę flock. Wykonaj eksperymenty uruchamiając program jednocześnie w dwóch terminalach dla tego samego pliku. Próbuj ryglować (do odczytu lub do zapisu) te same znaki pliku i modyfikować (lub odczytywać) wybrane znaki pliku wykorzystując zarówno blokujące, jak i nieblokujące wersje operacji. Zwróć uwagę, że domyślnie ryglowanie działa w trybie rygli zalecanych (advisory), a nie rygli wymuszanych (mandatory).
