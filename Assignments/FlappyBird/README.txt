========README Tema1 EGC=======

Functionalitatea temei porneste de la laboratorul 3.
S-au folosit clase separate pentru desenarea obiectelor, transformarile 2D
si pentru implementare propriu-zisa.

Namespace-ul Object are definite functii pentru desenarea diverselor forme geometrice
necesare implementarii.

Namespace-ul Transform2D retine functii pentru scalare, rotatie si translatie.

Clasa Tema1 inlcude functionalitatile principale are jocului.
Pasarea este creata din 4 forme geometrice: un cerc, doua triunghiuri si inca un cerc pentru ochi.

In functia Init setam camera ca fiind false. Si initializam valorile de care avem nevoie. Cele mai
importante sunt variabila isJumping si vectorul pipes(care retine pozitia relativ la rezolutia ecranului
a fiecarei perechi de tevi). In afara de aceasta, in functie se creaza meshele pentru diversele obiecte.
Din considerente de memorie, pentru generarea tevilor cream un singur obiect pe care il translatam si 
il randam in diferite pozitii din scena.

In functia FrameStart setam culoarea la negru si golim ecranul. De aseamenea, se initializeaza viewportul.

Se verifica daca tasta SPACE a fost apasata si se modifica valoarea isJumping la true. Daca tasta a fost 
eliberata, se reseteaza valoarea la false.

In functia Update se creaza functionalitatea de gameplay.
Se deseneaza pasarea aplicand transformari 2D asupra cercurilor si triunghiurilor. 
Se folosesc doua cadre pentru a desena aripa pasarii, tocmai pentru a crea iluzia miscarii aripii.
Se foloseste o variabila Gravity pentru a simula atractia gravitationala.

Fizica pasarii:
	- viteza pasarii este acceleratia pasarii ori deltaTime;
	- pozitia pasarii scade cu viteza * deltaTime;
	- acceleratia pasarii creste proportional cu gravitatia, daca tasta SPACE nu este apasata.
	  in cazul in care tasta este apasata, setam acceleratia la 0 si modificam viteza pasarii pentru a
	  propulsa pasarea in sus cu o forta egala cu o fractiune din gravitatie.

Pentru a desena tevile, vom incepe din dreapta ecranului pentru a ne asigura ca prima pereche care paraseste ecranul
va fi prima care intra, pentru a crea iluzia unei lumi infinite. Vom retine mereu coordonatele colturilor celei mai 
apropiate perechi de tevi, pentru a testa ulterior coliziunile. Se creaza 5 perechi de tevi pentru a spori dificultatea
parcurgerii jocului.
In momentul cand pasarea trece de o pereche de tevi, se incrementeaza scorul jucatorului. Pentru a crea iluzia miscarii
obstacolelor, se translateaza toata scena periodic spre stanga cu o constanta.

Pentru a verifica daca avem coliziune, calculam coordonatele x si y ale centrului pasarii(pentru simplitate, aproximam
forma pasarii la un cerc). Trebuie sa vedem daca lovim obstacolele din lateral, sau daca suntem in curs de a parcurge 
o pereche de obstacole si o lovim cu partea de sus sau de jos a pasarii.

Improvement ideea: generarea perechilor de tevi la inaltimi random.
