#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

int limit=5;               //koliko prog posluzujemo prije prebacivanja na drugu tvrtku
int eat=5;                  //koliko ce dugo biti u restoranu
int inside[2] = {0,0};      //koliko prog u rest {linux,win}
int waiting[2] = {0,0};     //koliko prog ceka {linux,win}
int served[2] = {0,0};      //koliko prog posluzeno {linux,win}
pthread_mutex_t mon;
pthread_cond_t red[2];

void udiur(int tvrtka){                 //0 = linux, 1 = windows                                    
    pthread_mutex_lock(&mon);
    while(inside[1-tvrtka] > 0 || (waiting[1-tvrtka] > 0 && served[tvrtka] >= limit)){              
        waiting[tvrtka]++;            
            //    if(tvrtka==0) printf ("\nProgramer: \033[1;33mJoj ja bas dodem kad je red.\033[0m");             //dialogue
            //    else printf ("\nProgramer: \033[1;36mJoj ja bas dodem kad je red.\033[0m");
        pthread_cond_wait(&red[tvrtka], &mon);                                                       
        waiting[tvrtka]--;
    }
    inside[tvrtka]++;
            if(tvrtka==0) printf ("\nProgramer: \033[1;33mNjam njam food time.\033[0m");                 //dialogue
            else printf ("\nProgramer: \033[1;36mNjam njam food time.\033[0m");
    served[1-tvrtka] = 0;
    if(waiting[1-tvrtka] > 0) {
        served[tvrtka]++;
        printf("\nStaff: \033[1;31mLjudi vani cekaju! Posluzite jos %d pa pustite druge programere.\033[0m", limit - served[tvrtka]);
    }
    pthread_mutex_unlock(&mon);
}

void vanizr(int tvrtka){                //0 = linux, 1 = windows
    pthread_mutex_lock(&mon);
    inside[tvrtka]--;
    if (inside[tvrtka] == 0){
        pthread_cond_broadcast(&red[1-tvrtka]);            //druga ekipa moze uc
        printf("\nStaff: \033[1;31mRestoran je sada prazan!\033[0m");
    }
    pthread_mutex_unlock(&mon);
}

void *Programer(void *n){             //0 = linux, 1 = windows
    long tvrtka = (long)n;
    while(1){
                if(tvrtka==0) printf ("\nProgramer: \033[1;33mMislim da cu otici do restorana.\033[0m");     //dialogue text
                else printf ("\nProgramer: \033[1;36mMislim da cu otici do restorana.\033[0m");
        udiur(tvrtka);                      //zovi funkc
        sleep(eat);                         //jedi njam njam
                if(tvrtka==0) printf ("\nProgramer: \033[1;33mTo je bilo bas fino. Izlazim iz restorana.\033[0m");     //dialogue
                else printf ("\nProgramer: \033[1;36mTo je bilo bas fino. Izlazim iz restorana.\033[0m");
        vanizr(tvrtka);                     //idi van
    }
}

int main(void){
    srand(time(NULL));
    printf("\nProgrameri po bojama:     \033[1;33mLinux Programer     \033[1;36mWindows Programer\033[0m")
    pthread_mutex_init (&mon, NULL);
	pthread_cond_init (&red[0], NULL);
    pthread_cond_init (&red[1], NULL);
    pthread_t thr_id[0];
    int x=0, delay=1, ocekivanje=2, r;
    long tvrtka;
    while(1){
        r=rand();
        //printf("r=%d", r);
        if (r%ocekivanje==0){  //prog oce jest
            r=rand();
            if (r%2==0) tvrtka=0;       //linux
            else tvrtka=1;              //windows
            x++;
            pthread_create(&thr_id[x], NULL, *Programer, (void*)tvrtka);
        }
        sleep(delay);
    }
}