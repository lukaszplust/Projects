import numpy as np
import matplotlib.pyplot as plt
from sklearn import datasets
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
  
def run_linear_regression():  
    # wygenerowanie 100 probek, n_features = 1 (jedna cecha - niezalezna) + szum  
    X,y = datasets.make_regression(n_samples=100, n_features= 1, noise= 10, random_state =40)

    # podział na treningowe 80% i testowe 20%
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=40)

    #print("Dane wejściowe (X):\n", X)
    #print("Rzeczywiste wartości (y):\n", y)
    #print("Dane treningowe (X_train):\n", X_train)
    #print("Dane testowe (X_test):\n", X_test)
    #print("Rzeczywiste wartości treningowe (y_train):\n", y_train)
    #print("Rzeczywiste wartości testowe (y_test):\n", y_test)
    # Create a LinearRegression object
    model = LinearRegression()

    # Trenowanie modelu na zbiorze treningowym
    model.fit(X_train, y_train)

    # Przewidywanie wartosci na zbiorze testowym
    y_pred = model.predict(X_test)
    
    #print("X_test: ", X_test)
    #print("Predictions:", y_pred)

    # sortuje X_test i odpowiednie wartości y_pred

    # sortuje względem X_test
    sorted_indices = np.argsort(X_test[:, 0])
    X_test_sorted = X_test[sorted_indices]
    y_pred_sorted = y_pred[sorted_indices]

    # wyświetlenie posortowanych wyników

    # flatten() dla lepszej czytelności
    print("X_test (sorted):", X_test_sorted.flatten())
    print("Predictions (sorted):", y_pred_sorted)

    def mean_squared_error(y_test, y_pred):
        return np.mean((y_test - y_pred) ** 2)

    mse = mean_squared_error(y_test, y_pred)
    print("Mean Squared Error:", mse)

    #print("Actual values:", y)

    # Plotting the data and the regression line
    plt.scatter(X_test, y_test, color="blue", label="Actual Data")
    plt.plot(X_test, y_pred, color="red", linewidth=2, label="Regression Line")
    plt.legend()
    plt.xlabel("Feature (X)")
    plt.ylabel("Target (y)")
    plt.title("Linear Regression Model")
    plt.show()