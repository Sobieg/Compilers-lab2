CXX=g++
CXXFLAGS=-ggdb
LEX=flex
LFLAGS=-d
YACC=yacc
YFLAGS=-d -v -t
RM=rm -f

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
	$(CXX) $(CXXFLAGS) 2_structs.hpp
	$(CXX) $(CXXFLAGS) 2_ex.hpp

compile: lex.yy.c y.tab.c y.tab.h external_headers
	$(CXX) $(CXXFLAGS) lex.yy.c y.tab.c