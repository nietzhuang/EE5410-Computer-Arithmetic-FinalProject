#include <iostream>
#include <stdio.h>
#include <math.h>
#include <algorithm>

#include "ALU.h"

#define FIXED_BIT 4

using namespace std;


float ALU::FP16_mul(float input, float weight) {
        FP16 fp16_input;
        FP16 fp16_weight;
        FP16 fp16_output;

        fp16_input.value = input;
        fp16_weight.value = weight;

        // Compute sign, mantissa, and exponent separatelly.
        float sign = fp16_input.raw.sign ^ fp16_weight.raw.sign;
        float mantissa = (1.0 + fp16_input.raw.mantissa / pow(2, 23)) * (1.0 + fp16_weight.raw.mantissa / pow(2, 23));
        float exponent = (fp16_input.raw.exponent - 127) + (fp16_weight.raw.exponent - 127);  // minus biases
        fp16_output.value = pow(-1, sign) * mantissa * pow(2, exponent);

        //cout << sign << "  " << mantissa << " = " << exponent << endl;
        //cout << "Output Value: " << fp16_output.value << endl;
        
        return fp16_output.value;
    };

    float ALU::FP16_add(float input, float weight) {
        FP16 fp16_input;
        FP16 fp16_weight;
        FP16 fp16_output;

        fp16_input.value = input;
        fp16_weight.value = weight;

        // Compute mantissa and exponent separatelly.
        // The float datatype will process postshift and round by itself.
        float mantissa =  pow(-1, fp16_input.raw.sign) * (1.0 + fp16_input.raw.mantissa / pow(2, 23))
                        + pow(-1, fp16_weight.raw.sign) * (1.0 + fp16_weight.raw.mantissa / pow(2, 23))
                        * pow(2, (fp16_weight.raw.exponent - fp16_input.raw.exponent));  // preshift the mantissa of weight 
        float exponent = fp16_input.raw.exponent;
        fp16_output.value = mantissa * pow(2, exponent - 127);

        //cout << "Output Value: " << fp16_output.value << endl;
        
        return fp16_output.value;
    };

    float ALU::signed_fix_mul(float input, float weight) {
    // shift decimal point to get 16-bit fixed point
        int fixed_input = float2fix(input);
        int fixed_weight = float2fix(weight);

        // fixed point multiplication
        int fixed_result = fixed_input * fixed_weight;

        // right shift to fit 16-bit fixed point format
        fixed_result >>= FIXED_BIT;

        // converse back to floating point
        return static_cast<float>(fixed_result) / (1 << FIXED_BIT);
    };

    float ALU::signed_fix_add(float input, float weight) {
         // shift decimal point to get 16-bit fixed point
        int fixed_input = float2fix(input);
        int fixed_weight = float2fix(weight);

        // fixed point addition
#if TASK == problem3
        int fixed_result = CarryLookaheadAdder(fixed_input, fixed_weight);
        // int fixed_result = rippleCarryAdder(fixed_input, fixed_weight);
#else
        int fixed_result = fixed_input + fixed_weight;
#endif

        // converse back to floating point
        return static_cast<float>(fixed_result) / (1 << FIXED_BIT);
    };

    float ALU::signed_fix_sub(float input, float weight) {
        // shift decimal point to get 16-bit fixed point 
        int fixed_input = float2fix(input);
        int fixed_weight = float2fix(weight);

        // fixed point addition
#if TASK == problem3
        int fixed_result = CarryLookaheadAdder(fixed_input, -fixed_weight);
        // int fixed_result = rippleCarryAdder(fixed_input, -fixed_weight);
#else
        int fixed_result = fixed_input - fixed_weight;
#endif

        // converse back to floating point
        return static_cast<float>(fixed_result) / (1 << FIXED_BIT);
    };

    int ALU::CarryLookaheadAdder(int a, int b){
        int bitA[8], bitB[8], cg[8], cp[8], carry[8];
        int sum = 0;
        int carry_in = 0;

        carry[0] = carry_in;

        for (int i = 0; i < 8; i++){
            bitA[i] = (a >> i) & 1;
            bitB[i] = (b >> i) & 1;
            cg[i] = bitA[i]&bitB[i]; // g_i generation
            cp[i] = bitA[i]^bitB[i]; // p_i generation
        }

        for (int j = 0; j < 2; j++){
            carry[4*j + 1] = cg[4*j]| (carry[4*j]&cp[4*j]);
            carry[4*j + 2] = cg[4*j+1] | (cg[4*j]&cp[4*j+1]) | (carry[4*j]&cp[4*j]&cp[4*j+1]);
            carry[4*j + 3] = cg[4*j+2] | (cg[4*j+1]&cp[4*j+2]) | (cg[4*j]&cp[4*j+1]&cp[4*j+2]) | (carry[4*j]&cp[4*j]&cp[4*j+1]&cp[4*j+2]);
            carry[4*j + 4] = (carry[4*j + 3]&cp[4*j+3]) | cg[4*j+3];
        }

        for (int i = 0; i < 8; i++){
            int xorResult = bitA[i] ^ bitB[i] ^ carry[i]; // XOR of input bits and previous carry
            sum |= (xorResult << i); // Set the i-th bit of 'sum' based on XOR result
        }
        
        int signA = (a >> 7) & 1; // get sign a 
        int signB = (b >> 7) & 1; // get sign b
        int signResult = (sum >> 7) & 1; // get sign sum

        // overflow detection
        if ((signA == signB) && (signResult != signA)) {
            if (signA) {
                sum = 0x80; // 1000 0000
            } else {
                sum = 0x7F; // 0111 1111
            }
        }

        // sign extension
        if (sum & 0x80) { // check MSB
            sum |= 0xFFFFFF00; // sign extension
        } else {
            sum &= 0xFF; 
        }

        return sum;
    };

    int ALU::rippleCarryAdder(int a, int b) {
        int sum = 0; // Initialize the sum to 0
        int carry = 0; // Initialize the carry to 0

        // only have one FA to do all 8-bit adder calculation
        for (int i = 0; i < 8; i++) {
            int bitA = (a >> i) & 1; // Get the i-th bit of 'a'
            int bitB = (b >> i) & 1; // Get the i-th bit of 'b'

            int xorResult = bitA ^ bitB ^ carry; // XOR of input bits and previous carry
            sum |= (xorResult << i); // Set the i-th bit of 'sum' based on XOR result
            carry = (bitA & bitB) | (carry & (bitA ^ bitB)); // Calculate the new carry
        }

        int signA = (a >> 7) & 1; // get sign a 
        int signB = (b >> 7) & 1; // get sign b
        int signResult = (sum >> 7) & 1; // get sign sum

        // overflow detection
        if ((signA == signB) && (signResult != signA)) {
            if (signA) {
                sum = 0x80; // 1000 0000
            } else {
                sum = 0x7F; // 0111 1111
            }
        }

        // sign extension
        if (sum & 0x80) { // check MSB
            sum |= 0xFFFFFF00; // sign extension
        } else {
            sum &= 0xFF; 
        }

        return sum;
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


