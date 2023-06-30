#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define PALLETE_SPACE_SMALL 0.2
#define PALLETE_SPACE_HALF 0.5
#define PALLETE_SPACE_FULL 1
#define PALLETE_SPACE_DOUBLE 2

#define NUM_DRIVERS_STANDARD 4
#define MAX_DRIVERS 10

// #define NUM_DIRECTIONS 2 // mesto a venkov
#define NUM_DIRECTIONS 4 // mesto a venkov(breclav + vyskov + trebic)
#define WAREHOUSE_SPACE 500


// zakladni promenne pouzite v celem programu
int num_drivers_start = NUM_DRIVERS_STANDARD;
int num_drivers;
int inc_drivers = 0; //false


struct vysledky{
  // kolik penez se vydelalo za sluzby a doruceni
  long long int MONEY_EARNED;

  // kolikrat se jelo, a celkove jak dlouho
  long double TIME_RIDE_SPENT[2];
  
  // jak moc bylo auto vytizene
  long double CAR_SPACE_USED[2]; 

  // kolik prumerne jezdi aut
  long int NUM_OF_DRIVERS[2];

  // kolik penez prumerne za den se vydela
  long int AVERAGE_MONEY;
}vysled;

// vypis chybu (s libovolnymi argumenty) a ukonci
#define printf_err(fmt, ...) \
	do { printf(fmt, ##__VA_ARGS__); exit(1); } while(0)

// celkovy pocet zasilek, ktery je na skladu
int sklad = 0;

int trucks = -1; //def=1 
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

// oznacuje hodnotu poctu zasilek, ktere dojedou ten den (prepocteno i s intervaly pokud jsou zadany)
int in_today_small;
int in_today_half;
int in_today_full;
int in_today_double;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

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
ps_del_carry DELS_CARRY = {250,350,500,1000};

// int price_serv_delivery_build = -1; // mezi 300-400,-

typedef struct price_serv_delivery_all{
  const int halfpall;
  const int pall;
  const int doubpall;
}ps_del_all;
ps_del_all DELS_ALL = {800,1400,2500};

enum velikost_zasilky{
  NIC = 0,
  BALIK = 20,
  PULPALETA,
  PALETA,
  DVOJPALETA,
};

// je potreba zasilky odvezt po 'meste' nebo na 'venkov' (venkov -> + ~90min cesta)
enum rozvoz_smer{
  NIKDE = 0,
  MESTO, // toto musi byt 1
  // VENKOV,
  VENKOV_BRECLAV,
  VENKOV_TREBIC,
  VENKOV_VYSKOV,
};

enum service{
  S_NIC = 0,
  S_DELIV = 10,
  S_DELIV_CARRY,
  S_DELIV_BUILD,
  S_DELIV_ALL,
};

typedef struct rozvoz_auto{
  int id;
  int doba; // jak dlouho zabere jizda == pracovni doba ridice
  int cena; // kolik vydela
  enum rozvoz_smer smer; //smer kterym jede
  float zaplneny_prostor;
}rozvoz_auto;


// defaultne mame 4 auta na rozvoz
struct rozvoz_auto auta[MAX_DRIVERS];

//struktura pro jednu zasilku
typedef struct zasilka{
  int id;
  enum velikost_zasilky typ;
  enum rozvoz_smer smer;
  enum service servis;
  int cas;
  int cena;
}zasilka;

struct zasilka zasilky[WAREHOUSE_SPACE] = {0};

// rika co je prave na skladu (kazdy den privezou nove)
int curr_small = 0;
int curr_half = 0;
int curr_full = 0;
int curr_double = 0;

double totalDriversHours = 0;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/* ------- DECLARATIONS ------- */
void initAnswers();
void showAnswers();
void printState(const char* s);
void printHelp();
int getRand(int low,int high);
int parser(int argc, char **argv);
void parserInterval(char *s, char *arg);
void validateTrucks();
enum service getRandomService(enum velikost_zasilky vz);
int nOfPkgs(int *a,int *b,int *c,int *d);
enum rozvoz_smer getDirection();
int getPkgTime(enum service s);
int getPkgPrice(enum service s,enum velikost_zasilky vz);
enum velikost_zasilky getPkgType(int *a, int *b, int *c, int *d);
void getDelivery();
void removePackageFromWarehouse(struct zasilka z);
int getHiDirByNumOfPckgs();
enum rozvoz_smer* getDirs();
zasilka findPackage(enum velikost_zasilky vz,enum rozvoz_smer rs,int doba);
int getPossiblePackage(struct rozvoz_auto **car);
void unloadCar(struct rozvoz_auto *car);
void loadCar(struct rozvoz_auto *car);
/* ------- DECLARATIONS END------- */

void initAnswers(){

  // [0] == celkovy prostor, [1] == pocet aut
  vysled.CAR_SPACE_USED[0] = 0.0;
  vysled.CAR_SPACE_USED[1] = 0.0;

  vysled.MONEY_EARNED = 0;
  vysled.AVERAGE_MONEY = 0;

  // [0] == celkovy cas, [1] == pocet jizd
  vysled.TIME_RIDE_SPENT[0] = 0.0;
  vysled.TIME_RIDE_SPENT[1] = 0.0;

  vysled.NUM_OF_DRIVERS[0] = 0;
  vysled.NUM_OF_DRIVERS[1] = 0;
  return;
}

void showAnswers(int pocet_dni){
  printf("\n ~~~~~~~~~~~~ Ziskane vysledky ~~~~~~~~~~~~ \n");
  printf("Celkove hruby zisk: %lld,-\n",vysled.MONEY_EARNED);
  printf("Prumerny vydelek za den pro jedno auto %ld,-\n",vysled.AVERAGE_MONEY);
  printf("Prumerny cas za jizdu: %.2Lfmin\n",vysled.TIME_RIDE_SPENT[0]/vysled.TIME_RIDE_SPENT[1]);
  printf("Auto bylo prumerne zaplneno na %.4Lf palet\n",vysled.CAR_SPACE_USED[0]/vysled.CAR_SPACE_USED[1]);
  printf("Prumerne kazdy den jelo %ld ridicu.\n",vysled.NUM_OF_DRIVERS[0]/vysled.NUM_OF_DRIVERS[1]);

  if(pocet_dni == 365){
    printf("--\n");
    printf("[[pocteno pouze pro jeden rok]]\n");

    float driver_pay = 5*12*(45000+0.34*45000);
    vysled.MONEY_EARNED -= driver_pay;
    printf("Pokud mam standardne %d ridicu, jejichz mesicni plat je 45000,- a za 12 mesicu za ne zaplatim %.0f,- (prepocteno s 34%% dane)\n",num_drivers_start,driver_pay);
    
    int car_pay = 600000*5;
    vysled.MONEY_EARNED -= car_pay;
    printf("Koupil jsem techto %d aut, kazde za ~600.000,- tedy celkem: %d,-\n",num_drivers_start,car_pay);

    int car_service = 5000*12*5 + 20000*12*5;
    vysled.MONEY_EARNED -= car_service;
    printf("Kazdy mesic si odlozim 5.000,- na opravy kazdeho auta, 20.000,- na naftu, celkem: %d,-\n",car_service);
    
    int cars_rented = vysled.NUM_OF_DRIVERS[0]/vysled.NUM_OF_DRIVERS[1]-num_drivers_start;
    vysled.MONEY_EARNED -= cars_rented;
    printf("Prumerne si pujcim kazdy den %d aut(o). Za jedno zaplatim 3000,- za den, tedy: %d,-\n",cars_rented,cars_rented*3000*365);

    printf("Ve vysledku mam  %lld (%lld,- mesicne)\n",vysled.MONEY_EARNED,vysled.MONEY_EARNED/12);
  }
  printf("\n");
  printf(" ~~~~~~~~~~~~ Ziskane vysledky ~~~~~~~~~~~~ \n");
}

void printState(const char* s){
  printf("Aktualni stav (%s) ~~~~~\n",s);
  printf("kamion:     %2d; ",trucks);
  printf("Vkamion:    %2d\n",dist_tructs);
  printf("balik       %2d; ",packages_small);
  printf("Vbalik      %2d\n",dist_pkgs_small);
  printf("pulpaleta   %2d; ",packages_half_pallete);
  printf("Vpulpaleta  %2d\n",dist_pkgs_half_pallete);
  printf("paleta      %2d; ",packages_pallete);
  printf("Vpaleta     %2d\n",dist_pkgs_pallete);
  printf("dvojpaleta  %2d; ",packages_double_pallete);
  printf("Vdvojpaleta %2d\n",dist_pkgs_double_pallete);
  printf("~~~~~ ~~~~~ ~~~~~ ~~~~~ ~~~~~\n");

}

void printHelp(){
  printf("Simulace skladu rozvazejiciho zbozi\n");
  printf("INPUT: kamion(y) se 4 druhy nakladu (podle velikosti)\n");
  printf("\n");
  printf("Nastavitelne parametry:\n");
  printf("  --kamion N[,M]          | N je pocet kamionu(def=1) za jeden den(s nahodnymi zasilkami)\n");
  printf("  Pokud je tento parametr zadan, vsechny atributy nize jsou ignorovany(a prepsany!) (pro testovani neefektivniho kamionu pridej (TODO) )\n");
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
  printf("Maximalni pocet dostupnych aut je 10. Lze nastavit, ze se automaticky zvysi pocet pouzivanych aut pro den, pokud narusta pocet zasilek na skladu:\n");
  printf("  --vic_aut               | pokud je to mozne zvysi pocet pouzivanych aut pro rozvoz v pripade\n");
  printf("\n");
  exit(0);
}

int getRand(int low, int high){
  int tmp = (rand() % (high - low)) + low + 1;
  // printf("random:%d-%d: %d\n",low,high,tmp);
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
    } else if(!strcmp(argv[i],"--vic_aut")){
      inc_drivers = 1; 
    } else if(!strcmp(argv[i],"--ridici")){
      num_drivers_start = strtol(argv[++i],&end,10);
    }
  }
  return 0;
}


