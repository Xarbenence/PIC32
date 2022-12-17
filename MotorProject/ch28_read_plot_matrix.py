# chapter 28 in python

# sudo apt-get install python3-pip
# python3 -m pip install pyserial
# sudo apt-get install python3-matplotlib


import matplotlib.pyplot as plt 
from statistics import mean 
def read_plot_matrix():
    n_str = ser.read_until(b'\n');  # get the number of data points to receive
    n_int = int(n_str) # turn it into an int
    print('Data length = ' + str(n_int))
    ref = []
    data = []
    data_received = 0
    while data_received < n_int:
        dat_str = ser.read_until(b'\n');  # get the data as a string, ints seperated by spaces
        dat_f = list(map(float,dat_str.split())) # now the data is a list
        ref.append(dat_f[0])
        data.append(dat_f[1])
        data_received = data_received + 1
    meanzip = zip(ref,data)
    meanlist = []
    for i,j in meanzip:
        meanlist.append(abs(i-j))
    score = mean(meanlist)
    t = range(len(ref)) # index array
    plt.plot(t,ref,'r*-',t,data,'b*-')
    plt.title('Score = ' + str(score))
    plt.ylabel('value')
    plt.xlabel('index')
    plt.show()

from genref import genRef

import serial
ser = serial.Serial('/dev/tty.usbserial-DT03EQZT',230400,rtscts=1)
print('Opening port: ')
print(ser.name)

has_quit = False
# menu loop
while not has_quit:
    print('PIC32 MOTOR DRIVER INTERFACE')
    # display the menu options; this list will grow
    print('\tb: get current \tc: get encoder counts \td: Get Motor Position in degrees\te: reset encoder \tf: Set Duty Cycle \tg: Set Current Gains \th: Get Current Gains \ti: Set Position Gains \tj: Get Position Gains \tk: ITEST \tl: Go to angle \tm: Load Step Trajectory \tn: Load Cubic Trajectory ) \to: Execute \tp: IDLE \tr: Get Mode \tq: Quit') # '\t' is a tab
    # read the user's choice
    selection = input('\nENTER COMMAND: ')
    selection_endline = selection+'\n'
     
    # send the command to the PIC32
    ser.write(selection_endline.encode()); # .encode() turns the string into a char array
    
    # take the appropriate action
    # there is no switch() in python, using if elif instead

    if (selection == 'b'): # current reading
        print('Requesting current: ')
        current = float(ser.read_until(b'\n')); 

        # n_int = int(n_str)
        print('Current is ',current,'ma \n')

        
    elif (selection == 'c'): # encoder counts
        print('Requesting encoder counts: ')
        n_str = ser.read_until(b'\n'); 
        n_int = int(n_str)
        print('Encoder counts = '+str(n_int)+'\n')

    elif (selection == 'd'): # position in degrees
        print('Requesting position in degrees: ')
        n_str = str(ser.read_until(b'\n')); 
        dstrp = n_str.lstrip("b'")
        ndstrp = dstrp.rstrip("n\\'")
        
        #strp = gains_str.lstrip("b'")
        #nstrp = strp.rstrip("n\\'")
        # n_int = int(n_str)
        print("Motor position is ",ndstrp," degrees\n")

    elif (selection == 'e'): # position reset
        print("Motor position reset")

    elif (selection == 'f'): # duty cycle
        duty = input('\n Set Duty Cycle from -100 to 100 percent:')
        duty_endline = duty + '\n'
        ser.write(duty_endline.encode())
      
        duty = int(ser.read_until(b'\n'))
        print('Duty Cycle set to',duty,"%")

    elif (selection == 'g'): # current gains
        Kp = input("Enter Kp: ")
        Kp_str = Kp + '\n'
        ser.write(Kp_str.encode()) # send to pic
        #Kd = input("Enter Kd: ")
        #Kd_str = Kd + '\n'
        #ser.write(Kd_str.encode()) # send to pic
        Ki = input("Enter Ki: ")
        Ki_str = Ki + '\n'
        ser.write(Ki_str.encode()) # send to pic
        # print("hello")
        #gains = ser.read_until(b'\n')
        #print(gains)
        #gains_str = str(gains)
        #gains_str = gains_str.split()
        #Kp = float(gains_str[0])
        #Ki = float(gains_str[1])
        # Ki = ser.read_until(b'n')

        print("Kp set to ",Kp," Ki set to ",Ki)   #"Kp set to "+str(Kp)+ "and Ki set to "+str(Ki)+"\n")

    elif (selection == 'h'):    
        #gains = ser.read_until(b'\n')
        #gains = gains.split()

        #Kp = int(gains[0])
        #Ki = int(gains[1])
        print("Kp is ",Kp," and Ki is ",Ki)


    elif (selection == 'i'):
        Kpp = input("Enter Kpp:")
        Kpp_str = Kpp + '\n'
        ser.write(Kpp_str.encode()) # send to pic

        Kip = input("Enter Kip:")
        Kip_str = Kip + '\n'
        ser.write(Kip_str.encode()) # send to pic

        Kdp = input("Enter Kdp:")
        Kdp_str = Kdp + '\n'
        ser.write(Kdp_str.encode()) # send to pic
    

        print("Kpp is",Kpp,"Kip is",Kip,"Kdp is",Kdp)


    
    elif (selection == 'j'):
        #gains = ser.read_until(b'\n'); 
        #gains_str = str(gains)
        #strp = gains_str.lstrip("b'")
        #nstrp = strp.rstrip("n\\'")

        #gns = nstrp.split()
        print("Kpp is",Kpp,"Kip is",Kip,"Kdp is",Kdp)

    elif (selection == 'k'): # ITEST set current gains
        print('Testing current gains\n')
        read_plot_matrix()

    #elif (selection == 'r'):
    #   print('Requesting mode: ')
     #   n_str = ser.read_until(b'\n'); 
     #   n_int = int(n_str)
     #   print('Mode = '+str(n_int)+'\n')

    elif (selection == 'n'): # cubic
        ref = genRef('cubic')
        
        #t = range(len(ref))
        #plt.plot(t,ref,'r*-')
        #plt.ylabel('angle in degrees')
        #plt.xlabel('index')
        #plt.show()
        #print(len(ref))
        # send 
        ser.write((str(len(ref))+'\n').encode())
        for i in ref:
            ser.write((str(i)+'\n').encode())

    elif (selection == 'o'):
        read_plot_matrix()
    
    elif (selection == 'p'):
        print("Motor powered off")

    elif (selection == 'r'):
        mode = ser.read_until(b'\n')
        if (int(mode) == 0):
            print("Motor is in IDLE mode")
        elif int(mode) == 1:
            print("Motor is in PWM mode")
        elif int(mode) == 2:
            print("Motor is in ITEST mode")
        elif int(mode) == 3:
            print("Motor is in HOLD mode")
        elif int(mode == 4):
            print("Motor is in TRACK mode")

    elif (selection == 'q'):
        print('Exiting client')
        has_quit = True; # exit client
        # be sure to close the port
        ser.close()
    else:
        print('Invalid Selection ' + selection_endline)
