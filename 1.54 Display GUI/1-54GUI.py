from tkinter import *
from tkinter import ttk


class App(Tk):
    def __init__(self):
        super(App, self).__init__()

        self.title("Tkinter Tab Widgets")
        self.minsize(600, 400)
        self.config(bg='white')

        tabControl = ttk.Notebook(self)
        self.tab1 = ttk.Frame(tabControl)
        tabControl.add(self.tab1, text="Tab 1")

        self.tab2 = ttk.Frame(tabControl)
        tabControl.add(self.tab2, text="Tab 2")
        tabControl.pack(expand=1, fill="both")
        self.widgets()
        self.attributes("-fullscreen", True)
        
    
    
    def widgets(self):
        labelFrame = LabelFrame(self.tab1, text="")
        labelFrame.grid(column=0, row=0, padx=8, pady=4)
        label = Label(labelFrame, text="Enter Your Name:")
        label.grid(column=0, row=0, sticky='W')
        vOpenText = Label(labelFrame, text="V-Open: ", fg="#333")
        vOpenText.grid(column=0, row=1, sticky=W)
        vOpenDis = Label(labelFrame, text="1", fg="#333")
        vOpenDis.grid(column=1, row=2, sticky=W)

# This is the 2nd tab for the shit
        labelFrame2 = LabelFrame(self.tab2, text="Second Tab")
        labelFrame2.grid(column=0, row=0, padx=8, pady=4)
        label = Label(labelFrame2, text="Enter Your Name:")
        label.grid(column=0, row=0, sticky='W')
        textEdit = Entry(labelFrame2, width=20)
        textEdit.grid(column=1, row=0)
        label2 = Label(labelFrame2, text="Enter Your Password:")
        label2.grid(column=0, row=1)
        textEdit = Entry(labelFrame2, width=20)
        textEdit.grid(column=1, row=1)


app = App()
app.mainloop()
