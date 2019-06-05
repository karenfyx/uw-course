#!/usr/bin/python
from __future__ import division
import numpy as np
from collections import namedtuple
import matplotlib.pyplot as plt

MAX_LAMBDA = 4
PATH = './regression-dataset/'
FOLD = 10
Input = namedtuple('Input', ['x','t'], verbose=False)

def read_file(file1, file2):
    file1 = open(PATH+file1)
    file2 = open(PATH+file2)
    input = [l.strip('\n').strip('\r').split(',') for l in file1]
    target = [l.strip('\n').strip('\r') for l in file2]
    data = [Input(np.array([1]+[float(xn) for xn in x]), float(t)) for x, t in zip(input, target)]
    file1.close()
    file2.close()
    return data

def train_lsr(coefficient, dependantVar, n, l): # Solve linear system Aw = b for w
    A = -l * np.identity(coefficient[0].shape[0]) + sum(coefficient[0:n * 20] + coefficient[(n + 1) * 20:])
    b = sum(dependantVar[0:n * 20] + dependantVar[(n + 1) * 20:])
    w = np.linalg.solve(A, b)
    return w

def test_lsr(testSet, w):
    acc = 0
    for d in testSet:
        h_d = np.dot(w.squeeze(), d.x)
        acc_d = (h_d - d.t) ** 2
        acc += acc_d
    acc = acc/len(testSet)
    return acc

# Main program: least square regression
def main():
    # Prepare data
    trainData = []
    for i in range(1,FOLD+1):
        data = read_file('trainInput' + str(i) + '.csv', 'trainTarget' + str(i) + '.csv')
        trainData += data
    testData = read_file('testInput.csv', 'testTarget.csv')
    coefficient = [-d.x.reshape((-1, 1)) * d.x for d in trainData]
    dependantVar = [-d.t * d.x.reshape((-1, 1)) for d in trainData]

    bestAcc,bestLambda = float('inf'),0
    Debug = True

    if Debug:
        print(len(trainData), len(coefficient), len(dependantVar))
        print(coefficient[0:1])
        print('\n')
        print(dependantVar[0:1])

    lambdas, lossrate = [],[]

    # Train and validate model
    for l in np.arange(0.1, MAX_LAMBDA+0.1, 0.1):
        acc = 0
        for n in range(0, FOLD):
            validateSet = trainData[n*20:(n+1)*20]
            w = train_lsr(coefficient, dependantVar, n, l)
            acc += test_lsr(validateSet, w)
        acc = acc/FOLD
        lambdas.append(l)
        lossrate.append(acc)
        print("average acc for L = " + str(l) + " is " + str(acc))
        if acc < bestAcc:
            bestAcc, bestLambda = acc, l

    print("bestAcc:",bestAcc, " bestLambda:", bestLambda)

    # Train with all data
    w = train_lsr(coefficient, dependantVar, FOLD, bestLambda)
    testAccuracy = test_lsr(testData, w)
    print("testAccuracy for for best lambda is "+ str(testAccuracy))

    plt.plot(lambdas, lossrate)
    plt.xlabel('Lambda')
    plt.ylabel('Average Loss Rate on Validation Set')
    plt.ioff()
    plt.show()


if __name__ == "__main__":
    main()