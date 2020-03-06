from tkinter import *
from tkinter import ttk
import serial
import time

pCurr = vCurr= iCurr= iShort =vOpen = "NA"

ser=serial.Serial("/dev/ttyACM0", 9600)

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
        tabControl.add(self.tab2, text="Battery")
        tabControl.pack(expand=1, fill="both")
        self.widgets()
        self.attributes("-fullscreen", True)
        #Delay for stable Serial
        self.after(0, self.startSerial)
        #Here to start the loop for updating data
        self.after(2, self.updateData)

    #startSerial
    #delay so arduino Serial has enough time to bootup
    def startSerial(self):
        time.sleep(2)   
     
    #getShortOpen()
    #send out request to arduino and update VOpen, IShort   
    def getShortOpen(self):
        print("HI")

    #updateData
    #send signal to arduino to get current data for Voltage, Current, Power, Temp...
    #auto loop after 5 seconds (asynchronous)
    def updateData(self):
        print("HELLO")
        global vOpen, iShort, pCurr, vCurr, iCurr

        widgets()
        vCurrSolar 		= ser.readline().decode("utf-8").rstrip("\r\n")
        iCurrSolar 		= ser.readline().decode("utf-8").rstrip("\r\n")
        pCurrSolar 		= ser.readline().decode("utf-8").rstrip("\r\n")
        internalTemp    = ser.readline().decode("utf-8").rstrip("\r\n")
        leadAcidTemp    = ser.readline().decode("utf-8").rstrip("\r\n")
        liIonTemp 	    = ser.readline().decode("utf-8").rstrip("\r\n")
        vBatt 	        = ser.readline().decode("utf-8").rstrip("\r\n")
        iBatt 	        = ser.readline().decode("utf-8").rstrip("\r\n")
        iLoad 	        = ser.readline().decode("utf-8").rstrip("\r\n")

        widgets.vCurrDis.configure(text=str(vCurrSolar))
        widgets.iCurrDis.configure(text=str(iCurrSolar))
        widgets.pCurrDis.configure(text=str(pCurrSolar))
        widgets.internalTempDis.configure(text=str(internalTemp))
        widgets.leadAcidTempDis.configure(text=str(leadAcidTemp))
        widgets.liIonTempDis.configure(text=str(liIonTemp))
        widgets.voltageBatDis.configure(text=str(vBatt))
        widgets.currentBatDis.configure(text=str(iBatt))
        widgets.currentLoadDis.configure(text=str(iLoad))

        self.after(1000, self.updateData) #asynchronous 

    def widgets(self):
        ## Tab1 start here
        # Basic layout start
        labelFrame = LabelFrame(self.tab1, text="")
        labelFrame.grid(column=0, row=0, padx=8, pady=4)
        labelFrame.config(bg='white')

        vOpenText = Label(labelFrame, text="V Open: ", fg ="#333", bg = "#fff",font=('Helvetica', '15')).grid(column=0, row = 0, sticky = W)
        vOpenDis = Label(labelFrame, text="1", fg ="#333", bg = "#fff", font=('Helvetica', '15'))
        vOpenDis.grid(column=1, row=0, sticky = W)                                     

        iShortText = Label(labelFrame, text="I Short: ", fg ="#333", bg = "#fff",font=('Helvetica', '15')).grid(column=0, row = 1, sticky = W) 
        iShortDis = Label(labelFrame, text="2", fg ="#333", bg = "#fff", font=('Helvetica', '15'))
        iShortDis.grid(column=1, row=1, sticky = W)

        vCurrText = Label(labelFrame, text="V Current: ", fg ="#333", bg = "#fff", font=('Helvetica', '15')).grid(column=0, row = 2, sticky = W)
        vCurrDis = Label(labelFrame, text="3", fg ="#333", bg = "#fff", font=('Helvetica', '15'))
        vCurrDis.grid(column=1, row=2, sticky = W)

        iCurrText = Label(labelFrame, text="I Current: ", fg ="#333", bg = "#fff", font=('Helvetica', '15')).grid(column=0, row = 3, sticky = W)
        iCurrDis = Label(labelFrame, text="4", fg ="#333", bg = "#fff", font=('Helvetica', '15'))
        iCurrDis.grid(column=1, row=3, sticky = W)

        pCurrText = Label(labelFrame, text="P Current: ", fg ="#333", bg = "#fff", font=('Helvetica', '15')).grid(column=0, row = 4, sticky = W)
        pCurrDis = Label(labelFrame, text="5", fg ="#333", bg = "#fff", font=('Helvetica', '15'))
        pCurrDis.grid(column=1, row=4, sticky = W)

        vSymbol = lambda:Label(labelFrame, text="V", fg ="#333", bg = "#fff", font=('Helvetica', '15'))
        vSymbol().grid(column=3, row=0)
        vSymbol().grid(column=3, row=2)

        iSymbol = lambda:Label(labelFrame, text="I", fg ="#333", bg = "#fff", font=('Helvetica', '15'))
        iSymbol().grid(column=3, row=1)
        iSymbol().grid(column=3, row=3)

        pSymbol = Label(labelFrame, text="W", fg ="#333", bg = "#fff",font=('Helvetica', '15')).grid(column=3, row=4)
        # Button here
        btn = Button(labelFrame, text="Reset Solar Info", width = 15, height= 2, command=self.getShortOpen, border="0", bg="#E85B2A", activebackground = "#fff", activeforeground = "#E85B2A", fg = "#fff", font=('Helvetica', '15'))
        btn.grid(column=0, columnspan=2, row = 5, padx=12, sticky = EW)
        # Basic layout end


        ## Tab2 start here
        # Basic layout start
        labelFrame2 = LabelFrame(self.tab2, text="")
        labelFrame2.config(bg='white')
        labelFrame2.grid(column=0, row=0, padx=8, pady=4)
        
        voltageBatText = Label(labelFrame2, text="Battery Voltage: ", fg ="#333", bg = "#fff", font=('Helvetica', '15')).grid(column=0, row = 0, sticky = W)
        voltageBatDis = Label(labelFrame2, text="5", fg ="#333", bg = "#fff", font=('Helvetica', '15'))
        voltageBatDis.grid(column=1, row=0, sticky = W)

        currentBatText = Label(labelFrame2, text="Battery Current: ", fg ="#333", bg = "#fff", font=('Helvetica', '15')).grid(column=0, row = 1, sticky = W)
        currentBatDis = Label(labelFrame2, text="5", fg ="#333", bg = "#fff", font=('Helvetica', '15'))
        currentBatDis.grid(column=1, row=1, sticky = W)

        currentLoadText = Label(labelFrame2, text="Load Current: ", fg ="#333", bg = "#fff", font=('Helvetica', '15')).grid(column=0, row = 2, sticky = W)
        currentLoadDis = Label(labelFrame2, text="5", fg ="#333", bg = "#fff", font=('Helvetica', '15'))
        currentLoadDis.grid(column=1, row=2, sticky = W)        
        
        internalTempText = Label(labelFrame2, text="Internal Temp: ", fg ="#333", bg = "#fff", font=('Helvetica', '15')).grid(column=0, row = 3, sticky = W)
        internalTempDis = Label(labelFrame2, text="5", fg ="#333", bg = "#fff", font=('Helvetica', '15'))
        internalTempDis.grid(column=1, row=3, sticky = W)

        leadAcidTempText = Label(labelFrame2, text="Lead-Acid Temp: ", fg ="#333", bg = "#fff", font=('Helvetica', '15')).grid(column=0, row = 4, sticky = W)
        leadAcidTempDis = Label(labelFrame2, text="5", fg ="#333", bg = "#fff", font=('Helvetica', '15'))
        leadAcidTempDis.grid(column=1, row=4, sticky = W)

        liIonTempText = Label(labelFrame2, text="Li-ion Temp: ", fg ="#333", bg = "#fff", font=('Helvetica', '15')).grid(column=0, row = 5, sticky = W)
        liIonTempDis = Label(labelFrame2, text="5", fg ="#333", bg = "#fff", font=('Helvetica', '15'))
        liIonTempDis.grid(column=1, row=5, sticky = W)

        VSymbol = Label(labelFrame2, text="V", fg ="#333", bg = "#fff",font=('Helvetica', '15')).grid(column=3, row=0)

        iSymbol = lambda:Label(labelFrame2, text="I", fg ="#333", bg = "#fff", font=('Helvetica', '15'))
        iSymbol().grid(column=3, row=1)
        iSymbol().grid(column=3, row=2)

        CSymbol = lambda:Label(labelFrame2, text="°C", fg ="#333", bg = "#fff", font=('Helvetica', '15'))
        CSymbol().grid(column=3, row=3)
        CSymbol().grid(column=3, row=4)
        CSymbol().grid(column=3, row=5)
        # Basic layout end


app = App()
app.mainloop()
