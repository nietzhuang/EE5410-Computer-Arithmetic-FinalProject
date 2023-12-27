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

class ALU
{
public:
    //ALU(float input, float weight);
    float FP16_mul(float input, float weight);
    float FP16_add(float input, float weight);
    float signed_fix_mul(float input, float weight);
    float signed_fix_add(float input, float weight);
    float signed_fix_sub(float input, float weight);
    int float2fix(float n);

private:    
    /*~ALU() {
        m_input = 0;
        m_weight = 0;
    };*/
};

#endif
