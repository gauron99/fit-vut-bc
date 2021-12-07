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
ps_del_carry DELS_CARRY = {150,250,500,1000};

int price_serv_delivery_build = -1; // between 300-400,-

typedef struct price_serv_delivery_all{
  const int halfpall;
  const int pall;
  const int doubpall;
}ps_del_all;
ps_del_all DELS_ALL = {500,1000,2000};



enum velikost_zasilky{
  BALIK = 20,
  PULPALETA,
  PALETA,
  DVOJPALETA,
  NIC,
};

// je potreba zasilky odvezt po 'meste' nebo na 'venkov' (venkov -> + ~2h cesta)
enum rozvoz_smer{
  MESTO = 1,
  VENKOV,
};

enum service{
  S_DELIV = 10,
  S_DELIV_CARRY,
  S_DELIV_BUILD,
  S_DELIV_ALL,
};

typedef struct rozvoz_auto{
  int doba; // jak dlouho zabere jizda == pracovni doba ridice 
  int cena; // kolik vydela
  enum rozvoz_smer smer; //smer kterym jede
}rozvoz_auto;

// defaultne mame 3 auta na rozvoz
struct rozvoz_auto auta[3];

//struktura pro jednu zasilku
typedef struct zasilka{
  int id;
  enum velikost_zasilky typ;
  enum rozvoz_smer smer;
  enum service servis;
  int cas;
  int cena;
}zasilka;

struct zasilka zasilky[10000] = {0};


// rika co je prave na skladu (kazdy den privezou nove)
int curr_small = 0;
int curr_half = 0;
int curr_full = 0;
int curr_double = 0;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/* ------- DECLARATIONS ------- */
void printHelp();
int getRand(int low,int high);
int parser(int argc, char **argv);
void parserInterval(char *s, char *arg);
void validateTrucks();
enum rozvoz_smer getRandomDest();
void getDelivery();
int getRandomServicePriceAndTime(int count); //TODO
/* ------- DECLARATIONS END------- */

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
  printf("\n");
  printf("\n");
  printf("\n");
  exit(0);
}

int getRand(int low, int high){
  int tmp = (rand() % (high - low)) + low;
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

void validateTrucks(){
// aspon 80% = 26.4 z 33 (paletovych mist)
  if(trucks <= 0){ //default
    if(packages_small != -1 || packages_half_pallete != -1 || packages_pallete != -1 || packages_double_pallete != -1){ 
      //alespon jeden parameter je zadan
      int space = 0;
      if(packages_small >= 0){space += packages_small*PALLETE_SPACE_SMALL;}
      if(packages_half_pallete >= 0){space +=packages_half_pallete*PALLETE_SPACE_HALF;}
      if(packages_pallete >= 0){space +=packages_pallete*PALLETE_SPACE_FULL;}
      if(packages_double_pallete>=0){space +=packages_double_pallete*PALLETE_SPACE_DOUBLE;}
      printf("Zabrane misto: %d palet\n",space);
      trucks = (int)(space / 33);
      int space_lt = space % 33; //space_last_truck
      if(space_lt <= 26){
        if(trucks <= 0){
          printf_err("Nedostatecny pocet zasilek(%d). Nepojede ani jeden kamion\n",space);
        }
        printf("Aktualni pocet zasilek %d (poveze %d kamionu) nevytizi posledni kamion dostatecne(volne misto: %d), snizuji pocet zasilek\n",space,trucks,space_lt);
        while(space_lt > 0){
          printf("space_remaining: %d\n",space_lt);
          if(packages_double_pallete > 0 && space_lt >= 2){packages_double_pallete-=2;space_lt-=2;}
          else if(packages_pallete > 0 && space_lt >= 1){packages_pallete-=1;space_lt-=1;}
          else if(packages_half_pallete > 1 && space_lt >= 1){packages_half_pallete-=2;space_lt-=1;}
          else if(packages_small > 4 && space_lt >= 1){packages_small-=5;space_lt-=1;}
        }
      }
    }
    else {
      //zadny parametr neni zadan a trucks taky neni (truck == 1 & napln nahodne hodnoty)
      //DEFAULT TRUCK == 1, prumerny pocet kombinovanych zasilek ~50
      trucks = 1;
      packages_double_pallete = 6;
      packages_pallete = 9;
      packages_half_pallete = 16;
      packages_small = 20;
    }
  }
  else {
    // trucks je vetsi nez 0
      packages_double_pallete = 6*trucks;
      packages_pallete = 9*trucks;
      packages_half_pallete = 16*trucks;
      packages_small = 20*trucks;
  }
}


enum rozvoz_smer getRandomDest(){
  int cnt = getRand(0,100);
  return cnt >=50 ? MESTO : VENKOV;
}

enum service getRandomService(enum velikost_zasilky vz){
  switch(vz){
    case BALIK:
      int rd = getRand(0,99);
      if(rd < 33){return(S_DELIV);}
      if(rd >= 33 && rd < 66){return(S_DELIV_BUILD);}
      if(rd >= 66){return(S_DELIV_CARRY);}
      break;
    case PULPALETA:
    case PALETA:
    case DVOJPALETA:
      int r = getRand(0,100);
      if(r < 25){return(S_DELIV);}
      if(r >= 25 && r < 50){return(S_DELIV_BUILD);}
      if(r >= 50 && r < 75){return(S_DELIV_CARRY);}
      if(r >= 75){return(S_DELIV_ALL);}
      break;
    default:
      return -1;
  }
  return -1;
}

int nOfPkgs(){
  int res = 0;
  res += packages_small;
  res += packages_half_pallete;
  res += packages_pallete;
  res += packages_double_pallete;

  sklad += res;
  // printf("new pkgs: %d(%d)\n",res,sklad);
  return res;
}

int getDirection(){
  int tmp = getRand(0,2);
  return tmp==0 ? MESTO : VENKOV;
}

// ~ cas cisteho servisu (bez pridani 2h za cestovani)
int getPkgTime(enum service s){
//   S_DELIV,S_DELIV_CARRY,S_DELIV_BUILD,S_DELIV_ALL
  if(s == S_DELIV)            {return(30);}
  else if(s == S_DELIV_CARRY){return(getRand(40,70));}
  else if(s == S_DELIV_BUILD){return(getRand(70,100));}
  else if(s == S_DELIV_ALL)  {return(getRand(190,220));}
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
    return -1;
  }
}

