#define REG(REGN)   register_.at(REGN)

#define PAR(PARN)   *cmdpars[PARN] //assert parn < npars ;(((

#define READPARFROMCONSOLE  readparfromconsole_()

#define GETCOUNTER COUNTER // not const((

#define SETCOUNTER(N) COUNTER = (N)

//CMD_(NAME, ID, NPARS, BODY)  headers

CMD_ (PUSH,  0x01,  1,
    {
    stack_.push_back(PAR(0));
    })

CMD_ (POP,   0x02,  1,
    {
    int popper = stack_.top();

    stack_.pop_back();

    PAR(0) = popper;
    })

CMD_ (IN ,   0x03,  0,
    {
    stack_.push_back(READPARFROMCONSOLE);
    })

CMD_ (OUT,   0x04,  0,
    {
    if (stack_.empty()) { printf ("empty\n"); break; }

    printf ("%d\n", stack_.top());
    stack_.pop_back ();
    })

CMD_ (ADD,   0x05,  0,
    {
    if (stack_.size() < 2) { printf ("too few elements to execute\n"); break; }

    int push = 0;

    push += stack_.top();
    stack_.pop_back();


    push += stack_.top();
    stack_.pop_back();

    stack_.push_back (push);

    })

CMD_ (SUB,   0x06,  0,
    {
    if (stack_.size() < 2) {  printf ("too few elements to execute\n"); break; }

    int push = 0;

    push += stack_.top();
    stack_.pop_back();

    push -= stack_.top();
    stack_.pop_back();

    stack_.push_back (push);
    })

CMD_ (MUL,   0x07,  0,
    {
    if (stack_.size() < 2) {  printf ("too few elements to execute\n"); break; }

    int push = 0;

    push += stack_.top();
    stack_.pop_back();

    push *= stack_.top();
    stack_.pop_back();

    stack_.push_back (push);

    })

CMD_ (DIV,   0x08,  0,
    {
    if (stack_.size() < 2) {  printf ("too few elements to execute\n"); break; }

    int push = 0;

    push += stack_.top();
    stack_.pop_back();

    push /= stack_.top();
    stack_.pop_back();

    stack_.push_back (push);

    })

CMD_ (HLT,   0x09,  0,
    {
    $d;
    printf ("program ended successfully");
    return 1;
    })

CMD_ (DUMP,  0x0A, 0,
    {
    stack_.dump();
    })

CMD_ (MEOW,  0x0B, 1,
    {
    for (int i = 0; i < PAR(1); i++) printf ("MEOW\n");
    })

CMD_ (JMP,   0x0C, 1,
    {
    SETCOUNTER(PAR(0));
    })

CMD_ (JE,    0x0D, 1,  //Jump_if_Equal
    {
    int a = stack_.top();
    stack_.pop_back();


    int b = stack_.top();
    stack_.pop_back();

    if (a == b) SETCOUNTER(PAR(0));
    })

CMD_ (JNE,   0x0E, 1,
    {
    int a = stack_.top();
    stack_.pop_back();


    int b = stack_.top();
    stack_.pop_back();

    if (a != b) SETCOUNTER(PAR(0));
    })

CMD_ (JA,    0x0F, 1,
    {
    int a = stack_.top();
    stack_.pop_back();


    int b = stack_.top();
    stack_.pop_back();

    if (a > b) SETCOUNTER(PAR(0));
    })

CMD_ (JNA,   0x10, 1,
    {
    int a = stack_.top();
    stack_.pop_back();


    int b = stack_.top();
    stack_.pop_back();

    if (a <= b) SETCOUNTER(PAR(0));
    })

CMD_ (JB,    0x11, 1,
    {
    int a = stack_.top();
    stack_.pop_back();


    int b = stack_.top();
    stack_.pop_back();

    if (a < b) SETCOUNTER(PAR(0));
    })

CMD_ (JNB,   0x12, 1,
    {
    int a = stack_.top();
    stack_.pop_back();


    int b = stack_.top();
    stack_ .pop_back();

    if (a >= b) SETCOUNTER(PAR(0));
    })

CMD_ (CALL,  0x13, 1,
    {
    assert ((size_t)INT_MAX >= COUNTER);    //my_todo: type casts control

    stack_.push_back ((int) COUNTER);

    SETCOUNTER (PAR(0));

    })

CMD_ (RET,   0x14, 0,
    {
    SETCOUNTER (stack_.top());

    stack_.pop_back();

    })

CMD_ (PAUSE, 0x15, 1,
    {
    printf ("pause \"%d\"\n", PAR(0));
    getch();
    })

#undef READPAR
#undef READPARFROMCONSOLE

