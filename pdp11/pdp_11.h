typedef unsigned char byte;         // 2^8 - 8 bit
typedef unsigned short int word;    // 2^16 - 16 bit
typedef word Adress;                // 2^16 - 64KB

typedef struct Argument {
    word val;    // значение аргумента - что
    Adress adr;  // адрес аргумента(номер регистра) - куда
    word res;    // результат
    word space;  // где: =REG - в регистре, иначе по адресу
}Argument;

typedef struct Flag {
    char N;    // = 1, если результат команды <0, иначе 0 (знаковый бит)
    char Z;    // = 1, если результат команды 0, иначе = 0
    char V;    // дополнительный 16-й бит, отвечает за переполнение
    char C;    // отвечает за знаковое переполнение(если после операции из '+'числа получилось '-'число)
}Flag;

typedef struct P_Command {
    int B;        // старший бит в слове
    word r1;      // регистр ss
    word r2;      // регистр dd
}P_Command;

typedef struct Command {
    word mask;
    word opcode;
    const char * name;
    void (*do_func)(struct P_Command PC);
    word param;
}Command;


#define MEMSIZE (64*1024)
#define ODATA 0177566               // регистр данных дисплея
#define OSTAT 0177564               // регистр состояния дисплея
#define pc reg[7]                   // R7 = pc
#define sp reg[6]
#define NO_PARAM	0
#define HAS_SS		1
#define HAS_DD		2
#define HAS_NN		4
#define HAS_XX		8
#define REG 0
#define MEM 1

byte mem[MEMSIZE];
word reg[8];             // R0, R1 ... R7, R7 = pc
char do_trace;           // -t -T трассировка


void b_write(Adress adr, byte b);
byte b_read(Adress adr);
void w_write(Adress adr, word w);
word w_read(Adress adr);
void trace(const char* format, ...);
void load_file(const char* filename);
void mem_dump(Adress start, word n);
void print_registers();
void print_flags();
word byte_to_word(byte b);
Argument get_nn(word w);
Argument get_mr(word w, int B);
char get_xx(word w);
void run();