void initTrucks(){
  printf("trucksinit:%d\n",trucks);
  if(trucks <= 0){ // pokud neni zadan truck
    if(packages_small != -1 || packages_half_pallete != -1 || packages_pallete != -1 || packages_double_pallete != -1){ //pokud je alespon jeden typ zasilky zadan
      int space = 0;
      // spocitej prostor
      if(packages_small         >= 0){space +=  packages_small          *PALLETE_SPACE_SMALL;}
      if(packages_half_pallete  >= 0){space +=  packages_half_pallete   *PALLETE_SPACE_HALF;}
      if(packages_pallete       >= 0){space +=  packages_pallete        *PALLETE_SPACE_FULL;}
      if(packages_double_pallete>= 0){space +=  packages_double_pallete *PALLETE_SPACE_DOUBLE;}
      trucks = (int)(space / 33);
      int space_lt = space % 33; //space_last_truck
      if(space_lt <= 26 && trucks > 0){
        //odeber zasilky
        printf("Aktualni pocet zasilek %d (poveze %d kamionu) nevytizi posledni kamion dostatecne(volne misto: %d), snizuji pocet zasilek\n",space,trucks,space_lt);
        while(space_lt > 0){
          if      (packages_double_pallete > 0 && space_lt >= 2){packages_double_pallete -=1; space_lt-=2;}
          else if (packages_pallete        > 0 && space_lt >= 1){packages_pallete        -=1; space_lt-=1;}
          else if (packages_half_pallete   > 1 && space_lt >= 1){packages_half_pallete   -=2; space_lt-=1;}
          else if (packages_small          > 4 && space_lt >= 1){packages_small          -=5; space_lt-=1;}
        }
      } 
      else {
        // pridej zasilky
        while(space_lt < 26){
          if      (space_lt <= 24)  {packages_double_pallete +=1; space_lt+=2;}
          else if (space_lt <= 25)  {packages_pallete        +=1; space_lt+=1;}
          else if (space_lt <= 25){packages_half_pallete     +=2; space_lt+=1;}
          else if (space_lt <= 25){packages_small            +=5; space_lt+=1;}
        }
      }
    } 
    else {
    packages_small = 20;
    packages_half_pallete = 16;
    packages_pallete = 9;
    packages_double_pallete = 6;
    }
  } 
  else {
  //pokud je nejaky kamion, nastav standardni hodnoty (podle poctu kamionu)
  packages_small          = 20*trucks;
  packages_half_pallete   = 16*trucks;
  packages_pallete        = 9*trucks;
  packages_double_pallete = 6*trucks;
  }
}

