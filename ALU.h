#ifndef ALU_H
#define ALU_H

typedef union {
    float value;
    struct
    {
        unsigned int mantissa : 23;
        unsigned int exponent : 8;
        unsigned int sign : 1;
    } raw;
} FP16;

struct Adder
{
    int sum;
    int carry;
};

class ALU
{
public:
    //ALU(float input, float weight);
    float FP16_mul(float input, float weight);
    float FP16_add(float input, float weight);
    float signed_fix_mul(float input, float weight);
    float signed_fix_add(float input, float weight);
    float signed_fix_sub(float input, float weight);
    float Radix4_mul(float input, float weight); 
    int CarryLookaheadAdder(int a, int b);
    int rippleCarryAdder(int a, int b);
    int CLA_CPA(int a, int b);
    int float2fix(float n);
    Adder FA(int bitA, int bitB, int c); 

private:    
};

#endif
