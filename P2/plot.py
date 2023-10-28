import pandas as pd
import matplotlib.pyplot as plt

# Cargar los datos desde el archivo CSV
data1 = pd.read_csv("cyclictestURJC.csv", low_memory=False)
data2 = pd.read_csv("cyclictestURJC2.csv", low_memory=False)
# Seleccionar la columna de datos que quieres usar para el histograma
data_column1 = data1[" latencia"]  # Reemplaza "datos" con el nombre de tu columna
data_column2 = data2[" latencia"]
# Crear un histograma


plt.hist(data_column1, bins=2000, alpha=0.5, label='Datos del Archivo 1', color='blue')  # Puedes ajustar el número de bins según tus necesidades
plt.hist(data_column2, bins= 2000, alpha=0.5, label='Datos del Archivo 2', color='red')

plt.xlabel("latencia (us)")
plt.ylabel("Frecuencia")
plt.title("Histograma de Datos")
plt.xscale('linear')
plt.legend()
# Mostrar el histograma
plt.show()