typedef unsigned char byte;         // 2^8 - 8 bit
typedef unsigned short int word;    // 2^16 - 16 bit
typedef word Adress;                // 2^16 - 64KB

#define MEMSIZE (64*1024)
#define ODATA 0177566
#define OSTAT 0177564
#define pc reg[7]

byte mem[MEMSIZE];
word reg[8];                        // R0, R1 ... R7, R7 = pc
byte trac;

void b_write(Adress adr, byte b);
byte b_read(Adress adr);
void w_write(Adress adr, word w);
word w_read(Adress adr);
void load_file(const char* filename);
void mem_dump(Adress start, word n);
