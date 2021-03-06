#include <stdio.h>
#include "pdp_11.h"

extern Command commands[];
Argument ss, dd, nn;
char xx;

P_Command create_command(word w) {
    P_Command res;
    res.B = (w >> 15);
    res.r1 = (w >> 6) & 7;
    res.r2 = w & 7;
    return res;
}

void run() {
    extern int size_of_commands;
    P_Command PC;

    pc = 01000;
    word w;
    mem[OSTAT] |= 128;       // старший бит в младшем байте

    while(1) {
        w = w_read(pc);
        if (do_trace == 1 || do_trace == 2)
            printf("\t\t%06o : ", pc);

        pc += 2;
        PC = create_command(w);

        for (int i = 0; i < size_of_commands; i++)
            if ((commands[i].mask & w) == commands[i].opcode) {
                if (do_trace == 1 || do_trace == 2)
                    printf("%s    ", commands[i].name);

                if (commands[i].param == HAS_SS) {
                    ss = get_mr(w >> 6, PC.B);
                    dd = get_mr(w, PC.B);
                    //                    printf("\nss = %o, %o\n", ss.val, ss.adr);
                    //                    printf("dd = %o, %o\n", dd.val, dd.adr);
                }
                if (commands[i].param == HAS_DD) {
                    dd = get_mr(w, PC.B);
                    //                    printf("\ndd = %o, %o\n", dd.val, dd.adr);
                }
                if (commands[i].param == HAS_NN) {
                    nn = get_nn(w);
                    //                    printf("\nnn = %o, %o\n", nn.val, nn.adr);
                }
                if (commands[i].param == HAS_XX) {
                    xx = get_xx(w);
                    //                    printf("\nxx = %o\n", xx.val);
                }
                commands[i].do_func(PC);
                if (do_trace == 1 || do_trace == 2)
                    printf("\n");
            }
        if (do_trace == 2) {                  // -T -большая трассировка
            print_registers();
            print_flags();
        }
    }
}