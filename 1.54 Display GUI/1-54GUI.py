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
        tabControl.add(self.tab1, text="Solar")

        self.tab2 = ttk.Frame(tabControl)
        tabControl.add(self.tab2, text="Temperature")
        tabControl.pack(expand=1, fill="both")
        self.widgets()
        self.attributes("-fullscreen", True)
        
    
    
    def widgets(self):
        labelFrame = LabelFrame(self.tab1, text="")
        labelFrame.grid(column=0, row=0, padx=8, pady=4)

        vOpenText = Label(labelFrame, text="V-Open: ", fg ="#333", bg = "#fff").grid(column=0, row = 0, sticky = W)
        vOpenDis = Label(labelFrame, text="1", fg ="#333", bg = "#fff")
        vOpenDis.grid(column=1, row=0, sticky = W)                                     

        iShortText = Label(labelFrame, text="I-Short: ", fg ="#333", bg = "#fff").grid(column=0, row = 1, sticky = W) 
        iShortDis = Label(labelFrame, text="2", fg ="#333", bg = "#fff")
        iShortDis.grid(column=1, row=1, sticky = W)

        vCurrText = Label(labelFrame, text="V-Curr: ", fg ="#333", bg = "#fff").grid(column=0, row = 2, sticky = W)
        vCurrDis = Label(labelFrame, text="3", fg ="#333", bg = "#fff")
        vCurrDis.grid(column=1, row=2, sticky = W)

        iCurrText = Label(labelFrame, text="I-Curr: ", fg ="#333", bg = "#fff").grid(column=0, row = 3, sticky = W)
        iCurrDis = Label(labelFrame, text="4", fg ="#333", bg = "#fff")
        iCurrDis.grid(column=1, row=3, sticky = W)

        pCurrText = Label(labelFrame, text="P-now: ", fg ="#333", bg = "#fff").grid(column=0, row = 4, sticky = W)
        pCurrDis = Label(labelFrame, text="5", fg ="#333", bg = "#fff")
        pCurrDis.grid(column=1, row=4, sticky = W)

        vSymbol = lambda:Label(labelFrame, text="V", fg ="#333", bg = "#fff")
        vSymbol().grid(column=3, row=0)
        vSymbol().grid(column=3, row=2)

        iSymbol = lambda:Label(labelFrame, text="I", fg ="#333", bg = "#fff")
        iSymbol().grid(column=3, row=1)
        iSymbol().grid(column=3, row=3)

        pSymbol = Label(labelFrame, text="W", fg ="#333", bg = "#fff").grid(column=3, row=4)

# This is the 2nd tab for the shit
        labelFrame2 = LabelFrame(self.tab2, text="")
        labelFrame2.grid(column=0, row=0, padx=8, pady=4)

        internalTempText = Label(labelFrame2, text="Internal Temp: ", fg ="#333", bg = "#fff").grid(column=0, row = 0, sticky = W)
        internalTempDis = Label(labelFrame2, text="5", fg ="#333", bg = "#fff")
        internalTempDis.grid(column=1, row=0, sticky = W)

        leadAcidTempText = Label(labelFrame2, text="LeadAcid Temp: ", fg ="#333", bg = "#fff").grid(column=0, row = 1, sticky = W)
        leadAcidTempDis = Label(labelFrame2, text="5", fg ="#333", bg = "#fff")
        leadAcidTempDis.grid(column=1, row=1, sticky = W)

        liIonTempText = Label(labelFrame2, text="Li-ion Temp: ", fg ="#333", bg = "#fff").grid(column=0, row = 2, sticky = W)
        liIonTempDis = Label(labelFrame2, text="5", fg ="#333", bg = "#fff")
        liIonTempDis.grid(column=1, row=2, sticky = W)


app = App()
app.mainloop()
