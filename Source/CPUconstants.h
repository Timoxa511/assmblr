
typedef int VARTP;

const size_t START_PROGRAM_BUF_SZ = 100;
const size_t LABELS_BUF_SZ = 100;
const int INVALID_LABEL = -1;
const int NPASSES = 2;

namespace REG
{

const int  NREGS = 4;
typedef int REGTP;

const char AX [] = "ax";
const char BX [] = "bx";
const char CX [] = "cx";
const char DX [] = "dx";

const BYTE AXKEYNUM = 0;
const BYTE BXKEYNUM = 1;
const BYTE CXKEYNUM = 2;
const BYTE DXKEYNUM = 3;

}
