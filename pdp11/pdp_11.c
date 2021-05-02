#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "pdp_11.h"


#define SIGN(w, is_byte) (is_byte ? ((w)>>7)&1 : ((w)>>15)&1 )   // вычисляю старший(знаковый) бит для слова или байта
extern Flag flag;

void trace(const char* format, ...) {
    if (do_trace == 1 || do_trace == 2) {
        va_list ap;
        va_start(ap, format);
        vprintf(format, ap);
        va_end(ap);
    }
}

byte b_read(Adress adr) {
    return mem[adr];
}

void b_write(Adress adr, byte b) {
    if(adr == ODATA) {
        fprintf(stderr, "%c", b);
    }
    else
        mem[adr] = b;
}

word w_read(Adress a) {
    word w = ((word)mem[a+1]) << 8;
    w = w | mem[a];
    return w;
}

void w_write(Adress adr, word w) {
    if(adr == ODATA) {
        fprintf(stderr, "%c", w);
        return;
    }
    mem[adr] = (byte)(w & 0xFF);                  // w % 256
    mem[adr + 1] = (byte)((w >> 8) & 0xFF);       // w / 256
}

word byte_to_word(byte b) {
    word w;
    if (SIGN(b, 1) == 0) {       // для положительного числа
        w = 0;                          // 0000000000000000
        w |= b;
    }
    else {                             // для отрицательного числа
        w = ~0xFF;                     // 1111111100000000
        w |= b;
    }
    return w;
}

void mem_dump(Adress start, word n) {
    printf("----------------mem_dump---------------\n");
    for (int i = 0; i <= n; i += 2)
        printf("%06o : %06o\n", start + i , w_read(start + i));
    printf("----------------mem_dump---------------\n");
}

void load_file(const char * filename) {
    FILE * fin = fopen(filename, "r");

    if (fin == NULL) {
        perror(filename);
        exit(1);
    }

    unsigned int val, n, adr;

    while (fscanf(fin, "%x%x", &adr, &n) == 2) {
        for(unsigned int i = 0; i < n; i++) {
            fscanf(fin, "%x", &val);
            b_write(adr + i, val);
        }
    }

    mem_dump(01000, n);
    fclose(fin);
}

void print_registers() {
    for(int i = 0; i < 8; i += 2)
        printf("r%d = %06o   ", i, reg[i]);
    printf("\n");
    for(int i = 1; i < 8; i += 2)
        printf("r%d = %06o   ", i, reg[i]);
    printf("\n\n");
}

void print_flags() {
    trace("N = %d\n Z= %d\n C= %d\n", flag.N, flag.Z, flag.C);
}

void test_mem() {
    //пишем байт, читаем байт
    byte b0 = 0x0a;
    b_write(2, b0);
    byte bres1 = b_read(2);
    printf("%02hhx = %02hhx\n", b0, bres1);
    assert(b0 == bres1);

    //пишем слово читаем слово
    word w1 = 0x0a0b;
    w_write(0, w1);
    word wres1 = w_read(0);
    assert(wres1 == w1);

    //пишем 2 байта, читаем слово
    Adress a = 4;
    byte b1 = 0x0b;
    b0 = 0x0a;
    word w = 0x0b0a;
    b_write(a, b0);
    b_write(a + 1, b1);
    word wres2 = w_read(a);
    printf("ww/br \t %04hx=%02hhx%02hhx\n", wres2, b1, b0);
    assert(w == wres2);

    //пишем слово, читаем 2 байта
    word w0 = 0x0a0b;
    w_write(2, w0);
    b0 = b_read(2);
    b1 = b_read(3);
    printf("%04hx\n%02hhx%02hhx\n", w0, b1, b0);
    assert(b0 == (byte)0x0b);
    assert(b1 == (byte)0x0a);

}

int main(int argc, char * argv[]) {
    mem[OSTAT] = -1;                              // регистр состояния дисплея

    if (argc == 1) {                              // если введен лишь запуск программы без файла
        printf("Usage: %s [options] initial-core-file.\n"
               "\t-t\tshow trace to stderr\n"
               "\t-T\tshow verbose trace to stderr\n", argv[0]);
        exit(1);
    }
    else if (!strcmp(argv[1], "-t")) {
        do_trace = 1;
        load_file(argv[2]);
    }
    else if (!strcmp(argv[1], "-T")) {
        do_trace = 2;
        load_file(argv[2]);
    }
    else {
        load_file(argv[1]);
    }

    run();

    return 0;
}