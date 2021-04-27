#include <stdio.h>
#include <stdlib.h>
#include "pdp_11.h"
#include "do_functions.c"


word reg[8];                                            // R0, R1 ... R7, R7
#define pc reg[7]                                       // R7 = pc

typedef struct Argument{
    word val;                                            // значение аргумента
    word adr;                                            // адрес аргумента
}Argument;

Argument get_mr(word w){
    Argument res;
    int n_reg = w & 7;                                   // номер регистра
    int mode = (w >> 3) & 7;                             // номер моды
    switch(mode) {
        case 0:                                          // R3
            res.adr = n_reg;
            res.val = reg[n_reg];
            trace("R%o ", n_reg);
            break;

        case 1:                                           // (R3)
            res.adr = n_reg;
            res.val = w_read(res.adr);                    // b_read
            trace("(R%o) ", n_reg);
            break;

        case 2:                                           // (R3)+  или  #3
            res.adr = n_reg;
            res.val = w_read(res.adr);                    // todo b_read
            reg[n_reg] += 2;
            if (n_reg == 7)                               // если адресс находится в pc = R7
                trace("#%o ", res.val);            // #3
            else
                trace("(R%o)+ ", n_reg);           // (R3)+

        case 3:                                           // @(R3)+  или  @#3
            res.adr = reg[n_reg];
            if (n_reg == 7 || n_reg == 6) {               // todo b_read
                res.adr = w_read((Adress) reg[n_reg]);
                res.val = w_read((Adress) res.adr);
                reg[n_reg] += 2;
                trace("@#%o ", res.adr);         // @#3
            }
            else {
                res.adr = w_read((Adress) reg[n_reg]);
                res.val = w_read((Adress) res.adr);
                reg[n_reg] += 2;
                trace("@(R%o)+", n_reg);         // @(R3)+
            }
            break;

        case 4:                                           // -(R3)
            res.adr = n_reg;
            res.val = w_read(res.adr);                    // todo b_read
            reg[n_reg] -= 2;
            trace("(R%o)+ ", n_reg);               // (R3)+

        case 5:                                           // @-(R3)
            res.adr = reg[n_reg];                         // todo b_read
            res.adr = w_read((Adress) reg[n_reg]);
            res.val = w_read((Adress) res.adr);
            reg[n_reg] -= 2;
            trace("@R%o ", n_reg);               // @#3

        default:
            fprintf(stderr, "MODE %o NOT IMPLEMENTED YET!\n", mode);
            exit(1);
    }
    return res;
}


void run(){
    Argument  ss, dd;
    pc = 01000;                                        // восьмиричные константы начинаются с 0
    while(1) {
        word w = w_read(pc);
        trace("%06o %06o: ", pc, w);
        pc += 2;
        if (w == 0){
            trace("halt ");
            do_halt();
        }
        else if ((w & 0170000) == 0010000 ) {          // - восьмиричная система счисления, 01SSDD
            trace("mov ");
            ss = get_mr(w >> 6);
            dd = get_mr(w);
            do_mov();
        }
        else if ((w & 0170000) == 0060000 ) {          // - восьмиричная система счисления, 01SSDD
            trace("add ");
            do_add();
        }

    }
}