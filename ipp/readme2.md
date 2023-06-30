## Dokumentace k projektu IPP 2020 (interpret) 
### autor: Fridrich David
### login: xfridr08
### datum: 14.04.2020

Program jako prvni po spuštění parsuje argumenty které byly předány příkazovou řádkou.
Pro správnou funkčnost programu je potřeba takto předat alespoň jeden ze dvou možných vstupních argumentů ( --source nebo --input) jelikož pokud jeden z nich předán není, načítá se z stdin.

Jestliže všechny soubory byly načtené vpořádku, následuje parsování xml. Tento vstupní soubor má danou strukturu, je tedy zkontrolováná (např. root.tag se jmenuje 'program', a obsahuje elementy 'instruction', které obsahují elementy arg1/arg2/arg3 a každý element má dané atributy). elementy 'instruction' neboli instrukce jsou uspořádáný od nejmenšího čísla operace (pomocí bubblesortu) a uloženy do proměnné.

Před zpracováním samotných instrukcí jsou z listu instrukcí vybrány všechny instrukce "LABEL"
jelikož program podporuje skoky na později definované návěští. V této části programu je zároveň provedena kontrola, že nejsou duplicitní názvy návěští, duplicitní číslo instrukce

Hlavní cyklus programu bude využívat tuto proměnnou a bude cyklovat všemi instrukcemi.
Každý řádek (instrukce) je nejdříve rozložena na své jméno a argumenty(elementy), které obsahuje. Argumenty jsou uloženy stejně jako instrukce (vzestupně). Zde se kontroluje zda argumenty mají správnou formu (jestli nějaký nechybí). Následně se pomocí if-elif-elsu vybere zadaná instrukce. Zkontroluje se jestli má správný počet argumentů a pro různé instrukce se provádí různé kontroly. Např. instrukce POPFRAME (neobsahuje žádné argumenty) vyjme lokální rámec (LF) ze zásobníku rámců (implementováno jako pole, kde poslední objekt pole je jako nejvyšší místo zásobníku) a tedy je potřeba zajistit, že takový rámec LF již existuje a je možné akci provést. Pokud vstupní kontrola projde, provede se instrukce. Pro instrukce, které obsahují argumenty (např. MOVE), je zapotřebí zkontrolovat, zda argumenty jsou správného typu. Např. instrukce [ADD 'var' 'symb' 'symb'] tedy je zkontrolováno, jestli první argument je existující proměnná, a jestli druhý a třetí argument je symbol. Pro toto jsou implementováný vlastní funkce. Stejně jako aritmetické, relační, booleovské a konverzní instrukce jsou zpracovávány ve vlastní funkci 'work_args()'. Funkce využívá dalších funkcí pro kontrolu argumentů (typy, existence proměnných, validní hodnoty) a takové instrukce jsou zároveň v této funkci zpracováný.
