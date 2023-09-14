# Importing the matplotlib.pyplot
import matplotlib.pyplot as plt
import csv

# Abra o arquivo CSV para leitura
with open('C:\download\putty_acelera_desacelera_acelera_extrapola.log', newline='') as arquivo_csv:
    leitor = csv.reader(arquivo_csv, delimiter=';')
    
    # Inicialize uma lista vazia para armazenar os dados
    dados = []
    
    # Loop pelo arquivo CSV e armazene os dados na lista
    for linha in leitor:
        dados.append(linha)
        
print('# de linhas lidas: ', len(dados))
for i in range(10):
    print('Task: ', dados[i][0], ', Event: ', dados[i][1], ', Timestamp: ', dados[i][2])
        
# gnt.broken_barh([(start_time, duration)], (lower_yaxis, height), facecolors=('tab:colours'))
# We can also add edge color by setting “edgecolor” attribute to any color. 

# Declaring a figure "gnt"
fig, gnt = plt.subplots()

# Setting Y-axis limits
gnt.set_ylim(0, 110)

# Setting X-axis limits
gnt.set_xlim(0, 160)

# Setting labels for x-axis and y-axis
gnt.set_xlabel('Microseconds since start')
gnt.set_ylabel('Tasks')

# Setting ticks on y-axis
gnt.set_yticks([15, 35, 55, 75, 95])

# Labelling tickes of y-axis
gnt.set_yticklabels(['Display Task', 'Timer Task', 'PWM Task', 'IR Task', 'Failure Task'])

# Setting graph attribute
gnt.grid(True)

# Declaring a bar in schedule
gnt.broken_barh([(40, 50)], (10, 10), facecolors=[(100/255, 150/255, 200/255)], edgecolor=[(0,0,0)])

# Declaring multiple bars in at same level and same width
gnt.broken_barh([(110, 10), (150, 10)], (30, 10), facecolors='tab:blue')

gnt.broken_barh([(10, 50), (100, 20), (130, 10)], (50, 10), facecolors=('tab:red'), edgecolor=('tab:green'))

#plt.savefig("gantt1.png")
plt.show()
