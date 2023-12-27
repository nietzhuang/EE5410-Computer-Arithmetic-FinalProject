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

int main()
{
    // record start time 
    auto start = std::chrono::high_resolution_clock::now();
   
    //get data
    vector< vector<float> > X = getIrisX();
    vector<float> y = getIrisy();

    //random data
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(X.begin(), X.end(), std::default_random_engine(seed));
    shuffle(y.begin(), y.end(), std::default_random_engine(seed)); 

    //set 70 for training, 30 for validation
    vector<vector<float>> X_train(X.begin(), X.begin() + 70);
    vector<float> y_train(y.begin(), y.begin() + 70);
    vector<vector<float>> X_test(X.begin() + 70, X.end());
    vector<float> y_test(y.begin() + 70, y.end());

    // //set 80 for training, 20 for validation
    // vector<vector<float>> X_train(X.begin(), X.begin() + 80);
    // vector<float> y_train(y.begin(), y.begin() + 80);
    // vector<vector<float>> X_test(X.begin() + 80, X.end());
    // vector<float> y_test(y.begin() + 80, y.end());

    // //set 90 for training, 10 for validation
    // vector<vector<float>> X_train(X.begin(), X.begin() + 90);
    // vector<float> y_train(y.begin(), y.begin() + 90);
    // vector<vector<float>> X_test(X.begin() + 90, X.end());
    // vector<float> y_test(y.begin() + 90, y.end());

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
        cout <<cout << "Sample: " << i 
             << " - Actual class: " << y_test[i] 
             << "Predicted class: " << predicted << endl;
    }
    
    // calculate overall accuracy
    double accuracy = static_cast<double>(correct_predictions) / X_test.size();
    cout << "Accuracy: " << accuracy * 100 << "%" << endl;

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
