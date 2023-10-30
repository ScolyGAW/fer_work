#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>

/* funkcije za obradu signala, navedene ispod main-a */
void obradi_dogadjaj(int sig);
void obradi_sigterm(int sig);
void obradi_sigint(int sig);
int nije_kraj = 1;
int i = 0;
int main(){
    FILE *obrada;
    obrada = fopen("obrada.txt", "a+");
    FILE *status;
    status = fopen("status.txt", "r");

    struct sigaction act;
    /* 1. maskiranje signala SIGUSR1 */
    act.sa_handler = obradi_dogadjaj; /* kojom se funkcijom signal obrađuje */
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGTERM); /* blokirati i SIGTERM za vrijeme obrade */
    act.sa_flags = 0; /* naprednije mogućnosti preskočene */
    sigaction(SIGUSR1, &act, NULL); /* maskiranje signala preko sučelja OS-a */

    /* 2. maskiranje signala SIGTERM */
    act.sa_handler = obradi_sigterm;
    sigemptyset(&act.sa_mask);
    sigaction(SIGTERM, &act, NULL);

    /* 3. maskiranje signala SIGINT */
    act.sa_handler = obradi_sigint;
    sigaction(SIGINT, &act, NULL);

    printf("Program s PID=%ld krenuo s radom\n", (long) getpid());
    fscanf(status, "%d", &i);
    if (i>0){
        fclose(status);
        status = fopen("status.txt", "w");
        fprintf(status, "%d", 0);
    }else{
        int help;
        while(fscanf(obrada, "%d", &help) == 1){
            i = help;
        }
        i = sqrt(i);
        if (i==0) exit(1);
    }
        while(nije_kraj) {
            ++i;
            fprintf(obrada, "%d\n", i*i);
            sleep(5);
        }
    fclose(status);
    status = fopen("status.txt", "w");
    fprintf(status, "%d", i);
    fclose(obrada);
    fclose(status);
    printf("Program s PID=%ld zavrsio s radom\n", (long) getpid());
    return 0;
}

void obradi_dogadjaj(int sig)
{
    printf("%d\n", i);
    sleep(1);
}
void obradi_sigterm(int sig)
{
    nije_kraj = 0;
}
void obradi_sigint(int sig)
{
    exit(1);
}