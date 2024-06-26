#include "mem.h"
#include "debug.h"
#include <iostream>
#include <string>
#include<unordered_map>
#include<bitset>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <cmath>
using namespace std;
//create a 32 sized array to simulate the register, and a hashmap that maps a uint64 address to a uint8 to serve as memory
uint64_t rgster[32];
unordered_map<uint64_t,uint8_t> umap;
bool failOrNot = false;
void mem_write8(uint64_t addr, uint8_t data) 
{
    umap[addr]=data;
}
//the logic for these functions follows by first analyzing the OPCODE for data, then performing actions demanded by the operation type and operation information
//branching resets the PC to the value that I calculate
uint64_t CBNZ(string s, uint64_t PC)
{
    string regStr = s.substr(27,5);
    bitset<5> set(regStr);
    long regNum = set.to_ulong();
    int rNum = (int) regNum;
    uint64_t regVal = rgster[rNum];
    if(s[0]=='0')
    {
        uint32_t regValLower = static_cast<uint32_t>(regVal);
        if(regValLower == 0)
        {
            return PC;
        }
    }
    else
    {
        if(regVal == 0)
        {
            return PC;
        }
    }
    string toModify = s.substr(8,19);
    toModify = toModify + "00";
    while(toModify.length()!=64)
    {
        char c = toModify[0];
        toModify = c + toModify;
    }
    bitset<64> set2(toModify);
    uint64_t toAdd = set2.to_ullong();
    PC = PC + toAdd - 4;
    return PC;
}
//ADRP simply pages and adds the correct value to the intended register
void ADRP(string s, uint64_t PC)
{
    string regStr = s.substr(27,5);
    bitset<5> set(regStr);
    long regNum = set.to_ulong();
    int rNum = (int) regNum;
    string s1 = s.substr(1,2);
    string s2 = s.substr(8,19);
    string s3 = "000000000000";
    string toModify = s2+s1+s3;
    while(toModify.length()!=64)
    {
        char c = toModify[0];
        toModify = c + toModify;
    }
    string s4 = bitset<64>(PC).to_string();
    for(int i=0;i<12;i++)
    {
        s4[63-i]='0';
    }
    bitset<64> b1(s4);
    bitset<64> b2(toModify);
    uint64_t val1 = b1.to_ullong();
    uint64_t val2 = b2.to_ullong();
    //cout<<b1<<endl;
    //cout<<b2<<endl;
    uint64_t val3 = val1+val2;
    rgster[rNum]=val3;
    return;
}
//adds a set value to a register value
void ADDI(string s)
{
    string regStr1 = s.substr(27,5);
    bitset<5> set(regStr1);
    long regNum1 = set.to_ulong();
    int rNum1 = (int) regNum1;
    string regStr2 = s.substr(22,5);
    bitset<5> set2(regStr2);
    long regNum2 = set2.to_ulong();
    int rNum2 = (int) regNum2;
    int size;
    if(s[0]=='1')
    {
        size = 64;
    }
    else
    {
        size = 32;
    }
    string oops = s.substr(10,12);
    if(s[9]=='0')
    {
        while(oops.size()!=(unsigned long)size)
        {
            oops = '0' + oops;
        }
    }
    else
    {
        for(int i=0;i<12;i++)
        {
            oops = oops + '0';
        }
        while(oops.size()!=(unsigned long)size)
        {
            oops = '0' + oops;
        }
    }
    if(s[0]=='1')
    {
        uint64_t val1 = rgster[rNum2];
        bitset<64> b1(oops);
        uint64_t toAdd = b1.to_ullong();
        rgster[rNum1]=toAdd + val1;
    }
    else
    {
        uint32_t val1 = static_cast<uint32_t>(rgster[rNum2]);
        bitset<32> b1(oops);
        uint32_t toAdd = b1.to_ulong();
        rgster[rNum1]=static_cast<uint64_t>(toAdd+val1);
    }
    return;
}

uint32_t concatenate_uint8_to_uint32(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    uint32_t result = static_cast<uint32_t>(a) << 24;
    result |= static_cast<uint32_t>(b) << 16;
    result |= static_cast<uint32_t>(c) << 8;
    result |= static_cast<uint32_t>(d);
    
    return result;
}

