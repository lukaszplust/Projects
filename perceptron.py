import numpy as np
from linear_regression import train_test_split
from sklearn import datasets
import matplotlib.pyplot as plt
import tkinter as tk

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
"""
def run_perceptron(learning_rate, epochs):
    X, y = datasets.make_blobs(n_samples=150, n_features=2, centers=2, cluster_std=1.45, random_state=42)
    p = Perceptron(learning_rate=learning_rate, epochs=epochs)
    p.fit(X, y)
    
    # po zakończeniu treningu wyświetl wagi w interfejsie
    weights_str = ", ".join([f"{w:.3f}" for w in p.weights])
    bias_str = f"{p.bias:.3f}"
    weights_label.config(text=f"Wagi: {weights_str}\nBias: {bias_str}")

    fig, ax = plt.subplots()
    plt.scatter(X[:, 0], X[:, 1], c=y, marker='o')
    x_min, x_max = np.amin(X[:, 0]), np.amax(X[:, 0])
    y_min, y_max = np.amin(X[:, 1]), np.amax(X[:, 1])
    x1_1 = (-p.weights[0] * x_min - p.bias) / p.weights[1]
    x1_2 = (-p.weights[0] * x_max - p.bias) / p.weights[1]
    ax.plot([x_min, x_max], [x1_1, x1_2], "k")

    # dodaje wagi na wykresie
    ax.text(x_min + 1, x1_1, f"w1: {p.weights[0]:.2f}", fontsize=12, color='red')
    ax.text(x_max - 1, x1_2, f"w2: {p.weights[1]:.2f}", fontsize=12, color='blue')

    # dodajem bias
    ax.text(x_min + 1, x1_1 - 1, f"Bias: {p.bias:.2f}", fontsize=12, color='green')

    ax.set_ylim([y_min - 3, y_max + 3])
    plt.show()

def draw_neural_network(canvas, layers):
    canvas.delete("all")
    width, height = canvas.winfo_width(), canvas.winfo_height()
    layer_spacing = width // (len(layers) + 1)
    neuron_radius = 20
    positions = []
    
    for i, neurons in enumerate(layers):
        x = (i + 1) * layer_spacing
        y_spacing = height // (neurons + 1)
        layer_positions = []
        for j in range(neurons):
            y = (j + 1) * y_spacing
            layer_positions.append((x, y))
            canvas.create_oval(x - neuron_radius, y - neuron_radius, x + neuron_radius, y + neuron_radius, fill="skyblue")
        positions.append(layer_positions)
    
    for i in range(len(positions) - 1):
        for neuron1 in positions[i]:
            for neuron2 in positions[i + 1]:
                canvas.create_line(neuron1[0], neuron1[1], neuron2[0], neuron2[1], fill="gray", width=1)

def update_network():
    try:
        new_layers = list(map(int, entry_layers.get().split(',')))
        if all(n > 0 for n in new_layers):
            draw_neural_network(canvas, new_layers)
    except ValueError:
        pass

def train_perceptron():
    learning_rate = float(entry_lr.get())
    epochs = int(entry_epochs.get())
    run_perceptron(learning_rate, epochs)

root = tk.Tk()
root.title("Perceptron Visualization")
root.geometry("800x500")
canvas = tk.Canvas(root, bg="white")
canvas.pack(fill=tk.BOTH, expand=True)
frame = tk.Frame(root)
frame.pack()

tk.Label(frame, text="Layers (comma separated):").pack(side=tk.LEFT)
entry_layers = tk.Entry(frame)
entry_layers.pack(side=tk.LEFT)
entry_layers.insert(0, "3,1")
tk.Button(frame, text="Update", command=update_network).pack(side=tk.LEFT)

tk.Label(frame, text="Learning Rate:").pack(side=tk.LEFT)
entry_lr = tk.Entry(frame, width=5)
entry_lr.pack(side=tk.LEFT)
entry_lr.insert(0, "0.01")

tk.Label(frame, text="Epochs:").pack(side=tk.LEFT)
entry_epochs = tk.Entry(frame, width=5)
entry_epochs.pack(side=tk.LEFT)
entry_epochs.insert(0, "1000")

tk.Button(frame, text="Train Perceptron", command=train_perceptron).pack(side=tk.LEFT)

# dodaje etykietę, która wyświetli wagi
weights_label = tk.Label(root, text="Wagi: 0.0\nBias: 0.0", font=("Arial", 12))
weights_label.pack(side=tk.BOTTOM)
root.mainloop()
"""

