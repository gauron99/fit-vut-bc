## Readme IPK packet sniffer 2020 
### Author: David Fridrich

Popis programu:

	IPK packet sniffer je program, který odchytává, sbírá a zaznamenává
	(sniffuje) packety, které prochází skrz počítačovou síť dle zvoleného
	 zařízení. Podporovány jsou UDP a TCP packety internetového protokolu č. 4 (IPv4) a všechny ostatní jsou ignorovány.

Argumenty programu:
	
	'-i [dev]'  =>  vyber rozhraní na kterém chceš poslouchat
	'-p [port]' =>  vyber specifický port
	'-n [num]' => num je počet packetů, které chceš zachytit
	'-t / --tcp' => zobraz pouze TCP pakety
	'-u / --udp' => zobraz pouze UDP pakety
	'-h / --help' => vypíše help message

Příklady použití:

	./ipk-sniffer -i (rozhraní) [-p ­­_port_] [--tcp|-t] [--udp|-u] [-n _num_]
	./ipk-sniffer 
	./ipk-sniffer -i eth0
	./ipk-sniffer -i eth0 -p 80
	./ipk-sniffer -i eth0 -p 80 --tcp -n -1
	
Doplňující informace:	

	- Parametr -i může být zadán pouze jeden (tedy je možné poslouchat pouze na jednom zařízení v jednu chvíli) 
	- Pokud je program spuštěn bez argumentu, pouze vypíše všechna možná zařízení a úspěšně skončí.
	- Pokud je přes parametr -n zadáno záporné číslo (výše v příkladech použití) bude program chytat pakety do nekonečna (dokud nebude např. uživatelem ukončen).

Seznam soubor:

	- ipk-sniffer.cpp
	- ipk-sniffer.h
	- makefile
	- dokument.pdf