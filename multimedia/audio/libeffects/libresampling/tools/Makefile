OBJ_DIR= obj/Linux
BINDIR = bin/Linux

all: $(BINDIR)/evaldist $(BINDIR)/convrt $(BINDIR)/singen $(BINDIR)/addsig $(BINDIR)/findmax $(BINDIR)/functgen $(BINDIR)/deintlv $(BINDIR)/diffmaxb $(BINDIR)/evdist $(BINDIR)/up_down $(BINDIR)/filter $(BINDIR)/filteri


$(BINDIR)/evaldist: $(OBJ_DIR)/evaldist.o $(OBJ_DIR)/cfuncts.o
	gcc -Wall -g -o $@ $^ -lm

$(BINDIR)/convrt: $(OBJ_DIR)/convrt.o $(OBJ_DIR)/cfuncts.o
	gcc -Wall -g -o $@ $^ -lm

$(BINDIR)/singen: $(OBJ_DIR)/singen.o
	gcc -Wall -g -o $@ $^ -lm

$(BINDIR)/addsig: $(OBJ_DIR)/addsig.o
	gcc -Wall -g -o $@ $^ -lm

$(BINDIR)/findmax: $(OBJ_DIR)/findmax.o
	gcc -Wall -g -o $@ $^ -lm

$(BINDIR)/deintlv: $(OBJ_DIR)/deintlv.o
	gcc -Wall -g -o $@ $^ -lm

$(BINDIR)/diffmaxb: $(OBJ_DIR)/diffmaxb.o
	gcc -Wall -g -o $@ $^ -lm

$(BINDIR)/functgen: $(OBJ_DIR)/functgen.o
	g++ -Wall -g -o $@ $^ -lm

$(BINDIR)/evdist: $(OBJ_DIR)/evdist.o  $(OBJ_DIR)/cfuncts.o
	gcc -Wall -g -o $@ $^ -lm

$(BINDIR)/up_down: $(OBJ_DIR)/up_down.o  
	gcc -Wall -g -o $@ $^ -lm

$(BINDIR)/filter: $(OBJ_DIR)/filter.o  
	gcc -Wall -g -o $@ $^ -lm

$(BINDIR)/filteri: $(OBJ_DIR)/filteri.o  
	gcc -Wall -g -o $@ $^ -lm



$(OBJ_DIR)/evaldist.o: evaldist.c $(OBJ_DIR)
	gcc -c -g -D__flexcc2__  -o $@ $<
$(OBJ_DIR)/cfuncts.o: cfuncts.c $(OBJ_DIR)
	gcc -c -g -D__flexcc2__  -o $@ $<
$(OBJ_DIR)/convrt.o: convrt.c $(OBJ_DIR)
	gcc -c -g -D__flexcc2__  -o $@ $<
$(OBJ_DIR)/singen.o: singen.c $(OBJ_DIR)
	gcc -c -g -D__flexcc2__  -o $@ $<
$(OBJ_DIR)/addsig.o: addsig.c $(OBJ_DIR)
	gcc -c -g -D__flexcc2__  -o $@ $<
$(OBJ_DIR)/findmax.o: findmax.c $(OBJ_DIR)
	gcc -c -g -D__flexcc2__  -o $@ $<
$(OBJ_DIR)/deintlv.o: deintlv.c $(OBJ_DIR)
	gcc -c  -g -o $@ $<
$(OBJ_DIR)/diffmaxb.o: diffmaxb.c $(OBJ_DIR)
	gcc -c -g -o $@ $<
$(OBJ_DIR)/functgen.o: functgen.cpp $(OBJ_DIR)
	g++ -c -g  -o $@ $<
$(OBJ_DIR)/evdist.o: evdist.c $(OBJ_DIR)
	gcc -c -g  -o $@ $<
$(OBJ_DIR)/up_down.o: up_down.c $(OBJ_DIR)
	gcc -c -g  -o $@ $<
$(OBJ_DIR)/filter.o: filter.c $(OBJ_DIR)
	gcc -c -g  -o $@ $<
$(OBJ_DIR)/filteri.o: filteri.c $(OBJ_DIR)
	gcc -c -g  -o $@ $<


$(OBJ_DIR):
	@mkdir -p $@
