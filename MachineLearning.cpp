#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <math.h>


#include "MachineLearning.h"

namespace MachineLearning 
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
                float update = m_eta * (y[j] - predict(X[j]));
                // MAC
                for (int w = 1; w < m_w.size(); w++){ m_w[w] += update * X[j][w - 1]; }
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
            probabilities += X[i] * m_w[i + 1];
        }
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