void validateTrucks(){
// aspon 80% = 26.4 z 33 (paletovych mist)
  if(trucks <= 0){ //default
    if(in_today_small != -1 || in_today_half != -1 || in_today_full != -1 || in_today_double != -1){ 
      //alespon jeden parameter je zadan
      int space = 0;
      if(in_today_small >= 0){space += in_today_small *PALLETE_SPACE_SMALL;}
      if(in_today_half >= 0){space +=in_today_half    *PALLETE_SPACE_HALF;}
      if(in_today_full >= 0){space +=in_today_full    *PALLETE_SPACE_FULL;}
      if(in_today_double>=0){space +=in_today_double  *PALLETE_SPACE_DOUBLE;}
      trucks = (int)(space / 33);

      int space_lt = space % 33; //space_last_truck
      if(space_lt <= 26){
        if(trucks <= 0){
          // navyseni o 1 cely kamion
          in_today_double += 6 ;
          in_today_full   += 9 ;
          in_today_half   += 16;
          in_today_small  += 20;

          // znovu prirad misto
          space  =in_today_double *PALLETE_SPACE_DOUBLE; 
          space +=in_today_full   *PALLETE_SPACE_FULL;
          space +=in_today_half   *PALLETE_SPACE_HALF;
          space +=in_today_small  *PALLETE_SPACE_SMALL;

          printf("space:%d\n",space);
          trucks = (int)(space / 33);
          space_lt = space % 33; //space_last_truck

        } else {
          printf("Aktualni pocet zasilek %d (poveze %d kamionu) nevytizi posledni kamion dostatecne(volne misto: %d), snizuji pocet zasilek\n",space,trucks,space_lt);
          while(space_lt > 0){
            if(in_today_double > 0 && space_lt >= 2){in_today_double-=2;space_lt-=2;}
            else if(in_today_full > 0 && space_lt >= 1){in_today_full-=1;space_lt-=1;}
            else if(in_today_half > 1 && space_lt >= 1){in_today_half-=2;space_lt-=1;}
            else if(in_today_small > 4 && space_lt >= 1){in_today_small-=5;space_lt-=1;}
          }
        }
      }
    }
    else {
      //zadny parametr neni zadan a trucks taky neni (truck == 1 & napln default hodnoty)
      //DEFAULT TRUCK == 1, prumerny pocet kombinovanych zasilek ~50
      trucks = 1;
      in_today_double = 6;
      in_today_full = 9;
      in_today_half = 16;
      in_today_small = 20;
    }
  }
  else {

    // trucks je vetsi nez 0
    if(in_today_small != -1 || in_today_half != -1 || in_today_full != -1 || in_today_double != -1){
      // pokud uz je nastaveno, nenastavovat znovu
      return;
    } 
      in_today_small = packages_small = 20*trucks;
      in_today_half = packages_half_pallete = 16*trucks;
      in_today_full = packages_pallete = 9*trucks;
      in_today_double = packages_double_pallete = 6*trucks;
  }
}

