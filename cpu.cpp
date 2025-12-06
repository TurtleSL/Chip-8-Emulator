#include "cpu.hpp"
#include <fstream>
#include <vector>

void CPU::initialize()
{
    pc     = 0x200; // Most chip-8 programs begin here
    opcode = 0; // Clear opcode
    I      = 0; // Clear I register
    sp     = 0; // Clear stack pointer

    unsigned char chip8_fontset[80] = // Font set
        { 
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };

    // Clear the display
    for(unsigned char pixel : gfx)
    {
        pixel = 0;
    }

    // Clear the stack
    for(unsigned short level : stack)
    {
        level = 0;
    }

    // Clear all registers
    for(unsigned short reg : V)
    {
        reg = 0;
    }

    // Clear the memory

    for(unsigned short byte : memory)
    {
        byte = 0;
    }

    for(int i=0;i < 80;i++)
    {
        memory[i] = chip8_fontset[i];
    }
}

int CPU::load(std::string program)
{
    std::ifstream of(program, std::ios::binary | std::ios::ate);
    if(!of.is_open())
    {
        std::cerr << "Could not open: " << program << std::endl;
        return 1;
    }

    std::streamsize size = of.tellg();
    of.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);

    if(!of.read(buffer.data(), size))
    {
        std::cerr << "Could not read buffer data." << std::endl;
        return 1;
    }

    // Load file at 0x200
    for (int i = 0; i < size; i++) {
        memory[0x200 + i] = static_cast<unsigned char>(buffer[i]);
    }


}

