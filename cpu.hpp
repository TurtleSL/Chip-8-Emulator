#pragma once
#include <iostream>

class CPU
{
public:
    bool draw_flag;
    void initialize(); //clear memory, registers, and screen
    int  load(std::string program); //load program into memory
    void emulateCycle(); //emulate one full cycle of the program

    void close();

    //Accessor methods
    const unsigned char* getGfx() const { return gfx; }
    const unsigned char getDelay() const { return delay_timer; }
    const unsigned char getSound() const { return sound_timer; }

    //Mutator methods
    void setKey(unsigned char k, bool isPressed) { key[k] = isPressed; }
    void setDelay(unsigned char t) { delay_timer = t; }
    void setSound(unsigned char t) { sound_timer = t; }

private:
    unsigned short opcode;       //current opcode
    unsigned char  V[16];        //16 8-bit registers 0-F (hexadecimal)
    unsigned short I;            //12-bit register for memory addresses (void pointer)
    unsigned short pc;           //Points to the current instruction
    unsigned short stack[16];    //16 stack levels only for return addresses
    unsigned char  sp;           //Pointer to the stack
    unsigned char  memory[4096]; //4096 bytes of memory
    unsigned char  gfx[64 * 32]; //64x32 pixel display; monochrome
    unsigned char  delay_timer;  //timer for events
    unsigned char  sound_timer;  //timer for sound
    unsigned char  key[16];      //16 keys
};