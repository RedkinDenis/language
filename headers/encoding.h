#ifndef ENCODING_H
#define ENCODING_H

enum byte_codes
{
    ERR = 20,
    PUSH = 1,                 // 2
    ADD = 22,                  // 1
    SUB = 3,                  // 1
    OUT = 4,                  // 1
    HLT = 19,                 // 1
    DIV = 5,                  // 1
    MUL = 6,                  // 1
    IN = 7,                   // 1
    RPUSH = 65,               // 2
    RAMPUSH = 33,             // 2
    RAMPUSHR = 161,           // 2
    POP = 66,                 // 2
    RAMPOP = 34,             // 2
    RAMRPOP = 162,            // 2
    JMP = 8,                 // 2
    JB = 9,                  // 2
    JBE = 10,                // 2
    JA = 11,                 // 2
    JAE = 12,               // 2
    JE = 13,                // 2
    JNE = 14,               // 2
    CALL = 15,              // 2
    RET = 16,                // 1
    OUTC = 17,               // 1
    SQRT = 18,               // 1
    ax = 1,
    bx = 2,
    cx = 3,
    dx = 4
};

#endif //ENCODING_H
