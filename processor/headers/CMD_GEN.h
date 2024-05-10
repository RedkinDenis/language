CMD_GEN("add", ADD,
{
    buffer[ptr] = (char)comm;
    ptr += command;
},
{
    MATH_COMM(ADD, +)
},
{

})

CMD_GEN("sub", SUB,
{
    buffer[ptr] = (char)comm;
    ptr += command;
},
{
    MATH_COMM(SUB, -)
},
{

})

CMD_GEN("div", DIV,
{
    buffer[ptr] = (char)comm;
    ptr += command;
},
{
    MATH_COMM(DIV, /)
},
{

})

CMD_GEN("mul", MUL,
{
    buffer[ptr] = (char)comm;
    ptr += command;
},
{
    MATH_COMM(MUL, *)
},
{

})

CMD_GEN("jb", JB,
{
    ASSM_JUMPS("jb", JB)
},
{
    PROC_JUMPS(JB, >)
},
{
    DISASSM_JUMPS()
})

CMD_GEN("jbe", JBE,
{
    ASSM_JUMPS("jbe", JBE)
},
{
    PROC_JUMPS(JBE, >=)
},
{
    DISASSM_JUMPS()
})

CMD_GEN("ja", JA,
{
    ASSM_JUMPS("ja", JA)
},
{
    PROC_JUMPS(JA, <)
},
{
    DISASSM_JUMPS()
})

CMD_GEN("jae", JAE,
{
    ASSM_JUMPS("jae", JAE)
},
{
    PROC_JUMPS(JAE, <=)
},
{
    DISASSM_JUMPS()
})

CMD_GEN("je", JE,
{
    ASSM_JUMPS("je", JE)
},
{
    PROC_JUMPS(JE, ==)
},
{
    DISASSM_JUMPS()
})

CMD_GEN("jne", JNE,
{
    ASSM_JUMPS("jne", JNE)
},
{
    PROC_JUMPS(JNE, !=)
},
{
    DISASSM_JUMPS()
})


CMD_GEN("push", PUSH,
{
    if(isdigit(*(data[i].str + strlen("push "))) || ((*(data[i].str + strlen("push ")) == '-') && isdigit(*(data[i].str + strlen("push -")))) )
    {
        sscanf(data[i].str + strlen("push "), "%d", &num);
        buffer[ptr] = PUSH;
    }
    else PUSH_POP_DET ("push", RAMPUSH, RAMPUSHR, RPUSH);
    ptr += command;
    memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(int));
    ptr += number;
},
{
    if((cmd & 0xE0) == 0)
        {
            STPUSH(cmd_stk, proc->data[proc->ip + command])
            proc->ip += (command + number);
        }

        else if((cmd & 0xE0) == 0x40)
        {
            if(proc->data[proc->ip + command] > 4)
                return UNKNOWN_REGISTER_NAME;

            STPUSH(cmd_stk, proc->reg[(int)proc->data[proc->ip + command] - 1])
            proc->ip += (command + reg);
        }

        else if((cmd & 0x20) == 0x20)
        {
            if((cmd & 0x80) == 0x80)
            {
                if(proc->data[proc->ip + command] > 4)
                    return UNKNOWN_REGISTER_NAME;

                STPUSH(cmd_stk, proc->RAM[(int)proc->reg[(int)proc->data[proc->ip + command] - 1]])
                proc->ip += (command + reg);
            }
            else
            {
                STPUSH(cmd_stk, proc->RAM[(int)proc->data[proc->ip + command]])
                proc->ip += (command + number);
            }
        }
},
{
    if (cmd == PUSH)
    {
        memcpy(&num, data + ptr, sizeof(int));
        fprintf(out, " %d", num);
        ptr += number;
    }
    else if (cmd == RPUSH)
    {
        switch(data[ptr])
        {
            case ax:
                fprintf(out, " ax");
                break;
            case bx:
                fprintf(out, " bx");
                break;
            case cx:
                fprintf(out, " cx");
                break;
            case dx:
                fprintf(out, " dx");
                break;
            default:
                return UNKNOWN_REGISTER_NAME;
        }

        ptr += reg;
    }
    else if (cmd == RAMPUSH)
    {
        memcpy(&num, data + ptr, sizeof(int));
        fprintf(out, " [%d]", num);
        ptr += number;
    }
    else if (cmd == RAMPUSHR)
    {
        switch(data[ptr])
        {
            case ax:
                fprintf(out, " [ax]");
                break;
            case bx:
                fprintf(out, " [bx]");
                break;
            case cx:
                fprintf(out, " [cx]");
                break;
            case dx:
                fprintf(out, " [dx]");
                break;
            default:
                return UNKNOWN_REGISTER_NAME;
        }

        ptr += reg;
    }
})

