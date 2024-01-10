#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h> 

#include <chrono>
#include <random>

#include "Perceptron.h"

using namespace std;
using namespace Perceptron;

vector< vector<float> > getIrisX();
vector<float> getIrisy();

#define Training_data 60

int main()
{
#ifdef PROBLEM1
    cout << "\n======== Problem 1 ========\n"  << endl;
#elif defined(PROBLEM2)
    cout << "\n======== Problem 2 ========\n"  << endl;
#elif defined(PROBLEM3)
    cout << "\n======== Problem 3 ========\n"  << endl;
#elif defined(PROBLEM4)
    cout << "\n======== Problem 4 ========\n"  << endl;
#else
    cout << "\n======== Origin ========\n"  << endl;
#endif


    // record start time 
    auto start = std::chrono::high_resolution_clock::now();
   
    //get data
    vector< vector<float> > X = getIrisX();
    vector<float> y = getIrisy();

    //random data
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(X.begin(), X.end(), std::default_random_engine(seed));
    shuffle(y.begin(), y.end(), std::default_random_engine(seed)); 

    vector<vector<float>> X_train(X.begin(), X.begin() + Training_data);
    vector<float> y_train(y.begin(), y.begin() + Training_data);
    vector<vector<float>> X_test(X.begin() + Training_data, X.end());
    vector<float> y_test(y.begin() + Training_data, y.end());

    // train model learning rate = 0.1, epochs = 14
    perceptron clf(0.1, 14);
    clf.fit(X_train, y_train);

    // print error
    clf.printErrors();

    // validate model
    int correct_predictions = 0;
    for (size_t i = 0; i < X_test.size(); i++) {
        if (clf.predict(X_test[i]) == y_test[i])
            correct_predictions++;
    }

    // record end time
    auto end = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < X_test.size(); i++) {
        int predicted = clf.predict(X_test[i]);
        cout << "Sample: " << i 
             << " - Actual class: " << y_test[i] 
             << "\tPredicted class: " << predicted << endl;
    }
    
    // calculate overall accuracy
    double accuracy = static_cast<double>(correct_predictions) / X_test.size();
    cout << "Accuracy: " << accuracy * 100 << "%" << endl;


    // Calculatate latency and gate cost         
#ifdef PROBLEM3
    /* 1 delay for calculate p, g. 2 delay for calculate 4-bit lookahead carry generator, however we are tring to 
        implement 8-bit binary adder, so we need extra two delay to calculate 8 bit result. 1 delay for sum.
        In this simulation, we treat overflow detection and sign extension for 2 gate-level delay. */
    //int add_latency = (1 + 2*2 + 1 + 2) * X.size(); // for carry lookahead adder
    int add_latency = (1 + 2*2 + 1 + 2) * X_test.size() * (X_test[0].size() / 2); // for carry lookahead adder
    /* 4 delay for the precomputation of 3a when using FAs. Wallace CSA tree takes 4*2 and the 14-CPA using CLAs 
       takes (1 + 2*2 + 1 + 2) delay. Last, sign detection takes 2 delay. */ 
    int mul_latency = (4 + (1 + 2*2 + 1 + 2) + 2) * X_test.size(); 
    
    cout << endl;
    cout << "Total latency for additions by carry-lookahead adder: " << add_latency << endl;
    cout << "Total latency for multiplications by Wallace CSA tree: " << mul_latency << endl;

    /* In carry lookahead adder, we need 2*8 and/or gates to generate 8-bit p,g signal. 11 and/or gates for 4-bit 
        lookahead carry generator. 1*8 and/or gates to generate sum. Also we need 3 and/or gates to detect overflow.*/
    int add_gate = (35 + 3) * 2; // for carry lookahead adder
    /* For multiplication, we use a fixed-point adder to precompute the 3a using CLAs, which requires 35+3 gates.
      The Wallace tree uses 20 FA and HA, and two CLA adders, totally requires 5*20 + (35+3)*2 gates */
    int mul_gate = (5 * 20 + (35 + 3)) * 4;

    cout << endl;
    cout << "Total gates from the carry-lookahead adder: " << add_gate << endl;
    cout << "Total gates from the Wallace CSA tree: " << mul_gate << endl;
#elif defined(PROBLEM4)
    /* 4 delay for full adder, but we need to calculate 8 bit binary adder, so we need 8*4 delay for calculation.
        In this simulation, we treat overflow detection and sign extension for 2 gate-level delay */
    int add_latency = (32 + 2) * X_test.size() * (X_test[0].size() - 1); // for ripple carry adder
    /* Using one AND gate to get all partial products at the beginning requres 1*8*8 delay. Moreover, 
       using a FA to compute addition among all partial products requires 8 * 4 + 8 * 2 + 8 delay
       among the 1st to 3rd level additions. Additionally, sign detecion requires 2 delay. */
    int mul_latency = ((1 * 8 * 8) + (8 * 4 + 8 * 2 + 8)) * X_test.size() * X_test[0].size(); 

    cout << endl;
    cout << "Total latency for additions by carry-lookahead adder: " << add_latency << endl;
    cout << "Total latency for multiplications by Wallace CSA tree: " << mul_latency << endl;
       
    /* In ripple carry adder, we only need 5 and/or gates to implement one full adder, and reuse the full adder
        for whole calculation. Also we need 3 and/or gates to detect overflow.*/
    int add_gate = (5 + 3); // for ripple carry adder
    /* Problem 4 is decided to use one AND gate to obtain the partial products and one FA to compute additions among products.
       Therefore, it requres only 1 + 5 gates. */
    int mul_gate = 1 + 5;

    cout << endl;
    cout << "Total gates from the carry-lookahead adder: " << add_gate << endl;
    cout << "Total gates from the Wallace CSA tree: " << mul_gate << endl;
#endif


    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " seconds." << std::endl;

    return 0;
}

vector<float> getIrisy()
{
    ifstream inFile("./Iris/iris.data");
    vector<float> y;
    string line;

    while (getline(inFile, line)) {
        string label = line.substr(line.find_last_of(',') + 1);
        y.push_back(label == "Iris-setosa" ? -1 : 1);
    }

    return y;
}

vector< vector<float> > getIrisX()
{
    ifstream inFile("./Iris/iris.data");
    vector<vector<float>> X;
    string line;

    while (getline(inFile, line)) {
        stringstream ss(line);
        vector<float> features;
        string value;
        for (int i = 0; i < 4; i++) {
            getline(ss, value, ',');
            features.push_back(stof(value));
        }
        X.push_back(features);
    }

    return X;
}
