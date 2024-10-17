from tkinter import *
from tkinter.ttk import Combobox, Progressbar
import serial
import serial.tools.list_ports
from tkinter import filedialog
import urllib.request
import numpy as np
import cv2
from PIL import Image, ImageTk
import time
import threading

port = ''
portlist = []
readData = ''
portWindow = None
ser = None

def get_cam1():
    return cv2.VideoCapture(0)
def get_cam2():
    return cv2.VideoCapture(1)
def get_cam3():
    return cv2.VideoCapture(2)

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
    
    for i in range(1,numDivision+1):
        cap1 = get_cam1()
        
        # Capture a frame
        # cap0 = cv2.VideoCapture(portChoosen1)
        progressLabel['text'] = "Scanning {}/{} division.".format(i, numDivision)
        loadingBar['value']+= 100/(numDivision*3+2)

        cap1.set(3, 1920)
        cap1.set(4, 1080)
        _, frame1 = cap1.read()
        cv2.imwrite(path+'/'+'cam1_'+str(i)+'.jpg', frame1)
        open_cam1(False)
        cap1.release()

        # cap1 = cv2.VideoCapture(portChoosen2)
        cap2 = get_cam2()

        progressLabel['text'] = "Scanning {}/{} division.".format(i, numDivision)
        loadingBar['value']+= 100/(numDivision*3+2)
        cap2.set(3, 1920)
        cap2.set(4, 1080)
        _, frame2 = cap2.read()
        cv2.imwrite(path+'/'+'cam2_'+str(i)+'.jpg', frame2)
        open_cam2(False)
        cap2.release()

        # cap2 = cv2.VideoCapture(portChoosen3)
        cap3 = get_cam3()
        progressLabel['text'] = "Scanning {}/{} division.".format(i, numDivision)
        loadingBar['value']+= 100/(numDivision*3+2)
        cap3.set(3, 1920)
        cap3.set(4, 1080)
        _, frame3 = cap3.read()
        cv2.imwrite(path+'/'+'cam3_'+str(i)+'.jpg', frame3)
        open_cam3(False)
        cap3.release()
        
        ser.write(b'A')
        ser.write(str.encode(str(360/numDivision)))
        line = NONE
        while TRUE:
            line = ser.readline()   # read a '\n' terminated line
            print(len(line))
            print(line)
            if len(line)<7:
                break
    
    loadingBar['value'] = 100
    progressLabel['text'] = "Scanning is completed."
    time.sleep(1)
    loadingBar['value'] = 0
    progressLabel['text'] = ""

        
        
def scan():
    global path
    global divisionSelectionBox
    global loadingBar,  progressLabel

    divsionChoosen = int(divisionSelectionBox.get())
    print('Scan')
    print("Number of Division:{}".format(divsionChoosen))
    path = selectFile()

    progressLabel['text'] = "Please wait start scanning."
    loadingBar['value']+= 5
    
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

def open_cam():
    open_cam1()
    open_cam2()
    open_cam3()


def open_cam1(release = True):
    global loadingBar,  progressLabel
        
    progressLabel['text'] = "Please wait opening camera 1..."
    loadingBar['value'] = 20

    cap1 = get_cam1()

    widthCamera = int(widthScreen/3) - 30
    heigtCamera = int(heightScreen/2.5) - 10

    loadingBar['value'] = 50

    _, frame0 = cap1.read()

    loadingBar['value'] = 70

    # Convert image from one color space to other
    cv2image= cv2.cvtColor(frame0,cv2.COLOR_BGR2RGB)

    # Capture the latest frame and transform to image
    captured_image = Image.fromarray(cv2image)

    # Convert image to PhotoImage
    photo_image = ImageTk.PhotoImage(image = captured_image)

    # Displaying photoimage in the label
    camera1.photo_image = photo_image

    # Configure image in the label
    camera1.configure(image = photo_image, width= widthCamera, height=heigtCamera)
    
    if release:
        cap1.release()

    loadingBar['value'] = 100
    progressLabel['text'] = "Camera1: Completed."
    time.sleep(1)
    loadingBar['value'] = 0
    progressLabel['text'] = ""

def open_cam2(release = True):
    global loadingBar,  progressLabel
        
    progressLabel['text'] = "Please wait opening camera 2..."
    loadingBar['value'] = 20

    cap2 = get_cam2()

    widthCamera = int(widthScreen/3) - 30
    heigtCamera = int(heightScreen/2.5) - 10


    loadingBar['value'] = 50

    loadingBar.pack(padx=5, pady=5, fill = BOTH, expand = True)

    _, frame = cap2.read()

    loadingBar['value'] = 70

    # Convert image from one color space to other
    cv2image= cv2.cvtColor(frame,cv2.COLOR_BGR2RGB)

    # Capture the latest frame and transform to image
    captured_image = Image.fromarray(cv2image)

    # Convert image to PhotoImage
    photo_image = ImageTk.PhotoImage(image = captured_image)

    # Displaying photoimage in the label
    camera2.photo_image = photo_image

    # Configure image in the label
    camera2.configure(image = photo_image,  width= widthCamera, height=heigtCamera)
    if release:
        cap2.release()

    loadingBar['value'] = 100
    progressLabel['text'] = "Camera2: Completed."
    time.sleep(1)
    loadingBar['value'] = 0
    progressLabel['text'] = ""


