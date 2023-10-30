//   0036529468    ->    varijanta a:     ul+radna ||| iz

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>

int idul;
int idiz;
int *ulzaj;
int *izzaj;

void *radna(void *pon){
    int x;
    printf("\nPokrenuta RADNA DRETVA");
        for(int i=0;i<*((int*)pon);i++){
            while(*ulzaj==0);                //radno cekanje
            printf("\nRADNA DRETVA: Procitan broj %d", *ulzaj);
            x = *ulzaj + 1;
            printf(" i povecan na %d\n", x);
            *izzaj = x;
            *ulzaj = 0;              
        }
    printf ("\nZavrsila RADNA DRETVA\n");
}

void brisi(int sig){
    (void) shmdt((char*) ulzaj);
    (void) shmdt((char*) izzaj);
    (void) shmctl(idul, IPC_RMID, NULL);
    (void) shmctl(idiz, IPC_RMID, NULL);
    exit(0);
}

int main(int argc, char *argv[]){
    int pon = atoi(argv[1]);
    int r, y;
    FILE *output;
    output = fopen("out.txt", "w");
    pthread_t thr_id[1];
    srand(time(NULL));
    idul = shmget(IPC_PRIVATE, sizeof(int), 0600);
    idiz = shmget(IPC_PRIVATE, sizeof(int), 0600);
    ulzaj = (int *) shmat(idul, NULL, 0);
    izzaj = (int *) shmat(idiz, NULL, 0);
    *ulzaj=0;
    *izzaj=0;
    if(fork() == 0){                 //proces 2 - izlazna dretva
        printf("\nPokrenut IZLAZNI PROCES");
        for(int i=0; i<pon; i++){
            while(*izzaj==0);
            y = *izzaj;
            fprintf(output, "%d\n", y);
            printf("\nIZLAZNI PROCES: broj %d upisan u file", y);    
            *izzaj = 0;   
        }
        printf("\nZavrsio IZLAZNI PROCES\n");
        exit(0);
    }else{                          //proces 1 - ulazna + radna
        pthread_create(&thr_id[0], NULL, radna, &pon);
        printf("\nPokrenuta ULAZNA DRETVA");
        for(int i=0; i<pon; i++){
            while(*ulzaj!=0);
            r = rand() % (100+1-1) + 1;
            printf("\nULAZNA DRETVA: broj %d", r);
            *ulzaj = r;
            if (i!=pon-1) sleep(5);
        }
        printf("\nZavrsila ULAZNA DRETVA\n");
        pthread_join(thr_id[0], NULL);
    }
    brisi(0);
    return 0;
}