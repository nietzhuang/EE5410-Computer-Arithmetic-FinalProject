#include <iostream>
#include <stdio.h>
#include <math.h>
#include <algorithm>

#include "ALU.h"

#define FIXED_BIT 4

using namespace std;


float ALU::FP16_mul(float input, float weight) {
/*        FP16 fp16_input;
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
        */
        //!!
        
    
    }

    float ALU::FP16_add(float input, float weight) {
/*        FP16 fp16_input;
        FP16 fp16_weight;

        fp16_input.value = input;
        fp16_weight.value = weight;
        //!!
*/
    };

    float ALU::signed_fix_mul(float input, float weight) {
        ALU ALU;
    // shift decimal point to get 16-bit fixed point
        int fixed_input = ALU.float2fix(input);
        int fixed_weight = ALU.float2fix(weight);

        // fixed point multiplication
        int fixed_result = fixed_input * fixed_weight;

        // right shift to fit 16-bit fixed point format
        fixed_result >>= FIXED_BIT;

        // transfer back to floating point
        return static_cast<float>(fixed_result) / (1 << FIXED_BIT);
    };

    float ALU::signed_fix_add(float input, float weight) {
        ALU ALU;
         // shift decimal point to get 16-bit fixed point
        int fixed_input = ALU.float2fix(input);
        int fixed_weight = ALU.float2fix(weight);

        // fixed point addition
        int fixed_result = fixed_input + fixed_weight;

        // transfer back to floating point
        return static_cast<float>(fixed_result) / (1 << FIXED_BIT);
    };

    float signed_fix_sub(float input, float weight) {
        ALU ALU;
        // shift decimal point to get 16-bit fixed point 
        int fixed_input = ALU.float2fix(input);
        int fixed_weight = ALU.float2fix(weight);

        // fixed point addition
        int fixed_result = fixed_input - fixed_weight;

        // transfer back to floating point
        return static_cast<float>(fixed_result) / (1 << FIXED_BIT);
    };

int ALU::float2fix(float n)
   {
       int int_part = 0, frac_part = 0;
       int i;
       float t;

       int_part = (int)floor(n);

       if (int_part > 7) {
           int_part = 7;  //  (1111 in binary)
       } else if (int_part < -8) {
           int_part = -8;  //  (-1000 in binary)
       }

       // std::cout << "integer part: " << int_part << std::endl;
       
        int_part <<= FIXED_BIT;
       
       n -= (int)floor(n);

       t = 0.5;
       for (i = 0; i < FIXED_BIT; i++) {
           if ((n - t) >= 0) {
               n -= t;
               frac_part += (1 << (FIXED_BIT - 1 - i));
           }
           t = t /2;
       }

       // std::cout << "fraction part: " << frac_part << std::endl;

       return int_part + frac_part;
   };


