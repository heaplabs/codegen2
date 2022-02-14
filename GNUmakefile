
OBJS= build/sql.tab.o build/sql-lex.yy.o build/driver.o \
      build/common_defs.o build/top_sort.o
SRC=sql.y sql-lex.l
CC=g++ -g -O0 -std=c++17

.PHONY: clean

code_gen.exe: $(OBJS)
	$(CC) -o $@ $(OBJS) -lstdc++fs

build/driver.o: driver.cpp common_defs.h sql.tab.h
	$(CC) -std=c++11 -I. -c $< -o $@

build/common_defs.o: common_defs.cpp common_defs.h
	$(CC) -std=c++11 -I. -c $< -o $@

build/sql.tab.o: sql.tab.c sql.tab.h
	$(CC) -std=c++11 -I. -c $< -o $@

build/sql-lex.yy.o: sql-lex.yy.c  sql.tab.h sql-lex.l
	$(CC) -c $< -o $@

sql.tab.h sql.tab.c: sql.y GNUmakefile common_defs.h
	bison --defines sql.y

sql-lex.yy.c: sql-lex.l common_defs.h
	flex --backup -o $@ $<

build/top_sort.o: top_sort.cpp graph.h 
	$(CC) -c $< -o $@

clean:
	rm build/*.o build/sql.tab.[hc] build/sql-lex.yy.c build/*.exe

