#include <TXLib.h>

#undef IN
#undef OUT


#include "CPUconstants.h"



#define CMD_(NAME, ID, NPARS, BODY) \
    const char CMD_##NAME [] = #NAME;            \
    const int NUM_##NAME = ID;
#include "commmands.h"
#undef CMD_


bool Fcompare (FILE* a, FILE* b)
    {
    assert (a && b);

    int cura = fgetc (a);
    int curb = fgetc (b);

    while ( cura != EOF && curb != EOF )
        {
        if (cura != curb) return false;

        while (std::isspace (cura = fgetc (a)));
        while (std::isspace (curb = fgetc (b)));
        }

    return true;
    }



class Translator
    {
    std::array<int, LABELS_BUF_SZ> labels_;     // <int> for -1 val in translate_()
    std::vector<BYTE> program_;


    bool translate_ (FILE* _in, int pass_number = NPASSES); //high label diagnostics default
    void translate_par_ (char* buf, int* nchars, int pass_number = NPASSES);

    bool export_ ();

    public:
    Translator () : labels_(), program_() { program_.resize(START_PROGRAM_BUF_SZ);
                                            for (auto& label_ : labels_) label_ = INVALID_LABEL; }

    bool proccess (FILE* file);
    };

bool  Translator::proccess (FILE* file)
    {
    bool success = true;

    success *= translate_ (file, 1);

    std::rewind (file);

    success *= translate_ (file, 2);

    success *= export_();

    return success;
    }

void  Translator::translate_par_ (char* buf, int* nchars, int pass_number)
    {
    VARTP par = -1;
    int parnchars = -1;

    int scanfed_num = sscanf (buf + *nchars, "%d%n", &par, &parnchars);


    BYTE info = 0;

    if (scanfed_num == 1)
        { info |= 1; /*0000.0001*/ }
    else
        {
        char str [20] = "";
        BYTE regn = 0;

        int scanfed_str = sscanf (buf + *nchars, "%s%n", str, &parnchars);
        assert (scanfed_str == 1);

             if (stricmp (str, REG::AX) == 0) { regn = REG::AXKEYNUM; }      //
        else if (stricmp (str, REG::BX) == 0) { regn = REG::BXKEYNUM; }      // REG KOSTYLs
        else if (stricmp (str, REG::CX) == 0) { regn = REG::CXKEYNUM; }      //
        else if (stricmp (str, REG::DX) == 0) { regn = REG::DXKEYNUM; }      //
        else assert ("SYNTAX ERROR: invalid parameter" /*"\"%s\"" str*/);


        //rrrr.0000
        info |= (BYTE)(regn << 4);
        }

    program_.push_back(info);

    if (scanfed_num == 1)
        {
        if ( *(buf + *nchars + parnchars) == ':')   //  par == label_i
            {
            *nchars-= 5; //concerning ':' after %d
            if (labels_.at(par) == INVALID_LABEL && pass_number == NPASSES)
                assert ("SYNTAX ERROR: invalid label" /*"\"%s\"", str*/);

            par = labels_.at(par);
            }



        size_t i_end = program_.size();

        program_.resize ( i_end  + sizeof(VARTP) );   //reserve 4 bytes

        *(VARTP*)(program_.data() + i_end) = par; // program_ << par
        }

    *nchars += parnchars;
    }
// 1 type for values and 2 type for pointers  << par = labels_.at(par);  l 94
//TODO curChar / cur / char << *(buf + *nchars + parnchars)    l 91

