#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <math.h> 

namespace MachineLearning
{
    class perceptron
    {
    public:
        perceptron(float eta,int epochs);
        float netInput(std::vector<float> X);
        int predict(std::vector<float> X);
        void fit(std::vector< std::vector<float> > X, std::vector<float> y);
        void printErrors();
        void exportWeights(std::string filename);
        void importWeights(std::string filename);
        void printWeights();
    private:
        float m_eta;
        int m_epochs;
        std::vector < float > m_w;
        std::vector < float > m_errors;
    };
}