void CPU::emulateCycle()
{
    // Current opcode being executed
    opcode = (memory[pc] << 8) | memory[pc+1];
    bool update_pc = true;

    //Decode the opcode
    /**
     * Legend:
     * -NNN = address
     * -NN = 8-bit constant
     * -N = 4 bit constant
     * -X and Y = 4-bit register identifier
     * -PC = program counter
     * -V = register
     */
    switch(opcode & 0xF000)
    {
        case 0x0000:
            switch(opcode & 0x000F)
            {
                case 0x0000:
                    //Clear the screen
                    for (unsigned char pixel : gfx)
                        pixel = 0;
                    // need a draw flag still so we know when to update screen
                break;

                case 0x000E:
                    // Return to the stack
                    sp--;
                    pc = stack[sp];
                break;
                default:
                    printf("Unknown opcode: 0x%X", opcode);
                    return;
            }
        break;
        case 0x1000:
            // go to address NNN
            pc = opcode & 0x0FFF;
            update_pc = false;
        break;
        case 0x2000:
            //call subroutine at address NNN
            stack[sp] = pc + 2;
            sp +=1;
            pc = memory[opcode & 0x0FFF];
            update_pc = false;
        break;
        case 0x3000:
            // check if VX == NN
            if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
            {
                pc += 2;
            }
        break;
        case 0x4000:
            // check if VX != NN
            if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
            {
                pc += 2;
            }
        break;
        case 0x5000:
            // check if VX == VY
            if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
            {
                pc += 2;
            }
        break;
        case 0x6000:
            // set VX = NN
            V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
        break;
        case 0x7000:
            // Vx += NN
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
        break;
        case 0x8000:
            switch(opcode & 0x000F)
            {
                case 0x0000:
                    // set VX = VY
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                break;
                case 0x0001:
                    // VX |= VY
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                break;
                case 0x0002:
                    // VX &= VY
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                break;
                case 0x0003:
                    // VX ^= VY
                    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                break;
                case 0x0004:
                    // VX += VY
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                break;
                case 0x0005:
                    // VX -= VY
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                break;
                case 0x0006:
                    // VX >>= 1
                    V[(opcode & 0x0F00) >> 8] >>= 1;
                break;
                case 0x0007:
                    // VX = VY-VX
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                break;
                case 0x000E:
                    // VX <<= 1
                    V[(opcode & 0x0F00) >> 8] <<= 1;
                break;
                default:
                    printf("Unknown opcode: 0x%X", opcode);
                    return;
            }
        break;
        case 0x9000:
            // check if VX != VY
            if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
            {
                pc += 2;
            }
        break;
        case 0xA000:
            // set I to NNN
            I = opcode & 0x0FFF;
        break;
        case 0xB000:
            // PC = V0 + NNN
            pc = V[0] + (opcode & 0x0FFF);
        break;
        case 0xC000:
            // VX = rand() & NN
            V[(opcode & 0x0F00) >> 8] = (std::rand() % 256) & (opcode & 0x00FF);
        break;
        case 0xD000:
            // draw(VX, VY, N) read up on this one to understand
            unsigned short x      = V[(opcode & 0x0F00) >> 8];
            unsigned short y      = V[(opcode & 0x00F0) >> 4];
            unsigned short height = opcode & 0x000F;
            unsigned short pixel;

            V[0xF] = 0;
            for(int y_line = 0; y_line < height; y_line++)
            {
                pixel = memory[I + y_line];
                for(int x_line = 0; x_line < 8; x_line++)
                {
                    if(pixel & (0x80 << x_line) != 0)
                    {
                        if(gfx[(64 * (y+y_line)) + (x+x_line)] == 0)
                            V[0xF] = 1;
                        gfx[(64 * (y+y_line)) + (x+x_line)] ^= 1;
                    }
                }
            }

            draw_flag = true;
        break;
        case 0xE000:
            switch(opcode & 0x000F)
            {
                case 0x000E:
                    // checks if key stored in VX is pressed
                    if(key[V[(opcode & 0x0F00) >> 8]] != 0)
                        pc += 2;
                break;
                case 0x0001:
                    // checks if key stored in VX is not pressed
                    if(key[V[(opcode & 0x0F00) >> 8]] == 0)
                        pc += 2;
                break;
                default:
                    printf("Unknown opcode: 0x%X", opcode);
                    return;
            }
        break;
        case 0xF000:
            switch(opcode & 0x00FF)
            {
                case 0x0007:
                    // set VX = delay_timer
                    V[(opcode & 0x0F00) >> 8] = delay_timer;
                break;
                case 0x000A:
                    // store keypress in VX
                    bool key_pressed = false;
                    for(int i = 0;i < 16; i++)
                    {
                        if(key[i])
                        {
                            key_pressed = true;
                            V[(opcode & 0x0F00) >> 8] = i;
                            break;
                        }
                    }

                    if(!key_pressed)
                    {
                        update_pc = false;
                    }
                break;
                case 0x0015:
                    // sets delay-timer to VX  
                    delay_timer = V[(opcode & 0x0F00) >> 8]; 
                break;
                case 0x0018:
                    // sets sound_timer to VX
                    sound_timer = V[(opcode & 0x0F00) >> 8];
                break;
                case 0x001E:
                    // adds VX to I
                    I += V[(opcode & 0x0F00) >> 8];
                break;
                case 0x0029:
                    // sets I to location of the sprite
                    // for the character in VX
                    // read more on this
                    I = V[(opcode & 0x0F00) >> 8] * 5;
                break;
                case 0x0033:
                    // stores the BCD of VX in I, I+1, I+2
                    memory[I]     =  V[(opcode & 0x0F00) >> 8] / 100;
                    memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                break;
                case 0x0055:
                    // Load V0-F values into memory starting at I
                    for(int i = 0;i < 16;i++)
                    {
                        memory[I+i] = V[i];
                    }
                break;
                case 0x0065:
                    // Load 1 byte of memory into V0-F starting at I
                    for(int i = 0;i < 16;i++)
                    {
                        V[i] = memory[I + i];
                    }
                break;
            }
        break;
        default:
            printf("Unknown opcode: 0x%X", opcode);
            return;
    }

    if(update_pc) pc += 2; // update the program counter if flag not unset
}