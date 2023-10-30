#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <pthread.h>

int idvrt, idsjo, idkrj, iddow;
sem_t *vrtuljak;
sem_t *sjeo;
sem_t *kraj;
sem_t *down;

void *Visitor(){
    sem_wait (vrtuljak);
    printf("\nVisitor: \033[1;32mSjedoh na vrtuljak.\033[0m");
    sem_post (sjeo);
    sem_wait (kraj);
    printf("\nVisitor: \033[1;33mTo je bilo bas fora. Ustajem s vrtuljka.\033[0m");
    sem_post (down);
}
int main (void){
    srand(time(NULL));
    int r, x=0;
    int fakeinfinity = 10000;       //nakon ovoliko visitora gasim program
    int ocekivanje = 2;     //koliko visitora ocekujemo (za rand)
    int voznja = 5;         //koliko traje voznja u sekundama
    int kapvrt = 10;        //kapacitet vrtuljka
    int delay = 1;          //svakih koliko se rolla novi visitor

    idvrt = shmget(IPC_PRIVATE, sizeof(sem_t), 0600);
    idsjo = shmget(IPC_PRIVATE, sizeof(sem_t), 0600);
    idkrj = shmget(IPC_PRIVATE, sizeof(sem_t), 0600);
    iddow = shmget(IPC_PRIVATE, sizeof(sem_t), 0600);
    vrtuljak = shmat (idvrt, NULL, 0);
    sjeo = shmat (idsjo, NULL, 0);
    kraj = shmat (idkrj, NULL, 0);
    down = shmat (iddow, NULL, 0);
    shmctl (idvrt, IPC_RMID, NULL);
    shmctl (idsjo, IPC_RMID, NULL);
    shmctl (idkrj, IPC_RMID, NULL);
    shmctl (iddow, IPC_RMID, NULL);
    sem_init (vrtuljak, 1, 0);
    sem_init (sjeo, 1, 0);
    sem_init (kraj, 1, 0);
    sem_init (down, 1, 0);

    pthread_t thr_id[fakeinfinity];
    if(fork()==0){
        while(1){             //vrtuljak
            for (int i=0;i<kapvrt;i++) sem_post (vrtuljak);
            for (int i=0;i<kapvrt;i++) sem_wait (sjeo);
            sleep(voznja);
            for (int i=0;i<kapvrt;i++) sem_post (kraj);
            for (int i=0;i<kapvrt;i++) sem_wait (down);
        }
    } else {            //visitor stream
        while(x<fakeinfinity){
            r = rand();
            //printf("r=%d", r);
            if (r%ocekivanje==0){  //visitor oce na vrtuljak
                x++;
                printf("\nVisitor: \033[1;31mJa bi na vrtuljak i stat cu u red!\033[0m");
                pthread_create(&thr_id[x], NULL, Visitor, NULL);
            }
            sleep(delay);
        }
        printf("\nNarrator: \033[0;34mPark se zatvorio.\033[0m");
        return 0;
    }
}
