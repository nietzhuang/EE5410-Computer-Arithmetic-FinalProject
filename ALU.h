class ALU
{
public:
    ALU(float input, 
        float weight)
    {
    m_input = input;
    m_weight = weight;
    };
    
    ~ALU() {
        m_input = 0;
        m_weight = 0;
    };

    float FP16_mul(float input, float weight) {
        FP16 fp16_input;
        FP16 fp16_weight;
        FP16 fp16_output;
        
        m_input = input;
        m_weight = weight;

        fp16_input.value = m_input;
        fp16_weight.value = m_weight;

        fp16_output.sign = fp16_input.raw.sign * fp16_weight.raw.sign; 
        fp16_output.mantissa = fp16_input.raw.mantissa * fp16_weight.raw.mantissa; 
        fp16_output.exponent = fp16_input.raw.exponent + fp16_weight.raw.exponent; 

        return fp16_output.value;
        //!!
        
    
    };

    float FP16_add(float input, float weight) {
        FP16 fp16_input;
        FP16 fp16_weight;

        fp16_input.value = input;
        fp16_weight.value = weight;
        //!!

    };

    float signed_fix_mul(float input, float weight) {
    //!!
    // Test git branch and pull
    
    };

    float signed_fix_add(float input, float weight) {

    };


};

typedef union {
    float value;
    struct
    {
        unsigned int mantissa : 23;
        unsigned int exponent : 8;
        unsigned int sign : 1;
    } raw;
} FP16;
