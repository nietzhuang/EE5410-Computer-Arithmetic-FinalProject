#include <iostream>
#include "ALU.h"

using namespace std;

float ALU::FP16_mul(float input, float weight) {
        FP16 fp16_input;
        FP16 fp16_weight;
        FP16 fp16_output;

        fp16_input.value = input;
        fp16_weight.value = weight;

        fp16_output.sign = fp16_input.raw.sign * fp16_weight.raw.sign; 
        fp16_output.mantissa = fp16_input.raw.mantissa * fp16_weight.raw.mantissa; 
        fp16_output.exponent = fp16_input.raw.exponent + fp16_weight.raw.exponent; 
        
        cout << "Input value: " << fp16_input.value << endl;
        cout << "Input mantissa: " << fp16_input.raw.mantissa << endl;

        return fp16_output.value;
        //!!
        
    
    }

    float ALU::FP16_add(float input, float weight) {
        FP16 fp16_input;
        FP16 fp16_weight;

        fp16_input.value = input;
        fp16_weight.value = weight;
        //!!

    };

    float ALU::signed_fix_mul(float input, float weight) {
    //!!
    // Test git branch and pull
    // Test branch with collaboration
    
    };

    float ALU::signed_fix_add(float input, float weight) {

    };
};

