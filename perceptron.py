import numpy as np
from linear_regression import train_test_split
from sklearn import datasets
import matplotlib.pyplot as plt

def unit_step_function(x):
    return np.where( x > 0 , 1, 0)

class Perceptron:

    def __init__(self, learning_rate = None, epochs = None):
        print(learning_rate)
        print(epochs)
        self.learning_rate = learning_rate
        self.epochs = epochs
        self.activation_function = unit_step_function
        self.weights = None
        self.bias = None

    def fit(self, X, y):

        number_of_samples, number_of_features = X.shape

        # INITIALIZE WEIGHTS

        # try random initliaization

        self.weights = np.zeros(number_of_features)
        self.bias = 0

        # y_to_bin = lambda y: 1 if y > 0 else 0
        y_to_bin = np.where(y > 0 , 1, 0)
        

        # learn weights
        for _ in range(self.epochs):
            for index, x_i in enumerate(X):
                linear_output = np.dot(x_i, self.weights) + self.bias
                # CALCULATE Y_PRED
                y_predicted = self.activation_function(linear_output)
                
                # APPLY UPDATE RULE
                # perceptron update rule
                
                update = self.learning_rate * (y_to_bin[index] - y_predicted)

                # update weights

                # we wzorze jest przy delta w jeszcze razy (x_i)
                self.weights = self.weights + update * x_i

                self.bias = self.bias + update

    def predict(self, X):
        linear_output = np.dot(X, self.weights) + self.bias
        y_predicted = self.activation_function(linear_output)

        return y_predicted
    

def run_perceptron():

    X, y = datasets.make_blobs(n_samples=150, n_features=2, centers=3, cluster_std=1.45, random_state=42)

    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

    p = Perceptron(learning_rate=0.01, epochs=1000)

    p.fit(X_train, y_train)
    predictions = p.predict(X_test)



    fig = plt.figure()
    ax = fig.add_subplot(1, 1, 1)
    plt.scatter(X_train[:, 0], X_train[:, 1], marker="o", c=y_train)

    x0_1 = np.amin(X_train[:, 0])
    x0_2 = np.amax(X_train[:, 0])

    x1_1 = (-p.weights[0] * x0_1 - p.bias) / p.weights[1]
    x1_2 = (-p.weights[0] * x0_2 - p.bias) / p.weights[1]

    ax.plot([x0_1, x0_2], [x1_1, x1_2], "k")

    ymin = np.amin(X_train[:, 1])
    ymax = np.amax(X_train[:, 1])
    ax.set_ylim([ymin - 3, ymax + 3])

    plt.show()



