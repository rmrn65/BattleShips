***IMPORTANT !***
**PENTRU A PUTEA FI JUCAT, CONSOLA TREBUIE SA FIE FULLSCREEN (sau minim
125x41)**
___________________________________________________________________________

***FEREASTRA PRINCIPALA***
- Fereastra principala e compusa din un asciiart care reprezinta numele 
jocului, marginita de # si * pentru design.
- Ascii-Art-ul a fost salvat intr-un fisier separat, din care s-a citit linie 
cu linie si s-a printat deasupra ferestrei principale a jocului.
- Atat fereastra principala este proiectata sa apare mereu in centrul consolei.
- Am facut asta folosind parametrii LINS si COLS ale consolei care reprezinta
numarul de linii si de coloane din care e formata (minim 125 de coloane si 41
de linii pentru a arata estetic)

***MENIUL CU OPTIUNI***
- Optiunile sunt pozitionate intr-o fereastra pe coloana. Navigarea printre 
optiuni e evidentiata cu hilight folosind functia attron(REVERSE) la fiecare
apasare a cheilor KEY_UP si KEY_DOWN a tastaturii.
- Daca este aleasa optiunea **New Game** se vor initializa toti parametrii 
structurilor si fereastra necesara desfasurarii jocului.
- Daca este aleasa optiunea **Resume Game**, se vor pastra parametrii 
structurilor si se va copia fereastra cu functiile scr_dump(savefile) 
scr_restore(savefile), unde **savefile** este fisierul in care se retina starea
vizuala a jocului in fiecare etapa. Resume Game devine valabil doar daca 
anterior s-a apasat pe New Game
- Daca este aleasa optiunea **Quit** ies complet din joc

***FEREASTRA JOCULUI***
- Fereastra jocului e compusa din tabla jucatorului pe care o printeaza in 
partea stanga.Tabla jucatorului poate fi editata din fisierul **my_board.txt**,
iar in dreapta o tabla goala unde urmeaza ca jucatorul sa ghiceasca pozitia
navelor inamicului. Tabla inamicului se afla desenata in fisierul 
**enemy_board.txt**.
- In partea de stanga jos sunt afisate posibilele actiuni care pot fi facute
pe parcusrsul jocului: * **Q** care e folosit pentru a iesi din joc
					   * **R** pentru randomizarea tablei de joc
					   * **D** pentru a distruge cate 10 de fiecare parte

***PROIECTAREA JOCULUI***
- Ideea principala este in proiectarea celor doua structuri *SHIPS* si *PLAYER*
- Structura SHIPS se refera la nevele pozitionate pe tabla
- Ships este formata din : len -> lungimea navelor
						   startx -> coordonata x a pivotului navei
						   starty -> coordonata y a pivotului navei
						   direction -> daca e orizontala sau verticala
						   damaged -> de cate ori a fost lovta
- Navele au un punct de referinta - un punct pivot - de acolo incepe 
dezvoltarea lor. Este aleasa o orientare si in functie de lungimea lor
se adauga cate o bucata de nava de la punctul pivot in directia corespunzatoare
La generarea acestora se actualizeaza o matrice numita "interzis" care decide
care casute pot fi acceesate si care nu.
- Player este format din : ships -> navele lor
						   board -> tabla lor sub forma unei matrice de 0 si 1
						   visited -> ce casute au incercat pe tabla inamica
						   hit -> de cate ori au lovit o nava
						   count_destroyed -> cate nave inamice au distrus
- Jucatorii incearca o coordonata de tip y x de pe fiecare tabla. Daca celula 
corespunzatoare nu a fost vizitata (adica visited[y][x] == 0) atunci se 
actualizeaza tabla. Daca pe "board"-ul inamic se afla "1" in locul atacat,
se considera ca a fost lovita o nava, si creste atat "hit", cat si atributul
"damaged" din cadrul structurii navei inamice  
**NOTATII**
- Se noteaza cu "-" daca a fost aleasa o celula fara nava
- Se noteaza cu "X" daca jucatorul a nimerit o bucata de nava inamica
- Se noteaza cu "#" daca o nava proprie a fost lovita
