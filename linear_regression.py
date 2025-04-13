import tkinter as tk
import numpy as np
import matplotlib.pyplot as plt
from sklearn import datasets
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
#from sklearn.model_selection import train_test_split

class LinearRegression:

  def __init__(self, lr = 0.001, n_iters = 1000):
    self.lr = lr # learning rate
    self.n_iters = n_iters
    self.weights = None
    self.bias = None

  def fit(self, X, y):
    n_samples, n_features = X.shape
    self.weights = np.zeros(n_features)
    self.bias = 0

    for _ in range(self.n_iters):
      y_pred = np.dot(X, self.weights) + self.bias

      dw = (1/n_samples) * np.dot(X.T, (y_pred - y))
      db = (1/n_samples) * np.sum(y_pred - y)

      self.weights -= self.lr * dw
      self.bias -= self.lr * db

  def predict(self, X):
    y_pred = np.dot(X, self.weights) + self.bias
    return y_pred

def train_test_split(X, y, test_size=0.2, random_state=None):
    if random_state is not None:
        # ustawiam ziarno losowości
        np.random.seed(random_state)

    # przemieszanie danych
    indices = np.arange(len(X))
    np.random.shuffle(indices)

    # określam liczby próbek w zbiorze testowym
    test_count = int(len(X) * test_size)

    # dzielę na zbiór testowy i treningowy
    test_indices = indices[:test_count]
    train_indices = indices[test_count:]

    X_train, X_test = X[train_indices], X[test_indices]
    y_train, y_test = y[train_indices], y[test_indices]

    return X_train, X_test, y_train, y_test  
  
class LinearRegressionWindow:
    def __init__(self, n_samples=200, noise=10):
        self.root = tk.Toplevel()
        self.root.title("Linear Regression Plot")

        self.samples = n_samples
        self.noise = noise

        # wykres matplotlib
        self.figure, self.ax = plt.subplots(figsize=(6, 4))
        self.canvas = FigureCanvasTkAgg(self.figure, master=self.root)
        self.canvas.get_tk_widget().pack()

        # suwak
        self.samples_slider = tk.Scale(self.root, from_=10, to=1000, orient=tk.HORIZONTAL,
                               label="Number of Samples", command=self.on_slider_change)
        self.samples_slider.set(n_samples)
        self.samples_slider.pack()

        self.noise_slider = tk.Scale(self.root, from_=0, to=100, orient=tk.HORIZONTAL,
                             label="Noise Level", command=self.on_slider_change)
        self.noise_slider.set(noise)
        self.noise_slider.pack()

        # początkowy wykres
        self.update_plot()

    def on_slider_change(self, val):
        self.update_plot()

    def update_plot(self):
        n_samples = self.samples_slider.get()
        noise = self.noise_slider.get()

        X, y = datasets.make_regression(n_samples=n_samples, n_features=1, noise=noise, random_state=40)
        X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=40)

        model = LinearRegression()
        model.fit(X_train, y_train)
        y_pred = model.predict(X_test)

        sorted_idx = np.argsort(X_test[:, 0])
        X_sorted = X_test[sorted_idx]
        y_pred_sorted = y_pred[sorted_idx]

        self.ax.clear()
        self.ax.scatter(X_test, y_test, color="blue", label="Actual Data")
        self.ax.plot(X_sorted, y_pred_sorted, color="red", linewidth=2, label="Regression Line")
        self.ax.set_xlabel("Feature (X)")
        self.ax.set_ylabel("Target (y)")
        self.ax.set_title("Linear Regression")
        self.ax.legend()
        self.canvas.draw()