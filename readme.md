# Hiper Strategia jeszcze bez tytułu
## Opis
Projekt autorskiej gry strategicznej zbudowanej w języku C++ z wykorzystaniem biblioteki SFML. Silnik opiera się na wydajnej architekturze Data-Oriented Design (DOD) i Object-Oriented Programming (OOP), oferuje proceduralną generację świata przy użyciu diagramów Voronoia oraz zaawansowanych algorytmów szumu.
## Jak włączyć?
Projekt wykorzystuje system budowania CMake. Do uruchomienia potrzebujesz kompilatora wspierającego standard C++17 oraz zainstalowanej biblioteki SFML.
<ol>
    <li>Otwórz terminal w głównym folderze projektu
    <li>Wygeneruj konfigurację projektu komendą: * cmake -S . -B build *
    <li>Skompiluj projekt za pomocą komendy: * cmake --build build *
    <li>Uruchom wygenerowany plik wykonywalny z folderu build (na przykład build\Debug\strategia.exe).
</ol>

## Sterowanie

<ul>
    <li> LPM - Wybranie pola lub jednostki
    <li> PPM - Kiedy wybrano jednostke (widać to w panelu po prawo) wydaje rozkaz ruchu na kliknięte pole
    <li> Scroll - Oddalanie/przybliżanie
    <li> Scroll wciśnięty - Przesuwanie po mapie
</ul>

