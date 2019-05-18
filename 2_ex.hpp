#include <iostream>
#include <fstream>
#include "stdlib.h"
#include "2_structs.hpp"
#include "y.tab.h" //c
#include <stack>



#define NUM_OF_REGISTERS 16




typedef struct register_struct {
	int value;
	std::string name;
	std::string var_name;
	bool is_free;

public:
	void free() {
		is_free = true;
		value = INT_MAX;
		var_name.clear();
	}
} register_struct;

struct register_list {
	register_struct* lastUsed;
	register_list* preLastUsed;
};

register_struct cpu[NUM_OF_REGISTERS];
register_struct nilreg;
// register_list* lu;
extern int lbl;


std::stack<register_struct*> lu;

/**
TODO:
1) сделать выбор свободного регистра -- done
2) очистка регистров в операциях, когда два регистра или выгрузка в память. -- done
3) указатели на регистры -- ПРОВЕРИТЬ ЧТО РАБОТАЕТ ХЗ КАК -- done 
4) НАХОДИТЬ В КАКОМ РЕГИСТРЕ ЛЕЖИТ НАШЕ ЗНАЧЕНИЕ И В add и типа того изменять этот регистр, а не че попало
**/

void outp(const char* what);
inline void mov(const char* mem1, register_struct* reg2);


register_struct* getFreeReg() { 
	for (int i = 0; i<NUM_OF_REGISTERS; i++) {
		if (cpu[i].is_free) {
			cpu[i].is_free = false;
			return &cpu[i];
		}
	}
	if (yydebug) {
		std::cerr << "Not enough registers" << std::endl;
	}
	return &nilreg;
}

register_struct* getRegByVarName(const char* varName, register_struct* defReg) {
	for (int i = 0; i<NUM_OF_REGISTERS; i++){
		if (cpu[i].var_name == varName) {
			if (yydebug) {
				outp("Found name");
			}
			return &cpu[i];
		}
	}
	return defReg;
}


//функция аккуратного вывода в файл чего угодно
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

//функция аккуратного вывода в файл метки
void lable(int lbl_op) {
	std::string str = std::to_string(lbl_op);
	str += ":";
	outp(str.c_str());
}

void load(register_struct* reg) {
	if (!reg->var_name.empty()) {
		mov(reg->var_name.c_str(), reg);
	}
}



