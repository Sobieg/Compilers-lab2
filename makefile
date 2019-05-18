CXX=g++
CCXFLAGS=-ggdb
LEX=flex
LFLAGS=-d
YACC=yacc
YFLAGS=-d -v -t
RM=rm -f
PY=python3
RESULTDIR=results\\
TESTSDIR=tests\\
ETDIR=etalon\\
GIT=git

TARGET=compile

.PHONY: tests external_headers


all: $(TARGET)


clean clear: 
	-$(RM) *.c *.h *.out*

lex.yy.c: 2.lex
	$(LEX) $(LFLAGS) 2.lex

y.tab.c: 2.yacc
	$(YACC) $(YFLAGS) 2.yacc


external_headers: 2_structs.hpp 2_ex.hpp
# 	$(RM) 2_id_table.hpp.gch
# 	$(RM) 2_structs.hpp.gch	
#	$(CXX) 2_id_table.hpp
	$(CXX) 2_structs.hpp
	$(CXX) 2_ex.hpp

git:
	$(GIT) checkout everymake-commit
	$(GIT) add .
#	$(GIT) commit -am "make commit on $(shell date)"

compile: lex.yy.c y.tab.c y.tab.h external_headers git 
	$(CXX) lex.yy.c y.tab.c

tests: 
	-$(RM) $(RESULTDIR)*.result
	-$(RM) errors.log
	$(PY) autotests.py

etupd: tests
	-$(RM) errors.log
	-$(RM) $(ETDIR) 