def open_cam3(release = True):
    global loadingBar,  progressLabel
        
    progressLabel['text'] = "Please wait opening camera 3..."
    loadingBar['value'] = 20

    cap3 = get_cam3()

    widthCamera = int(widthScreen/3) - 30
    heigtCamera = int(heightScreen/2.5) - 10


    loadingBar['value'] = 50

    _, frame = cap3.read()

    loadingBar['value'] = 70

    # Convert image from one color space to other
    cv2image= cv2.cvtColor(frame,cv2.COLOR_BGR2RGB)

    # Capture the latest frame and transform to image
    captured_image0 = Image.fromarray(cv2image)

    # Convert image to PhotoImage
    photo_image0 = ImageTk.PhotoImage(image = captured_image0)

    # Displaying photoimage in the label
    camera3.photo_image = photo_image0

    # Configure image in the label
    camera3.configure(image = photo_image0, width= widthCamera, height=heigtCamera)
    if release:
        cap3.release()

    loadingBar['value'] = 100
    progressLabel['text'] = "Camera3: Completed."
    time.sleep(1)
    loadingBar['value'] = 0
    progressLabel['text'] = ""

def loopCamera():
    global mainWindow, cameraFrame1, cameraFrame2, cameraFrame3
    global camera1, camera2, camera3
    global widthCamera, heigtCamera
    global loadingBar, progressLabel

    ####################
    # Camera Frame
    ####################
    
    cameraFrame = LabelFrame(mainWindow, text="Cameras",font=("Verdana", 20))
    cameraFrame.pack(padx=5, pady=5, fill = BOTH, expand = True)
     
    widthCamera = int(widthScreen/30)
    heigtCamera = int(heightScreen/35)

    cameraTopFrame = LabelFrame(cameraFrame)
    cameraTopFrame.pack(padx=5, pady=5, fill = BOTH, expand = True)

    # Camera 1
    cameraFrame1 = Frame(cameraTopFrame)
    cameraFrame1.pack(side = LEFT, padx=5, pady=5, fill = BOTH, expand = True)
    
    camera1 = Label(cameraFrame1, width=widthCamera, height=heigtCamera, bg="black")
    camera1.pack(padx=5, pady=5, fill = BOTH, expand = True)

    
    # Snapshot button   

    openCam1Button = Button(cameraFrame1, text='Open Camera 1', command=lambda: threading.Thread(target=open_cam1).start())
    openCam1Button.pack(padx=5, pady=5, fill = 'x', expand = True)

    # Camera 2
    cameraFrame2 = Frame(cameraTopFrame)
    cameraFrame2.pack(side = LEFT, padx=5, pady=5, fill = BOTH, expand = True)
    
    camera2 = Label(cameraFrame2, width=widthCamera, height=heigtCamera, bg="black")
    camera2.pack(padx=5, pady=5, fill = BOTH, expand = True)
   
    # Snapshot button   
    
    openCam2Button = Button(cameraFrame2, text='Open Camera 2', command=lambda: threading.Thread(target=open_cam2).start())
    openCam2Button.pack(padx=5, pady=5, fill = 'x', expand = True)


    # Camera 3
    cameraFrame3 = Frame(cameraTopFrame)
    cameraFrame3.pack(side = LEFT, padx=5, pady=5, fill = BOTH, expand = True)
    
    camera3 = Label(cameraFrame3, width=widthCamera, height=heigtCamera, bg="black")
    camera3.pack( padx=5, pady=5, fill = BOTH, expand = True)
   
    # Snapshot button   
    
    openCam3Button = Button(cameraFrame3, text='Open Camera 3', command=lambda: threading.Thread(target=open_cam3).start())
    openCam3Button.pack(padx=5, pady=5, fill = 'x', expand = True)

    # Snapshot Button For 3 Camera
    cameraBottomFrame = LabelFrame(cameraFrame)
    cameraBottomFrame.pack(padx=5, pady=5, fill = BOTH, expand = True)

    # progressbar
    loadingBar = Progressbar(cameraBottomFrame, orient='horizontal', mode='determinate', length=280)
    loadingBar.pack(padx=5, pady=5, fill = BOTH, expand = True)

    progressLabel = Label(cameraBottomFrame)
    progressLabel.pack(padx=5, pady=5, fill = BOTH, expand = True)

    openCamButton = Button(cameraBottomFrame, text='Open All Cameras', command=lambda: threading.Thread(target=open_cam).start())
    openCamButton.pack(side = TOP,padx=5, pady=5, fill = 'x', expand= True)

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
    scanbutton = Button(operationFrame, text="Scan", command=lambda: threading.Thread(target=scan).start(), fg="black")
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