enum service getRandomService(enum velikost_zasilky vz){
  int rd = getRand(0,100);
  switch(vz){
    case BALIK:
      if(rd <= 55){return(S_DELIV);}
      if(rd > 55 && rd <= 75){return(S_DELIV_CARRY);}
      if(rd > 75){return(S_DELIV_BUILD);}
      break;
    case PULPALETA:
    case PALETA:
    case DVOJPALETA:
      if(rd <= 30){return(S_DELIV);}
      if(rd > 30 && rd <= 50){return(S_DELIV_BUILD);}
      if(rd > 50 && rd <= 75){return(S_DELIV_CARRY);}
      if(rd > 75){return(S_DELIV_ALL);}
      break;
    default:
      return -1;
  }
  return -1;
}

int nOfPkgs(int *a,int *b,int *c,int *d){
  int res = 0;
  if(dist_pkgs_small>0){
    *a = getRand(packages_small-dist_pkgs_small, packages_small+dist_pkgs_small);
  } else {
    *a = packages_small;
  }
  if(*a < 0){
    *a = 0;
  }
  res += *a;

  if(dist_pkgs_half_pallete > 0){
    *b = getRand(packages_half_pallete-dist_pkgs_half_pallete, packages_half_pallete+dist_pkgs_half_pallete);
  } else {
    *b = packages_half_pallete;
  }
  if(*b < 0){
    *b = 0;
  }
  res += *b;


  if(dist_pkgs_pallete > 0){
    *c = getRand(packages_pallete-dist_pkgs_pallete, packages_pallete+dist_pkgs_pallete);
  } else {
    *c = packages_pallete;
  }
  if(*c < 0){
    *c = 0;
  }
  res += *c;


  if(dist_pkgs_double_pallete > 0){
    *d = getRand(packages_double_pallete-dist_pkgs_double_pallete, packages_double_pallete+dist_pkgs_double_pallete);
  }else {
    *d = packages_double_pallete;
  }
  if(*d < 0){
    *d = 0;
  }
  res += *d;

  sklad += res;
  // printf("new pkgs: %d(%d)\n",res,sklad);
  return res;
}

enum rozvoz_smer getDirection(){
  int tmp = getRand(0,5); //cislo od 1 od 5
  switch (tmp)
  {
  case MESTO:
    return MESTO;
    break;
  case VENKOV_BRECLAV:
    return VENKOV_BRECLAV;
    break;
  case VENKOV_TREBIC:
    return VENKOV_TREBIC;
    break;
  case VENKOV_VYSKOV:
    return VENKOV_VYSKOV;
    break;
  default:
    return MESTO;
    break;
  }
}