uint64_t concatenate_uint8_to_uint64(uint8_t a, uint8_t b, uint8_t c, uint8_t d, 
                                     uint8_t e, uint8_t f, uint8_t g, uint8_t h) {
    uint64_t result = static_cast<uint64_t>(a) << 56;
    result |= static_cast<uint64_t>(b) << 48;
    result |= static_cast<uint64_t>(c) << 40;
    result |= static_cast<uint64_t>(d) << 32;
    result |= static_cast<uint64_t>(e) << 24;
    result |= static_cast<uint64_t>(f) << 16;
    result |= static_cast<uint64_t>(g) << 8;
    result |= static_cast<uint64_t>(h);
    
    return result;
}
//the three LDR method gets the next 4-8 bytes of data
void LDRONE(string s)
{
    string regStr1 = s.substr(27,5);
    bitset<5> set(regStr1);
    long regNum1 = set.to_ulong();
    int rNum1 = (int) regNum1;
    string regStr2 = s.substr(22,5);
    bitset<5> set2(regStr2);
    long regNum2 = set2.to_ulong();
    int rNum2 = (int) regNum2;
    string oops = s.substr(11,9);
    int size;
    if(s[1]=='0')
    {
        size = 32;
    }
    else
    {
        size = 64;
    }
    while(oops.size()!=(unsigned long)size)
    {
        oops = oops[0] + oops;
    }
    if(s[1]=='0')
    {
        bitset<32> b1(oops);
        uint32_t temp1 = static_cast<uint32_t>(rgster[rNum2]);
        uint8_t store1 = umap[static_cast<uint64_t>(temp1)];
        uint8_t store2 = umap[static_cast<uint64_t>(temp1+1)];
        uint8_t store3 = umap[static_cast<uint64_t>(temp1+2)];
        uint8_t store4 = umap[static_cast<uint64_t>(temp1+3)];
        uint32_t cast1 = concatenate_uint8_to_uint32(store4,store3,store2,store1);
        rgster[rNum1]=static_cast<uint64_t>(cast1);
        uint32_t val1 = b1.to_ulong();
        temp1 = temp1 + val1;
        rgster[rNum2] = static_cast<uint64_t>(temp1);
    }
    else
    {
        bitset<64> b1(oops);
        uint64_t temp1 = rgster[rNum2];
        uint8_t store1 = umap[static_cast<uint64_t>(temp1)];
        uint8_t store2 = umap[static_cast<uint64_t>(temp1+1)];
        uint8_t store3 = umap[static_cast<uint64_t>(temp1+2)];
        uint8_t store4 = umap[static_cast<uint64_t>(temp1+3)];
        uint8_t store5 = umap[static_cast<uint64_t>(temp1+4)];
        uint8_t store6 = umap[static_cast<uint64_t>(temp1+5)];
        uint8_t store7 = umap[static_cast<uint64_t>(temp1+6)];
        uint8_t store8 = umap[static_cast<uint64_t>(temp1+7)];
        uint64_t cast1 = concatenate_uint8_to_uint64(store8,store7,store6,store5,store4,store3,store2,store1);
        rgster[rNum1]=cast1;
        uint64_t val1 = b1.to_ullong();
        temp1 = temp1 + val1;
        rgster[rNum2] = temp1;
    }
    return;
}

