#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <math.h>

#include "Perceptron.h"

using namespace std;

namespace Perceptron
{
    perceptron::perceptron(float eta, int epochs)
    {
        m_epochs = epochs;
        m_eta = eta;
    }

    void perceptron::fit(std::vector< std::vector<float> > X, std::vector<float> y)
    {
        for (int i = 0; i < X[0].size() + 1; i++) // X[0].size() + 1 -> I am using +1 to add the bias term
        {
            m_w.push_back(0);
        }
        for (int i = 0; i < m_epochs; i++)
        {
            int errors = 0;
            for (int j = 0; j < X.size(); j++)
            {

                // update weight value
#ifdef PROBLEM1
                // Using Floating-point 16
                float update = alu.FP16_mul(m_eta, alu.FP16_add(y[j], (-1 * predict(X[j]))));
                for (int w = 1; w < m_w.size(); w++) {
                    m_w[w] = alu.FP16_add(m_w[w], alu.FP16_mul(update, X[j][w - 1]));
                }
#elif defined(PROBLEM2)
                // Using Fixed-point
                float update = alu.signed_fix_mul(m_eta, alu.signed_fix_sub(y[j], predict(X[j])));
                for (int w = 1; w < m_w.size(); w++) {
                    m_w[w] = alu.signed_fix_add(m_w[w], alu.signed_fix_mul(update, X[j][w - 1]));
                }
#else
                float update = m_eta * (y[j] - predict(X[j]));
                // MAC
                for (int w = 1; w < m_w.size(); w++){ m_w[w] += update * X[j][w - 1]; }
#endif

                m_w[0] = update;
                // threshold function
                errors += update != 0 ? 1 : 0;
            }
            m_errors.push_back(errors);
        }
    }

    float perceptron::netInput(std::vector<float> X)
    {
        // inference prediction
        // Sum(Vector of weights * Input vector) + bias
        float probabilities = m_w[0];
        // MAC
        for (int i = 0; i < X.size(); i++)
        {
#ifdef PROBLEM1
            // Using Floating-point 16
            probabilities = alu.FP16_add(probabilities, alu.FP16_mul(X[i], m_w[i + 1]));
#elif defined(PROBLEM2)
            // Using Fixed-point
            probabilities = alu.signed_fix_add(probabilities, alu.signed_fix_mul(X[i], m_w[i + 1]));
#elif defined(PROBLEM3)
            // Using Fixed-point
            probabilities = alu.signed_fix_add(probabilities, alu.Radix4_mul(X[i], m_w[i + 1])); 
#elif defined(PROBLEM4)
            // Using Fixed-point
            probabilities = alu.signed_fix_add(probabilities, alu.LowCostMul(X[i], m_w[i + 1]));
#else
            probabilities += X[i] * m_w[i + 1];
#endif

        }
#ifdef PROBLEM3
        /* 1 delay for full adder, but we need to calculate 8 bit binary adder, so we need 8*1 delay for calculation.
            In this simulation, we treat overflow detection and sign extension for 2 gate-level delay */
        int add_latency = (8 + 2) * X.size(); // for ripple carry adder
        /* In ripple carry adder, we only need 5 and/or gates to implement one full adder, and reuse the full adder
            for whole calculation. Also we need 3 and/or gates to detect overflow.*/
        int add_gate = (5 + 3) * X.size(); // for ripple carry adder
#elif defined(PROBLEM4)
        /* 1 delay for calculate p, g. 2 delay for calculate 4-bit lookahead carry generator, however we are tring to 
            implement 8-bit binary adder, so we need extra two delay to calculate 8 bit result. 1 delay for sum.
            In this simulation, we treat overflow detection and sign extension for 2 gate-level delay. */
        int add_latency = (1 + 2*2 + 1 + 2) * X.size(); // for carry lookahead adder
        /* In carry lookahead adder, we need 2*8 and/or gates to generate 8-bit p,g signal. 11 and/or gates for 4-bit 
            lookahead carry generator. 1*8 and/or gates to generate sum. Also we need 3 and/or gates to detect overflow.*/
        int add_gate = (35 + 3) * X.size(); // for carry lookahead adder
#else
#endif
        return probabilities;
    }

    int perceptron::predict(std::vector<float> X)
    {
        return netInput(X) > 0 ? 1 : -1; //Step Function
    }

    void perceptron::printErrors()
    {
        for (const auto& error : m_errors) {
            std::cout << error << " ";
        }
        std::cout << std::endl;
    }

    void perceptron::exportWeights(std::string filename)
    {
        std::ofstream outFile;
        outFile.open(filename);

        for (int i = 0; i < m_w.size(); i++)
        {
            outFile << m_w[i] << std::endl;
        }

        outFile.close();
    }

    void perceptron::importWeights(std::string filename)
    {
        std::ifstream inFile;
        inFile.open(filename);

        for (int i = 0; i < m_w.size(); i++)
        {
            inFile >> m_w[i];
        }
    }

    void perceptron::printWeights()
    {
        std::cout << "weights: ";
        for (int i = 0; i < m_w.size(); i++)
        {
            std::cout << m_w[i] << " ";
        }
        std::cout << std::endl;
    }
}