// ~ cas cisteho servisu (bez pridani 2h za cestovani)
int getPkgTime(enum service s){
//   S_DELIV,S_DELIV_CARRY,S_DELIV_BUILD,S_DELIV_ALL
  if(s == S_DELIV)           {return(20);}
  else if(s == S_DELIV_CARRY){return(getRand(25,50));}
  else if(s == S_DELIV_BUILD){return(getRand(60,100));}
  else if(s == S_DELIV_ALL)  {return(getRand(160,190));}
  else{printf_err("Vnitrni chyba v getPkgTime(): enum service: %d\n",s);}
}

int getPkgPrice(enum service s,enum velikost_zasilky vz){
    // vz = BALIK,PULPALETA,PALETA,DVOJPALETA,NIC,
  if(s == S_DELIV){
    switch(vz){
      case BALIK:
        return(DELS.small);
      case PULPALETA:
        return(DELS.halfpall);
      case PALETA:
        return(DELS.pall);
      case DVOJPALETA:
        return(DELS.doubpall);
      default:
        return -1; 
    }
  }
  else if(s == S_DELIV_CARRY){
    switch(vz){
      case BALIK:
        return(DELS_CARRY.small);
      case PULPALETA:
        return(DELS_CARRY.halfpall);
      case PALETA:
        return(DELS_CARRY.pall);
      case DVOJPALETA:
        return(DELS_CARRY.doubpall);
      default:
        return -1; 
    }
    }
  else if(s == S_DELIV_BUILD){
    switch(vz){
      case BALIK:
        return(DELS.small + getRand(300,400));
      case PULPALETA:
        return(DELS.halfpall + getRand(300,400));
      case PALETA:
        return(DELS.pall + getRand(300,400));
      case DVOJPALETA:
        return(DELS.doubpall + getRand(300,400));
      default:
        return -1; 
    }
  }
  else if(s == S_DELIV_ALL){
    switch(vz){
      case PULPALETA:
        return(DELS_ALL.halfpall);
      case PALETA:
        return(DELS_ALL.pall);
      case DVOJPALETA:
        return(DELS_ALL.doubpall);
      default:
        return -1; 
    }
  }
  else{printf_err("Vnitrni chyba v getPkgPrice()");}
}

enum velikost_zasilky getPkgType(int *a, int *b, int *c, int *d){
  if(*a > 0){
    *a -= 1;
    return(BALIK);
  }
  else if (*b > 0){
    *b -= 1;
    return(PULPALETA);
  }
  else if (*c > 0){
    *c -= 1;
    return(PALETA);
  }
  else if (*d > 0){
    *d -= 1;
    return(DVOJPALETA);
  }
  else{
    return NIC;
  }
}

void getDelivery(){
  int a,b,c,d;

  // pridej do a,b,c,d pocty novych zasilek podle typu a vrat celkovy pocet do 'new'
  int new = nOfPkgs(&a,&b,&c,&d);

  printf("Dnesni zasilky(%d): %d,%d,%d,%d\n",new,a,b,c,d);
  // vytvor denni dodavku zasilek
  in_today_small  = a;
  in_today_half   = b;
  in_today_full   = c;
  in_today_double = d;

  // pripocitej konkretni zasilky do aktualniho stavu skladu
  curr_small  += a;
  curr_half   += b;
  curr_full   += c;
  curr_double += d;
  enum velikost_zasilky tmp_rs;
  // actually vytvor zasilky ve skladu

  for(int i = 0; i < new; ++i){
    int found_space = 0;
    for(int j = 0;j < WAREHOUSE_SPACE;++j){
      tmp_rs = NIC;
      // najdi volne misto v poli zasilek
      if(zasilky[j].typ == NIC){
        found_space = 1;

        tmp_rs = getPkgType(&a,&b,&c,&d); 
        if(tmp_rs == NIC){break;} //pokud jsme prebrali vsechny zasilky

        zasilky[j].typ    = tmp_rs;
        zasilky[j].servis = getRandomService(zasilky[j].typ);
        zasilky[j].smer   = getDirection();
        zasilky[j].cas    = getPkgTime(zasilky[j].servis);//cas v minutach
        zasilky[j].cena   = getPkgPrice(zasilky[j].servis,zasilky[j].typ);//cena v korunach
        break;
      }
    }
    if(!found_space){
      printf_err("Na skladu je vice nez %d zasilek. Je zastaven provoz!\n",WAREHOUSE_SPACE);
    }
  }
}

void printZasilkyNaSkladu(){
  printf("~~Zasilky na skladu~~\n");
  for(int i = 0; i <WAREHOUSE_SPACE;++i){
    if(zasilky[i].typ != NIC){
      printf("%d: typ=%d, smer=%d, servis=%d\n",zasilky[i].id,zasilky[i].typ,zasilky[i].smer,zasilky[i].servis);
    }
  }
  printf("~~ ~~ ~~ ~~~ ~~ ~~ ~~\n");
}

