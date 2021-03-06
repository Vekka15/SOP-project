#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 64
#define MAXU 50
#define MAXS 50



static struct sembuf buf;

typedef struct uzytkownik{
char nick[6];
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

int main(){
int shmid , shmidS, semidG ,shmidG, i;
char *bufG ,*bufZapG,*bufOdczG;
uzytkownik *buf, *bufZaczU;
serwer *bufS, *bufZaczS,*bufOdczS;
uzytkownik tab[10];
serwer tabSerwerow[20];
int liczbaSerwerow=0;
int liczWiersze,liczKolumny;
char tabSzachy[8][8];
int klucz;
int liczbaPionB=12;
int liczbaPionC=12;

srand(time(NULL));

//UZUPELNIANIE SZACHOWNICY

for(liczWiersze=0;liczWiersze<2;liczWiersze++){
	for(int liczKolumny=0;liczKolumny<8;liczKolumny++){
		tabSzachy[liczWiersze][liczKolumny]='B';
	}
}

for(liczWiersze=6;liczWiersze<8;liczWiersze++){
	for(int liczKolumny=0;liczKolumny<8;liczKolumny++){
		tabSzachy[liczWiersze][liczKolumny]='C';
	}
}

for(liczWiersze=2;liczWiersze<6;liczWiersze++){
	for(int liczKolumny=0;liczKolumny<8;liczKolumny++){
		tabSzachy[liczWiersze][liczKolumny]='0';
	}
}

for(liczWiersze=0;liczWiersze<8;liczWiersze++){
	for(int liczKolumny=0;liczKolumny<8;liczKolumny++){
		printf("[ %c ]",tabSzachy[liczWiersze][liczKolumny]);
	}
	printf("\n");
}

klucz = rand() % 100000;
printf("klucz : %d\n",klucz);

//SEMAFORY

semidG = semget (klucz, 6, IPC_CREAT|0666) ;
if ( semidG == -1) {
perror (" Utworzenie tablicy semaforow ") ;
exit (1) ;
}
if ( semctl ( semidG , 0, SETVAL , ( int ) MAX ) == -1) {
perror (" Nadanie wartosci semaforowi 0") ;
exit (1) ;
}
if ( semctl ( semidG , 1, SETVAL , ( int  ) 0) == -1) {
perror (" Nadanie wartosci semaforowi 1") ;
exit (1) ;
}
if (semctl(semidG , 2, SETVAL , (int)1) == -1){
perror("Nadanie wartosci semaforowi 2");
exit(1);
}
if (semctl(semidG , 3, SETVAL , (int)1) == -1){
perror("Nadanie wartosci semaforowi 3");
exit(1);
}
if (semctl(semidG , 4, SETVAL , (int)MAX) == -1){
perror("Nadanie wartosci semaforowi 4");
exit(1);
}
if (semctl(semidG , 5, SETVAL , (int)0) == -1){
perror("Nadanie wartosci semaforowi 5");
exit(1);
}

//SEGMENT DANYCH O GRZE W SZACHY

shmidG = shmget (klucz ,MAX, IPC_CREAT |0666) ;
if ( shmidG == -1) {
perror (" Utworzenie segmentu pamieci wspoldzielonej 1") ;
exit (1) ;
}
bufG = ( char *) shmat ( shmidG , NULL , 0) ;
if ( bufG == NULL ){
perror (" Przylaczenie segmentu pamieci wspoldzielonej ") ;
exit (1) ;
}

bufZapG=bufG;

//SEGMENT DANYCH NA DANE O UZYTKOWNIKACH

shmid = shmget(24000, MAXU*sizeof(uzytkownik), IPC_CREAT |0600);
if (shmid == -1){
	perror("Utworzenie segmentu pamieci wspoldzielonej2");
	exit(1);
}

buf = (uzytkownik*)shmat(shmid , NULL , 0);
if (buf == NULL){
	perror("Przylaczenie segmentu pamieci wspoldzielonej");
	exit(1);
}

bufZaczU = buf;

//SEGMENT DANYCH O SERWERACH

	shmidS = shmget(40000, MAXS*sizeof(serwer), IPC_CREAT |0600);
	if (shmidS == -1){
		perror("Utworzenie segmentu pamieci wspoldzielonej3");
		exit(1);
	}

	bufS = (serwer*)shmat(shmidS , NULL , 0);
	if (bufS == NULL){
		perror("Przylaczenie segmentu pamieci wspoldzielonej");
		exit(1);
	}

	bufZaczS = bufS;
	bufOdczS= bufS;
	for(i=0;i<10;i++){
		tabSerwerow[i]=*bufZaczS++;
	}

	for(i=0;i<10;i++){
		if(tabSerwerow[i].jest==1){
			*bufOdczS++;
			liczbaSerwerow++;
		}
	}
//printf("%d",liczbaSerwerow);

	tabSerwerow[liczbaSerwerow].kluczServ=klucz;
	tabSerwerow[liczbaSerwerow].czyZajete=0;
	tabSerwerow[liczbaSerwerow].jest=1;


	*bufOdczS++=tabSerwerow[liczbaSerwerow];


//WYMIENIANIE DANYCH

while(1){
opusc ( semidG , 0) ;
liczbaPionB=0;
liczbaPionC=0;
for(liczWiersze=0;liczWiersze<8;liczWiersze++){
	for(int liczKolumny=0;liczKolumny<8;liczKolumny++){		
		*bufZapG++ = tabSzachy[liczWiersze][liczKolumny];
		if(tabSzachy[liczWiersze][liczKolumny]=='B'){
			liczbaPionB++;
		}
		if(tabSzachy[liczWiersze][liczKolumny]=='C'){
			liczbaPionC++;
		}			
	}
}
for(liczWiersze=0;liczWiersze<8;liczWiersze++){
	for(int liczKolumny=0;liczKolumny<8;liczKolumny++){		
		tabSzachy[liczWiersze][liczKolumny]=*bufG++;
			
	}
}
if((liczbaPionB==0)||(liczbaPionC==0)){
//usuwanie struktury
shmctl( shmidG, IPC_RMID, 0);
exit(0);
}
podnies ( semidG , 1) ;
}


}



