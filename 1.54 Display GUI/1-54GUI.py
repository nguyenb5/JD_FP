from tkinter import*
import serial
import time
#   Solar 17 - Bao Nguyen
#   Program will only get V open - I short if button is pressed
#   I-curr, V-curr, Pnow will be updated every 5 seconds

# global vOpen, iShort, pCurr, vCurr, iCurr
pCurr = vCurr= iCurr= iShort =vOpen = "NA"

ser=serial.Serial("/dev/ttyACM0", 9600)

# Send serial request to Arduino to get Vcurr, Icurr
# Update global variables, update vCurr, iCurr, pCurr
# Exit function

# Let's create the Tkinter window
window = Tk()
window.config(bg='white')

vOpenText = Label(window, text="V-Open: ", fg ="#333", bg = "#fff").grid(column=0, row = 0, sticky = W)     # Not changing so can do .grid at the same time
vOpenDis = Label(window, text="1", fg ="#333", bg = "#fff")
vOpenDis.grid(column=1, row=0, sticky = W)                                      # Changing so need to call .grid seperate

iShortText = Label(window, text="I-Short: ", fg ="#333", bg = "#fff").grid(column=0, row = 1, sticky = W) 
iShortDis = Label(window, text="2", fg ="#333", bg = "#fff")
iShortDis.grid(column=1, row=1, sticky = W)

vCurrText = Label(window, text="V-Curr: ", fg ="#333", bg = "#fff").grid(column=0, row = 2, sticky = W)
vCurrDis = Label(window, text="3", fg ="#333", bg = "#fff")
vCurrDis.grid(column=1, row=2, sticky = W)

iCurrText = Label(window, text="I-Curr: ", fg ="#333", bg = "#fff").grid(column=0, row = 3, sticky = W)
iCurrDis = Label(window, text="4", fg ="#333", bg = "#fff")
iCurrDis.grid(column=1, row=3, sticky = W)

pCurrText = Label(window, text="P-now: ", fg ="#333", bg = "#fff").grid(column=0, row = 4, sticky = W)
pCurrDis = Label(window, text="5", fg ="#333", bg = "#fff")
pCurrDis.grid(column=1, row=4, sticky = W)

internalTempText = Label(window, text="Internal Temp: ", fg ="#333", bg = "#fff").grid(column=0, row = 5, sticky = W)
internalTempDis = Label(window, text="5", fg ="#333", bg = "#fff")
internalTempDis.grid(column=1, row=5, sticky = W)

leadAcidTempText = Label(window, text="LeadAcid Temp: ", fg ="#333", bg = "#fff").grid(column=0, row = 6, sticky = W)
leadAcidTempDis = Label(window, text="5", fg ="#333", bg = "#fff")
leadAcidTempDis.grid(column=1, row=6, sticky = W)

liIonTempText = Label(window, text="Li-ion Temp: ", fg ="#333", bg = "#fff").grid(column=0, row = 6, sticky = W)
liIonTempDis = Label(window, text="5", fg ="#333", bg = "#fff")
liIonTempDis.grid(column=1, row=7, sticky = W)

# Reuse Label multiple times
vSymbol = lambda:Label(window, text="V", fg ="#333", bg = "#fff")
vSymbol().grid(column=3, row=0)
vSymbol().grid(column=3, row=2)

iSymbol = lambda:Label(window, text="I", fg ="#333", bg = "#fff")
iSymbol().grid(column=3, row=1)
iSymbol().grid(column=3, row=3)

pSymbol = Label(window, text="W", fg ="#333", bg = "#fff").grid(column=3, row=4)

CSymbol = lambda:Label(window, text="°C", fg ="#333", bg = "#fff")
CSymbol().grid(column=3, row=5)
CSymbol().grid(column=3, row=6)
CSymbol().grid(column=3, row=7)


def startSerial():
    time.sleep(2)

def updateData():
    global vOpen, iShort, pCurr, vCurr, iCurr
    #global counter                          #debug
    #counter +=1                             #debug
    #vCurrDis.configure(text=str(counter))   #debug
    vCurrDis.configure(text="...")
    iCurrDis.configure(text="...")
    pCurrDis.configure(text="...")

    ser.write(b'0')
    vCurr 		= ser.readline().decode("utf-8").rstrip("\r\n")
    iCurr 		= ser.readline().decode("utf-8").rstrip("\r\n")
    pCurr 		= ser.readline().decode("utf-8").rstrip("\r\n")
	internalTemp = ser.readline().decode("utf-8").rstrip("\r\n")
	leadAcidTemp = ser.readline().decode("utf-8").rstrip("\r\n")
	liIonTemp 	= ser.readline().decode("utf-8").rstrip("\r\n")
    
    vCurrDis.configure(text=str(vCurr))
    iCurrDis.configure(text=str(iCurr))
    pCurrDis.configure(text=str(pCurr))
	internalTempDis.configure(text=str(internalTemp))
	leadAcidTempDis.configure(text=str(leadAcidTemp))
	liIonTempDis.configure(text=str(liIonTemp))
	
    print("HELLO")
    
    # each after call would create it own infinity 500ms delay loop (need to find another way)
    window.after(1000, updateData) # seem to be asynchronous
    
# Send serial request to Arduino if button is pressed
# to get Open Voltage and Short Circuit data
# Display "Waiting" until data is received, update vOpen, reading  a byte from serialiShort
# Exit function
def getShortOpen():
    print("HI")
    global vOpen, iShort, pCurr, vCurr, iCurr
    # Null all data for pretty
#    vOpenDis.configure(text= "..." )
#    iShortDis.configure(text="...")
#    vCurrDis.configure(text="...")
#    iCurrDis.configure(text="...")
#    pCurrDis.configure(text="...")
    
    #ser.write("shortAndOpen\r".encode())
    ser.write(b'1')

    vOpen = ser.readline().decode("utf-8").rstrip("\r\n")
    iShort = ser.readline().decode("utf-8").rstrip("\r\n")
    
    vOpenDis.configure(text=str(vOpen))
    iShortDis.configure(text=str(iShort))
    vCurrDis.configure(text=str(vCurr))
    iCurrDis.configure(text=str(iCurr))
    pCurrDis.configure(text=str(pCurr))
    
    #window.after(0, getShortOpen2)
    #iShort = ser.readline().decode("utf-8").rstrip("\r\n")

btn = Button(window, text="Reset Solar Info", width = 15, height= 2, command=getShortOpen, border="0", bg="#E85B2A", activebackground = "#fff", activeforeground = "#E85B2A", fg = "#fff", font=('Helvetica', '10'))
btn.grid(column=0, columnspan=2, row = 5, padx=12, sticky = EW)

window.after(0,startSerial)	
window.after(2,updateData)
window.title("Solar Charger 1.0")
window.geometry('350x200')
window.mainloop()

    


