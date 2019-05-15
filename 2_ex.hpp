#include <iostream>
#include <fstream>
#include "2_structs.hpp"
//include "2.tab.hacc" //c++
#include "y.tab.h" //c

extern int lbl;


void outp(const char* what) {
	std::ofstream output;
	output.open("2_asm.txt", std::ofstream::app);
	if (!output.is_open()) {
    	std::cerr << "Problem with file" << std::endl;
    	exit(1);
    }
	output << what << std::endl;
	output.close();
}
void lable(int lbl_op) {
	std::string str = std::to_string(lbl_op);
	str += ":";
	outp(str.c_str());
}
void jz(int lbl) {
	std::string str = "\tjz\t";
	str += std::to_string(lbl);
	outp(str.c_str());
}
void jmp(int lbl) {
	std::string str = "\tjmp\t";
	str += std::to_string(lbl);
	outp(str.c_str());
}
void add() {
	outp("\tadd\t");
}
void sub() {
	outp("\tsub\t");
}
void mul() {
	outp("\tmul\t");
}
void dv() {
	outp("\tdv\t");
}
void mod() {
	outp("\tmod\t");
}
void compLT() {
	outp("\tcompLT\t");
}
void compGT() {
	outp("\tcompGT\t");
}
void compGE() {
	outp("\tcompGE\t");
}
void compLE() {
	outp("\tcompLE\t");
}
void compNE() {
	outp("\tcompNE\t");
}
void compEQ() {
	outp("\tcompEQ\t");
}
void load(char* name) {
	//некрасиво, но что поделать...
	std::string str = "\tpop\t";
	//str += "[";
	str += name;
	//str += "]";
	outp(str.c_str());
}
void push(char* what) {
	std::string str = "\tpush\t";
	str += "[";
	str += what;
	str += "]";
	outp(str.c_str());
}
void push(int what) {
	std::string str = "\tpush\t";
	str += std::to_string(what);
	outp(str.c_str());
}
void print() {
	outp("\tprint\t");
}
void ret() {
	outp("\tret\t");
}
void lnot() {
	outp("\tlnot\t");
}
void inc(char* name) {
	push(name);
	push(1);
	add();
	load(name);
	push(name);
}
void dec(char* name) {
	push(name);
	push(1);
	sub();
	load(name);
	push(name);
}




int ex(nodeType *p) {
	int lbl1 = 0, lbl2 = 0;

/*DEBUG */
	if(yydebug) {
		std::cout << "Now ex " << p <<" <<<<<<<<<<" << std::endl;
		//std::cout << "type: " << p->type << std::endl;
		switch(p->type){
			case typeCon:
				std::cout << "type: typeCon" << std::endl;
				std::cout << "con.value: " << p->con.value << std::endl;
				break;
			case typeId:

				std::cout << "type: typeId" << std::endl;
				std::cout << "name: " << p->id.id.name << std::endl;
				break;
			case typeOpr:
				std::cout << "type: typeOpr" << std::endl;
				break;
		}
	}
/*
*/




	if (!p) {
		return 0;
	}
	switch(p->type) {
		case typeCon:
			push(p->con.value);
			break;
		case typeId:
			//обязательно надо сделать проверку на инкремент и декремент.
			switch(p->id.ch){
				case no:
					push(p->id.id.name);
					break;
				case preinc:
					inc(p->id.id.name);
					break;
				case predec:
					dec(p->id.id.name);
					break;
				case postinc:
					inc(p->id.id.name);
					break;
				case postdec:
					dec(p->id.id.name);
					break;
				default:
					std::cerr << "Something went wrong" << std::endl;

			}
			
			break;
		case typeOpr: {
			switch(p->opr.oper) {
			case WHILE:
				if (p->opr.nops == 2){ 
					/*while(expr) stmt;*/
					lbl1 = lbl;
					lbl = lbl+1;
					//std::cout << "lbl1 = " << lbl1 << std::endl;
					lable(lbl1);
					ex(p->opr.op[0]); //прверка условия
					lbl2 = lbl++;
					//std::cout << "lbl2 = " << lbl2 << std::endl;
					jz(lbl2);
					ex(p->opr.op[1]);
					jmp(lbl1);
					lable(lbl2);
					break;
				}
				else {
					/*while(expr);*/
					lbl1 = lbl++;
					lable(lbl1);
					ex(p->opr.op[0]);
					lbl2 = lbl++;
					jz(lbl2);
					jmp(lbl1);
					lable(lbl2);
					break;
				}
			case IF:
				ex(p->opr.op[0]);
				if(p->opr.nops == 3) {
					/*if (expr) stmt else stmt*/
					lbl1 = lbl++;
					jz(lbl1);
					ex(p->opr.op[1]);
					lbl2 = lbl++;
					jmp(lbl2);
					lable(lbl1);
					ex(p->opr.op[2]);
					lable(lbl2);
					break;
				}
				else if (p->opr.nops == 4) { 
					/*if(expr) else stmt*/
					lbl1 = lbl++;
					jz(lbl1);
					lbl2 = lbl++;
					jmp(lbl2);
					lable(lbl1);
					ex(p->opr.op[1]);
					lable(lbl2);
					break;
				}
				else if (p->opr.nops == 2) {
					/*if(expr) stmt*/
					//std::cout << "THERE" << std::endl;
					lbl1 = lbl++;
					jz(lbl1);
					ex(p->opr.op[1]);
					lable(lbl1);
					break;
				}
				else {
					/*if(expr);
					  if(expr) else;*/
					/*Тут ничего выполнять не надо 
					(потому что выполнили перед свитчом),*/
					break;
				}
			case PRINT:
			/*print(expr);*/
				ex(p->opr.op[0]);
				print();
				break;
			case RETURN:
				if (p->opr.nops == 0){
					ret();
					break;
				}
				else {
					ex(p->opr.op[0]);
					ret();
					break;
				}
			case '=':
			/*varaible = expr2*/
				//std::cout << "want to ex " << p->opr.op[1] << std::endl;
				ex(p->opr.op[1]);
				load(p->opr.op[0]->id.id.name);
				break;
			case '-':
				if(p->opr.nops == 1){
					/*-var*/ 
					ex(p->opr.op[0]);
					push(-1);
					mul();
					break;
				}
				else {
					ex(p->opr.op[0]);
					ex(p->opr.op[1]);
					sub();
					break;
				}
			case '!':
				ex(p->opr.op[0]);
				lnot(); //логическое не
				break;
			case 0:
				exit(0);
				break;
			default:
				ex(p->opr.op[0]);
				ex(p->opr.op[1]);
				switch(p->opr.oper){
					case PLUS_ASS: add(); break;
					case MIN_ASS: sub(); break;
					case DIV_ASS: dv(); break;
					case MOD_ASS: mod(); break;
					case MULT_ASS: mul(); break;
					case EQ: compEQ(); break;
					case NOTEQ: compNE(); break;
					case MORE_EQ: compGE(); break;
					case LESS_EQ: compLE(); break;
					case '<': compLT(); break;
					case '>': compGT(); break;
					case '+': add(); break;
					case '*': mul(); break;
					case '/': dv(); break;
					case '%': mod(); break;
					case ';': break;
					//default: std::cerr << "something went wrong: " << p->opr.oper<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<< std::endl;
				}
			}
		}
	}

	return 0;
}