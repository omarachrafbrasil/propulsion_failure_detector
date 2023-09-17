# Importing the matplotlib.pyplot
import matplotlib.pyplot as plt
import csv
import pandas as pd

##########
def DFCalcAverage(label, pairs):
    if len(pairs) == 0:
        return
    
    average = 0
    
    for pair in pairs:
        v1, v2 = pair
        average += v2
    average /= len(pairs)
   
    print ('AVG ', label, "{:.1f}".format(average), 'micro seconds')
##########    

##########
def DF2List(df, filterString, minWidth, labelFrom, labelTo):
    filter = df.iloc[:, 0] == filterString
    df2 = df[filter]
    list = []
    startT = 0
    stopT = 0
    count = 0
        
    for indice, linha in df2.iterrows():
        e = linha.iloc[1]
        t = linha.iloc[2]
        
        if e == labelFrom:
            startT = t
            count += 1
        
        if e == labelTo and count == 1:
            stopT = t
            width = stopT - startT
            count = 0
            
            if minWidth > 0:
                width = width if width > minWidth else minWidth
                
            tupla = (startT, width)
            
            #print(filterString,tupla)
            
            list.append(tupla)
            
    return list
##########



nome_arquivo = 'C:\download\putty_acelera_desacelera_acelera_extrapola.log'
      

MIN_TASK_WIDTH = 1000 # 0 uses current else uses minWIdth
NUM_LINE = 25000000
FROM_TIME = 89070324 # 90650000 #15000000
TO_TIME =   97063441 # 90800000  #20000000
   
# DataFrame 'df'        
dfcsv = pd.read_csv(nome_arquivo, sep=';', header=None) #, nrows=NUM_LINE)

filter_from = dfcsv.iloc[:, 2] >= FROM_TIME
filter_to   = dfcsv.iloc[:, 2] <= TO_TIME
dfcsv = dfcsv[filter_from]
df = dfcsv[filter_to] #.iloc[:NUM_LINE]

lowMicrosec = df.iloc[0, 2]
highMicrosec = df.iloc[-1, 2]

tasksPWM = DF2List(df, 'PWM', MIN_TASK_WIDTH, 'T3', 'T2')  
tasksIR = DF2List(df, 'IR', MIN_TASK_WIDTH, 'T3', 'T2')     
tasksTMR = DF2List(df, 'TMR', MIN_TASK_WIDTH, 'T3', 'T2')
tasksDISP = DF2List(df, 'DISP', MIN_TASK_WIDTH, 'T3', 'T2')
tasksFAIL = DF2List(df, 'FAIL', MIN_TASK_WIDTH, 'T3', 'T2')

isrPWM = DF2List(df, 'PWM', MIN_TASK_WIDTH, 'T0', 'T4')  
isrIR = DF2List(df, 'IR', MIN_TASK_WIDTH, 'T0', 'T4')     
isrTMR = DF2List(df, 'TMR', MIN_TASK_WIDTH, 'T0', 'T4')

isr2taskPWM = DF2List(df, 'PWM', MIN_TASK_WIDTH, 'T0', 'T3')
isr2taskIR = DF2List(df, 'IR', MIN_TASK_WIDTH, 'T0', 'T3')
isr2taskTMR = DF2List(df, 'TMR', MIN_TASK_WIDTH, 'T0', 'T3')

DFCalcAverage('Task PWM', tasksPWM)
DFCalcAverage('Task IR', tasksIR)
DFCalcAverage('Task TMR', tasksTMR)
DFCalcAverage('Task DISP', tasksDISP)
DFCalcAverage('Task FAIL', tasksFAIL)
DFCalcAverage('ISR PWM', tasksPWM)
DFCalcAverage('ISR IR', tasksIR)
DFCalcAverage('ISR TMR', tasksTMR)
DFCalcAverage('Activation PWM from ISR', isr2taskPWM)
DFCalcAverage('Activation IR from ISR', isr2taskIR)
DFCalcAverage('Activation TMR from ISR', isr2taskTMR)
        
        
# gnt.broken_barh([(start_time, duration)], (lower_yaxis, height), facecolors=('tab:colours'))
# We can also add edge color by setting “edgecolor” attribute to any color. 

# Declaring a figure "gnt"
fig, gnt = plt.subplots()


# Setting Y-axis limits
gnt.set_ylim(4, 86)

# Setting X-axis limits
gnt.set_xlim(lowMicrosec, highMicrosec)

# Setting labels for x-axis and y-axis
gnt.set_xlabel('Microseconds')
gnt.set_ylabel('Tasks')

# Setting ticks on y-axis
gnt.set_yticks([10, 20, 30, 40, 50, 60, 70, 80])

# Labelling tickes of y-axis
gnt.set_yticklabels(['Timer ISR', 'Timer Task', 'PWM ISR', 'PWM Task', 'IR ISR', 'IR Task', 'Failure Task', 'Display Task'])

# Setting graph attribute
gnt.grid(True)

gnt.broken_barh(isrTMR,    ( 6, 8), facecolors='tab:blue')
gnt.broken_barh(tasksTMR,  (16, 8), facecolors='tab:blue')

gnt.broken_barh(isrPWM,    (26, 8), facecolors='tab:orange')
gnt.broken_barh(tasksPWM,  (36, 8), facecolors='tab:orange')

gnt.broken_barh(isrIR,     (46, 8), facecolors='tab:purple')
gnt.broken_barh(tasksIR,   (56, 8), facecolors='tab:purple')

gnt.broken_barh(tasksFAIL, (66, 8), facecolors='tab:red')

gnt.broken_barh(tasksDISP, (76, 8), facecolors='tab:green')




plt.subplots_adjust(left=0.06, right=0.99)
#plt.xlim(250000, 350000)

tx = [] 
for t in range(FROM_TIME, TO_TIME, 1000000):
    tx.append(t)
    
plt.xticks(tx) #[90650000, 90660000, 90670000, 90680000, 90690000, 90700000, 90750000, 90800000])

plt.title('Tasks/ISRs scheduling')

fig = plt.gcf()
fig.set_size_inches(16, 8) #polegadas


plt.savefig("tasks.png")
plt.show()
