MAN ANDREI VLAD 336CA -- TEMA 3

1. Detalii implementare
- Am folosit doar functioi blocante din MPI pentru a scapa de alte elemente de sincronizare
- Am implementat pentru 1 si 3 in acelasi timp ca sa nu muncesc in plus (avand si aceeasi functionalitate in final)

2. Topologia
- Incepand de la 0 si prin 3 2 1, fiecare sef completeaza intr-o matrice in dreptul lui ce workeri au iar apoi o da mai departe.
- 1 completeaza finalul si o trimite si workerilor, iar apoi o trimite la 2, 2 o trimite la workeri si la 3 si asa mai departe pana la 0
- fiecare nod are matricea finala si raspunsul e construit printr-o functie.

3. Vectorul
- Initial se trimite catre sefi numarul N alaturi de un index de inceput
- Fiecare nod stie sa isi ia singur (in fct de numarul de copii / total) un nr de indexi asignat egal.
- Fiecare sef trimite workerilor numarul N si un index de inceput.
- Workerii prelucreaza sirul, iar apoi il trimit catre sefi.
- Sefii vor primi A vectori de la workeri, pe care ii vor redirectiona catre urmatorul sef.
- Sefii vor primi si B vectori de la alti sefi, pe care iar ii vor redirectiona catre urmatorul sef (se acumuleaza, pana la 0).
- Seful 0 uneste vectorii si il afiseaza.