###------------------------------
### Main rules 
###------------------------------------------------------------
core :core.o ordonnanceur.o
	gcc -Wall -ansi -m32 -pedantic -g -o core core.o ordonnanceur.o -L./lib/core -lhardware -pthread

main :main.o gest_contexte.o shell.o drive.o volumes.o
	gcc -Wall -ansi -m32 -pedantic -g -o main main.o gest_contexte.o shell.o -L./lib -lhardware


ordonnanceur.o : ordonnanceur.c ordonnanceur.h
	gcc -Wall -m32 -std=gnu99 -g -c ordonnanceur.c
core.o : core.c core.h
	gcc -Wall -ansi -m32 -pedantic -g -c core.c
main.o :main.c gest_contexte.h shell.h
	gcc -Wall -ansi -m32 -pedantic -g -c main.c 
gest_contexte.o : gest_contexte.c gest_contexte.h
	gcc -Wall -std=gnu99 -m32 -g -c gest_contexte.c
shell.o : shell.c shell.h
	gcc -Wall -std=gnu99 -m32 -g -c shell.c
drive.o : drive.c drive.h
	gcc -m32 -g -c drive.c
volumes.o : volumes.c volumes.h
	gcc -Wall -std=gnu99 -m32 -g -c volumes.c
###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean realclean depend
clean:
	$(RM) *.o 
realclean: clean 
	$(RM) main
