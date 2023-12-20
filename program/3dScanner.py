from tkinter import *
from tkinter.ttk import Combobox
import serial
import serial.tools.list_ports
from tkinter import filedialog
import urllib.request
import numpy as np
import cv2
from PIL import Image, ImageTk
import time

port = ''
portlist = []
readData = ''
portWindow = None
ser = None



def refreshPorts():
    global portlist
    global portsSelectionBox
    portlist.clear()
    ports = serial.tools.list_ports.comports()
    for port in sorted(ports):
        portlist.append((port.device, port.description))
    print(portlist)
    portsSelectionBox.configure(values=[port[0] for port in portlist])

def cw():
    global degreeSpin
    print('Cw:{}'.format(degreeSpin.get()))
    deg = degreeSpin.get()
    ser.write(b'A')
    ser.write(str.encode(deg))

def ccw():
    global degreeSpin
    print('Ccw:{}'.format(degreeSpin.get()))
    deg = degreeSpin.get()
    ser.write(b'B')
    ser.write(str.encode(deg))

def scanDivision(numDivision):
    global ser
    urlCam1 = urlCam1Entry.get()
    urlCam2 = urlCam2Entry.get()
    urlCam3 = urlCam3Entry.get()
    
    
    for i in range(1,numDivision+1):

        image1 = getImage(urlCam1)
        image2 = getImage(urlCam2)
        image3 = getImage(urlCam3)
        cv2.imwrite(path+'/'+'cam1_'+str(i)+'.tiff', image1)
        cv2.imwrite(path+'/'+'cam2_'+str(i)+'.tiff', image2)
        cv2.imwrite(path+'/'+'cam3_'+str(i)+'.tiff', image3)

        time.sleep(7)

        ser.write(b'A')
        ser.write(str.encode(str(360/numDivision)))
        line = NONE
        while TRUE:
            line = ser.readline()   # read a '\n' terminated line
            print(len(line))
            print(line)
            if len(line)<7:
                break
        
def scan():
    global path
    global divisionSelectionBox
    divsionChoosen = int(divisionSelectionBox.get())
    print('Scan')
    print("Number of Division:{}".format(divsionChoosen))
    path = selectFile()

    if divsionChoosen == 2:
        print('Scan with 2 divisions')
        scanDivision(2)
    elif divsionChoosen == 4:
        print('Scan with 4 divisions')
        scanDivision(4)
    elif divsionChoosen == 8:
        print('Scan with 8 divisions')
        scanDivision(8)
    elif divsionChoosen == 16:
        print('Scan with 16 divisions')
        scanDivision(16)
    elif divsionChoosen == 32:
        print('Scan with 32 divisions')
        scanDivision(32)

def selectFile():
    global operationFrame
    print('Select file')

    path = filedialog.askdirectory()
    print(path)

    # File loccation feedback
    fileLabel = Label(operationFrame, text=path, font=13)
    fileLabel.pack( padx=5, pady=5, fill = BOTH, expand = True)
    return path

def getImageTk(urlCam):
    imgResponse = urllib.request.urlopen (urlCam)
    imgNp = np.array(bytearray(imgResponse.read()),dtype=np.uint8)
    image = cv2.imdecode (imgNp, -1)

    b,g,r = cv2.split(image)
    img = cv2.merge((r,g,b))
    
    im = Image.fromarray(img)
    imgtk = ImageTk.PhotoImage(image = im)

    return imgtk

def getImage(urlCam):
    imgResponse = urllib.request.urlopen(urlCam)
    imgNp = np.array(bytearray(imgResponse.read()),dtype=np.uint8)
    image = cv2.imdecode(imgNp, -1)

    return image

def open_cam1():
    urlCam1 = urlCam1Entry.get()
    imgtk1 = getImageTk(urlCam1)
    camera1.configure(image = imgtk1)
    camera1.image = imgtk1
    camera1.pack( padx=5, pady=5, fill = BOTH, expand = True)

def open_cam2():
    urlCam2 = urlCam2Entry.get()
    imgtk2 = getImageTk(urlCam2)
    camera2.configure(image = imgtk2)
    camera2.image = imgtk2
    camera2.pack( padx=5, pady=5, fill = BOTH, expand = True)

def open_cam3():
    urlCam3 = urlCam3Entry.get()
    imgtk3 = getImageTk(urlCam3)
    camera3.configure(image = imgtk3)
    camera3.image = imgtk3
    camera3.pack( padx=5, pady=5, fill = BOTH, expand = True)

