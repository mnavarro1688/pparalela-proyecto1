#!/usr/bin/python
# -*- coding: latin-1 -*-
import matplotlib.pyplot as plt
import numpy as np
import argparse
#validacion del numero de procesos que sean iguales 
def ValidarNumProc(r, speed, rango):
    diccionario = {}
    cont = 0
    for i, elem in enumerate(r):
        if elem in diccionario:
            promedio = (speed[diccionario[elem]] + speed[i]) / 2
            speed[diccionario[elem]] = promedio
            speed.remove(speed[i])
            cont += 1
            return np.delete(np.arange(rango), np.where(np.arange(rango) == i+1))
        else:
            diccionario[elem] = i
    if cont == 0:
        return np.arange(rango)

rango1 = 0
rango2 = 0
rango3 = 0
r1=[]
r2=[]
r3=[]
speed1=[]
speed2=[]
speed3=[]
resolucion1=''
resolucion2=''
resolucion3=''
contador=0


# Creamos el objeto ArgumentParser
parser = argparse.ArgumentParser(description='Procesa un archivo de texto.')

# Agregamos el argumento del archivo
parser.add_argument('archivo', type=str, help='La ruta del archivo a procesar.')

# Parseamos los argumentos
args = parser.parse_args()
#lectura y validacion del ducuemento de entrada 
#with open('datos.txt', 'r') as f:args.archivo
with open(args.archivo, 'r') as f:
    lineas = f.readlines()
    
    for linea in lineas:
        #print(linea)
        if linea.strip() == '':
          contador=contador+1
        if ord(linea[0])>=48 and ord(linea[0])<=57:
          texto=linea.split()
          
          if contador==0:
            resolucion1=texto[2]+texto[3]+texto[4]
            if texto[1]>str(rango1):
              rango1=int(texto[1])
            r1.append(int(texto[1]))
            speed1.append(int(texto[0]))
            
          elif texto[2]!=resolucion1 and contador==1:
            resolucion2=texto[2]+texto[3]+texto[4]
            if texto[1]>str(rango2):
              rango2=int(texto[1])
            r2.append(int(texto[1]))
            speed2.append(int(texto[0]))
            
          elif texto[2]!= resolucion1 and texto[2]!= resolucion2 and contador==2:
            resolucion3 = texto[2]+texto[3]+texto[4]
            if texto[1]>str(rango3):
              rango3=int(texto[1])
            r3.append(int(texto[1]))
            speed3.append(int(texto[0])) 
            
x1 = ValidarNumProc(r1, speed1,rango1)        
x2 = ValidarNumProc(r2, speed2,rango2)
x3 = ValidarNumProc(r3, speed2,rango3)

#creacion de cada uno de los graficos 
if rango1>0 and rango2>0 and rango3>0:
  fig, (ax0, ax1, ax2) = plt.subplots(nrows=1, ncols=3, sharex=True,
                                    figsize=(12, 6))
  ax0.set_title(resolucion1)
  ax0.errorbar(x1, speed1,yerr=0,uplims=True, lolims=True,barsabove=True)
  ax0.set_ylabel('SpeedUp')
  ax0.set_xlabel('Num procesos')
  ax1.set_title(resolucion2)
  ax1.errorbar(x2, speed2,yerr=0, uplims=True, lolims=True,barsabove=True)
  ax1.set_ylabel('SpeedUp')
  ax1.set_xlabel('Num procesos')
  ax2.set_title(resolucion3)
  ax2.errorbar(x3, speed3,yerr=0, uplims=True, lolims=True,barsabove=True)
  ax2.set_ylabel('SpeedUp')
  ax2.set_xlabel('Num procesos')
  fig.suptitle('Cantidad de Procesos vs Speedup segun su resolucion')
  # Guardar gráfico como archivo de imagen
  fig.savefig('grafico3.png')
  
elif rango1>0 and rango2>0 and rango3==0 or rango1==0 and rango2>0 and rango3>0 or rango1>0 and rango2==0 and rango3>0:

  fig, ax= plt.subplots()
  
  if rango1>0:
    ax.errorbar(x1,speed1 , yerr=0, label=resolucion1)
  if rango2>0:
    ax.errorbar(x2, speed2, yerr=0, uplims=True, label=resolucion2)
  if rango3>0:
    ax.errorbar(x3, speed3, yerr=0, uplims=True, lolims=True,
               label=resolucion3)
  ax.set_ylabel('SpeedUp')
  ax.set_xlabel('Num procesos')
  fig.suptitle('Cantidad de Procesos vs Speedup segun su resolucion')
  ax.legend(loc='lower right')
  # Guardar gráfico como archivo de imagen
  fig.savefig('grafico2.png')
  
elif rango1>0 and rango2==0 and rango3==0 or rango1==0 and rango2>0 and rango3==0 or rango1==0 and rango2==0 and rango3>0:
  fig, ax= plt.subplots()
 
  if rango1>0:
    ax.errorbar(x1,speed1 , yerr=0, label=resolucion1)
  if rango2>0:
    ax.errorbar(x2, speed2, yerr=0, uplims=True, label=resolucion2)
  if rango3>0:
    ax.errorbar(x3, speed3, yerr=0, uplims=True, label=resolucion3)
  ax.set_ylabel('SpeedUp')
  ax.set_xlabel('Num procesos')
  fig.suptitle('Cantidad de Procesos vs Speedup segun su resolucion')
  ax.legend(loc='lower right')
  # Guardar gráfico como archivo de imagen
  fig.savefig('grafico1.png')