// jakmile nalozis zasilku do auta, je odstranena ze skladu
void removePackageFromWarehouse(struct zasilka z){
  sklad -= 1;
  
  switch (z.typ)
  {
  case BALIK:
    curr_small -= 1;
    break;
  case PULPALETA:
    curr_half -= 1;
    break;
  case PALETA:
    curr_full -= 1;
    break;
  case DVOJPALETA:
    curr_double -= 1;
    break;
  
  default:
    break;
  }
  
  zasilky[z.id].cas = 0;
  zasilky[z.id].cena = 0;
  zasilky[z.id].servis = 0;
  zasilky[z.id].smer = 0;
  zasilky[z.id].typ = 0;

}


// spocitej pocet zasilek rozdelenych podle smeru, vrat smer
int getHiDirByNumOfPckgs(){ // WORKS

  int highest = 0;
  int dir;
  // mesto zacne na jednicce, NIKDE je 0, tzn. preskoceno
  for(int i = MESTO; i <= NUM_DIRECTIONS; ++i){
    int tmp = 0;
    for(int j = 0;j<WAREHOUSE_SPACE;++j){
      if(zasilky[j].smer == i){
        tmp += 1;
      }
    }
    if(tmp > highest){
      highest = tmp;
      dir = i;
    }
  }
  return dir;
}

enum rozvoz_smer* getDirs(){ //WORKS
  enum rozvoz_smer* rs = malloc(NUM_DIRECTIONS*sizeof(enum rozvoz_smer));

  int hd = getHiDirByNumOfPckgs();
  rs[0] = hd;
  int cnt = 1;
  // uloz smery do pole
  for(int i = 1; i <= NUM_DIRECTIONS;++i){
    if(i != hd){
      rs[cnt] = i;
      cnt++;
    }
  }
  return rs;
}


// najdi zasilku ve skladu vhodnou pro nalozeni, podle doby kterou zabere na doruceni
zasilka findPackage(enum velikost_zasilky vz,enum rozvoz_smer rs,int doba){ //WORKS
  for (int i = 0; i < WAREHOUSE_SPACE; ++i){
    if(zasilky[i].typ == vz && zasilky[i].smer == rs){//pokud sedi typ a smer

      // (POSSIBLE TODO upravit na vice nahodnou dobu, mozna nahodne zvolit 8 az 10 (standard. prac. doba ridice) aby vice reflektovala tuto flaktuaci / nestalost
      if(doba + zasilky[i].cas <= 600){ //casove se vleze do 10 hodin 
        return zasilky[i];
      }
    } 
  } 
  zasilka z = {.id = -1,.typ=-1};
  return z;
}

//pro vypis
char *getServis(int i){
  if(i == S_DELIV){
    return "Dodavka";
  }
  else if( i == S_DELIV_CARRY){
    return "Vynos";
  }
  else if (i == S_DELIV_BUILD){
    return "Lehka Montaz";
  }
  else if( i == S_DELIV_ALL){
    return "Tezka montaz";
  }
  else {
    return "nevim";
  }

}

// pro vypis
char* getDirectionSTRING(enum rozvoz_smer rs){
  switch (rs)
  {
  case MESTO:
    return "Brno";
    break;
  case VENKOV_BRECLAV:
    return "Breclav";
    break;
  case VENKOV_TREBIC:
    return "Trebic";
    break;
  case VENKOV_VYSKOV:
    return "Vyskov";
    break;
  default:
    return "Mesto";
    break;
  }
}

