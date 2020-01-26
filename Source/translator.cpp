#include <TXLib.h>

#undef IN
#undef OUT


#define CMD_(NAME, ID, NSYNPARS, NCPUPARS, BODY) \
    const char CMD_##NAME [] = #NAME;            \
    const int NUM_##NAME = ID;


#include "commmands.h"

#undef CMD_

class Translator
    {
    std::vector<int> program_;

    void read_par (char* buf, int* nchars);


    bool translate_ (FILE* _in);
    bool export_ ();

    public:
    Translator () : program_() {}

    bool proccess (FILE* file) { return translate_(file) && export_(); }
    };

namespace REGS
{

const char AX [] = "ax";
const char BX [] = "bx";
const char CX [] = "cx";
const char DX [] = "dx";

const int AXKEYNUM = 0;
const int BXKEYNUM = 1;
const int CXKEYNUM = 2;
const int DXKEYNUM = 3;

}

void Translator::read_par (char* buf, int* nchars)
    {
    int par = -1;
    int parnchars = -1;

    sscanf (buf + *nchars, "%d%n", &par, &parnchars);

    if (par == -1)  //       << register
        {
        program_.push_back(0);
        //printf ("pushed 0\n");

        char str [20] = "";
        //printf ("read_par: buf = %s", buf + *nchars);

        int sscanfed = sscanf (buf + *nchars, "%s%n", str, &parnchars);

        assert ( sscanfed == 1);

        if      (stricmp (str, REGS::AX) == 0) { par = REGS::AXKEYNUM; }
        else if (stricmp (str, REGS::BX) == 0) { par = REGS::BXKEYNUM; }
        else if (stricmp (str, REGS::CX) == 0) { par = REGS::CXKEYNUM; }
        else if (stricmp (str, REGS::DX) == 0) { par = REGS::DXKEYNUM; }
        else assert (0);
        }
    else   // << temp integer
        {
        program_.push_back(1);
        //printf ("pushed 1\n");
        }

    *nchars += parnchars;

    program_.push_back(par);
    }

//todo ������� � ����������� ������ ������� ������

//����� �������� ������� ������ ����� � �� ��������


bool Translator::translate_ (FILE* _in)    //const ~ fgets ?
    {
    assert (_in);

    const size_t bufsz = 20;
    const size_t cmdsz = bufsz;


    char buf [bufsz + 1] = "";  //for \0 in bufsz position as fgets doesn't always place it
    char cmd [cmdsz + 1] = "";


    while (fgets (buf, (int) bufsz, _in) != nullptr)
        {
        int nchars = -1;

    //printf ("buf = \"%s\"\n", buf);

        sscanf (buf, "%20s%n", cmd, &nchars);  //20 in 2 par is bufsz //no alternative in c++ method    //may be realized using macroporn power

        #define CMD_(NAME, ID, NSYNPARS, NCPUPARS, BODY)                         \
            else if (stricmp (cmd, (CMD_##NAME)) == 0)                          \
                {                                                              \
                program_.push_back((NUM_##NAME));                            \
                                                                            \
                for (int i = 0; i < NSYNPARS; i++)                          \
                    {                                                       \
                    /*printf ("i = %d, nchars = %d ", i, nchars);               \
                    printf ("buf + nchars =\"%s\"\n", buf + nchars);  */           \
                    read_par(buf, &nchars);                                     \
                    }                                                        \
                }


        if (0) {;}
        #include "commmands.h"

        else { printf ("unknown command \"%s\"  \n", buf); }

        #undef CMD_

        *buf = '\0'; //clear
        *cmd = '\0'; //clear
        }

    return true;
    }

bool Translator::export_ ()
    {
    const size_t output_name_sz = 8;                  //
    char output_name [output_name_sz] = "";           //
                                                      //
    static unsigned Number = 0;                       //get_cur_output_name()
    Number++;                                         //
    sprintf (output_name, "output/%u.txt", Number);   //

    FILE*   _out = fopen (output_name, "w");
    assert (_out);

    for (auto& el : program_)
        fprintf (_out, "%d ", el);


    if (_out) fclose (_out);
    Number++;

    return true;
    }




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

void Test (Translator* translator, int ntests)   // --test ?*
    {
    for (int i = 1; i <= ntests; i++)
        {
        char in_example_name [] = "";
        sprintf (in_example_name, "examples/%din.txt", i);      //open example in
        FILE* in_example = fopen (in_example_name, "r");
        assert (in_example);

        char out_example_name [] = "";
        sprintf (out_example_name, "examples/%dout.txt", i);
        FILE* out_example = fopen (out_example_name, "r");      //open example out
        assert (out_example);



        bool translation = translator->proccess (in_example);


        char output_name [] = "";
        sprintf (output_name, "output/%d.txt", i);
        FILE* output = fopen (output_name, "r");       //open translated output
        assert (output);


        bool comparison = Fcompare (output, out_example);          // compare with example out


        fclose (in_example);
        fclose (out_example);
        fclose (output);



        printf ("TEST (%d) ", i);

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

    if (argc == 2 && stricmp(argv[1], "--test") == 0) { Test(&translator, 2); return 1; }

    for (int i = 1; i < argc; i++)
        {
        FILE*   f = fopen (argv[i], "r");
        assert (f);

        translator.proccess(f);

        fclose (f);
        }

    return 1;
    }





//syntax error tests

//todo new cmds: sin cos tg root(sqrt)
        //thinkabout hooman-friendlier commands addition