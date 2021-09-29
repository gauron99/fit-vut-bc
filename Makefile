
.PHONY: all simple-one-unit-big-boss

MAIN=secret.cpp
OUT_BIN=secret

all: simple-one-unit-big-boss
	


simple-one-unit-big-boss: secret.cpp secret.h
	g++ $(MAIN) -o $(OUT_BIN)

run:
	./secret