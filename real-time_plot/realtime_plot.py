#!/usr/bin/env python

from threading import Thread
import serial
import time
import collections
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import struct
import pandas as pd


class serialPlot:
    def __init__(self, serialPort = 'COM8', serialBaud = 115200, plotLength = 100, dataNumBytes = 4):
        self.port = serialPort
        self.baud = serialBaud
        
        self.plotMaxLength = plotLength
        self.dataNumBytes = dataNumBytes
        self.numPlots = 6
        self.rawData  = bytearray(dataNumBytes)
        self.data = []
        for i in range(self.numPlots):   # give an array for each type of data and store them in a list
            self.data.append(collections.deque([0] * plotLength, maxlen=plotLength))
        
        self.isRun          = True
        self.isReceiving    = False
        self.thread         = None

        self.plotTimer      = 0
        self.previousTimer  = 0

        #value from sensors
        self.pres_mark = bytes('[0]', encoding = 'utf-8')
        self.temp_mark = bytes('[1]', encoding = 'utf-8')
        self.xval_mark = bytes('[2]', encoding = 'utf-8')
        self.yval_mark = bytes('[3]', encoding = 'utf-8')
        self.zval_mark = bytes('[4]', encoding = 'utf-8')
        self.cough_mark = bytes('[C]', encoding = 'utf-8')

        self.pres_data = collections.deque([0] * plotLength, maxlen=plotLength)
        self.temp_data = collections.deque([0] * plotLength, maxlen=plotLength)
        self.xval_data = collections.deque([0] * plotLength, maxlen=plotLength)
        self.yval_data = collections.deque([0] * plotLength, maxlen=plotLength)
        self.zval_data = collections.deque([0] * plotLength, maxlen=plotLength)
        self.cough_probability = collections.deque([0] * plotLength, maxlen=plotLength)

        print('Trying to connect to: ' + str(serialPort) + ' at ' + str(serialBaud) + ' BAUD.')
        try:
            self.serial = serial.Serial(serialPort, serialBaud, timeout = 4)
            print('Connected to ' + str(serialPort) + ' at ' + str(serialBaud) + ' BAUD.')
        except:
            print("Failed to connect with " + str(serialPort) + ' at ' + str(serialBaud) + ' BAUD.')

    def readSerialStart(self):
        if self.thread == None:
            self.thread = Thread(target=self.backgroundThread)
            self.thread.start()
            # Block till we start receiving values
    
            while self.isReceiving != True:
                time.sleep(0.1)

    def getSerialData(self, frame, lines, lineLabel):
        
        identifier = self.rawData[0:3]
        value = self.rawData[4:]

        if identifier == self.pres_mark:
            self.data[0].append(value)
            lines[0].set_data(range(self.plotMaxLength), self.data[0])
        elif identifier == self.temp_mark:
            self.data[1].append(value)
            lines[1].set_data(range(self.plotMaxLength), self.data[1])
        elif identifier == self.xval_mark:
            self.data[2].append(value)
            lines[2].set_data(range(self.plotMaxLength), self.data[2])
        elif identifier == self.yval_mark: 
            self.data[3].append(value)
            lines[3].set_data(range(self.plotMaxLength), self.data[3])
        elif identifier == self.zval_mark: 
            self.data[4].append(value)
            lines[4].set_data(range(self.plotMaxLength), self.data[4])
        elif identifier == self.cough_mark: 
            self.data[5].append(value)
            lines[5].set_data(range(self.plotMaxLength), self.data[5])
        return lines

    def backgroundThread(self):     # retrieve data
        time.sleep(1.0)             # give some buffer time for retrieving data
        self.serial.reset_input_buffer()
        while (self.isRun):
            self.rawData = self.serial.readline()
            self.isReceiving = True
            #print(self.rawData)

    def close(self):
        self.isRun = False
        self.thread.join()
        self.serial.close()
        print('Disconnected...')
        # df = pd.DataFrame(self.csvData)
        # df.to_csv('/home/rikisenia/Desktop/data.csv')


def main():
    portName = 'COM8'     
    baudRate = 112500
    maxPlotLength   = 100
    dataNumBytes    = 4        # number of bytes of 1 data point
    numPlots        = 6

    s = serialPlot(portName, baudRate, maxPlotLength, dataNumBytes)   # initializes all required variables
    s.readSerialStart()                                               # starts background thread

    # plotting starts below
    pltInterval = 100    # Period at which the plot animation updates [ms]
    xmin = 0
    xmax = maxPlotLength
    ymin = -(1)
    ymax = 1
    #fig = plt.figure(figsize=(20,12))
    fig, (ax0, ax1, ax2, ax3) = plt.subplots(4,1)
    
    title = ['Pressure', 'Temperature', 'Acceleration']
    lineLabel = ['P','T','X', 'Y', 'Z','C']
    style = ['g-','y-','r-', 'c-', 'b-','k-']  # linestyles for the different plots
   
    ax0.set_xlim(xmin, xmax)
    ax0.set_ylim(994, 1002)
    ax0.set_title('Data Plot')
    ax0.set_ylabel('hPa')

    ax1.set_xlim(xmin, xmax)
    ax1.set_ylim(30, 36)
    ax1.set_ylabel('Celsius')

    ax2.set_xlim(xmin, xmax)
    ax2.set_ylim((float(ymin - (ymax - ymin) / 10), float(ymax + (ymax - ymin) / 10)))
    ax2.set_ylabel('g')
    ax2.set_xlabel('time')
    
    
    ax3.set_xlim(xmin, xmax)
    ax3.set_ylim(0,1)
    ax3.set_ylabel('cough prediction')
    #plt.show()

    lines = []

    for i in range(numPlots):
        if (i == 0):
            lines.append(ax0.plot([], [], style[i], label = lineLabel[i])[0])
        elif (i == 1):
            lines.append(ax1.plot([], [], style[i], label = lineLabel[i])[0])
        elif (i == 5):
            lines.append(ax3.plot([], [], style[5], label = lineLabel[5])[0])
        else :
            lines.append(ax2.plot([], [], style[i], label = lineLabel[i])[0])
        
    
    anim = animation.FuncAnimation(fig, s.getSerialData, fargs=(lines, lineLabel), interval=pltInterval)    # fargs has to be a tuple
    ax2.legend(loc='upper left')
    plt.show()
    s.close()


if __name__ == '__main__':
    main()