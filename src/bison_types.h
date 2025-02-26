#pragma once


enum number_type {
    NUMBER_INT,
    NUMBER_FLOAT
};

struct number {
    enum number_type type;
    union {
        int ival;
        double dval;
    };
};


enum operator_type {
    OP_EQ,
    OP_NEQ,
    OP_LT,
    OP_GT,
    OP_LTE,
    OP_GTE,
    OP_PLUS,
    OP_MINUS,
    OP_MUL,
    OP_DIV
};

enum cast_type {
    CAST_INT,
    CAST_FLOAT
};