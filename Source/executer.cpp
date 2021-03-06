#include <TXLib.h>
#include "Stack.h"


#include "CPUconstants.h"



#define CMD_(NAME, ID, NPARS, BODY)     const int NUM_##NAME = ID;
#include "commmands.h"
#undef  CMD_



size_t FSize (FILE* file)
    {
    assert (file);

    assert( fseek(file, 0, SEEK_END) == 0);

    size_t sz = ftell(file);

    assert( fseek(file, 0, SEEK_SET) == 0);

    return sz;
    }




class  CPU
    {
    cool::Stack stack_;

    std::vector<BYTE> program_;

    std::array<REG::REGTP, REG::NREGS> register_;

    public:
    CPU () : stack_(), program_(), register_() { program_.resize(START_PROGRAM_BUF_SZ); }

    //DSL
    size_t fillCmdPars_ (VARTP* cmd [], size_t npars, size_t counter);
    VARTP    readparfromconsole_ ();

    //process
    int  import_  (FILE* exe);
    int  run_     ();

    //interface
    bool process (FILE* exe) { return import_(exe) && run_(); }

    };

size_t CPU::fillCmdPars_ (VARTP* cmd [], size_t npars, size_t counter)
    {
    printf (" pars:");

    size_t startbound = counter;

    counter += sizeof(BYTE);   //cmd Byte

    for (size_t i = 0; i < npars; i++)
        {
        if (program_[counter] & 1)
            {
            cmd[i] = (VARTP*)(program_.data() + counter + sizeof (BYTE));
            printf (" (%d)", *cmd[i]);;

            counter += sizeof(VARTP);  //VARTP Bytes
            }
        else
            {
            cmd[i] = &register_.at(program_.at(counter) >> 4);
            printf (" reg[%d] (%d)", program_.at(counter) >> 4, *cmd[i]);
            }

        counter += sizeof(BYTE);  //info Byte
        }

    printf ("\n");

    return (counter - startbound);
    }

VARTP  CPU::readparfromconsole_ ()
    {
    VARTP ret = -1;
    fscanf (stdin, "%d", &ret);
    return ret;
    }

int    CPU::import_ (FILE* exe)
    {
    assert (exe);


    //BYTE buf = -1;
    //for (int i = 0; fscanf (exe, "%u", &buf) != -1; i++)    txt fmt
    //    {
    //    printf ("[%d] > %d\n", i, buf);
    //    program_.push_back((BYTE) buf);
    //    }

    assert (FSize(exe) <= program_.size());

    fread (program_.data(), sizeof(BYTE), FSize(exe), exe);     //bin fmt

    return 1;
    }

int    CPU::run_ ()
    {
    for (size_t COUNTER = 0; COUNTER < program_.size();)
        {
        int cmd = program_[COUNTER];

        switch(cmd)
            {
            #define CMD_(NAME, ID, NPARS, BODY)                                     \
                case ID:                                                                \
                    {                                                                       \
                    printf ("[%3llu] %5s" , COUNTER, #NAME);                                     \
                                                                                                    \
                    VARTP* cmdpars [NPARS] = {};                                                         \
                                                                                                            \
                    size_t cmdsz = fillCmdPars_ (cmdpars, NPARS, COUNTER);                                      \
                    COUNTER += cmdsz;                                                                              \
                                                                                                                      \
                                                                                                                        \
                    { $sg; BODY; }                                                                                    \
                                                                                                                  \
                                                                                                     \
                    break;                                                                 \
                    }

            #include "commmands.h"
            default: { printf ("EXE ERROR: unknown command \"%d\"\n", cmd); return 0; }

            #undef CMD_
            }

        }

    printf ("EOF reached, no HLT met");
    return 0;
    }




int main (int argc, char* argv [])
    {
    if (argc != 2) { $sr; printf ("input error"); return 0; }

    FILE* exe = fopen (argv[1], "rb");
    assert (exe);

    CPU cpu;
    cpu.process (exe);

    return 1;
    }




//TODO addr mode byte fmt: rrrr000m, r = reg#, m = 0: val, m = 1: reg


/* TODO +=dump: cur stack, cur reg, cur prog place

reg: ax 0100 bx 0C10 cx 1111 dx 1A34, stack: [0] 0150, [1] 45C7, [2] ....

      00 01 02 03 04 05 06 07 08 09 0A
      01 01 05 00 00 00 02 .. .. .. ..
 [06] .  .  .  .  .  .  ^

                           */

