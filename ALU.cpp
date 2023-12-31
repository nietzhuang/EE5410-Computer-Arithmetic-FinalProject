#include <iostream>
#include <stdio.h>
#include <math.h>
#include <algorithm>

#include "ALU.h"

#define BIT_NUM 8
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
#ifdef PROBLEM3
        int fixed_result = CarryLookaheadAdder(fixed_input, fixed_weight);
#elif defined(PROBLEM4)
        int fixed_result = rippleCarryAdder(fixed_input, fixed_weight);
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
#ifdef PROBLEM3
        int fixed_result = CarryLookaheadAdder(fixed_input, -fixed_weight);
#elif defined(PROBLEM4)
        int fixed_result = rippleCarryAdder(fixed_input, -fixed_weight);
#else
        int fixed_result = fixed_input - fixed_weight;
#endif

        // converse back to floating point
        return static_cast<float>(fixed_result) / (1 << FIXED_BIT);
    };

    float ALU::Radix4_mul(float input, float weight) {
        Adder res_obj[10];

        int bitB[8];
        int _2bitB;
        int add;
        bool sign_flag_in, sign_flag_w = 0;
        int partprod[4] = {0};
        int partprod_tree[2] = {0};
        int product = 0;
        
        int fixed_input;
        int fixed_weight;
        
        // Format conversion and sign detection 
        if(std::signbit(input)) {
            fixed_input = float2fix(-input);
            sign_flag_in = 1;
        }
        else 
            fixed_input = float2fix(input);

        if(std::signbit(weight)) {
            fixed_weight = float2fix(-weight);
            sign_flag_w = 1;
        }
        else 
            fixed_weight = float2fix(weight);


        for(int i = 0; i < 8; i++) {
            bitB[i] = (fixed_weight >> i) & 0x0001;
        }
    
        // precompute 0, a, 2a, and 3a
        for(int i = 0; i < BIT_NUM; i = i + 2) {
            _2bitB = bitB[i+1] * 2 + bitB[i];

            add = fixed_input << i;
            if(_2bitB == 0)
                partprod[i/2] = 0;
            else if(_2bitB == 1)
                partprod[i/2] = add;
            else if(_2bitB == 2)
                partprod[i/2] = add << 1;
            else if(_2bitB == 3)
                partprod[i/2] = (add << 1) + add;
        }
        
        // Wallace tree at the 1st level.
        res_obj[0] = FA((partprod[0] >> 2) & 0x0001, (partprod[1] >> 2) & 0x0001, 0);  
        res_obj[1] = FA((partprod[0] >> 3) & 0x0001, (partprod[1] >> 3) & 0x0001, 0);  
        res_obj[2] = FA((partprod[0] >> 4) & 0x0001, (partprod[1] >> 4) & 0x0001, (partprod[2] >> 4) & 0x0001);  
        res_obj[3] = FA((partprod[0] >> 5) & 0x0001, (partprod[1] >> 5) & 0x0001, (partprod[2] >> 5) & 0x0001);  
        res_obj[4] = FA((partprod[0] >> 6) & 0x0001, (partprod[1] >> 6) & 0x0001, (partprod[2] >> 6) & 0x0001);  
        res_obj[5] = FA((partprod[0] >> 7) & 0x0001, (partprod[1] >> 7) & 0x0001, (partprod[2] >> 7) & 0x0001);  
        res_obj[6] = FA((partprod[1] >> 8) & 0x0001, (partprod[2] >> 8) & 0x0001, (partprod[3] >> 8) & 0x0001);  
        res_obj[7] = FA((partprod[1] >> 9) & 0x0001, (partprod[2] >> 9) & 0x0001, (partprod[3] >> 9) & 0x0001);  
        res_obj[8] = FA(0,                           (partprod[2] >> 10) & 0x0001, (partprod[3] >> 10) & 0x0001);  
        res_obj[9] = FA(0,                           (partprod[2] >> 11) & 0x0001, (partprod[3] >> 11) & 0x0001);  

        partprod_tree[0] = (((partprod[0] >> 0) & 0x0001) << 0) + (((partprod[0] >> 1) & 0x0001) << 1)
                        + (res_obj[0].sum << 2) + (res_obj[1].sum << 3)
                        + (res_obj[2].sum << 4) + (res_obj[3].sum << 5)
                        + (res_obj[4].sum << 6) + (res_obj[5].sum << 7)
                        + (res_obj[6].sum << 8) + (res_obj[7].sum << 9)
                        + (res_obj[8].sum << 10) + (res_obj[9].sum << 11)
                        + (((partprod[3] >> 12) & 0x0001) << 12) + (((partprod[3] >> 13) & 0x0001) << 13);  
        partprod_tree[1] = (res_obj[0].carry << 3) + (res_obj[1].carry << 4)                                       
                        + (res_obj[2].carry << 5) + (res_obj[3].carry << 6)
                        + (res_obj[4].carry << 7) + (res_obj[5].carry << 8)
                        + (res_obj[6].carry << 9) + (res_obj[7].carry << 10)
                        + (res_obj[8].carry << 11) + (res_obj[9].carry << 12);

        // Wallace tree at the 2nd level.
        res_obj[0] = FA((partprod_tree[0] >> 3) & 0x0001, (partprod_tree[1] >> 3) & 0x0001, 0);
        res_obj[1] = FA((partprod_tree[0] >> 4) & 0x0001, (partprod_tree[1] >> 4) & 0x0001, 0);
        res_obj[2] = FA((partprod_tree[0] >> 5) & 0x0001, (partprod_tree[1] >> 5) & 0x0001, 0);
        res_obj[3] = FA((partprod_tree[0] >> 6) & 0x0001, (partprod_tree[1] >> 6) & 0x0001, (partprod[3] >> 6) & 0x0001);
        res_obj[4] = FA((partprod_tree[0] >> 7) & 0x0001, (partprod_tree[1] >> 7) & 0x0001, (partprod[3] >> 7) & 0x0001);
        res_obj[5] = FA((partprod_tree[0] >> 8) & 0x0001, (partprod_tree[1] >> 8) & 0x0001, 0);
        res_obj[6] = FA((partprod_tree[0] >> 9) & 0x0001, (partprod_tree[1] >> 9) & 0x0001, 0);
        res_obj[7] = FA((partprod_tree[0] >> 10) & 0x0001, (partprod_tree[1] >> 10) & 0x0001, 0);
        res_obj[8] = FA((partprod_tree[0] >> 11) & 0x0001, (partprod_tree[1] >> 11) & 0x0001, 0);
        res_obj[9] = FA((partprod_tree[0] >> 12) & 0x0001, (partprod_tree[1] >> 12) & 0x0001, 0);
       
        partprod_tree[0] = (((partprod[0] >> 0) & 0x0001) << 0) + (((partprod[0] >> 1) & 0x0001) << 1)
                        + (((partprod_tree[0] >> 2) & 0x0001) << 2) + (res_obj[0].sum << 3)
                        + (res_obj[1].sum << 4) + (res_obj[2].sum << 5)
                        + (res_obj[3].sum << 6) + (res_obj[4].sum << 7)
                        + (res_obj[5].sum << 8) + (res_obj[6].sum << 9)
                        + (res_obj[7].sum << 10) + (res_obj[8].sum << 11)
                        + (res_obj[9].sum << 12) + (((partprod[3] >> 13) & 0x0001) << 13);
        partprod_tree[1] = (res_obj[0].carry << 4) + (res_obj[1].carry << 5)
                        + (res_obj[2].carry << 6) + (res_obj[3].carry << 7) 
                        + (res_obj[4].carry << 8) + (res_obj[5].carry << 9) 
                        + (res_obj[6].carry << 10) + (res_obj[7].carry << 11) 
                        + (res_obj[8].carry << 12) + (res_obj[9].carry << 13); 

            
        // 14-bit CPA using carry-lookahead adder
        int lsb8[2] = {(partprod_tree[0] & 0x00FF), (partprod_tree[1] & 0x00FF)};
        int msb8[2] = {((partprod_tree[0] & 0xFF00) >> 8), ((partprod_tree[1] & 0xFF00) >> 8)};
        int lsb8_sum;
        int msb8_sum;

        lsb8_sum = CLA_CPA(lsb8[0], lsb8[1]);
        msb8_sum = CLA_CPA(msb8[0], msb8[1]);
        product = (msb8_sum << 8) + lsb8_sum;

        // Sign dectection and sign extension
        if(sign_flag_in ^ sign_flag_w)
            product = ~product + 1;        


        return static_cast<float>(product) / (1 << 8);
    };

    float ALU::LowCostMul(float input, float weight) {
        Adder res[16];

        bool sign_flag_in, sign_flag_w = 0;
        int partprod[BIT_NUM] = {0};
        
        // Format conversion
        int fixed_input;
        int fixed_weight;

        // Format conversion and sign detection 
        if(std::signbit(input)) {
            fixed_input = float2fix(-input);
            sign_flag_in = 1;
        }
        else 
            fixed_input = float2fix(input);

        if(std::signbit(weight)) {
            fixed_weight = float2fix(-weight);
            sign_flag_w = 1;
        }
        else 
            fixed_weight = float2fix(weight);

        // Obtain partial product using an AND gate
        for(int i = 0; i < BIT_NUM; i++) {
            if (fixed_weight & (1 << i)) {
                for(int j = 0; j < BIT_NUM; j++) {  // use only one AND gate to compute separatelly
                    partprod[i] += (fixed_input & (1 << j));
                }
            }
            else
                partprod[i] = 0;

            partprod[i] = partprod[i] << i;
        }

        // Compute the additions using a ripple carry adder by one FA
        // Compute the 1st level additions among the partial product 
        for(int i = 0; i < BIT_NUM/2; i++) {
            for(int j = 0; j < 16; j++) {  // Shift 16-bit since the final product is 16 bits
                if (j == 0) {
                    res[j] = FA((partprod[2*i] >> j) & 0x0001, (partprod[2*i+1] >> j) & 0x0001, 0);
                }
                else
                    res[j] = FA((partprod[2*i] >> j) & 0x0001, (partprod[2*i+1] >> j) & 0x0001 , res[j-1].carry);
            }            

            partprod[i] = 0;
            for(int j = 0; j < 16; j++) {
                partprod[i] += (res[j].sum << j);
            }
        }

        // Compute the 2nd level additions among the partial products
        for(int i = 0; i < BIT_NUM/4; i++) {
            //for(int j = 0; j < BIT_NUM; j++) {
            for(int j = 0; j < 16; j++) {
                if (j == 0) {
                    res[j] = FA((partprod[2*i] >> j) & 0x0001, (partprod[2*i+1] >> j) & 0x0001, 0);
                }
                else
                    res[j] = FA((partprod[2*i] >> j) & 0x0001, (partprod[2*i+1] >> j) & 0x0001 , res[j-1].carry);
            }            

            partprod[i] = 0;
            //for(int j = 0; j < BIT_NUM; j++) {
            for(int j = 0; j < 16; j++) {
                partprod[i] += (res[j].sum << j);
            }
        }

        // Compute the 3rd level additions among the partial products
        for(int i = 0; i < BIT_NUM/8; i++) {
            //for(int j = 0; j < BIT_NUM; j++) {
            for(int j = 0; j < 16; j++) {
                if (j == 0) {
                    res[j] = FA((partprod[2*i] >> j) & 0x0001, (partprod[2*i+1] >> j) & 0x0001, 0);
                }
                else
                    res[j] = FA((partprod[2*i] >> j) & 0x0001, (partprod[2*i+1] >> j) & 0x0001 , res[j-1].carry);
            }            

            partprod[i] = 0;
            for(int j = 0; j < 16; j++) {
                partprod[i] += (res[j].sum << j);
            }
        }

        // Sign dectection and sign extension
        if(sign_flag_in ^ sign_flag_w)
            partprod[0] = ~partprod[0] + 1;       
           
        
        return static_cast<float>(partprod[0]) / (1 << 8);
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

int ALU::CLA_CPA(int a, int b) {
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

        return sum;
    };

int ALU::float2fix(float n)
   {
       int int_part = 0, frac_part = 0;
       int i;
       float t;

       int_part = (int)floor(n);

       // Data is quantized since the range is limited.
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

    Adder ALU::FA(int bitA, int bitB, int c) {
        Adder result;        
    
        int sum = c ^ (bitA ^ bitB);
        int carry = bitA & bitB + (c & (bitA ^ bitB));
        //int carry = (bitA & bitB) + (bitB & c) + (bitA & c);
                     
        result.sum = sum;
        result.carry = carry;        

        return result;
    };

