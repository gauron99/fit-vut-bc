# ISA-FIT-VUT
project of ISA @ FIT BUT, Brno

it takes a lot ~

Variant: Prenos souboru skrz skryty kanal
deadline: 15-11-2021

CZ_Popis:
Vytvořte klient/server aplikaci, která umožní přenést soubor skrz skrytý kanál, kde data jsou přenášena uvnitř ICMP Echo-Request/Response zpráv. Soubor musí být před přenosem zašifrován, aby nebyl přenášen v textové podobě.

EN_Description:
Create a client/server app which allowes to transfer a file through hidden channel where said data is being transfered through ICMP Echo-Req/Res messages.
File has to be encrypted before transfer (so its not in text form during it)

# implementation info
pokud je dan volitelny parametr '-l', ignoruji se vsechny ostatni parametry
program musi byt spousten s "root privileges" (ex: sudo) kvuli pouziti SOCK_RAW


## files needed
- main, makefile, doku, readme, extra (speci. by variant)

# README struct
  krátký textový popis programu s případnými rozšířeními/omezeními, příklad spuštění a seznam odevzdaných souborů

# Docu STRUCT
  Bude obsahovat: uvedení do problematiky, návrhu aplikace, popis implementace, základní informace o programu, návod na použití.

  ## Titulni strana
  one
  ## obsah
  twi
  ## logicke strukturovani textu
  three
  ## orehled nastudovanych informaci z liter
  four
  ## popis zajimavejsich pasazi implementace
  five
  ## pouziti vytvorenych programu a literatura
  six

Segfault nelze.

body: x/20 (15 impl, 5 docu)

### sidenotes
  - catch all packets, filter after
  OR
  - catch specifically what I want

  https://en.wikipedia.org/wiki/List_of_IP_protocol_numbers
  list of the IP protocol numbers found in the field Protocol of the IPv4 header
  and the Next Header field of the IPv6 header