// pridej zasilky do auta (vyuziva funkci findPackage() v cyklu)
int getPossiblePackage(struct rozvoz_auto **car){
  zasilka z = {0};
  // printf("auto: \n");
  // printf("-  doba: %d, cena:%d, smer:%d, zapln.prostor:%.2f \n",(*car)->doba,(*car)->cena,(*car)->smer,(*car)->zaplneny_prostor);

  if((*car)->zaplneny_prostor <= 6){ // vleze se dvoj paleta
    z = findPackage(DVOJPALETA,(*car)->smer,(*car)->doba);
    if(z.id != -1){
      // printf("- nasel jsem dvojpaletu s id: %3d; jede na: %d; zabere: %3dmin; za: %3d (%s),-\n",z.id,z.smer,z.cas,z.cena,getServis(z.servis));
      removePackageFromWarehouse(z);
      (*car)->doba += z.cas;
      (*car)->cena += z.cena;
      (*car)->zaplneny_prostor += 2.0;
      return 1;
    }
  }
  if((*car)->zaplneny_prostor <= 7){ //vleze se paleta
    z = findPackage(PALETA,(*car)->smer,(*car)->doba);
    if(z.id != -1){
      // printf("- nasel jsem paletu     s id: %3d; jede na: %d; zabere: %3dmin; za: %3d (%s),-\n",z.id,z.smer,z.cas,z.cena,getServis(z.servis));
      removePackageFromWarehouse(z);
      (*car)->doba += z.cas;
      (*car)->cena += z.cena;
      (*car)->zaplneny_prostor += 1.0;
      return 1;
    }
  }
  if((*car)->zaplneny_prostor <= 7.5){ //vleze se pulpaleta
    z = findPackage(PULPALETA,(*car)->smer,(*car)->doba);
    if(z.id != -1){
      // printf("- nasel jsem pulpaletu  s id: %3d; jede na: %d; zabere: %3dmin; za: %3d (%s),-\n",z.id,z.smer,z.cas,z.cena,getServis(z.servis));
      removePackageFromWarehouse(z);
      (*car)->doba += z.cas;
      (*car)->cena += z.cena;
      (*car)->zaplneny_prostor += 0.5;
      return 1;
    }
  }
  if((*car)->zaplneny_prostor <= 7.8){ //vleze se balik
    z = findPackage(BALIK,(*car)->smer,(*car)->doba);
    if(z.id != -1){
      // printf("- nasel jsem balik      s id: %3d; jede na: %d; zabere: %3dmin; za: %3d (%s),-\n",z.id,z.smer,z.cas,z.cena,getServis(z.servis));
      removePackageFromWarehouse(z);
      (*car)->doba += z.cas;
      (*car)->cena += z.cena;
      (*car)->zaplneny_prostor += 0.2;
      return 1;
    }
  }
  return 0; //nenasel jsem ani jednu moznou zasilku
}
void unloadCar(struct rozvoz_auto *car){
  vysled.CAR_SPACE_USED[0] += car->zaplneny_prostor;
  vysled.CAR_SPACE_USED[1] += 1;

  car->smer = 0;
  car->zaplneny_prostor = 0.0;
}

void loadCar(struct rozvoz_auto *car){
  enum rozvoz_smer *rs = getDirs();

  int done = 0;
  // pro kazdy smer zkus (pocinaje tim co ma nejvice objednavek)
  for(int i = 0; i < NUM_DIRECTIONS;++i){
    car->smer = rs[i]; //nastav smer

    if(car->smer != MESTO){car->doba = 90;} //pokud se rozvazi na venkov, doba cesty jen tam+zpet je ~2hod
    else{car->doba = 0;}

    while(1){ //nakladej auto dokud nenastane jedna z podminek pro zastaveni nakladani
      if(!getPossiblePackage(&car)){//vrati 0 == jiz neni co nalozit -> (pokud je zaplneny prostor 0, znamena to ze se nenalozilo nic, zkus dalsi smer)
        if(car->zaplneny_prostor != 0.0){
          done=1;
        }
        break;
      } 
    }
    if(done){break;}
  }

  free(rs); //alokuje se ve funkci getDirs()
}

int lastMonthReview(int day, int avg[],int *drivers_per_month){
  if (day % 30 == 0){
    // kazdy treti mesic
    printf("\n~~~~~~MESICNI REVIEW BOI~~~~~~~~\n");
    long int tmp = 0;
    for (int i = 0; i < 30; i++){
      tmp += avg[day-31+i];
    }
    printf("Mesicni vydelek za auto: %ld,-\n",tmp/30);
    printf("Mesicni pocet ridicu prumerne: %d\n",(*drivers_per_month)/30);      
    *drivers_per_month = 0;
  }
  return 0;
}

