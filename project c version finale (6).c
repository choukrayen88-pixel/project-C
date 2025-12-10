#include<stdio.h>
#include<stdlib.h>
#include<time.h>      // pour time
#include<string.h>
#include<windows.h>   // pour sleep()

typedef struct{             // pour stocker les parametres de configuration du programme
    float seuil_min;
    float seuil_max;
    int intervalle_mesure;        // pause entre chaque mesure
}config;

typedef struct entry{                 // pour stocker les temperatures
    char timestamp[50];              // le temps de la mesure
    float temperature;
    int alerte;                        //Niveau d'alerte (0,1,2,3)
    struct entry*next;                
}entry;

entry*journal=NULL;                  // debut de la liste chainee vide

void lire_config(config*cfg){               // lire config depuis fichier
    FILE*f=fopen("config.txt","r");
    if(!f){                                 // si fichier inexistant ou vide
        cfg->seuil_min=10;                 // valeurs par defaut
        cfg->seuil_max=34;
        cfg->intervalle_mesure=2;
        return;
    }
    fscanf(f,"%f%f%d",&cfg->seuil_min,&cfg->seuil_max,&cfg->intervalle_mesure);
    fclose(f);
}

void ecrire_config(config*cfg){                       // ecrire config dans fichier
    FILE*f=fopen("config.txt","w");
    fprintf(f,"%.2f %.2f %d",cfg->seuil_min,cfg->seuil_max,cfg->intervalle_mesure);
    fclose(f);
}

float lire_temperature(){                           // generer temperature aleatoire entre -10 et 60
    return(rand()%71)-10;
}


int niveau_alerte(float t, config*cfg) {
    
    if(t >= cfg->seuil_min && t <= cfg->seuil_max)
        return 0;
    float ecart;
    if(t < cfg->seuil_min) {
        ecart = cfg->seuil_min - t;  // Écart en dessous du min
    } else {
        ecart = t - cfg->seuil_max;  // Écart au-dessus du max
    }
    
    // Déterminer le niveau selon l'écart
    if(ecart > 10)  return 3;  // Critique
    if(ecart > 5)   return 2;  // Modéré
    return 1;                  // Léger
}

void ajouter_entree(float t,int alerte){             // ajoute mesure dans journal
    entry*e=malloc(sizeof(entry));
    time_t now=time(NULL);                                // temps actuel
    struct tm*tm=localtime(&now);                        // convertir en structure tm
    strftime(e->timestamp,50,"%Y-%m-%d %H:%M:%S",tm);    // format date+heure
    e->temperature=t;
    e->alerte=alerte;
    e->next=journal;
    journal=e;
}

void sauvegarder_journal(){                    // sauvegarde toutes les entrees dans fichier
    FILE*f=fopen("journal.txt","w");
    entry*p=journal;                               // parcourir la liste
    while(p){
        fprintf(f,"%s | %.2f°C | alerte %d\n",p->timestamp,p->temperature,p->alerte);
        p=p->next;
    }
    fclose(f);
}

void generer_rapport(){                          // genere rapport journalier
    FILE*f=fopen("rapport_journalier.txt","w");
    entry*p=journal;
    if(!p){                                     // si liste vide
        fprintf(f,"aucune donnee.\n");
        fclose(f);
        return;
    }
    float min=p->temperature;
    float max=p->temperature;
    float total=0;
    int count=0;
    int alertes[4]={0};                     // compteur pour chaque niveau
    while(p){                  
        if(p->temperature<min)min=p->temperature; 
        if(p->temperature>max)max=p->temperature; 
        total+=p->temperature;                    
        alertes[p->alerte]++;                     
        count++;
        p=p->next;
    }
    float moy=total/count;
    fprintf(f,"===rapport journalier===\n");
    fprintf(f,"temperature min : %.2f°C\n",min);
    fprintf(f,"temperature max : %.2f°C\n",max);
    fprintf(f,"temperature moyenne : %.2f°C\n",moy);
    fprintf(f,"alertes niveau 1 : %d\n",alertes[1]);
    fprintf(f,"alertes niveau 2 : %d\n",alertes[2]);
    fprintf(f,"alertes niveau 3 : %d\n",alertes[3]);
    fclose(f);
}

void envoyer_notification(int niveau,float t){                  // notification si critique
    if(niveau==3)
        printf("notification critique envoyée! temp = %.2f°C\n",t);
}

char*noms_alertes[]={"normal","alerte faible","alerte moyenne","critique"};

void exporter_incidents(){
    FILE* f=fopen("incidents.txt","w");
    if(!f) return;
    entry* p =journal;
    int compteur =0;
    fprintf(f,"===LISTE DES INCIDENTS (Niveaur 2 et 3)===\n\n");
    while (p){
        if(p->alerte>=2){
            fprintf(f,"[%s] DANGER:%.2f°C (Niveau %d)\n",p->timestamp ,p->temperature, p->alerte);
            compteur++;
        }
        p=p->next;
    }
    if(compteur==0){
        fprintf(f,"Aucin incidents grave détecté sur cette période.\n");
    }
    printf("Fichier 'incidents.txt' genere avec %d alerte(s).\n", compteur);
    fclose(f);
}

int main(){
    srand(time(NULL));                 // initialise generateur aleatoire
    config cfg;
    lire_config(&cfg);                   // lire config
    ecrire_config(&cfg);                   // ecrire config
    int mesure;
    printf("combien de mesures voulez-vous prendre ?");
    scanf("%d",&mesure);

    for(int i=0;i<mesure;i++){                       // boucle sur toutes les mesures
        float t=lire_temperature();
        int al=niveau_alerte(t,&cfg);
        printf("mesure %d : %.2f°C | alerte %d (%s)\n",i+1,t,al,noms_alertes[al]);
        ajouter_entree(t,al);                             // ajouter au journal
        envoyer_notification(al,t);                        // envoyer notification si critique
        _sleep(cfg.intervalle_mesure*1000);                // pause entre mesures
    }
  
    sauvegarder_journal();                              // sauvegarder journal
    generer_rapport();                                  // generer rapport
    return 0;
}