import tkinter as tk
import threading
#from linear_regression import run_linear_regression
from linear_regression import LinearRegressionWindow
from knn import KNNWindow
from knn import run_knn
#from perceptron import run_perceptron

class MyGUI:

    def __init__(self):
        self.root = tk.Tk()

        self.root.geometry("800x500")
        self.root.title("Choose algorithm")
        
        self.selected_algorithm = tk.IntVar(value = 0)

        self.check = tk.Radiobutton(self.root, text= "Linear Regression", variable = self.selected_algorithm, value= 1)
        self.check.pack()

        self.is_clicked_logistic_regression = tk.IntVar()
        self.check = tk.Radiobutton(self.root, text= "Knn", variable = self.selected_algorithm, value = 2)
        self.check.pack()

        # suwak do wyboru liczby próbek
        self.sample_slider = tk.Scale(self.root, from_=10, to=1000, orient=tk.HORIZONTAL, label="Number of Samples")
        

        # suwak do wyboru poziomu szumu
        self.noise_slider = tk.Scale(self.root, from_=0, to=100, orient=tk.HORIZONTAL, label="Noise Level")

        # DLA KNN WYBOR METRYKI!!
        self.selected_metric = tk.StringVar(value="Euclidean")
    

        #self.is_clicked_logistic_regression = tk.IntVar()
        #self.check = tk.Radiobutton(self.root, text= "Perceptron", variable = self.selected_algorithm, value = 3)
        #self.check.pack()

        self.button = tk.Button(self.root, text = "Enter", command= self.show_message)
        self.button.pack()

        self.root.mainloop()
    
    def show_message(self):

        # ile mam probek
        n_samples = self.sample_slider.get()
        noise = self.noise_slider.get()
        
        if self.selected_algorithm.get() == 1:
            print("Linear Regression")
            LinearRegressionWindow(n_samples,noise)
            #threading.Thread(target=self.run_linear_regression, daemon=True).start()
        elif self.selected_algorithm.get() == 2:
            print("Knn")
            KNNWindow(n_samples, noise, k=3, distance_name=self.selected_metric.get())
        #elif self.selected_algorithm.get() == 3:
           # print("Perceptron")
           # self.root.after(0, run_perceptron)    
        else:
            print("Choose one")

    #def run_linear_regression(self):
        #self.root.after(0,run_linear_regression())
        

MyGUI()

