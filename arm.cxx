#include "elf.h"
#include "mem.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
int main(int argc, char** argv) {
    if (argc != 2) {
        printf("usage: %s <ARM executable file name>\n",argv[0]);
        exit(-1);
    }
    uint64_t entry = loadElf(argv[1]);
    uint64_t PC = entry;
    while(!checkFailure())
    {
        string s = opcode(PC);
        if(s.substr(1,7)=="0110101")
        {   
            PC=CBNZ(s,PC);
        }
        else if(s[0]=='1'&&s.substr(3,5)=="10000")
        {
            //cout<<"ADRP CALLED"<<endl;
            ADRP(s,PC);
        }
        else if(s.substr(1,8)=="00100010")
        {
            ADDI(s);
        }
        else if(s[0]=='1'&&s.substr(2,9)=="111000010"&&s.substr(20,2)=="01")
        {
            LDRONE(s);
        }
        else if(s[0]=='1'&&s.substr(2,9)=="111000010"&&s.substr(20,2)=="11")
        {
            LDRTWO(s);
        }
        else if(s[0]=='1'&&s.substr(2,8)=="11100101")
        {
            LDRTHREE(s);
        }
        else if(s.substr(1,8)=="10100101")
        {
            MOVZ(s);
        }
        else if(s.substr(0,11)=="00111000010"&&s.substr(20,2)=="01")
        {
            LDRBONE(s);
        }
        else if(s.substr(0,11)=="00111000010"&&s.substr(20,2)=="11")
        {
            LDRBTWO(s);
        }
        else if(s.substr(0,10)=="0011100101")
        {
            LDRBTHREE(s);
        }
        else if(s.substr(1,8)=="01100100")
        {
            ORR(s);
        }
        else if(s.substr(0,11)=="00111000000"&&s.substr(20,2)=="01")
        {
            STRBONE(s);
        }
        else if(s.substr(0,11)=="00111000000"&&s.substr(20,2)=="11")
        {
            STRBTWO(s);
        }
        else if(s.substr(0,10)=="0011100100")
        {
            STRBTHREE(s);
        }
        else
        {
            fail(PC);
            return 0;
        }
        //fail(PC);
        PC +=4;
    }
    /*
    ADRP("10010000000000000000000010000000",PC);
    PC+=4;
    ADDI("10010001000001101010110000000000");
    PC+=4;
    fail(PC);
    */
    return 0;
}