CMD_GEN("pop", (POP & 0x1F),
{
    PUSH_POP_DET ("pop", RAMPOP, RAMRPOP, POP);
    ptr += command;
    memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(char));
    ptr += number;
},
{
    if((cmd & 0x40) == 0x40)
        {

            STPOP(cmd_stk, proc->reg[(int)proc->data[proc->ip + command] - 1])
            proc->ip += (command + reg);
        }
        else if((cmd & 0x20) == 0x20)
        {

            STPOP(cmd_stk, proc->RAM[(int)proc->data[proc->ip + command]])
            proc->ip += (command + number);
        }
        else if((cmd & 0xA0) == 0xA0)
        {

            STPOP(cmd_stk, proc->RAM[(int)proc->reg[(int)proc->data[proc->ip + command] - 1]]);
            proc->ip += (command + reg);
        }
},
{
    if (cmd == POP)
    {
        switch(data[ptr])
        {
            case ax:
                fprintf(out, " ax");
                break;
            case bx:
                fprintf(out, " bx");
                break;
            case cx:
                fprintf(out, " cx");
                break;
            case dx:
                fprintf(out, " dx");
                break;
            default:
                return UNKNOWN_REGISTER_NAME;
        }

        ptr += reg;
    }
    else if (cmd == RAMPOP)
    {
        memcpy(&num, data + ptr, sizeof(int));
        fprintf(out, " [%d]", num);
        ptr += number;
    }
    else if (cmd == RAMRPOP)
    {
        switch(data[ptr])
        {
            case ax:
                fprintf(out, " [ax]");
                break;
            case bx:
                fprintf(out, " [bx]");
                break;
            case cx:
                fprintf(out, " [cx]");
                break;
            case dx:
                fprintf(out, " [dx]");
                break;
            default:
                return UNKNOWN_REGISTER_NAME;
        }

        ptr += reg;
    }
})

CMD_GEN("out", OUT,
{
    buffer[ptr] = (char)comm;
    ptr += command;
},
{
    STPOP(cmd_stk, x)
    proc->ip += command;
    printf("\nOUT - %f\n", x);
},
{

})

CMD_GEN("sqrt", SQRT,
{
    buffer[ptr] = (char)comm;
    ptr += command;
},
{
    STPOP(cmd_stk, x)
    x = (elem_t)sqrt(x);
    proc->ip += command;
    STPUSH(cmd_stk, x)
},
{

})

CMD_GEN("in", IN,
{
    buffer[ptr] = (char)comm;
    ptr += command;
},
{
    printf("\nenter push element ");
    scanf("%d", &x);
    STPUSH(cmd_stk, x)
    proc->ip += command;
},
{

})

CMD_GEN("hlt", HLT,
{
    buffer[ptr] = (char)comm;
    ptr += command;
},
{
    stack_dtor(&(proc->cmd_stk));
    proc->ip += command;
    running = 0;
},
{

})

CMD_GEN("outc", OUTC,
{
    buffer[ptr] = (char)comm;
    ptr += command;
},
{
    STPOP(cmd_stk, temp)
    liter = (int)temp;
    proc->ip += command;
    printf("%c", liter);
},
{

})

CMD_GEN("call", CALL,
{
    buffer[ptr] = CALL;
    ptr += command;

    CHECK_MARK("call")

    memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(int));
    ptr += number;
},
{
    x = proc->ip + command + number;
    STPUSH(call_stk, x)
    memcpy(&proc->ip, &proc->data[proc->ip + command], sizeof(int));
},
{
    DISASSM_JUMPS()
})

CMD_GEN("ret", RET,
{
    buffer[ptr] = (char)comm;
    ptr += command;
},
{
    STPOP(call_stk, temp)
    proc->ip = (int)temp;
},
{

})

CMD_GEN("jump", JMP,
{
    buffer[ptr] = JMP;
    ptr += command;

    CHECK_MARK("call")

    memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(int));
    ptr += number;
},
{
    memcpy(&proc->ip, &proc->data[proc->ip + command], sizeof(int));
},
{
    DISASSM_JUMPS()
})

