from tkinter import *
from tkinter import ttk
import serial
import time

ser=serial.Serial("/dev/ttyACM0", 9600)

class App(Tk):
    def __init__(self):
        super(App, self).__init__()

        self.title("Tkinter Tab Widgets")
        self.minsize(600, 400)
        self.config(bg='white')

        #change tab font
        self.s= ttk.Style()
        self.s.configure('TNotebook.Tab', font=('Helvetica','30'))
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

        #send serial to arduino
        ser.write(b'1')

        vOpen = ser.readline().decode("utf-8").rstrip("\r\n")
        iShort = ser.readline().decode("utf-8").rstrip("\r\n")
        
        self.vOpenDis.configure(text=str(vOpen))
        self.iShortDis.configure(text=str(iShort))

    #updateData
    #send signal to arduino to get current data for Voltage, Current, Power, Temp...
    #auto loop after 5 seconds (asynchronous)
    def updateData(self):
        print("HELLO")

        #send serial to arduino
        ser.write(b'0')

        vCurrSolar 		= ser.readline().decode("utf-8").rstrip("\r\n")
        iCurrSolar 		= ser.readline().decode("utf-8").rstrip("\r\n")
        pCurrSolar 		= ser.readline().decode("utf-8").rstrip("\r\n")
        internalTemp    = ser.readline().decode("utf-8").rstrip("\r\n")
        leadAcidTemp    = ser.readline().decode("utf-8").rstrip("\r\n")
        liIonTemp 	    = ser.readline().decode("utf-8").rstrip("\r\n")
        vBatt 	        = ser.readline().decode("utf-8").rstrip("\r\n")
        iBatt 	        = ser.readline().decode("utf-8").rstrip("\r\n")
        iLoad 	        = ser.readline().decode("utf-8").rstrip("\r\n")

        self.vCurrDis.configure(text=str(vCurrSolar))
        self.iCurrDis.configure(text=str(iCurrSolar))
        self.pCurrDis.configure(text=str(pCurrSolar))
        self.internalTempDis.configure(text=str(internalTemp))
        self.leadAcidTempDis.configure(text=str(leadAcidTemp))
        self.liIonTempDis.configure(text=str(liIonTemp))
        self.voltageBatDis.configure(text=str(vBatt))
        self.currentBatDis.configure(text=str(iBatt))
        self.currentLoadDis.configure(text=str(iLoad))

        self.after(1000, self.updateData) #asynchronous 

    def widgets(self):
        ## Tab1 start here
        # Basic layout start
        labelFrame = LabelFrame(self.tab1, text="")
        labelFrame.grid(column=0, row=0, padx=8, pady=4)
        labelFrame.config(bg='white')

        vOpenText = Label(labelFrame, text="V Open: ", fg ="#333", bg = "#fff",font=('Helvetica', '35')).grid(column=0, row = 0, sticky = W)
        self.vOpenDis = Label(labelFrame, text="1", fg ="#333", bg = "#fff", font=('Helvetica', '35'))
        self.vOpenDis.grid(column=1, row=0, sticky = W)                                     

        iShortText = Label(labelFrame, text="I Short: ", fg ="#333", bg = "#fff",font=('Helvetica', '35')).grid(column=0, row = 1, sticky = W) 
        self.iShortDis = Label(labelFrame, text="2", fg ="#333", bg = "#fff", font=('Helvetica', '35'))
        self.iShortDis.grid(column=1, row=1, sticky = W)

        vCurrText = Label(labelFrame, text="V Current: ", fg ="#333", bg = "#fff", font=('Helvetica', '35')).grid(column=0, row = 2, sticky = W)
        self.vCurrDis = Label(labelFrame, text="3", fg ="#333", bg = "#fff", font=('Helvetica', '35'))
        self.vCurrDis.grid(column=1, row=2, sticky = W)

        iCurrText = Label(labelFrame, text="I Current: ", fg ="#333", bg = "#fff", font=('Helvetica', '35')).grid(column=0, row = 3, sticky = W)
        self.iCurrDis = Label(labelFrame, text="4", fg ="#333", bg = "#fff", font=('Helvetica', '35'))
        self.iCurrDis.grid(column=1, row=3, sticky = W)

        pCurrText = Label(labelFrame, text="P Current: ", fg ="#333", bg = "#fff", font=('Helvetica', '35')).grid(column=0, row = 4, sticky = W)
        self.pCurrDis = Label(labelFrame, text="5", fg ="#333", bg = "#fff", font=('Helvetica', '35'))
        self.pCurrDis.grid(column=1, row=4, sticky = W)

        vSymbol = lambda:Label(labelFrame, text="V", fg ="#333", bg = "#fff", font=('Helvetica', '35'))
        vSymbol().grid(column=3, row=0)
        vSymbol().grid(column=3, row=2)

        iSymbol = lambda:Label(labelFrame, text="A", fg ="#333", bg = "#fff", font=('Helvetica', '35'))
        iSymbol().grid(column=3, row=1)
        iSymbol().grid(column=3, row=3)

        pSymbol = Label(labelFrame, text="W", fg ="#333", bg = "#fff",font=('Helvetica', '35')).grid(column=3, row=4)
        # Button here
        btn = Button(labelFrame, text="Reset Solar Info", width = 14, height= 2, command=self.getShortOpen, border="0", bg="#E85B2A", activebackground = "#fff", activeforeground = "#E85B2A", fg = "#fff", font=('Helvetica', '35'))
        btn.grid(column=0, columnspan=2, row = 5, padx=12, sticky = EW)
        # Basic layout end


        ## Tab2 start here
        # Basic layout start
        labelFrame2 = LabelFrame(self.tab2, text="")
        labelFrame2.config(bg='white')
        labelFrame2.grid(column=0, row=0, padx=8, pady=4)
        
        voltageBatText = Label(labelFrame2, text="Battery Voltage: ", fg ="#333", bg = "#fff", font=('Helvetica', '35')).grid(column=0, row = 0, sticky = W)
        self.voltageBatDis = Label(labelFrame2, text="5", fg ="#333", bg = "#fff", font=('Helvetica', '35'))
        self.voltageBatDis.grid(column=1, row=0, sticky = W)

        currentBatText = Label(labelFrame2, text="Battery Current: ", fg ="#333", bg = "#fff", font=('Helvetica', '35')).grid(column=0, row = 1, sticky = W)
        self.currentBatDis = Label(labelFrame2, text="5", fg ="#333", bg = "#fff", font=('Helvetica', '35'))
        self.currentBatDis.grid(column=1, row=1, sticky = W)

        currentLoadText = Label(labelFrame2, text="Load Current: ", fg ="#333", bg = "#fff", font=('Helvetica', '35')).grid(column=0, row = 2, sticky = W)
        self.currentLoadDis = Label(labelFrame2, text="5", fg ="#333", bg = "#fff", font=('Helvetica', '35'))
        self.currentLoadDis.grid(column=1, row=2, sticky = W)        
        
        internalTempText = Label(labelFrame2, text="Internal Temp: ", fg ="#333", bg = "#fff", font=('Helvetica', '35')).grid(column=0, row = 3, sticky = W)
        self.internalTempDis = Label(labelFrame2, text="5", fg ="#333", bg = "#fff", font=('Helvetica', '35'))
        self.internalTempDis.grid(column=1, row=3, sticky = W)

        leadAcidTempText = Label(labelFrame2, text="Lead-Acid Temp: ", fg ="#333", bg = "#fff", font=('Helvetica', '35')).grid(column=0, row = 4, sticky = W)
        self.leadAcidTempDis = Label(labelFrame2, text="5", fg ="#333", bg = "#fff", font=('Helvetica', '35'))
        self.leadAcidTempDis.grid(column=1, row=4, sticky = W)

        liIonTempText = Label(labelFrame2, text="Li-ion Temp: ", fg ="#333", bg = "#fff", font=('Helvetica', '35')).grid(column=0, row = 5, sticky = W)
        self.liIonTempDis = Label(labelFrame2, text="5", fg ="#333", bg = "#fff", font=('Helvetica', '35'))
        self.liIonTempDis.grid(column=1, row=5, sticky = W)

        VSymbol = Label(labelFrame2, text="V", fg ="#333", bg = "#fff",font=('Helvetica', '35')).grid(column=3, row=0)

        iSymbol = lambda:Label(labelFrame2, text="A", fg ="#333", bg = "#fff", font=('Helvetica', '35'))
        iSymbol().grid(column=3, row=1)
        iSymbol().grid(column=3, row=2)

        CSymbol = lambda:Label(labelFrame2, text="°C", fg ="#333", bg = "#fff", font=('Helvetica', '35'))
        CSymbol().grid(column=3, row=3)
        CSymbol().grid(column=3, row=4)
        CSymbol().grid(column=3, row=5)
        # Basic layout end


app = App()
app.mainloop()
