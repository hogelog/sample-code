#include <cstdio>
#include <cstring>
#include <vector>
#include <stack>

#define MEMSIZE 30000

enum Opcode {
    INC = 0, DEC, NEXT, PREV, GET, PUT, OPEN, CLOSE, END,
};
const char *OPCODE_NAMES[] = {
    "+", "-", ">", "<",
    ",", ".", "[", "]", "",
    "c", "m", "z",
    "C", "M", "s",
    "N"
};
class Instruction {
public:
    Opcode op;
    int value;
    Instruction(Opcode op) : op(op) {
    }
    Instruction(Opcode op, int value) : op(op) {
      this->value = value;
    }
};
struct ExeCode {
    void *addr;
    int value;
};
class Compiler {
private:
    std::vector<Instruction>* const insns;
    int calc, move;
    std::stack<int> pcstack;
public:
    Compiler(std::vector<Instruction>* insns) :
        insns(insns), calc(0), move(0) {
    }
    void push_simple(Opcode op) {
        insns->push_back(Instruction(op));
    }
    void push_open() {
        pcstack.push(insns->size());
        insns->push_back(Instruction(OPEN));
    }
    void push_close() {
        int open = pcstack.top();
        int diff = insns->size() - open;
        (*insns)[open].value = diff;
        insns->push_back(Instruction(CLOSE, diff + 1));
        pcstack.pop();
    }
};
void parse(std::vector<Instruction> &insns, FILE *input) {
    Compiler compiler(&insns);
    int ch = 0;
    while ((ch=getc(input)) != EOF) {
        switch (ch) {
            case '+':
                compiler.push_simple(INC);
                break;
            case '-':
                compiler.push_simple(DEC);
                break;
            case '>':
                compiler.push_simple(NEXT);
                break;
            case '<':
                compiler.push_simple(PREV);
                break;
            case ',':
                compiler.push_simple(GET);
                break;
            case '.':
                compiler.push_simple(PUT);
                break;
            case '[':
                compiler.push_open();
                break;
            case ']':
                compiler.push_close();
                break;
        }
    }
    compiler.push_simple(END);
}
void debug(std::vector<Instruction> &insns) {
    for (size_t pc=0;;++pc) {
        Instruction insn = insns[pc];
        const char *name = OPCODE_NAMES[insn.op];
        printf("%s", name);
        switch(insn.op) {
            case INC:
            case DEC:
            case NEXT:
            case PREV:
            case GET:
            case PUT:
            case OPEN:
            case CLOSE:
                break;
            case END:
                return;
        }
    }
}
void execute(std::vector<Instruction> &insns, int membuf[MEMSIZE]) {
    ExeCode exec[insns.size()];
    for (size_t pc=0;;++pc) {
        Instruction insn = insns[pc];
        exec[pc].value = insn.value;
        switch(insn.op) {
            case INC:
                exec[pc].addr = &&LABEL_INC;
                break;
            case DEC:
                exec[pc].addr = &&LABEL_DEC;
                break;
            case NEXT:
                exec[pc].addr = &&LABEL_NEXT;
                break;
            case PREV:
                exec[pc].addr = &&LABEL_PREV;
                break;
            case GET:
                exec[pc].addr = &&LABEL_GET;
                break;
            case PUT:
                exec[pc].addr = &&LABEL_PUT;
                break;
            case OPEN:
                exec[pc].addr = &&LABEL_OPEN;
                break;
            case CLOSE:
                exec[pc].addr = &&LABEL_CLOSE;
                break;
            case END:
                exec[pc].addr = &&LABEL_END;
                goto LABEL_START;
            default:
                return;
        }
    }
LABEL_START:
    int *mem = membuf;
    ExeCode *pc = exec - 1;

#define NEXT_LABEL \
    ++pc; \
    goto *pc->addr

    NEXT_LABEL;
LABEL_INC:
    ++*mem;
    NEXT_LABEL;
LABEL_DEC:
    --*mem;
    NEXT_LABEL;
LABEL_NEXT:
    ++mem;
    NEXT_LABEL;
LABEL_PREV:
    --mem;
    NEXT_LABEL;
LABEL_GET:
    *mem = getchar();
    NEXT_LABEL;
LABEL_PUT:
    putchar(*mem);
    NEXT_LABEL;
LABEL_OPEN:
    if (*mem == 0) {
        pc += pc->value;
    }
    NEXT_LABEL;
LABEL_CLOSE:
    pc -= pc->value;
    NEXT_LABEL;
LABEL_END:
    ;
}
int main(int argc, char *argv[]) {
    static int membuf[MEMSIZE];
    std::vector<Instruction> insns;
    parse(insns, stdin);
    if (argc == 1) {
        execute(insns, membuf);
    } else if (argc == 2) {
        const char *option = argv[1];
        if (strcmp(option, "-debug") == 0) {
            debug(insns);
        }
    }
    return 0;
}
