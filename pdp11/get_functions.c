#include <stdlib.h>
#include <stdio.h>
#include "pdp_11.h"


Argument get_mr(word w, int B) {
    Argument res;
    int n_reg = w & 7;                                   // номер регистра
    int mode = (w >> 3) & 7;                             // номер моды
    word ww;

    switch(mode) {
        case 0:                                          // R3
            res.adr = n_reg;                             // номер регистра
            res.val = reg[n_reg];                        // значение в регистре
            trace("R%o ", n_reg);
            res.space = REG;                             // находится в регистре
            break;

        case 1:                                           // (R3)
            res.adr = reg[n_reg];
            res.val = B ? b_read(res.adr) : w_read(res.adr);//читаю байт/слово в зависимости от значения знакового бита
            trace("(R%d) ", n_reg);
            break;

        case 2:                                           // (R3)+  или  #3
            res.adr = reg[n_reg];
            res.val = B ? b_read(res.adr) : w_read(res.adr);

            if (n_reg == 7 || n_reg == 6 || B == 0) {     // если слово или если значение находится в R6/R7
                reg[n_reg] += 2;
            }
            else {
                reg[n_reg]++;                             // если байт
            }

            if (n_reg == 7) {                             // если адресс находится в pc = R7
                trace("#%o ", res.val);
            }
            else {
                trace("(R%o)+ ", n_reg);
            }
            break;

        case 3:                                           // @(R3)+  или  @#3
            res.adr = w_read(reg[n_reg]);                 // регистр содержит адрес операнда
            res.val = w_read(res.adr);
            reg[n_reg] += 2;

            if (n_reg == 7 || n_reg == 6 || B == 0) {
                trace("@#%o ", res.adr);           // @#3
            }
            else {
                trace("@(R%o)+", n_reg);           // @(R3)+
            }
            break;

        case 4:                                           // -(R3)
            if (n_reg == 7 || n_reg == 6 || B == 0) {
                reg[n_reg] -= 2;
                res.adr = reg[n_reg];
                res.val = w_read(res.adr);
            }
            else {
                reg[n_reg]--;
                res.adr = reg[n_reg];
                res.val = b_read(res.adr);
            }

            trace("-(R%d) ", n_reg);
            break;

        case 5:                                           // @-(R3)
            trace("@-(R%o)", n_reg);
            reg[n_reg] -= 2;
            res.adr = w_read(reg[n_reg]);                 // регистр содержит адрес операнда
            res.val = w_read(res.adr);
            break;

        case 6:                                           // nn - константа
            ww = w_read(pc);
            pc += 2;
            res.adr = reg[n_reg] + ww;                    // адрес в pc складывается с константой nn (сдвиг на nn)
            res.val = w_read(res.adr);                    // читаю значение по полученному адрессу
            if (n_reg == 7)
                trace("%o ", res.adr);
            else
                trace("%o(r%d) ", ww, n_reg);
            break;

        default:
            fprintf(stderr, "MODE %o NOT IMPLEMENTED YET!", mode);
            exit(1);
    }
    return res;
}


Argument get_nn(word w) {                                // 077RNN
    Argument res;
    res.adr = (w >> 6) & 07;                             // R - номер регистра
    res.val = w & 077;                                   // NN - на столько слов сдвиг назад в программе
    trace("R%d, %o\n", reg[res.adr], pc - 2 * res.val);
    return res;
}

char get_xx(word w) {                                    // 0010XX
    char res;
    res = (char)(w & 0xFF);                              // XX
    unsigned int x = pc + 2 * res;                       // адрес сдвигается на XX слов
    trace("%06o\n", x);
    return res;
}