void getDelivery(){
  // update pole zasilek
  int new = nOfPkgs();

  int a = packages_small;
  int b = packages_half_pallete;
  int c = packages_pallete;
  int d = packages_double_pallete;

  for(int i = 0; i < new; ++i){
    int found_space = 0;
    for(int j = 0;j < 10000;++j ){
      // najdi volne misto v poli zasilek
      if(zasilky[i].typ == NIC){
        found_space = 1;

        zasilky[i].typ    = getPkgType(&a,&b,&c,&d);
        if(zasilky[i].typ == -1){zasilky[i].typ = 0; break;} //pokud jsme prebrali vsechny zasilky

        zasilky[i].servis = getRandomService(zasilky[i].typ);
        zasilky[i].smer   = getDirection();
        zasilky[i].cas    = getPkgTime(zasilky[i].servis);//cas v minutach
        zasilky[i].cena   = getPkgPrice(zasilky[i].servis,zasilky[i].typ);//cena v korunach
        break;
      }
    }
    if(!found_space){
      printf_err("Na skladu je vice nez 10 000 zasilek. Je zastaven provoz!\n");
    }
  }

  curr_small  += packages_small;
  curr_half   += packages_half_pallete;
  curr_full   += packages_pallete;
  curr_double += packages_double_pallete;
}

int main(int argc, char **argv){
  printf("------------------\n");
  srand(time(NULL)); // init randomizer

  //inicializace ID zasilek (pokud je zasilka dorucena, ID se uvolnuje a je znovu pouzitelne)
  for(int i = 0; i < 10000;++i){
    zasilky[i].id = i;
      zasilky[i].typ = NIC;
  }

  if(parser(argc,argv) < 0){
    printHelp();
  }
  printState("Pred kontrolou");
  validateTrucks();// spocitej, kolik kamionu dojede -- vytizeni alespon 80%
  printState("Po kontrole");

  if(trucks==1){
    printf("Kazdy den prijede 1 kamion\n");
  } else if(trucks==2 || trucks==3 || trucks==4){
    printf("Kazdy den prijedou %d kamiony\n",trucks);
  } else {
    printf("Kazdy den prijede %d kamionu\n",trucks);
  }
  const int pocet_dni = 1;
  for (int i = 1; i <= pocet_dni; ++i){
    getDelivery();
    printf("(Rano)Den %3d: [%2d,%2d,%2d,%2d]\n",i,curr_small,curr_half,curr_full,curr_double);
    // mame pouze 3 auta; rozvoz = Mesto || Venkov

  }  
}
