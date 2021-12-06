#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define PALLETE_SPACE_SMALL 0.2
#define PALLETE_SPACE_HALF 0.5
#define PALLETE_SPACE_FULL 1
#define PALLETE_SPACE_DOUBLE 2

#define printf_err(fmt, ...) \
	do { printf(fmt, ##__VA_ARGS__); exit(1); } while(0)


int trucks = 1; //def=1 
int dist_tructs = -1;

// number of packages sent to warehouse each day (or mid value)
int packages_small = -1;
int packages_half_pallete = -1;
int packages_pallete = -1;
int packages_double_pallete = -1;

// distance from mid value used randomly
int dist_pkgs_small = -1;
int dist_pkgs_half_pallete = -1;
int dist_pkgs_pallete = -1;
int dist_pkgs_double_pallete = -1;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

int price_serv_delivery_build_easy = -1; // between 300-400,-
typedef struct price_serv_delivery{
  const int small;
  const int halfpall;
  const int pall;
  const int doubpall;
}ps_del;
ps_del DELS = {100,180,250,400};

typedef struct price_serv_delivery_carry{
  const int small;
  const int halfpall;
  const int pall;
  const int doubpall;
}ps_del_carry;
ps_del_carry DELS_CARRY = {150,250,500,1000};

typedef struct price_serv_delivery_all{
  const int small;
  const int halfpall;
  const int pall;
  const int doubpall;
}ps_del_all;
ps_del_all DELS_ALL = {-1,500,1000,2000};


/* ------- DECLARATIONS ------- */
void printHelp();
int getRand(int low,int high);
int parser(int argc, char **argv);
void parserInterval(char *s, char *arg);

/* ------- DECLARATIONS END------- */

void printState(const char* s){
  printf("~~~~~ Aktualni stav (%s) ~~~~~\n",s);
  printf("kamion:     %d\n",trucks);
  printf("Vkamion:    %d\n",dist_tructs);
  printf("balik       %d\n",packages_small);
  printf("Vbalik      %d\n",dist_pkgs_small);
  printf("pulpaleta   %d\n",packages_half_pallete);
  printf("Vpulpaleta  %d\n",dist_pkgs_half_pallete);
  printf("paleta      %d\n",packages_pallete);
  printf("Vpaleta     %d\n",dist_pkgs_pallete);
  printf("dvojpaleta  %d\n",packages_double_pallete);
  printf("Vdvojpaleta %d\n",dist_pkgs_double_pallete);
  printf("~~~~~ ~~~~~ ~~~~~ ~~~~~ ~~~~~\n");

}

void printHelp(){
  printf("Simulace skladu rozvazejiciho zbozi\n");
  printf("INPUT: kamion(y) se 4 druhy nakladu (podle velikosti)\n");
  printf("\n");
  printf("Nastavitelne parametry:\n");
  printf("  --kamion N[,M]          | N je pocet kamionu(def=1) za jeden den(s nahodnymi zasilkami)\n");
  printf("  Pokud je tento parametr zadan, vsechny atributy nize jsou ignorovany(a prepsany!) (pro testovani neefektivniho kamionu pridej (TODO) )\n")
  printf("\n");
  printf("  Pokud chcete nastavit pouze specificke zasilky:\n");
  printf("  Nastav zbozi ktere dojede v poctu paletovych mist.\n");
  printf("  -b,  --balik N[,M]      | N baliku dovezou na sklad    [0.2]\n");
  printf("  -p,  --pulpaleta N[,M]  | N pulpalet dovezou na sklad  [0.5]\n");
  printf("  -P,  --paleta N[,M]     | N palet dovezou na sklad     [1]\n");
  printf("  -PP, --dvojpaleta N[,M] | N dvojpalet dovezou na sklad [2]\n");
  printf("  PS: tyto argumenty se daji kombinovat libovolne\n");
  printf("  PS2: Do kamionu se vleze 33 paletovych mist. Kazda zasilka vyse ma v [] konstantu velikosti palety\n");
  printf("  PS3: Kamiony jezdi pouze s minimalnim vytizenim 80%%, tedy pokud pocet zasilek nebude objemove sedet na dostatecne vytizeni kamionu, tento kamion nedojede (dojede mene zasilek)\n");
  printf("  Kazdy den dojede(ou) kamion(y) s vybranym zbozim.\n");
  printf("Parametr N se muze chovat jako stredni hodnota. Pri nastaveni -v\n");
  printf("\n");
  printf("\n");
  printf("\n");
  exit(0);
}

int getRand(int low, int high){
  printf("%d\n",rand());
  int tmp = (rand() % (high - low)) + low;
  printf("random: %d\n",tmp);
  return tmp;
}

void parserInterval(char *s,char *arg){
  char* div = 0;
  if(!strcmp(s,"kamion")){
    trucks = strtol(arg,&div,10);
    // printf("pred:%d;%s\n",trucks,div);
    arg = div + 1;
    dist_tructs = strtol(arg,&div,10);
    // printf("potom:%d;%s\n",dist_tructs,div);
  } else if(!strcmp(s,"balik")){
    packages_small=strtol(arg,&div,10);
    arg = div + 1;
    dist_pkgs_small = strtol(arg,&div,10);
  } else if(!strcmp(s,"pulpaleta")){
    packages_half_pallete=strtol(arg,&div,10);
    arg = div + 1;
    dist_pkgs_half_pallete = strtol(arg,&div,10);
    
  } else if(!strcmp(s,"paleta")){
    packages_pallete=strtol(arg,&div,10);
    arg = div + 1;
    dist_pkgs_pallete = strtol(arg,&div,10);

  } else if(!strcmp(s,"dvojpaleta")){
    packages_double_pallete=strtol(arg,&div,10);
    arg = div + 1;
    dist_pkgs_double_pallete = strtol(arg,&div,10);
  } else {
    printf_err("Chyba v parserInterval().\n");
  }
}

int parser(int argc, char **argv){
  if(argc == 1){
    return 0; //run with default values
  }

  char *end = NULL;
  for (int i = 1; i < argc; i++){
    end = NULL;
    printf("%d:%s\n",i,argv[i]);
    if(!strcmp(argv[i],"-h")){
      printHelp();
    } else if(strcmp(argv[i],"-b")==0 || strcmp(argv[i],"--balik")==0) {
      // if string containts "-" -> N,M
      if(strchr(argv[++i],',') != NULL){
        parserInterval("balik",argv[i]);
      } else {
        packages_small = strtol(argv[i],&end,10);
        if(end == 0){
          printf_err("Chyba! Chci cislo pro balik\n");
        }
      }
    } else if(strcmp(argv[i],"-p")==0 || strcmp(argv[i],"--pulpaleta")==0) {
      // if string containts "-" -> N,M
      if(strchr(argv[++i],',') != NULL){
        parserInterval("pulpaleta",argv[i]);
      } else {
        packages_half_pallete = strtol(argv[i],&end,10);
        if(end == 0){
          printf_err("Chyba! Chci cislo pro pulpaletu\n");
        }
      }
    } else if(strcmp(argv[i],"-P")==0 || strcmp(argv[i],"--paleta")==0) {
      // if string containts "-" -> N,M
      if(strchr(argv[++i],',') != NULL){
        parserInterval("paleta",argv[i]);
      } else {
        packages_pallete = strtol(argv[i],&end,10);
        if(end == 0){
          printf_err("Chyba! Chci cislo pro paletu\n");
        }
      }
    } else if(strcmp(argv[i],"-PP")==0 || strcmp(argv[i],"--dvojpaleta")==0) {
      // if string containts "-" -> N,M
      if(strchr(argv[++i],',') != NULL){
        parserInterval("dvojpaleta",argv[i]);
      } else {
        packages_double_pallete = strtol(argv[i],&end,10);
        if(end == 0){
          printf_err("Chyba! Chci cislo pro dvojpaletu\n");
        }
      } 
    } else if(!strcmp(argv[i],"--kamion")) {
      // if string containts "-" -> N,M
      if(strchr(argv[++i],',') != NULL){
        parserInterval("kamion",argv[i]);
      } else {  
        trucks = strtol(argv[i],&end,10);
        if(end == 0){
          printf_err("Chyba! Chci cislo pro dvojpaletu\n");
        } 
      }
    }      
  }
  return 0;
}

int main(int argc, char **argv){
  printf("------------------\n");
  srand(time(NULL)); // init randomizer

  if(parser(argc,argv) < 0){
    printHelp();
  }
  printState("Pred kontrolou");

  // spocitej, kolik kamionu dojede -- vytizeni alespon 80%
  calculateTrucks()

  printState("Po kontrole");

  const int pocet_dni = 2;
  for (int i = 0; i < pocet_dni; ++i){


  }  
}
