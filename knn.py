import numpy as np
import matplotlib.pyplot as plt
from collections import Counter
from sklearn import datasets
#from sklearn.model_selection import train_test_split
from linear_regression import train_test_split
from matplotlib.colors import ListedColormap
import pdb

# p = 1 - manhattan distance

# p = 2 - euclidan distance

# p = inf - chebyshev measure

# to do zmiany te pozostale oprocz euclidan distance
def euclidan_distance(x, y):
    return np.sqrt(np.sum((x - y)**2))

def manhattan_distance(x, y):
    return np.sum(np.abs(x - y))

def chebyshev_distance(x, y):
    return np.max(np.abs(np.array(x) - np.array(y))) 

def minkowski_distance(x, y, p):
    return (np.sum(np.abs(x - y)** p)) ** (1/p)



class KNN:

    def __init__(self, k = 3):
        # liczba najblizszych sasiadow
        self.k = k

    def fit(self, X, y):
        self.X_train = X
        self.y_train = y

    # przewiduje klase dla kazdego punktu startowego
    def predict(self, X):

        pred = [self._predict(x) for x in X]
        return pred

    def _predict(self, x):
        # obliczam odleglosc miedzy x, a wszystkimi punktami treningowymi

        # dla kazdego X_test obliczam odleglosc każdego X_train
        dist = [manhattan_distance(x, _) for _ in self.X_train]
        
        # wybieram nablizsze k

        # np.argsort - zwraca indeksy posortowane wedlug rosnacej odleglosci
        # [: self.k] - wybieram k indeksów odpowiadających k najblizszym punktom
        k_indexes = np.argsort(dist)[:self.k]
        
        # zwracam etykiety dla k najblizszych sasiadow
        # k_nearest - etykiety k najblizszych punktow

        # mając już indeksy k-najbliższych punktow wyznaczam dla nich y_train
        classes_nearest = [self.y_train[x] for x in k_indexes]

        #print(f"classes: {classes_nearest}")
        # gdy już mam klasy musze zliczyć, które najczęsciej występuje

        # glos wiekszosci

        # most_common() - zwraca liste par (etykieta, liczba wystapien) posortowana
        # od najczesciej wystepujacej etykiety do najrzadszej

        # przyklad:
        # ['A', 'B', 'A', 'C', 'A']
        # [('A', 3), ('B', 1), ('C', 1)]
        # w [0][0] przechowywana jest etykieta najczesciej wystepujacego
        major = Counter(classes_nearest).most_common()

        return major[0][0]


def run_knn():

    iris = datasets.load_iris()

    #print(iris.data[:5])
    
    X, y = iris.data, iris.target
    
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size= 0.2, random_state = 42)
    """
    print(X_train[:5])
    print("----")
    print(X_test[:5])
    print("----")
    print(y_train[:5])
    print("----")
    print(y_test[:5])
    """
    #plt.figure()
    #plt.scatter(X[:, 2], X[:, 3], c=y,edgecolor='k', s=20)
    #plt.show()

    # wlasny przyklad
    '''
    # Dane treningowe
    X_train = np.array([
        [2, 3],
        [5, 4],
        [9, 6],
        [4, 7],
        [8, 1]
    ])
    y_train = np.array([0, 1, 1, 0, 1])

    X_test = np.array([[6, 5]]) 
    '''

    clf = KNN(k = 3)

    clf.fit(X_train, y_train)
    predict = clf.predict(X_test)

    #print(f"Predicted: {predict[0]}")
    
    #print(f"Klasyfikacja punktu {X_test[0]}: Klasa {predict[0]}")
    #print(X_test[0][0])

    # czerwony - 0
    # niebieski - 2
    cmap = ListedColormap(['#FF0000','#00FF00','#0000FF'])
    plt.figure()
    
    # 
    plt.scatter(X[:,2],X[:,3], c=y, cmap=cmap, edgecolor='k', s=20)
    
    plt.scatter(X_test[1, 0], X_test[0, 1], c='black', edgecolor='k', s=100, label="Punkt testowy", marker='X')
    plt.xlabel('Cechy 1')
    plt.ylabel('Cechy 2')
    plt.title('Wykres rozrzutu danych i punktu testowego')
    plt.legend(loc='best')
    plt.show()
    
    #accuracy = np.sum(predict == y_test) / len(y_test)
    #print(accuracy)