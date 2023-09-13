/**
 * @(#) Correlation.h
 *
 * Copyright (c) 2023 UVSBR PROJETOS DE SISTEMAS LTDA.
 *
 * This software is the confidential and proprietary information of
 * UVSBR PROJETOS DE SISTEMAS LTDA.("Confidential Information").
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement you entered
 * into with Antheus Tecnologia Ltda.
 */

/**
 * Class for Correlation calculations.
 *
 * @author: Omar Achraf
 * @version: 10/jully/2023 11H01
 */

#ifndef CORRELATION_SECTION_H

#include <algorithm>
#include <iterator>
#include <vector>

#define CORRELATION_SECTION_H

template <typename T, int SIZE>
class Correlation {
private:
    std::vector<int> rank(const std::vector<T> &values) {
        std::vector<int> result;

        std::vector<T> sorted_values = values;
        std::sort(sorted_values.begin(), sorted_values.end());

        for (const T &value : values) {
            auto it = std::find(sorted_values.begin(), sorted_values.end(), value);
            if (it != sorted_values.end()) {                
                int r = std::distance(sorted_values.begin(), it) + 1;
                result.push_back(r);
            }
        }

        return result;
    }

public:
    float pearson(const std::vector<T> &valuesX, const std::vector<T> &valuesY) {
        unsigned long sumX = 0, sumY = 0;
        double averageX, averageY;
        double xi_avgxi, yi_avgyi;
        double sumNumerator = 0.0, sumDenominatorX = 0.0, sumDenominatorY = 0.0;
        double denominator;
        float rho;

        for (int i = 0; i < SIZE; i++)  {
            sumX += valuesX[i];
            sumY += valuesY[i];
        }
        averageX = sumX / (double)SIZE;
        averageY = sumY / (double)SIZE;

        for (int i = 0; i < SIZE; i++)  {
            xi_avgxi = valuesX[i] - averageX;
            yi_avgyi = valuesY[i] - averageY;

            yi_avgyi = valuesY[i] - averageY;
            sumNumerator += xi_avgxi * yi_avgyi;

            sumDenominatorX += xi_avgxi * xi_avgxi;
            sumDenominatorY += yi_avgyi * yi_avgyi;

            denominator = std::sqrt(sumDenominatorX * sumDenominatorY);
        }

        if (denominator == 0.0) {
            rho = 1.0f;
        } else {
            rho = (float)sumNumerator / denominator;
        }

        return rho;
    }

    float spearman(const std::vector<T> &valuesX, const std::vector<T> &valuesY) {
        std::vector<int> ranksX = rank(valuesX);
        std::vector<int> ranksY = rank(valuesY);       

        unsigned long Sq2;
        unsigned long sumSq2 = 0;
        unsigned long num, den;
        float rho;

        for (int i = 0; i < SIZE; i++)  {
            Sq2 = ranksX[i] - ranksY[i];
            sumSq2 += Sq2 * Sq2;
        }

        num = 6 * sumSq2;
        den = SIZE * (SIZE * SIZE - 1);
        rho = 1.0f - (num / (float) den);

        //printVector("PWM", valuesX);
        //printVector("RPS", valuesY);
        //Serial.printf("sumSq2: %ld num: %ld den: %ld rho: %f\n\n", sumSq2, num, den, rho);

        return rho;
    }

    void printVector(std::string name, const std::vector<T> &values) {
        Serial.printf("%s: ", name.c_str());
        for (const unsigned long value : values) {
            Serial.printf("%00000ld, ", value);
        }
        Serial.println();
    }
};

#endif 