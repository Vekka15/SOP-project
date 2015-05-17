#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX 64
#define MAXU 50
#define MAXS 50
#define MAXK 1
#define PUSTY 1
#define PELNY 2

static struct sembuf buf;

//STRUKTURY

typedef struct uzytkownik{
char nick[7];
int poziom;
int kolorPionkow;
int pid;
int jest;
}uzytkownik;

typedef struct serwer{
int kluczServ;
int czyZajete;
int jest;
}serwer;

typedef struct informacja{
int kluczServer;
int pidWybranego;
}informacja;

typedef struct mesg{
long mtype;
informacja dlaWybranego;
}mesg;

//FUNCKJE DO SEMAFOROW

void podnies(int semid , int semnum){
buf.sem_num = semnum;
buf.sem_op = 1;
buf.sem_flg = 0;
if (semop(semid , &buf, 1) == -1){
perror("Podnoszenie semafora");
exit(1);
}
}

void opusc(int semid , int semnum){
buf.sem_num = semnum;
buf.sem_op = -1;
buf.sem_flg = 0;
if (semop(semid , &buf, 1) == -1){
perror("Opuszczenie semafora");
exit(1);
}
}

//GLOWNA CZESC PROGRAMU

int main(){
int shmidS, semidG, shmidG, shmid, msgid, i=0;
uzytkownik *buf,  *bufZacz, *bufOdN;
serwer *bufS, *bufZaczS, *bufOdczS;
uzytkownik tabUzytkownikow[10];
serwer tabSerwerow[10];
informacja message;
int liczbaUzytkownikow=0;
char nazwaUz[7];
int poziomUz;
int kolorUz;
int pidUz;
int wyborTryb;
int wybranyPrzeciwnik;
int klucz;
//ZMIENNE SZACHOW
char * bufOdcz, *bufZap , *bufG ;
int liczWiersze,liczKolumny;
int wierszPionkaPrzed,kolumnaPionkaPrzed;
int wierszPionkaPo,kolumnaPionkaPo;
char tabSzachy[8][8];
int liczbaPionB=12;
int liczbaPionC=12;
int trybPracy;
int ktoraGra;

printf("Chcesz grac czy obserwowac gre: \n");
printf("0 - gracz \n1 - obserwator\n");
scanf("%d", &trybPracy);

if(trybPracy==0){

//TUTAJ BYL PROBLEM Z WYPISYWANIEM NICKU GDY CHAR MA WIECEJ ELEMENTOW I UZYWA SIE %S
printf("Podaj nick: ");
scanf(" %s", nazwaUz);
printf("%s", nazwaUz);
printf("Podaj poziom: \n");
printf("0 - podstawowy \n1 - zaawansowany\n");
scanf(" %d", &poziomUz);
printf("Podaj kolor: \n");
printf("0 - bialy \n1 - czarny\n");
scanf(" %d", &kolorUz);

pidUz=getpid();

printf("Czy chcesz wybrac przeciwnika czy oczekiwac na wybranie: ");
scanf("%d",&wyborTryb);

//KOLEJKA KOMUNIKATOW 

struct mesg elem;

msgid = msgget(9000, IPC_CREAT|0600);
	if (msgid == -1){
		msgid = msgget(45281, IPC_CREAT |0600);
		if (msgid == -1){
			perror("Utworzenie kolejki komunikatow");
			exit(1);
		}
	}
	else{
		elem.mtype = PUSTY;
		for (i=0; i<MAXK; i++){
			if (msgsnd(msgid , &elem , sizeof(elem.dlaWybranego), 0) == -1){
				perror("Wyslanie pustego komunikatu");
				exit(1);
			}
		}
	}

//SEGMENT DANYCH O UZYTKOWNIKACH


	shmid = shmget(24000, MAXU*sizeof(uzytkownik), 0600);
	if (shmid == -1){
		perror("Uzyskanie identyfikatora segmentu pamieci wspoldzielonej");
		exit(1);
	}

	buf = (uzytkownik*)shmat(shmid , NULL , 0);
	if (buf == NULL){
	perror("Przylaczenie segmentu pamieci wspoldzielonej");
	exit(1);
	}





//JEELI UZYTKOWNIK CHCE WYBRAC SOBIE PRZECIWNIKA
if(wyborTryb==1){

	bufZacz=buf;

	for(i=0;i<50;i++){
		tabUzytkownikow[i]=*buf++;
	}



	for(i=0;i<50;i++){
		if(tabUzytkownikow[i].jest==1){
			printf("%d Nick: %s \n", i,tabUzytkownikow[i].nick);
			printf("%d Poziom: %d \n", i,tabUzytkownikow[i].poziom);
			printf("%d Kolor Pionkow: %d \n", i,tabUzytkownikow[i].kolorPionkow);
			printf("%d Numer Pid: %d \n", i,tabUzytkownikow[i].pid);
			printf("%d Czy wolny: %d \n", i,tabUzytkownikow[i].jest);
			*bufZacz++;
			liczbaUzytkownikow++;
		}
	}
	printf("Ktorego przeciwnika wybierasz: ");
	scanf("%d", &wybranyPrzeciwnik);
	message.pidWybranego=tabUzytkownikow[wybranyPrzeciwnik].pid;
	//USTALANIE WOLNEGO SERWERA
	//SEGMENT DANYCH O SERWERACH

	shmidS = shmget(40000, MAXS*sizeof(serwer),0600);
	if (shmidS == -1){
		perror("Uzyskanie identyfikatora segmentu pamieci wspoldzielonej");
		exit(1);
	}

	bufS = (serwer*)shmat(shmidS , NULL , 0);
	if (bufS == NULL){
		perror("Przylaczenie segmentu pamieci wspoldzielonej");
		exit(1);	
	}
	bufZaczS=bufS;
	bufOdczS = bufS;
	for(i=0;i<50;i++){
		tabSerwerow[i]=*bufZaczS++;
	}

	for(i=0;i<50;i++){
		if((tabSerwerow[i].jest==1)&&(tabSerwerow[i].czyZajete==0)){		
		printf("Klucz serw: %d \n",tabSerwerow[i].kluczServ);
		printf("i: %d \n",i);
		message.kluczServer=tabSerwerow[i].kluczServ;
		klucz=tabSerwerow[i].kluczServ;
		tabSerwerow[i].czyZajete=1;
		*bufOdczS++=tabSerwerow[i];
		break;
		}
		*bufOdczS++=tabSerwerow[i];
	}
	
		elem.dlaWybranego = message;
		elem.mtype = PELNY;
		if (msgsnd(msgid , &elem , sizeof(elem.dlaWybranego), 0) == -1){
			perror("Wyslanie elementu");
			exit(1);
		}
		
	

//JEZELI CHCE OCZEKIWAC NA PRZECIWNIKA
}else{
	bufZacz=buf;
	bufOdN=buf;
	for(i=0;i<50;i++){
		tabUzytkownikow[i]=*buf++;
	}
	for(i=0;i<50;i++){
		if(tabUzytkownikow[i].jest==1){
			liczbaUzytkownikow++;
			*bufZacz++;
		}
	}
	strcpy(tabUzytkownikow[liczbaUzytkownikow].nick,nazwaUz) ;
	tabUzytkownikow[liczbaUzytkownikow].poziom=poziomUz;
	tabUzytkownikow[liczbaUzytkownikow].kolorPionkow=kolorUz;
	tabUzytkownikow[liczbaUzytkownikow].pid=pidUz;
	tabUzytkownikow[liczbaUzytkownikow].jest=1;

	*bufZacz++=tabUzytkownikow[liczbaUzytkownikow];
	printf("%d \n", pidUz);
	while(1){		
		if (msgrcv(msgid , &elem , sizeof(elem.dlaWybranego), PELNY , 0) == -1){	
			perror("Odebranie elementu");
			exit(1);
		}
		if(pidUz==elem.dlaWybranego.pidWybranego){
			klucz=elem.dlaWybranego.kluczServer;
			printf("klucz : %d\n",klucz);
			tabUzytkownikow[liczbaUzytkownikow].jest=0;
			for(i=0;i<liczbaUzytkownikow;i++){
				tabUzytkownikow[i]=*bufOdN++;
			}
			*bufOdN++=tabUzytkownikow[liczbaUzytkownikow];
			printf("znaleziono gre\n");
			break;
		}else{
			elem.mtype = PELNY;
			if (msgsnd(msgid , &elem , sizeof(elem.dlaWybranego), 0) == -1){
				perror("Wyslanie elementu");
				exit(1);
			}
		}

	}

}

	semidG = semget (klucz , 6, 0666) ;
	if ( semidG == -1) {
		perror (" Uzyskanie identyfikatora tablicy semaforow ") ;
		exit (1) ;
	}
	shmidG = shmget(klucz,MAX,0666) ;
	if ( shmidG == -1) {
		perror (" Uzyskanie identyfikatora segmentu pamieci wspoldzielonej ") ;
		exit (1) ;
	}


	bufOdcz=bufG;
	bufZap=bufG;

	while(1){
		opusc ( semidG , 1) ;
		opusc(semidG,3);
		//jezeli skonczyly sie pionki jednego badz drugiego uzytkownika to koniec dzialania
		if((liczbaPionB==0)||(liczbaPionC==0)){
			exit(0);
		}
		bufG = ( char *) shmat ( shmidG , NULL , 0) ;
		if ( bufG == NULL ){
			perror (" Przylaczenie segmentu pamieci wspoldzielonej ") ;
			exit (1) ;
		}	
		bufOdcz=bufG;
		for(liczWiersze=0;liczWiersze<8;liczWiersze++){
			for(int liczKolumny=0;liczKolumny<8;liczKolumny++){			
				tabSzachy[liczWiersze][liczKolumny]=*bufOdcz++;
			}
		}
		liczbaPionB=0;
		liczbaPionC=0;
		for(liczWiersze=0;liczWiersze<8;liczWiersze++){
			for(int liczKolumny=0;liczKolumny<8;liczKolumny++){
				printf("[ %c ]",tabSzachy[liczWiersze][liczKolumny]);
				if(tabSzachy[liczWiersze][liczKolumny]=='B'){
					liczbaPionB++;
				}
				if(tabSzachy[liczWiersze][liczKolumny]=='C'){
					liczbaPionC++;
				}
			}
		printf("\n");
		}

		printf("Wykonaj ruch: \n");
		printf("Pionkiem na ktorym polu chcesz ruszyc? \n");
		printf("Podaj wiersz: ");
		scanf("%d",&wierszPionkaPrzed);
		printf("Podaj kolumne: ");
		scanf("%d",&kolumnaPionkaPrzed);
		printf("Na jakie pole chcesz przesunac pionek \n");
		printf("Podaj wiersz: ");
		scanf("%d",&wierszPionkaPo);
		printf("Podaj kolumne: ");
		scanf("%d",&kolumnaPionkaPo);
		if(kolorUz==0){
		tabSzachy[wierszPionkaPrzed][kolumnaPionkaPrzed]='0';
		tabSzachy[wierszPionkaPo][kolumnaPionkaPo]='B';
		}
		if(kolorUz==1){
		tabSzachy[wierszPionkaPrzed][kolumnaPionkaPrzed]='0';
		tabSzachy[wierszPionkaPo][kolumnaPionkaPo]='C';
		}

	opusc(semidG,4);
	for(liczWiersze=0;liczWiersze<8;liczWiersze++){
		for(int liczKolumny=0;liczKolumny<8;liczKolumny++){		
			*bufG++ = tabSzachy[liczWiersze][liczKolumny];			
		}
	}
	podnies(semidG,5);
	podnies(semidG,3);
	podnies ( semidG , 0) ;
}
//KONIEC TRYBU GRACZA
}else{
//POCZATEK TRYBU OBSERWATORA

	shmidS = shmget(40000, MAXS*sizeof(serwer),0600);
	if (shmidS == -1){
		perror("Uzyskanie identyfikatora segmentu pamieci wspoldzielonej");
		exit(1);
	}

	bufS = (serwer*)shmat(shmidS , NULL , 0);
	if (bufS == NULL){
		perror("Przylaczenie segmentu pamieci wspoldzielonej");
		exit(1);
	}
	
	for(i=0;i<50;i++){
		tabSerwerow[i]=*bufS++;
	}

	for(i=0;i<50;i++){
		if((tabSerwerow[i].jest==1)&&(tabSerwerow[i].czyZajete==1)){
			printf("%d Klucz: %d \n", i,tabSerwerow[i].kluczServ);
			printf("%d Czy jest: %d \n", i,tabSerwerow[i].jest);
			printf("%d Czy zajete: %d \n", i,tabSerwerow[i].czyZajete);						
		}
	}
	
	printf("Ktora gre chcesz obserwowac: ");
	scanf("%d", &ktoraGra);
	klucz=tabSerwerow[ktoraGra].kluczServ;

	semidG = semget (klucz , 6, 0666) ;
	if ( semidG == -1) {
		perror (" Uzyskanie identyfikatora tablicy semaforow ") ;
		exit (1) ;
	}
	shmidG = shmget(klucz,MAX,0666) ;
	if ( shmidG == -1) {
		perror (" Uzyskanie identyfikatora segmentu pamieci wspoldzielonej ") ;
		exit (1) ;
	}


	bufOdcz=bufG;
	bufZap=bufG;

	while(1){
		bufG = (char*)shmat(shmidG,NULL,0) ;
		if ( bufG == NULL ){
			perror (" Przylaczenie segmentu pamieci wspoldzielonej ") ;
			exit (1) ;
		}
	opusc(semidG,5);
	liczbaPionB=0;
	liczbaPionC=0;
		for(liczWiersze=0;liczWiersze<8;liczWiersze++){
			for(int liczKolumny=0;liczKolumny<8;liczKolumny++){
				printf("[ %c ]",*bufOdcz++);
			}
		printf("\n");
	}
	podnies(semidG,4);
	}


}

}