int main(int argc, char **argv){
  setbuf(stdout, NULL);// zadny buffer na vypis
  printf("------------------\n");
  srand(time(NULL)); // init randomizer

  int num_drivers = num_drivers_start;
  //inicializace ID zasilek (pokud je zasilka dorucena, ID se uvolnuje a je znovu pouzitelne)
  for(int i = 0; i < WAREHOUSE_SPACE;++i){
    zasilky[i].id = i;
    zasilky[i].typ = NIC;
    zasilky[i].smer = NIKDE;
    zasilky[i].servis = S_NIC;
  }
  // inicializace ID aut (pro jednodussi rozpoznavani pri praci)
  for(int i = 0; i < MAX_DRIVERS;++i){
    auta[i].id = i;
  }
  initAnswers();

  if(parser(argc,argv) < 0){
    printHelp();
  }

  // const int pocet_dni = 10;
  const int pocet_dni = 365;  // 1 rok
  // const int pocet_dni = 730;  // 2 roky
  // const int pocet_dni = 1095; // 3 roky
  // const int pocet_dni = 3650; // 10 let

  int average_day_pay[pocet_dni];
  int drivers_month = 0;

  // pro zamezeni nekonecneho cyklu pouzij toto
  int inside_check_for_end_of_day = 0;    
  initTrucks();  
  // simulace DNY (jeden cyklus == jeden den)
  for (int den = 1; den <= pocet_dni; ++den){
    printf("\n!!! DEN %d !!! \n",den);
    if(sklad > 20 && num_drivers < MAX_DRIVERS && inc_drivers){
      num_drivers++;
      if(sklad > 50 && num_drivers < MAX_DRIVERS){
        num_drivers++;
        if(sklad > 100 && num_drivers < MAX_DRIVERS){
          num_drivers++;
        }
      }

    } else if(sklad == 0){
      num_drivers = num_drivers_start;
    } else if (sklad <= 5 && num_drivers >= 6){
      num_drivers -= 2;
    }

    printf("Mame %d ridicu dnes!\n",num_drivers);
    getDelivery();
    validateTrucks();
    printf("(Rano)Den %3d: [%2db,%2dp,%2dP,%2dPP](celkem: %d)\n",den,curr_small,curr_half,curr_full,curr_double,sklad);
    // printZasilkyNaSkladu(); //DEBUG

    int finishDay = 0;
    // simulace ridicu dovazejicich zasilky
    while(1){

      // simuluj jednu jizdu ridicu (kazdy cyklus jiny ridic)
      for(int ridic = 0; ridic < num_drivers; ++ridic){
        
        // nejsou zasilky na skladu
        if(sklad <= 0){finishDay = 1;break;}

        // pokud ma ridic za den najeto 8+ hodin(pracuje 8-10), uz nikam nejede
        if(auta[ridic].doba > 480){continue;}

        //simulace: naloz zasilky do auta (pokud pracovni doba nepresahne 480)
        loadCar(&auta[ridic]);
        //simulace: odvez zasilky klientum
        printf("nalozeno! autoid:%d:smer:%s,cena:%d,doba:%d prostor:%.2f\n",auta[ridic].id,getDirectionSTRING(auta[ridic].smer),auta[ridic].cena,auta[ridic].doba,auta[ridic].zaplneny_prostor);
        // printf("(aktualni stav: [%2db,%2dp,%2dP,%2dPP](celkem: %d)\n",curr_small,curr_half,curr_full,curr_double,sklad);

        unloadCar(&auta[ridic]);
      }

      // pokud maji vsichni vice nez 8 hodin, skonci den
      int allDriversDone = 1;
      for(int j = 0;j < num_drivers;++j){
        if(inside_check_for_end_of_day > 10){
          inside_check_for_end_of_day = 0;
          allDriversDone = 1;
          break;
        }
        if(auta[j].doba <= 480 && auta[j].zaplneny_prostor < 5){
          inside_check_for_end_of_day += 1;
          allDriversDone = 0;
          break;
        }
      }

      if(allDriversDone){finishDay = 1;}
      if(finishDay){
        int average_money_today = 0;
        // uloz hodnoty -- vynuluj ridice -- vynuluj ridice
        for (int i = 0; i < num_drivers; i++){
          //prumer vydelku dneska
          average_money_today += auta[i].cena;


          vysled.MONEY_EARNED += auta[i].cena;
          
          vysled.TIME_RIDE_SPENT[0] += auta[i].doba;
          vysled.TIME_RIDE_SPENT[1] += 1;

          // vyprazdni auta, priprav na novy den
          auta[i].doba = 0;
          auta[i].zaplneny_prostor = 0;
          auta[i].smer = 0;
          auta[i].cena = 0;
        }
        vysled.NUM_OF_DRIVERS[0] += num_drivers;
        vysled.NUM_OF_DRIVERS[1]++;
        drivers_month += num_drivers;

        average_day_pay[den-1] = average_money_today/num_drivers;
        printf("Dnes se vydelalo průměrně na auto %d,-\n",average_day_pay[den-1]);


        lastMonthReview(den,average_day_pay,&drivers_month);
        break;
      }
    }
    printf("~~~~~~~\n");
    printf("(Vecer)Den %3d: [%2db,%2dp,%2dP,%2dPP](celkem: %d)\n",den,curr_small,curr_half,curr_full,curr_double,sklad);
  }

  //spocitej prumerny vydelek
  long long int tmp_avg = 0;
  for (int i = 0; i < pocet_dni; i++){
    tmp_avg += average_day_pay[i];
  }
  vysled.AVERAGE_MONEY = tmp_avg/pocet_dni;

  showAnswers(pocet_dni);
}
