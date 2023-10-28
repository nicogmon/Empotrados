import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Rutas de los archivos CSV
csv_file1 = '~/Escritorio/SISSTEMAS_EMPOTRADOS/P2/Eva_Fernandez_Practica2/idle.csv'
csv_file2 = '~/Escritorio/SISSTEMAS_EMPOTRADOS/P2/Eva_Fernandez_Practica2/hackbench.csv'
csv_file3 = '~/Escritorio/SISSTEMAS_EMPOTRADOS/P2/Eva_Fernandez_Practica2/bonnie.csv'

# Leer los datos de latencia desde los archivos CSV
latency1 = pd.read_csv("cyclictestURJC.csv", usecols=[2], header=None, names=['LATENCIA'])['LATENCIA']
#latency2 = pd.read_csv("cyclictestURJC2.csv", usecols=[2], header=None, names=['LATENCIA'])['LATENCIA']
#latency3 = pd.read_csv(csv_file3, usecols=[2], header=None, names=['LATENCIA'])['LATENCIA']


# Definir los límites para el histograma
bin_limits = np.arange(min(latency1), max(latency1) + 0.01, 0.01)

fig, ax = plt.subplots(figsize=(8, 6))

# Crear los histogramas
ax.hist(latency1, bins=bin_limits, color='blue', alpha=0.4, label='Idle')
#ax.hist(latency2, bins=bin_limits, color='green', alpha=0.4, label='Hackbench')
#ax.hist(latency3, bins=bin_limits, color='red', alpha=0.4, label='Bonnie')

# Leyenda, título y etiquetas de los ejes
ax.legend()
ax.set_title('Raspberry Real Time')
ax.set_xlabel('Latencia')
ax.set_ylabel('Frecuencia')

# Mostrar el histograma
plt.xlim(0, 8)
plt.show()

