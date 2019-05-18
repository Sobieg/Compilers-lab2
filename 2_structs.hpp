//код взят из книги LEX & YACC TUTORIAL by Tom Niemann и изменен и дополнен мной

#define SYMSIZE 1024

/* возможные типы нод */
typedef enum { typeCon, typeId, typeOpr } nodeEnum;
typedef enum { no, preinc, postinc, predec, postdec} idCh;

/* константы */
typedef struct {
    int value;                  /* значения */
} conNodeType;

/* идентификаторы */
typedef struct {
	char* name;					/* имя идентификатора */
} idNodeType;

typedef struct {
    idNodeType id;
    int value;
    idCh ch;
} idVar;

/* операторы */
typedef struct {
    int oper;                   /* оператор */
    int nops;                   /* количество операндов */
    struct nodeTypeTag *op[1];	/* операнды, увеличивается во время исполнения */
} oprNodeType;

/* возможные типы операндов  */
typedef struct nodeTypeTag { 
    nodeEnum type;              /* тип ноды */

    union {
        conNodeType con;        /* константа */
        idVar id;          /* идентификатор */
        oprNodeType opr;        /* оператор */
    };
} nodeType;


/* массви для храниния констант */
extern idVar sym[SYMSIZE];