bool  Translator::translate_ (FILE* _in, int pass_number)
    {
    assert (_in);

    program_.clear();  // for npasses > 1

    const size_t bufsz = 64;
    const size_t cmdsz = bufsz;


    char buf  [bufsz + 1] = "";  //for \0 in bufsz position as fgets doesn't always place it
    char cmd  [cmdsz + 1] = "";


    while (fgets (buf, (bufsz + 1), _in) != nullptr)
        {
        assert (buf[bufsz] == '\0');

        char* comment = strstr (buf, "//");      // comment ignoring
        if   (comment) *comment = '\0';

        int nchars = 0; //for %n |
                        //       V
        sscanf (buf, "%64s%n", cmd, &nchars);  //64 in 2 par is bufsz //no alternative in c++ method    //may be realized using macroporn power

        #define CMD_(NAME, ID, NPARS, BODY)                                 \
            else if (stricmp (cmd, (CMD_##NAME)) == 0)                     \
                {                                                         \
                program_.push_back((NUM_##NAME));                        \
                                                                        \
                for (int i = 0; i < (NPARS); i++)                        \
                    translate_par_(buf, &nchars, pass_number);            \
                                                                           \
                }


        if (stricmp (cmd, "END") == 0) { return true; }  //else =>
        #include "commmands.h"

        else {
            int label = -1;
            if ( sscanf (cmd, "%d%n", &label, &nchars) == 1 && *(buf + nchars) == ':')
                {
                labels_.at(label) = (int) program_.size(); //set label cur place
                nchars++; //concerning ':' after %d
                }
            }

        printf ("buf: \"%s\"\n", buf);
        printf ("buf: \"%s\"\n", buf + nchars);

        for (size_t i = nchars; buf[i] != '\0'; i++)
            {
            if (!std::isspace(buf[i]))
                {
                printf ("SYNTAX ERROR: unknown command \"%s\"  \n", buf + i);
                break; //return false;
                }
            }


        #undef CMD_

        *buf = '\0'; //clear
        *cmd = '\0';

        }

    return true;
    }

//TODO to dump in diagnostics in one place and bind messages with call places

bool  Translator::export_ ()
    {
    const size_t output_name_sz = 8;                  //
    char output_name [output_name_sz] = "";           //
                                                      //
    static unsigned Number = 0;                       //get_cur_output_name()
    Number++;                                         //
    sprintf (output_name, "../exe/%u.txt", Number);   //

    FILE*   _out = fopen (output_name, "wb");
    assert (_out);

    //for (auto& el : program_)        text format
    //    fprintf (_out, "%d ", el);

    fwrite (program_.data(), program_.size(), sizeof(program_.front()), _out);     //bin format
                                                                            //TODO WriteFile open close

    fclose (_out);

    return true;
    }






//-----------------------------------------------------------------------------
void Test (Translator* translator)
    {
    const size_t NTESTS = 100;
    for (size_t i = 1; i < NTESTS; i++)
        {
        char in_example_name [] = "";
        sprintf (in_example_name, "../examples/%lluin.txt", i);      //open example in
        FILE* in_example = fopen (in_example_name, "r");

        char out_example_name [] = "";
        sprintf (out_example_name, "../examples/%lluout.txt", i);
        FILE* out_example = fopen (out_example_name, "r");      //open example out


        if (!(in_example && out_example)) { fclose (in_example); fclose (out_example); continue; }    //pair not found


        bool translation = translator->proccess (in_example);


        char output_name [] = "";
        sprintf (output_name, "../exe/%llu.txt", i);
        FILE* output = fopen (output_name, "r");       //open translated output
        assert (output);


        bool comparison = Fcompare (output, out_example);          // compare with example out
                                 //fcompare (i)

        fclose (in_example);
        fclose (out_example);
        fclose (output);



        printf ("TEST (%llu) ", i);

        $s txSetConsoleAttr ((translation)? (FOREGROUND_LIGHTGREEN | BACKGROUND_BLACK) :
                                            (FOREGROUND_LIGHTRED   | BACKGROUND_BLACK)    );
        printf ("TRANSLATED ");

        $s txSetConsoleAttr ((comparison)?  (FOREGROUND_LIGHTGREEN | BACKGROUND_BLACK) :
                                            (FOREGROUND_LIGHTRED   | BACKGROUND_BLACK)    );
        printf ("COMPARED\n");
        }
    }

int main (int argc, char* argv [])
    {
    Translator translator;

    if (argc <= 1) { $sr; printf ("input error"); return 0; }

    if (argc == 2 && stricmp(argv[1], "--test") == 0) { Test(&translator); return 1; }

    for (int i = 1; i < argc; i++)
        {
        FILE*   f = fopen (argv[i], "r");
        printf ("\nf = %s\n", argv[i]);
        assert (f);

        bool success = translator.proccess(f);

        if (success) { $sg; printf ("success"); }
        else         { $sr; printf ("translation error"); }

        fclose (f);
        }

    return 1;
    }


//addr mode byte fmt: rrrr000m, r = reg#, m = 0: val, m = 1: reg


//my_todo: calculate bounds of all values when all type casts valid and assert it with refs to lines of code and solutions


//TODO call + ret
//call stack >> cpu::stack_
// random access mem : [5]   /   [ax]   /   [ax + 5]
                  // ram[5]     ram[ax]    ram[ax + 5]



//todo lil sleep on ram

// �1
// equation solver
// f << a, b, c      f >> nroots, roots:

// �2
// fibonacci || fact via recursion (not loop)

// �3
// sort array in ram
//better selectsort via search function
