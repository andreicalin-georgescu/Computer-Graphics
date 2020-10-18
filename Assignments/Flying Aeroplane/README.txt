Tema 2 - Flying Aeroplane

Tema implementeaza un simulator de zbor functional.
Scena este creata dintr-o mare rotativa, avionul care se poate deplasa pe axa Oy
si fundal. In continuare vom descrie functionalitatiele si implementarile elementelor
individuale.

Marea este creata prin desenarea cinci cercuri din triunghiuri si unirea acestora, iar punctele critice sunt
modelate folosind Vertex Shader. Se permite controlul asupra deformarii acesteia prin modificarea
numarului de puncte folosite pentru desenarea cercurilor.

Avionul este realizat cu un singur Model Matrix asupra careia au fost aplicate diverse transformari 3D pentru 
a o putea randa in diferite pozitii. Folosirea unei singure matrice conduce la miscarea facila si unitara a avionului.
Exceptie face corpul avionului care a fost desenat folosind triunghiuri pentru un control mai fin a grosimii acestuia.
Astfel se poate obtine un efect de deformare selectiva.

Fundalul este alcatuit din nori care se invart in jurul marii. Norii sunt desenati din cate trei cuburi care se rotesc 
independent in jurul propriei axe Ox. Ei sunt randati la diferite raze fata de centrul marii, pentru a da impresia de 
generare aleatoare.

Exista si interfata cu utilizatorul. Acesta poate alege intre mod first - person sau third - person. 
Sunt disponibile trei vieti. Sunt prezente obstacole si combustibil care poate fi colectat pe parcursul jocului.
La ciocnirea cu un obstacol se va pierde o viata.
In timp, combustibilul curent al avionului scade treptat. Daca acesta ajunge la zero sau jucatorul ramane fara vieti,
avionul se va prabusi printr-o animatie iar jocul se va incheia la scurt timp dupa aceasta.
