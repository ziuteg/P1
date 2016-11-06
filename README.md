# Rozszerzony problem śpiącego golibrody (Hiltzer)

Łukasz Siudek

## Opis problemu

Klasyczny problem śpiącego golibrody opiera się na hipotetycznym gabinecie z jednym golibrodą. Golibroda w swoim gabinecie ma jedno krzesło, a w poczekalni obok znajduje się pewna liczba krzeseł dla klientów. Gdy golibroda kończy golić klienta pozwala mu wyjść, po czym sprawdza, czy w poczekalni oczekują na niego klienci. Jeśli tak, wybiera klienta z kolejki do obsłużenia. W przeciwnym przypadku wraca do gabinetu i zasypia na krześle.

Każdy klient po wejściu sprawdza, co robi golibroda. Jeśli śpi, klient budzi go i siada na krześle. Jeśli golibroda jest zajęty goleniem, to klient udaje się do poczekalni. Gdy znajduje puste krzesło, siada i czeka na swoją kolej. Jeśli nie ma wolnego krzesła, to klient opuszcza lokal.

Problem, jaki może się pojawić w naiwnej implementacji to przypadek, gdy klient zauważa, że golibroda jest zajęty. Udaje się do poczekalni, a w tym samym czasie golibroda kończy wykonanie swojej pracy i sprawdza poczekalnię, która jest jeszcze pusta, gdyż nowy klient jeszcze nie zajął tam miejsca. W tej sytuacji golibroda udaje się spać i nie obudzi się, dopóki nie pojawi się nowy klient, mimo, że jeden cały czas oczekuje na niego w kolejce.

## Wersja roszerzona

W wersji rozszerzonej gabinet składa się z trzech krzeseł, które obsługuje trzech golibrodów. W poczekalni znajduje jedynie się czteroosobowa kanapa oraz miejsca stojące, przy czym poczekalnia może pomieścić łącznie nie więcej niż 20 miejsc.

Klient po wejściu do poczekalni siada na kanapie, o ile jest miejsce. W przeciwnym przypadku zajmuje miejsce stojące. Gdy pewien golibroda jest wolny, wybiera klienta który najdłużej siedział na kanapie. Jego miejsce zajmuje klient, który najdłużej zajmował miejsce stojące. Gdy golibroda kończy golenie, zwraca się do klienta po zapłatę. Ponieważ jest tylko jedna kasa, opłata jest pobierana tylko od jednego klienta na raz.

Dodatkowy problem, jaki pojawia się w rozszerzonej wersji zadania, to m.in. wymagana implementacja systemu kolejkowania FIFO (First In, First Out), ponieważ klienci muszą zostać obsłużeni w odpowiedniej kolejności.