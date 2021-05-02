#include <stdlib.h>
#include "pdp_11.h"

extern Argument ss, dd, nn;
extern char xx;

typedef struct Flag {
    char N;    // = 1, если результат команды <0, иначе 0 (знаковый бит)
    char Z;    // = 1, если результат команды 0, иначе = 0
    char V;    // дополнительный 16-й бит, отвечает за переполнение
    char C;    // отвечает за знаковое переполнение(если после операции из '+'числа получилось '-'число)
}Flag;
Flag flag;

void change_flag(struct P_Command PC) {

    if (PC.B) {
        flag.N = (char)((dd.res >> 7) & 1);           // если байт
    }
    else {
        flag.N = (char)((dd.res >> 15) & 1);          // если слово
    }
    flag.Z = (char)(dd.res == 0);
}


void do_halt(P_Command PC) {
    trace("\n\n------------------------THE END!!!------------------------\n");
    print_registers();
    exit(0);
}

void do_mov(P_Command PC) {                       // положить число по адресу
    dd.res = ss.val;
    if(dd.space == REG)                           // если мода равна 0, то
        reg[dd.adr]= dd.res;                      // значение записываю в рeгистр,
    else
        w_write(dd.adr, dd.res);                  // иначе по адресу
    change_flag(PC);
}

void do_add(P_Command PC) {                       // сложить два числа и результат записать по адресу последнего
    dd.res = dd.val + ss.val;
    if (dd.space == REG) {                        // если мода равна 0, то
        reg[dd.adr] = dd.res;                     // значение записываю в рeгистр,
    }
    else {
        w_write(dd.adr, dd.res);                  // иначе по адресу
    }
    change_flag(PC);
}

void do_sob(P_Command PC) {                       // Subtract One and Branch
    if (--reg[nn.adr] != 0)                       // если содержимое регистра при уменьшении на один != 0, то
        pc -= 2 * nn.val;                         // идити по адресу со сдвигом назад в nn
}

void do_br(P_Command PC) {                        // Branch
    pc += 2 * xx;                                 // PC=PC+2*XX
}

void do_beq(P_Command PC) {                       // Branch if Equal
    if (flag.Z == 1)
        do_br(PC);
}

void do_movb(P_Command PC) {                      // Move byte
    dd.res = ss.val;
    if (dd.space == REG)
        reg[dd.adr] = byte_to_word(dd.res);
    else
        b_write(dd.adr, (byte)dd.res);
    change_flag(PC);
}

void do_clr(P_Command PC) {
    dd.res = dd.val = 0;
    if(dd.space == REG)
        reg[dd.adr] = dd.res;
    else
        w_write(dd.adr, dd.res);

    flag.N = 0;
    flag.Z = 1;
    change_flag(PC);
}

void do_tstb(P_Command PC) {
    dd.res = dd.val;
    change_flag(PC);
}

void do_tst(P_Command PC) {
    dd.res = dd.val;
    change_flag(PC);
}

void do_bpl(P_Command PC) {                       // Branch if Plus
    if(flag.N == 0)
        do_br(PC);
}

void do_dec(P_Command PC) {                       // Decrement
    dd.val--;
    change_flag(PC);
    if (dd.space == REG)
        reg[dd.adr] = dd.val;
    else
        w_write(dd.adr, dd.val);
}

void do_jsr(P_Command PC) {                       // Jump to Subroutine
    w_write(sp, reg[PC.r1]);
    sp -= 2;
    reg[PC.r1] = pc;
    pc = dd.adr;
}

void do_rts(P_Command PC) {                       // Return from Subroutine
    pc = reg[PC.r2];
    sp += 2;
    reg[PC.r2] = w_read(reg[6]);
}


const Command commands[] = {
        {0170000, 0010000, "mov",  do_mov,  HAS_SS},
        {0170000, 0060000, "add",  do_add,  HAS_SS},
        {0177000, 0077000, "sob",  do_sob,  HAS_NN},
        {0017700, 0005000, "clr",  do_clr,  HAS_DD},
        {0xFF00,  0000400, "br",   do_br,   HAS_XX},
        {0xFF00,  0001400, "beq",  do_beq,  HAS_XX},
        {0170000, 0110000, "movb", do_movb, HAS_SS},
        {0177700, 0105700, "tstb", do_tstb, HAS_DD},
        {0xFF00,  0100000, "bpl",  do_bpl,  HAS_XX},
        {0177700, 0005700, "tst",  do_tst,  HAS_DD},
        {0177700, 0005300, "dec",  do_dec,  HAS_DD},
        {0177000, 0004000, "jsr",  do_jsr,  HAS_DD},
        {0177770, 0000200, "rts",  do_rts,  HAS_DD},
        {0xFFFF,  0,       "halt", do_halt, NO_PARAM},
};
int size = sizeof(commands)/sizeof(Command);