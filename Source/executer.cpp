#include <TXLib.h>
#include "Stack.h"



#define CMD_(NAME, ID, NSYNPARS, NCPUPARS, BODY)     const int NUM_##NAME = ID;
#include "commmands.h"
#undef  CMD_

//TODO class  register + .h ...

class CPU
    {
    cool::Stack stack_;

    std::vector<int> program_;

    std::array<int, 4> register_;

    public:
    CPU () : stack_(), program_(), register_() {}

    int load (FILE* exe);
    int run ();
    bool proccess (FILE* exe) { return load(exe) && run(); }

    //for DSL
    int get_par (int counter, int parn);
    int readparfromconsole ();
    };


int CPU::load (FILE* exe)
    {
    assert (exe);


    int buf = -1;

    for (int i = 0; fscanf (exe, "%d", &buf) != -1; i++)
        {
        printf ("[%d] > %d\n", i, buf);
        program_.push_back(buf);
        }

    return 1;
    }

int CPU::run ()
    {
    for (unsigned COUNTER = 0; COUNTER < program_.size(); COUNTER++)
        {
        int cmd = program_[COUNTER];

        switch(cmd)
            {
            #define CMD_(NAME, ID, NSYNPARS, NCPUPARS, BODY)          \
                case ID:                                           \
                    {                                           \
                    printf (#NAME " STARTED\n");             \
                                                          \
                    BODY;                              \
                                                          \
                    printf (#NAME " ENDED\n");               \
                                                                \
                    COUNTER += NCPUPARS;                           \
                    break;                                            \
                    }

            #include "commmands.h"
            default: { printf ("unknown command [%d]\n", cmd); }

            #undef CMD_
            }

        }

    printf ("EOF reached, no HLT met");
    return 0;
    }


int CPU::get_par (int counter, int parn)
    {
    assert (parn > 0);

    //printf ("COUNTER = %d, parn = %d, (counter + (parn)*2 = %d\n", counter, parn, (counter + (parn)*2));

    if (program_.at(counter + (parn)*2 - 1)) { return program_.at(counter + (parn)*2); }

    return register_.at(program_.at(counter + (parn)*2));
    }

int CPU::readparfromconsole ()
    {
    int ret = -1;
    fscanf (stdin, "%d", &ret);
    return ret;
    }



int main (int argc, char* argv [])
    {
    if (argc != 2) { $sr; printf ("input error"); return 0; }

    FILE* exe = fopen (argv[1], "r");
    assert (exe);

    CPU cpu;
    cpu.proccess (exe);

    return 1;
    }







