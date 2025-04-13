import numpy as np
import tkinter as tk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.colors import ListedColormap
from sklearn.datasets import make_classification
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

distance_metrics = {
    "Euclidean": euclidan_distance,
    "Manhattan": manhattan_distance,
    "Chebyshev": chebyshev_distance
}

class KNN:

    def __init__(self, k = 3, distance_func=euclidan_distance):
        # liczba najblizszych sasiadow
        self.k = k
        self.distance_func = distance_func

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
        dist = [self.distance_func(x, _) for _ in self.X_train]
        
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

'''
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
    '''
class KNNWindow:
    def __init__(self, n_samples=200, noise=0.3, k=3, distance_name="Euclidean"):
        self.root = tk.Toplevel()
        self.root.title("KNN Visualization")

        self.samples = n_samples
        self.noise = noise
        self.k = k

        self.distance_func = distance_metrics.get(distance_name, euclidan_distance)

        self.figure, self.ax = plt.subplots(figsize=(6, 4))
        self.canvas = FigureCanvasTkAgg(self.figure, master=self.root)
        self.canvas.get_tk_widget().pack()

        self.samples_slider = tk.Scale(self.root, from_=10, to=1000, orient=tk.HORIZONTAL,
                                       label="Liczba próbek", command=self.update_from_slider)
        self.samples_slider.set(n_samples)
        self.samples_slider.pack()

        self.noise_slider = tk.Scale(self.root, from_=0, to=1, resolution=0.01, orient=tk.HORIZONTAL,
                                     label="Poziom szumu", command=self.update_from_slider)
        self.noise_slider.set(noise)
        self.noise_slider.pack()

        self.k_slider = tk.Scale(self.root, from_=1, to=15, orient=tk.HORIZONTAL,
                                 label="Liczba sąsiadów (k)", command=self.update_from_slider)
        self.k_slider.set(k)
        self.k_slider.pack()

        self.metric_var = tk.StringVar(value="Euclidean")

        tk.Label(self.root, text="Wybierz metrykę:").pack()

        tk.Radiobutton(self.root, text="Euclidean", variable=self.metric_var, value="Euclidean", command=self.update_plot).pack()
        tk.Radiobutton(self.root, text="Manhattan", variable=self.metric_var, value="Manhattan", command=self.update_plot).pack()
        tk.Radiobutton(self.root, text="Chebyshev", variable=self.metric_var, value="Chebyshev", command=self.update_plot).pack()

        self.update_plot()

    def update_from_slider(self, val):
        self.update_plot()

    def update_plot(self):
        n_samples = self.samples_slider.get()
        noise = self.noise_slider.get()
        k = self.k_slider.get()

        X, y = make_classification(n_samples=n_samples, n_features=2, n_redundant=0, 
                                   n_clusters_per_class=1, n_classes=3, flip_y=noise, random_state=42)

        X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

        distance_name = self.metric_var.get()
        distance_func = distance_metrics.get(distance_name, euclidan_distance)
        clf = KNN(k=k, distance_func=distance_func)

        clf.fit(X_train, y_train)
        y_pred = clf.predict(X_test)

        cmap = ListedColormap(['#FF0000', '#00FF00', '#0000FF'])

        self.ax.clear()

        # tworze siatke punktów (grid) do wyświetlenia granic decyzyjnych
        # rozdzielczość siatki
        h = 0.3
        x_min, x_max = X[:, 0].min() - 1, X[:, 0].max() + 1
        y_min, y_max = X[:, 1].min() - 1, X[:, 1].max() + 1
        xx, yy = np.meshgrid(np.arange(x_min, x_max, h),
                             np.arange(y_min, y_max, h))
        grid_points = np.c_[xx.ravel(), yy.ravel()]

        # przewiduje klasy dla każdego punktu w siatce
        Z = clf.predict(grid_points)
        Z = np.array(Z).reshape(xx.shape)

        # wyświetlam tło z granicami decyzyjnymi
        self.ax.contourf(xx, yy, Z, alpha=0.3, cmap=cmap)


        self.ax.scatter(X_test[:, 0], X_test[:, 1], c=y_pred, cmap=cmap, edgecolor='k', label='Test')
        self.ax.scatter(X_train[:, 0], X_train[:, 1], c=y_train, cmap=cmap, alpha=0.3, edgecolor='k', label='Train')
        self.ax.set_title(f'KNN (k={k}) | próbek: {n_samples} | szum: {noise:.2f}')
        self.ax.legend()
        self.canvas.draw()
def run_knn(n_samples=200, noise=0.3):
    KNNWindow(n_samples=n_samples, noise=noise / 100)