inline register_struct* mov(register_struct* reg1, register_struct* reg2) { // op1 -- куда, op2 -- откуда
	if(yydebug) {
		std::cout << "moving " << reg2->name << " to " << reg1->name << std::endl;
	}
	std::string str = "\tmov\t";
	str += reg1->name;
	str += ", ";
	str += reg2->name;
	outp(str.c_str());
	return reg1;
}
inline void mov(const char* mem1, register_struct* reg2) {
	if(yydebug) {
		std::cout << "moving " << reg2->name << " to [" << mem1 << "]" << std::endl;
	}
	std::string str = "\tmov\t";
	str += mem1;
	str += ", ";
	str += reg2->name;
	outp(str.c_str());
	reg2->free();
}
inline register_struct* mov(register_struct* reg1, int num) {
	if(yydebug) {
		std::cout << "moving " << num << " to " << reg1->name << std::endl;
	}
	std::string str = "\tmov\t";
	str += reg1->name;
	str += ", ";
	str += std::to_string(num);
	outp(str.c_str());
	return reg1;
}
inline register_struct* mov(register_struct* reg1, const char* mem2) {
	register_struct* reg = getRegByVarName(mem2, reg1);
	if(reg != reg1) {
		reg1->free();
		return reg;
	}
	if(yydebug) {
		std::cout << "moving [" << mem2 << "] to " << reg->name << std::endl;
	}
	std::string str = "\tmov\t";
	str += reg->name;
	str += ", [";
	str += mem2;
	str += "]";
	outp(str.c_str());
	reg->var_name = mem2;
	return reg;
}
void jz(int lbl_p) {
	std::string str = "\tjz\t";
	str += std::to_string(lbl_p);
	outp(str.c_str());
}
void jmp(int lbl_p) {
	std::string str = "\tjmp\t";
	str += std::to_string(lbl_p);
	outp(str.c_str());
}
inline register_struct* add(register_struct* reg1, int num) {
	std::string str = "\tadd\t";
	str += reg1->name;
	str += ", ";
	str += std::to_string(num);
	outp(str.c_str());
	return reg1;
}
inline register_struct* add(register_struct* reg1, register_struct* reg2) {
	std::string str = "\tadd\t";
	str += reg1->name;
	str += ", ";
	str += reg2->name;
	outp(str.c_str());
	reg2->free();
	return reg1;
}
inline register_struct* add(register_struct* reg1, const char* mem2) {
	std::string str = "\tadd\t";
	str += reg1->name;
	str += ", [";
	str += mem2;
	str += "]";
	outp(str.c_str());
	return reg1;
}
inline register_struct* sub(register_struct* reg1, int num) {
	std::string str = "\tsub\t";
	str += reg1->name;
	str += ", ";
	str += std::to_string(num);
	outp(str.c_str());
	return reg1;
}
inline register_struct* sub(register_struct* reg1, register_struct* reg2) {
	std::string str = "\tsub\t";
	str += reg1->name;
	str += ", ";
	str += reg2->name;
	outp(str.c_str());
	reg2->free();
	return reg1;
}
inline register_struct* sub(register_struct* reg1, const char* mem2) {
	std::string str= "\tsub\t";
	str += reg1->name;
	str += ", [";
	str += mem2;
	str += "]";
	outp(str.c_str());
	return reg1;
}
inline register_struct* mul(register_struct* reg1, int num) {
	std::string str = "\tmul\t";
	str += reg1->name;
	str += ", ";
	str += std::to_string(num);
	outp(str.c_str());
	return reg1;
}
inline register_struct* mul(register_struct* reg1, register_struct* reg2) {
	std::string str = "\tmul\t";
	str += reg1->name;
	str += ", ";
	str += reg2->name;
	outp(str.c_str());
	reg2->free();
	return reg1;
}
inline register_struct* mul(register_struct* reg1, const char* mem2) {
	std::string str = "\tmul\t";
	str += reg1->name;
	str += ", [";
	str += mem2;
	str += "]";
	outp(str.c_str());
	return reg1;
}
inline register_struct* dv(register_struct* reg1, int num) {
	std::string str = "\tdiv\t";
	str += reg1->name;
	str += ", ";
	str += std::to_string(num);
	outp(str.c_str());
	return reg1;
}
inline register_struct* dv(register_struct* reg1, register_struct* reg2) {
	std::string str = "\tdiv\t";
	str += reg1->name;
	str += ", ";
	str += reg2->name;
	outp(str.c_str());
	reg2->free();
	return reg1;
}
inline register_struct* dv(register_struct* reg1, const char* mem2) {
	std::string str = "\tdiv\t";
	str += reg1->name;
	str += ", [";
	str += mem2;
	str += "]";
	outp(str.c_str());
	return reg1;
}
inline register_struct* mod(register_struct* reg1, int num) {
	std::string str = "\tmod\t";
	str += reg1->name;
	str += ", ";
	str += std::to_string(num);
	outp(str.c_str());
	return reg1;
}
inline register_struct* mod(register_struct* reg1, register_struct* reg2) {
	std::string str = "\tmod\t";
	str += reg1->name;
	str += ", ";
	str += reg2->name;
	outp(str.c_str());
	reg2->free();
	return reg1;
}
inline register_struct* mod(register_struct* reg1, const char* mem2) {
	std::string str = "\tmod\t";
	str += reg1->name;
	str += ", [";
	str += mem2;
	str += "]";
	outp(str.c_str());
	return reg1;
}
register_struct* compLT(register_struct* reg1, register_struct* reg2) {
	std::string str = "\tcompLT\t";
	str += reg1->name;
	str += ", ";
	str += reg2->name;
	outp(str.c_str());
	reg2->free();
	return reg1;
}
register_struct* compGT(register_struct* reg1, register_struct* reg2) {
	std::string str = "\tcompGT\t";
	str += reg1->name;
	str += ", ";
	str += reg2->name;
	outp(str.c_str());
	reg2->free();
	return reg1;
}
register_struct* compGE(register_struct* reg1, register_struct* reg2) {
	std::string str = "\tcompGE\t";
	str += reg1->name;
	str += ", ";
	str += reg2->name;
	outp(str.c_str());
	reg2->free();
	return reg1;
}
register_struct* compLE(register_struct* reg1, register_struct* reg2) {
	std::string str = "\tcompLE\t";
	str += reg1->name;
	str += ", ";
	str += reg2->name;
	outp(str.c_str());
	reg2->free();
	return reg1;
}
register_struct* compNE(register_struct* reg1, register_struct* reg2) {
	std::string str = "\tcompNE\t";
	str += reg1->name;
	str += ", ";
	str += reg2->name;
	outp(str.c_str());
	reg2->free();
	return reg1;
}
register_struct* compEQ(register_struct* reg1, register_struct* reg2) {
	std::string str = "\tcompEQ\t";
	str += reg1->name;
	str += ", ";
	str += reg2->name;
	outp(str.c_str());
	reg2->free();
	return reg1;
}
void print(register_struct* reg) {
	std::string str = "\tprint\t";
	str += reg->name;
	outp(str.c_str());
	reg->free();
}
void ret() {
	std::string str = "\tret\t";
	outp(str.c_str());
}
void ret(register_struct* reg) {
	std::string str = "\tret\t";
	str += reg->name;
	outp(str.c_str());
	reg->free();
}
void ret(int num) {
	std::string str = "\tret\t";
	str += std::to_string(num);
	outp(str.c_str());
}
register_struct* lnot(register_struct* reg) {
	std::string str = "\tlnot\t";
	str += reg->name;
	outp(str.c_str());
	return reg;
}
inline register_struct* inc(register_struct* reg1) {
	add(reg1, 1);
	load(reg1);
	return reg1;
}
inline register_struct* dec(register_struct* reg1) {
	sub(reg1, 1);
	load(reg1);
	return reg1;
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
			lu.push(mov(getFreeReg(), p->con.value));
			break;
		case typeId:
			//обязательно надо сделать проверку на инкремент и декремент.
			switch(p->id.ch){
				case no:
					lu.push(mov(getFreeReg(), p->id.id.name));
					break;
				case preinc:
					lu.push(inc(mov(getFreeReg(), p->id.id.name)));
					break;
				case predec:
					lu.push(dec(mov(getFreeReg(), p->id.id.name)));
					break;
				case postinc:
					lu.push(inc(mov(getFreeReg(), p->id.id.name)));
					break;
				case postdec:
					lu.push(dec(mov(getFreeReg(), p->id.id.name)));
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
				print(lu.top());
				lu.pop();
				break;
			case RETURN:
				if (p->opr.nops == 0){
					ret();
					break;
				}
				else {
					ex(p->opr.op[0]);
					ret(lu.top());
					lu.pop();
					break;
				}
			case '=':
			/*varaible = expr2*/
				//std::cout << "want to ex " << p->opr.op[1] << std::endl;
				ex(p->opr.op[1]);
				mov(p->opr.op[0]->id.id.name, lu.top());
				lu.pop();
				break;
			case '-':
				if(p->opr.nops == 1){
					/*-var*/ 
					ex(p->opr.op[0]);
					mul(lu.top(), -1); //тут не надо пушить и попать потому что эти действия как бы взаимоуничтожатся
					break;
				}
				else {
					ex(p->opr.op[0]);
					ex(p->opr.op[1]);
					register_struct* r1 = lu.top();
					lu.pop();
					register_struct* r2 = lu.top();
					lu.pop();
					lu.push(sub(r2, r1));
					break;
				}
			case '!':
				ex(p->opr.op[0]);
				lnot(lu.top()); //логическое не, тоже не надо пушить и попать потому что эти действия как бы взаимоуничтожатся
				break;
			case 0:
				exit(0);
				break;
			default:
				ex(p->opr.op[0]);
				ex(p->opr.op[1]);
				register_struct* r1 = lu.top();
				lu.pop();
				register_struct* r2 = lu.top();
				lu.pop();
				switch(p->opr.oper){
					case PLUS_ASS: lu.push(add(r2, r1)); break;
					case MIN_ASS: lu.push(sub(r2, r1)); break;
					case DIV_ASS: lu.push(dv(r2, r1)); break;
					case MOD_ASS: lu.push(mod(r2, r1)); break;
					case MULT_ASS: lu.push(mul(r2, r1)); break;
					case EQ: lu.push(compEQ(r2, r1)); break;
					case NOTEQ: lu.push(compNE(r2, r1)); break;
					case MORE_EQ: lu.push(compGE(r2, r1)); break;
					case LESS_EQ: lu.push(compLE(r2, r1)); break;
					case '<': lu.push(compLT(r2, r1)); break;
					case '>': lu.push(compGT(r2, r1)); break;
				 	case '+': lu.push(add(r2, r1)); break;
					case '*': lu.push(mul(r2, r1)); break;
					case '/': lu.push(dv(r2, r1)); break;
					case '%': lu.push(mod(r2, r1)); break;
					case ';': break;
					//default: std::cerr << "something went wrong: " << p->opr.oper<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<< std::endl;
				}
			}
		}
	}

	return 0;
}