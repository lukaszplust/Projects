import numpy as np
import matplotlib.pyplot as plt


# p = 1 - manhattan distance

# p = 2 - euclidan distance

# p = inf - chebyshev measure

# to do zmiany te pozostale oprocz euclidan distance
def euclidan_distance(x, y):
    return np.sqrt(np.sum((x - y)**2))

def manhattan_distance(x, y):
    return sum(abs(a, b) for a, b in zip(x,y))

def chebyshev_distance(x, y):
    return np.max(np.abs(np.array(x) - np.array(y))) 

def minkowski_distance(x, y, p):
    return (sum(abs(a - b)** p for a, b in zip(x, y)))** (1/p)



class KNN:

    def __init__(self, k = 3):
        self.k = k

    def fit(self, X, y):
        self.X_train = X
        self.y_train = y


    def predict(self, X):
        pred = [self._predict(x) for x in X]
        return pred

    def _predict(self, x):
        pass    