void LDRTWO(string s)
{
    string regStr1 = s.substr(27,5);
    bitset<5> set(regStr1);
    long regNum1 = set.to_ulong();
    int rNum1 = (int) regNum1;
    string regStr2 = s.substr(22,5);
    bitset<5> set2(regStr2);
    long regNum2 = set2.to_ulong();
    int rNum2 = (int) regNum2;
    string oops = s.substr(11,9);
    int size;
    if(s[1]=='0')
    {
        size = 32;
    }
    else
    {
        size = 64;
    }
    while(oops.size()!=(unsigned long)size)
    {
        oops = oops[0] + oops;
    }
    if(s[1]==0)
    {
        bitset<32> b1(oops);
        uint32_t val1 = b1.to_ulong();
        uint32_t temp1 = static_cast<uint32_t>(rgster[rNum2]);      
        temp1 = temp1 + val1;
        rgster[rNum2] = static_cast<uint64_t>(temp1);
        uint8_t store1 = umap[static_cast<uint64_t>(temp1)];
        uint8_t store2 = umap[static_cast<uint64_t>(temp1+1)];
        uint8_t store3 = umap[static_cast<uint64_t>(temp1+2)];
        uint8_t store4 = umap[static_cast<uint64_t>(temp1+3)];
        uint32_t cast1 = concatenate_uint8_to_uint32(store4,store3,store2,store1);
        rgster[rNum1]=static_cast<uint64_t>(cast1);
    }
    else
    {
        bitset<64> b1(oops);
        uint64_t val1 = b1.to_ullong();
        uint64_t temp1 = rgster[rNum2];
        temp1 = temp1 + val1;
        rgster[rNum2] = temp1;
        uint8_t store1 = umap[static_cast<uint64_t>(temp1)];
        uint8_t store2 = umap[static_cast<uint64_t>(temp1+1)];
        uint8_t store3 = umap[static_cast<uint64_t>(temp1+2)];
        uint8_t store4 = umap[static_cast<uint64_t>(temp1+3)];
        uint8_t store5 = umap[static_cast<uint64_t>(temp1+4)];
        uint8_t store6 = umap[static_cast<uint64_t>(temp1+5)];
        uint8_t store7 = umap[static_cast<uint64_t>(temp1+6)];
        uint8_t store8 = umap[static_cast<uint64_t>(temp1+7)];
        uint64_t cast1 = concatenate_uint8_to_uint64(store8,store7,store6,store5,store4,store3,store2,store1);
        rgster[rNum1]=static_cast<uint64_t>(cast1);
    }
    return;
}
//not working
void LDRTHREE(string s)
{
    string regStr1 = s.substr(27,5);
    bitset<5> set(regStr1);
    long regNum1 = set.to_ulong();
    int rNum1 = (int) regNum1;
    string regStr2 = s.substr(22,5);
    bitset<5> set2(regStr2);
    long regNum2 = set2.to_ulong();
    int rNum2 = (int) regNum2;
    string oops = s.substr(10,12);
    if(s[1]=='0')
    {
        oops=oops+"00";
        while(oops.size()!=32)
        {
            oops = "0"+oops;
        }
    }
    else
    {
        oops=oops+"000";
        while(oops.size()!=64)
        {
            oops = "0"+oops;
        }
    }
    if(s[1]=='0')
    {
        bitset<32> b1(oops);
        uint32_t temp1 = static_cast<uint32_t>(rgster[rNum2]);
        uint32_t val1 = b1.to_ulong();
        temp1 = temp1 + val1;
        //rgster[rNum2] = static_cast<uint64_t>(temp1);
        uint8_t store1 = umap[static_cast<uint64_t>(temp1)];
        uint8_t store2 = umap[static_cast<uint64_t>(temp1+1)];
        uint8_t store3 = umap[static_cast<uint64_t>(temp1+2)];
        uint8_t store4 = umap[static_cast<uint64_t>(temp1+3)];
        uint32_t cast1 = concatenate_uint8_to_uint32(store4,store3,store2,store1);
        rgster[rNum1]=static_cast<uint64_t>(cast1);
    }
    else
    {
        bitset<64> b1(oops);
        uint64_t temp1 = rgster[rNum2];
        uint64_t val1 = b1.to_ullong();
        temp1 = temp1 + val1;
        //rgster[rNum2] = temp1;
        uint8_t store1 = umap[static_cast<uint64_t>(temp1)];
        uint8_t store2 = umap[static_cast<uint64_t>(temp1+1)];
        uint8_t store3 = umap[static_cast<uint64_t>(temp1+2)];
        uint8_t store4 = umap[static_cast<uint64_t>(temp1+3)];
        uint8_t store5 = umap[static_cast<uint64_t>(temp1+4)];
        uint8_t store6 = umap[static_cast<uint64_t>(temp1+5)];
        uint8_t store7 = umap[static_cast<uint64_t>(temp1+6)];
        uint8_t store8 = umap[static_cast<uint64_t>(temp1+7)];
        uint64_t cast1 = concatenate_uint8_to_uint64(store8,store7,store6,store5,store4,store3,store2,store1);
        rgster[rNum1]=static_cast<uint64_t>(cast1);
    }
    return;
}
//similar to LDR< but get only 1 byte
void LDRBONE(string s)
{

    string regStr1 = s.substr(27,5);
    bitset<5> set(regStr1);
    long regNum1 = set.to_ulong();
    int rNum1 = (int) regNum1;
    string regStr2 = s.substr(22,5);
    bitset<5> set2(regStr2);
    long regNum2 = set2.to_ulong();
    int rNum2 = (int) regNum2;
    string oops = s.substr(11,9);
    while(oops.size()!=64)
    {
        oops = oops[0] + oops;
    }
    bitset<64> b1(oops);
    uint64_t val1 = b1.to_ullong();
    //cout << "LDRBONE" << endl;
    //cout << hex << rNum2 << endl;
    uint64_t temp1 = rgster[rNum2];
    //cout << hex << temp1 << endl;
    uint8_t track = umap[temp1];
    rgster[rNum1]=static_cast<uint64_t>(track);
    temp1 = temp1 + val1;
    //cout << hex << temp1 << endl;
    rgster[rNum2] = temp1;
    return;
    
}
void LDRBTWO(string s)
{
    
    string regStr1 = s.substr(27,5);
    bitset<5> set(regStr1);
    long regNum1 = set.to_ulong();
    int rNum1 = (int) regNum1;
    string regStr2 = s.substr(22,5);
    bitset<5> set2(regStr2);
    long regNum2 = set2.to_ulong();
    int rNum2 = (int) regNum2;
    string oops = s.substr(11,9);
    while(oops.size()!=64)
    {
        oops = oops[0] + oops;
    }
    bitset<64> b1(oops);
    uint64_t val1 = b1.to_ullong();
    uint64_t temp1 = rgster[rNum2];
    temp1 = temp1 + val1;
    rgster[rNum2] = temp1;
    //cout << "LDRBTWO" << endl;
    //cout<<hex<<temp1<<endl;
    uint8_t track = umap[temp1];
    rgster[rNum1]=static_cast<uint64_t>(track);
    return;
    
}
void LDRBTHREE(string s)
{
    //cout << "LDRBTHREE" << endl;
    string regStr1 = s.substr(27,5);
    bitset<5> set(regStr1);
    long regNum1 = set.to_ulong();
    int rNum1 = (int) regNum1;
    string regStr2 = s.substr(22,5);
    bitset<5> set2(regStr2);
    long regNum2 = set2.to_ulong();
    int rNum2 = (int) regNum2;
    string oops = s.substr(10,12);
    //oops = oops + "0";
    while(oops.size()!=64)
    {
        oops = "0" + oops;
    }
    bitset<64> b1(oops);
    uint64_t val1 = b1.to_ullong();
    //cout << hex << val1 << endl;
    uint64_t temp1 = rgster[rNum2];
    //cout << hex << temp1 << endl;
    temp1 = temp1 + val1;
    //cout<<hex<<temp1<<endl;
    //cout << hex << temp1 << endl;
    //rgster[rNum2] = temp1;
    uint8_t track = umap[temp1];
    //cout << static_cast<unsigned>(track) << endl;
    rgster[rNum1]=static_cast<uint64_t>(track);
    return;
}
//writes to memory for STRB
void STRBONE(string s)
{
    string regStr1 = s.substr(27,5);
    bitset<5> set(regStr1);
    long regNum1 = set.to_ulong();
    int rNum1 = (int) regNum1;
    string regStr2 = s.substr(22,5);
    bitset<5> set2(regStr2);
    long regNum2 = set2.to_ulong();
    int rNum2 = (int) regNum2;
    string oops = s.substr(11,9);
    while(oops.size()!=64)
    {
        oops = oops[0] + oops;
    }
    bitset<64> b1(oops);
    uint64_t val1 = b1.to_ullong();
    uint64_t temp1 = rgster[rNum2];
    if(temp1 == 0xFFFFFFFFFFFFFFFF)
    {
        uint8_t temp = static_cast<uint8_t>(rgster[rNum1]);
        //cout << (int) temp;
        cout << (char) temp;
        temp1 = temp1 +val1;
        return;
    }
    //cout<<hex<<temp1<<endl;
    umap[temp1] = static_cast<uint8_t>(rgster[rNum1]);
    //cout<<static_cast<unsigned>(umap[temp1])<<endl;
    temp1 = temp1 + val1;
    rgster[rNum2] = temp1;
    return;
}
void STRBTWO(string s)
{
    string regStr1 = s.substr(27,5);
    bitset<5> set(regStr1);
    long regNum1 = set.to_ulong();
    int rNum1 = (int) regNum1;
    string regStr2 = s.substr(22,5);
    bitset<5> set2(regStr2);
    long regNum2 = set2.to_ulong();
    int rNum2 = (int) regNum2;
    string oops = s.substr(11,9);
    while(oops.size()!=64)
    {
        oops = oops[0] + oops;
    }
    bitset<64> b1(oops);
    uint64_t val1 = b1.to_ullong();
    uint64_t temp1 = rgster[rNum2];
    temp1 = temp1 + val1;
    if(temp1 == 0xFFFFFFFFFFFFFFFF)
    {
        uint8_t temp = static_cast<uint8_t>(rgster[rNum1]);
        //cout << (int) temp;
        cout << (char) temp;
        return;
    }
    //cout<<hex<<temp1<<endl;
    umap[temp1] = static_cast<uint8_t>(rgster[rNum1]);
    //cout<<static_cast<unsigned>(umap[temp1])<<endl;
    rgster[rNum2] = temp1;
    return;
}
void STRBTHREE(string s)
{
    string regStr1 = s.substr(27,5);
    bitset<5> set(regStr1);
    long regNum1 = set.to_ulong();
    int rNum1 = (int) regNum1;
    string regStr2 = s.substr(22,5);
    bitset<5> set2(regStr2);
    long regNum2 = set2.to_ulong();
    int rNum2 = (int) regNum2;
    string oops = s.substr(10,12);
    //oops = oops + "0";
    while(oops.size()!=64)
    {
        oops = "0" + oops;
    }
    bitset<64> b1(oops);
    uint64_t val1 = b1.to_ullong();
    uint64_t temp1 = rgster[rNum2];
    temp1 = temp1 + val1;
    //cout << hex << temp1 << endl;
    if(temp1 == 0xFFFFFFFFFFFFFFFF)
    {
        uint8_t temp = static_cast<uint8_t>(rgster[rNum1]);
        //cout << (int) temp;
        cout << (char) temp;
        return;
    }
    umap[temp1] = static_cast<uint8_t>(rgster[rNum1]);
    return;
}
//movz moves a number into register
void MOVZ(string s)
{
    string regStr1 = s.substr(27,5);
    bitset<5> set(regStr1);
    long regNum1 = set.to_ulong();
    int rNum1 = (int) regNum1;
    string str2 = s.substr(11,16);
    if(s[0]=='0')
    {
        int mlt = ((int)s[10]-48)*16;
        for(int i=0;i<mlt;i++)
        {
            str2=str2+'0';
        }
        bitset<32> b1(str2);
        rgster[rNum1] = static_cast<uint64_t>(b1.to_ullong());
    }
    else
    {
        int mlt = ((int)s[9]-48)*2+((int)s[10]-48);
        mlt = mlt *16;
        for(int i=0;i<mlt;i++)
        {
            str2=str2+'0';
        }
        bitset<64> b1(str2);
        uint64_t curr = b1.to_ullong();
        rgster[rNum1]=curr;
    }
    return;
}
uint32_t bitstringToUint32(const string& bitstring) {


    std::string paddedBitstring = std::string(32 - bitstring.length(), '0') + bitstring;

    return std::bitset<32>(paddedBitstring).to_ulong();
}
uint64_t DecodeBitMask(string N, string imms, string immr, int datasize){
    string notimms;
    //cout << imms << endl;
    for(char c : imms)
    {
        if(c=='0')
        {
            notimms = notimms + '1';
        }
        else
        {
            notimms = notimms + '0';
        }
    }
    //get highest 1 bit
    //cout << N << endl;
    string concat1 = N + notimms;
    //cout << concat1 << endl;
    int concatLen = (int) concat1.size() -1;
    int highestSetBit;
    for(char c : concat1)
    {
        if(c=='1')
        {
            highestSetBit = concatLen;
            break;
        }
        else
        {
            concatLen--;
        }
    }
    //cout << highestSetBit<<endl;
    //getting the pattern string
    string levelStr = "";
    for(int i = 0;i<highestSetBit;i++)
    {
        levelStr = levelStr + '1';
    }
    while((int)levelStr.size()!=6)
    {
        levelStr = "0" + levelStr;
    }
    string s = "";
    string r = "";
    for(int i=0;i<6;i++)
    {
        if(levelStr[i]=='1'&&imms[i]=='1')
        {
            s=s+'1';
        }
        else
        {
            s=s+'0';
        }
        if(levelStr[i]=='1'&&immr[i]=='1')
        {
            r=r+'1';
        }
        else
        {
            r=r+'0';
        }
    }
    bitset<6> bsets(s);
    bitset<6> bsetr(r);
    int sLong = (int)bsets.to_ulong();
    int rLong = (int)bsetr.to_ulong();
    int diff = sLong - rLong;
    int esize = 1 << highestSetBit;
    bitset<sizeof(diff)*8> diffBset(diff);
    string diffStr = diffBset.to_string();
    /*string tempD = "";
    for(int i = 0;i<len;i++)
    {
        int diffStrLen = (int) diffStr.size()-1;
        tempD = diffSr[diffStrLen-i] + tempD;
    }
    */
    string welem = "";
    for(int i = 0;i<sLong+1;i++)
    {
        welem = welem + "1";
    }
    while((int) welem.size()!=esize)
    {
        welem = "0"+welem;
    }
    string wmask = "";
    int rep = datasize / esize;
    for(int i=0;i<rLong;i++)
    {
        int welemLen = (int)welem.size();
        char c =  welem[welemLen-1];
        for(int j = welemLen-2;j>=0;j--)
        {
            welem[j+1]=welem[j];
        }
        welem[0]=c;
    }
    for(int i=0;i<rep;i++)
    {
        wmask=wmask + welem;
    }
    bitset<64> bruh(wmask);
    uint64_t toRetHere = bruh.to_ullong();
    //cout << hex << toRetHere << endl;
    return toRetHere;
}
//this one does ORR with a register value and an encoded bitmask
void ORR(string s)
{
    string regStr1 = s.substr(27,5);
    bitset<5> set(regStr1);
    long regNum1 = set.to_ulong();
    int rNum1 = (int) regNum1;
    string regStr2 = s.substr(22,5);
    bitset<5> set2(regStr2);
    long regNum2 = set2.to_ulong();
    int rNum2 = (int) regNum2;
    string temp2 = s.substr(16,6);
    string temp3 = s.substr(10,6);
    char bruhmoment = s[9];
    //cout<<bruhmoment << endl;
    string temp1;
    temp1 += bruhmoment;
    int datasize;
    if(s[0]=='0')
    {
        datasize=32;
    }
    else
    {
        datasize=64;
    }
    uint64_t maskedContent = DecodeBitMask(temp1, temp2, temp3, datasize);
    if(s[0]=='0')
    {
        uint32_t content32 = static_cast<uint32_t>(maskedContent);
        uint32_t reg32 = static_cast<uint32_t>(rgster[rNum2]);
        bitset<32> b1(content32);
        bitset<32> b2(reg32);
        string content32str = b1.to_string();
        string reg32str = b2.to_string();
        string toRegisterstr = "";
        for(int i=0;i<32;i++)
        {
            if(content32str[i]=='0'&&reg32str[i]=='0')
            {
                toRegisterstr = toRegisterstr + '0';
            }
            else
            {
                toRegisterstr = toRegisterstr + '1';
            }
        }
        bitset<32> b3(toRegisterstr);
        uint64_t toRegister = static_cast<uint64_t>(b3.to_ulong());
        rgster[rNum1]=toRegister;
        return;
    }
    else
    {
        uint64_t content64 = maskedContent;
        uint64_t reg64 = rgster[rNum2];
        bitset<64> b1(content64);
        bitset<64> b2(reg64);
        string content64str = b1.to_string();
        string reg64str = b2.to_string();
        string toRegisterstr = "";
        for(int i=0;i<64;i++)
        {
            if(content64str[i]=='0'&&reg64str[i]=='0')
            {
                toRegisterstr = toRegisterstr + '0';
            }
            else
            {
                toRegisterstr = toRegisterstr + '1';
            }
        }
        bitset<64> b3(toRegisterstr);
        uint64_t toRegister = b3.to_ulong();
        rgster[rNum1]=toRegister;
        return;
    }
}
//getting opcode works
string opcode(uint64_t addr) 
{
    string x = bitset<8>(umap[addr+3]).to_string();
    string y = bitset<8>(umap[addr+2]).to_string();
    string z = bitset<8>(umap[addr+1]).to_string();
    string o = bitset<8>(umap[addr+0]).to_string();
    return x+y+z+o;
}
//printing works
void fail(uint64_t xD)
{
    cout << "unknown instruction ";
    string s = opcode(xD);
    bitset<32> set(s);
    uint32_t l = set.to_ulong();
    printf("%08x",l);
    cout << " at ";
    printf("%06lx", xD);
    cout << endl;
    //failOrNot=true;
    for(int i=0;i<32;i++)
    {
        if(i == 31)
        {
            cout << "XSP : ";
        }
        else
        {
            cout << "X";
            cout <<setw(2)<<setfill('0')<<(int)i;
            cout << " : ";
        }
        uint64_t num = rgster[i];
        printf("%016lX", num);
        cout << endl;
    }
    return;
}
bool checkFailure()
{
    if(failOrNot)
    {
        return true;
    }
    return false;
}
