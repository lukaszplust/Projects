import tkinter as tk
import threading
from linear_regression import run_linear_regression

class MyGUI:

    def __init__(self):
        self.root = tk.Tk()

        self.root.geometry("800x500")
        self.root.title("Choose algorithm")
        
        self.selected_algorithm = tk.IntVar(value = 0)

        self.check = tk.Radiobutton(self.root, text= "Linear Regression", variable = self.selected_algorithm, value= 1)
        self.check.pack()

        self.is_clicked_logistic_regression = tk.IntVar()
        self.check = tk.Radiobutton(self.root, text= "Logistic Regression", variable = self.selected_algorithm, value = 2)
        self.check.pack()

        self.button = tk.Button(self.root, text = "Enter", command= self.show_message)
        self.button.pack()

        self.root.mainloop()

    def show_message(self):
        if self.selected_algorithm.get() == 1:
            print("Linear Regression")
            self.root.after(0, run_linear_regression)
            #threading.Thread(target=self.run_linear_regression, daemon=True).start()
        elif self.selected_algorithm.get() == 2:
            print("Logistic Regression")
        else:
            print("Choose one")

    #def run_linear_regression(self):
        #self.root.after(0,run_linear_regression())
        

MyGUI()