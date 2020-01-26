#define REG(REGN)   register_.at(REGN)

#define PAR(PARN)   get_par(COUNTER, PARN)
#define TYPE(PARN)  get

#define READPARFROMCONSOLE  readparfromconsole()


#define SETCOUNTER(N) COUNTER = (N)

CMD_ (PUSH, 1,  1,  2,
    {
    stack_.push_back(PAR(1));
    })

CMD_ (POP,  2,  1,  1,
    {
    int popper = stack_.top();

    stack_.pop_back();

    REG(PAR(1)) = popper;
    })

CMD_ (IN ,  3,  0,  0,
    {
    stack_.push_back(READPARFROMCONSOLE);
    })

CMD_ (OUT,  4,  0,  0,
    {
    if (stack_.empty()) { printf ("empty\n"); break; }

    printf ("%d\n", stack_.top());
    stack_.pop_back ();
    })

CMD_ (ADD,  5,  0,  0,
    {
    if (stack_.size() < 2) { printf ("too few elements to execute\n"); break; }

    int push = 0;

    push += stack_.top();
    stack_.pop_back();


    push += stack_.top();
    stack_.pop_back();

    stack_.push_back (push);

    })

CMD_ (SUB,  6,  0,  0,
    {
    if (stack_.size() < 2) {  printf ("too few elements to execute\n"); break; }

    int push = 0;

    push += stack_.top();
    stack_.pop_back();

    push -= stack_.top();
    stack_.pop_back();

    stack_.push_back (push);
    })

CMD_ (MUL,  7,  0,  0,
    {
    if (stack_.size() < 2) {  printf ("too few elements to execute\n"); break; }

    int push = 0;

    push += stack_.top();
    stack_.pop_back();

    push *= stack_.top();
    stack_.pop_back();

    stack_.push_back (push);

    })

CMD_ (DIV,  8,  0,  0,
    {
    if (stack_.size() < 2) {  printf ("too few elements to execute\n"); break; }

    int push = 0;

    push += stack_.top();
    stack_.pop_back();

    push /= stack_.top();
    stack_.pop_back();

    stack_.push_back (push);

    })

CMD_ (HLT,  9,  0,  0,
    {
    printf ("program ended successfully");
    return 1;
    })

CMD_ (DUMP, 10, 0,  0,
    {
    stack_.dump();
    })

CMD_ (MEOW, 11, 1,  1,
    {
    for (int i = 0; i < PAR(1); i++) printf ("MEOW\n");
    })

CMD_ (JMP,  12, 1,  1,
    {
    SETCOUNTER(PAR(1));
    })

CMD_ (JE,   13, 1,  1, //jump if equal
    {
    int a = stack_.top();
    stack_.pop_back();


    int b = stack_.top();
    stack_.pop_back();

    if (a == b) SETCOUNTER(PAR(1));
    })

CMD_ (JNE,  14, 1,  1,
    {
    int a = stack_.top();
    stack_.pop_back();


    int b = stack_.top();
    stack_.pop_back();

    if (a != b) SETCOUNTER(PAR(1));
    })

CMD_ (JA,   15, 1,  1,
    {
    int a = stack_.top();
    stack_.pop_back();


    int b = stack_.top();
    stack_.pop_back();

    if (a > b) SETCOUNTER(PAR(1));
    })

CMD_ (JNA,  16, 1,  1,
    {
    int a = stack_.top();
    stack_.pop_back();


    int b = stack_.top();
    stack_.pop_back();

    if (a <= b) SETCOUNTER(PAR(1));
    })

CMD_ (JB,   17, 1,  1,
    {
    int a = stack_.top();
    stack_.pop_back();


    int b = stack_.top();
    stack_.pop_back();

    if (a < b) SETCOUNTER(PAR(1));
    })

CMD_ (JNB,  18, 1,  1,
    {
    int a = stack_.top();
    stack_.pop_back();


    int b = stack_.top();
    stack_ .pop_back();

    if (a >= b) SETCOUNTER(PAR(1));
    })

// JS  if its sunday

//печать таблицу квадратов до введенного числа
//2 вводитс€ число, провер€ет€, €вл€етс€ ли оно простым
#undef READPAR
#undef READPARFROMCONSOLE