def loopCamera():
    global mainWindow, cameraFrame1, cameraFrame2, cameraFrame3
    global urlCam1Entry, urlCam2Entry, urlCam3Entry
    global camera1, camera2, camera3
    global widthCamera, heigtCamera

    ####################
    # Camera Frame
    ####################
    
    cameraFrame = LabelFrame(mainWindow, text="Cameras",font=("Verdana", 20))
    cameraFrame.pack(padx=5, pady=5, fill = BOTH, expand = True)
     
    widthCamera = int(widthScreen/30)
    heigtCamera = int(heightScreen/25)
    # Camera 1
    cameraFrame1 = Frame(cameraFrame)
    cameraFrame1.pack(side = LEFT, padx=5, pady=5, fill = BOTH, expand = True)
    
    camera1 = Label(cameraFrame1, width=widthCamera, height=heigtCamera, bg="black")
    camera1.pack(padx=5, pady=5, fill = BOTH, expand = True)

    urlCam1Entry = Entry(cameraFrame1)
    urlCam1Entry.insert(0,"http://cam1/capture")
    urlCam1Entry.pack(padx=5, pady=5, fill = 'x', expand = True)
   
    openCam1Button = Button(cameraFrame1, text='Snapshot', command=open_cam1)
    openCam1Button.pack(padx=5, pady=5, fill = 'x', expand = True)

    # Camera 2
    cameraFrame2 = Frame(cameraFrame)
    cameraFrame2.pack(side = LEFT, padx=5, pady=5, fill = BOTH, expand = True)
    
    camera2 = Label(cameraFrame2, width=widthCamera, height=heigtCamera, bg="black")
    camera2.pack(padx=5, pady=5, fill = BOTH, expand = True)

    urlCam2Entry = Entry(cameraFrame2)
    urlCam2Entry.insert(0,"http://cam2/capture")
    urlCam2Entry.pack(padx=5, pady=5, fill = 'x', expand = True)
    
    openCam2Button = Button(cameraFrame2, text='Snapshot', command=open_cam2)
    openCam2Button.pack(padx=5, pady=5, fill = 'x', expand = True)
 
    # Camera 3
    cameraFrame3 = Frame(cameraFrame)
    cameraFrame3.pack(side = LEFT, padx=5, pady=5, fill = BOTH, expand = True)
    
    camera3 = Label(cameraFrame3, width=widthCamera, height=heigtCamera, bg="black")
    camera3.pack( padx=5, pady=5, fill = BOTH, expand = True)

    urlCam3Entry = Entry(cameraFrame3)
    urlCam3Entry.insert(0,"http://cam3/capture")
    urlCam3Entry.pack(padx=5, pady=5, fill = 'x', expand = True)
    
    openCam3Button = Button(cameraFrame3, text='Snapshot', command=open_cam3)
    openCam3Button.pack(padx=5, pady=5, fill = 'x', expand = True)

    #cameraFrame.after(5, loopCamera)

def openPort():
    global portWindow, mainWindow
    global port
    global ser
    global divisionSelectionBox
    global operationFrame
    global degreeSpin

    # Open serial port
    port = portsSelectionBox.get()
    ser = serial.Serial(port, 115200)
    ser.setDTR()
    ser.flush()
    print("Opened port: " + port)

    # Main window
    mainWindow = Toplevel(root)
    mainWindow.geometry("900x600")
    mainWindow.title("Inmachines 3D Scanner")
    mainWindow.resizable(1, 1)

    loopCamera()

    ####################
    # Console Frame
    ####################
    consoleFrame = LabelFrame(mainWindow, text="Console",font=("Verdana", 20))
    consoleFrame.pack(side = LEFT, fill = BOTH, expand = True, padx=5, pady=5)

    # Spinbox for var degree
    degree = IntVar()
    degreeSpin = Spinbox(consoleFrame, from_=1, to=360, textvariable=degree)
    degreeSpin.pack(side=LEFT, padx=5, pady=5, fill = BOTH, expand = True)

    labelDegree = Label(consoleFrame, text='degree')
    labelDegree.pack(side=LEFT, padx=5, pady=5, fill = BOTH, expand = True)

    # Button for cw
    cwbutton = Button(consoleFrame, text="Clockwise", command=cw, fg="black")
    cwbutton.pack( padx=5, pady=5, fill = BOTH, expand = True)

    # Button for cw
    cwbutton = Button(consoleFrame, text="Counter Clockwise", command=ccw, fg="black")
    cwbutton.pack( padx=5, pady=5, fill = BOTH, expand = True)

    ####################
    # Operation Frame
    ####################
    operationFrame = LabelFrame(mainWindow, text="Operation",font=("Verdana", 20))
    operationFrame.pack(side = LEFT,fill = BOTH, expand = True, padx=5, pady=5)

    # Spinbox for num of division
    division = IntVar()
    divisionSelectionBox = Combobox(operationFrame, state="readonly", textvariable=division)
    divisionSelectionBox['values'] = (2, 4, 8, 16, 32)
    divisionSelectionBox.pack( padx=5, pady=5, fill = BOTH, expand = True)

    # Button for scanning
    scanbutton = Button(operationFrame, text="Scan", command=scan, fg="black")
    scanbutton.pack( padx=5, pady=5, fill = BOTH, expand = True)


if __name__ == '__main__':
    # root window
    root = Tk()
    root.geometry("400x150")
    root.title("Inmachines 3D Scanner")
    root.resizable(0, 0)

    #getting screen width and height of display
    widthScreen= root.winfo_screenwidth()
    heightScreen= root.winfo_screenheight()

    portsSelectionBox = Combobox(root, values=[], postcommand=refreshPorts)
    portsSelectionBox.set("Choose Serial Port")
    portsSelectionBox.pack(pady=10)

    startButton = Button(root, text="Open Serial Port", command=openPort)
    startButton.pack(pady=10)

    root.mainloop()
