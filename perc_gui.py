"""
import tkinter as tk
from tkinter import ttk
from perceptron import run_perceptron

def draw_neural_network(canvas, layers):
    canvas.delete("all")
    width = canvas.winfo_width()
    height = canvas.winfo_height()
    
    max_neurons = max(layers)
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
    
    # Draw connections
    for i in range(len(positions) - 1):
        for neuron1 in positions[i]:
            for neuron2 in positions[i + 1]:
                canvas.create_line(neuron1[0], neuron1[1], neuron2[0], neuron2[1], fill="gray", width=1)


def update_network():
    global layers
    try:
        new_layers = list(map(int, entry_layers.get().split(',')))
        if all(n > 0 for n in new_layers):
            layers = new_layers
            draw_neural_network(canvas, layers)
    except ValueError:
        pass


# Initialize GUI
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
entry_layers.insert(0, "3,5,2")  # Default layers

tk.Button(frame, text="Update", command=update_network).pack(side=tk.LEFT)

# Default layer setup
layers = [3, 1, 1]
draw_neural_network(canvas, layers)

root.mainloop()
"""