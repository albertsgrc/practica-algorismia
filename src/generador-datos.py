#!/usr/bin/python

import subprocess
import json
import os
import sys
from os import listdir
from os import path
from os.path import isfile, join, basename, splitext, devnull

ALGO_PATH = "./algoritmos/"
FILES_PATH = "./archivos/"
OUTPUT_PATH = "./salidas/"
STATS_OUTPUT_PATH = "./datos/"

TESTS_PER_EXPERIMENT = 4

def write_percentage(perc):
    equals = int(perc*20)
    perc = int(100*perc)
    sys.stdout.write('\r')
    sys.stdout.write("[%-20s] %d%%" % ('='*equals, perc))
    sys.stdout.flush()

# Inicializacion

algos = [ splitext(basename(f))[0] for f in listdir(ALGO_PATH) if f.endswith(".cc") and isfile(join(ALGO_PATH, f)) ]

subprocess.call("make")

for algo in algos:
    directory = OUTPUT_PATH + algo + "/"
    if not path.exists(directory): os.makedirs(directory)

if not path.exists(STATS_OUTPUT_PATH): os.makedirs(STATS_OUTPUT_PATH)

def calcular_prova(size, proportion, factor, isStats, variable, property, resultsfile, propertyfn = lambda x, _ : x ):
    data_map = {}

    for algo in algos: data_map[algo] = 0

    for _ in range(TESTS_PER_EXPERIMENT):
        subprocess.call(["generador-archivos", str(size), str(proportion), str(factor)])

        for algo in algos:
            with open(devnull, "w") as fnull:
                executable = ALGO_PATH + algo
                if isStats: executable += "-stats"
                file1 = FILES_PATH + "arxiu1"
                file2 = FILES_PATH + "arxiu2"
                output = OUTPUT_PATH + algo + "/data.json"

                subprocess.call([executable, file1, file2, output], stdout = fnull, stderr = fnull)

            with open(output) as data_json: data = json.load(data_json)

            data_map[algo] += data[property]

    for algo in algos: data_map[algo] /= TESTS_PER_EXPERIMENT

    if variable != None:
        resultsfile.write("\n" + str(variable))
        for algo in algos: resultsfile.write("," + str(data_map[algo])) 
    else:
        first = True
        for algo in algos: 
            if first: 
                resultsfile.write("\n")
                first = False
            else: resultsfile.write(",")
            resultsfile.write(str(propertyfn(data_map[algo])))   


# Temps vs tamany
# Temps vs proporcio
# Temps vs factor

# Proporcio fixada, Tamany fixat, Factor fixat
# Mitjana cerca satisfactoria
# Mitjana cerca fallida

# Mitjana cerca
# Mitjana insercio

# Memoria utilitzada

# Iteracions (comparacions, crides a hash, etc.)

def initfile(path, initialColumn = ""):
    file = open(STATS_OUTPUT_PATH + path, "w+")
    file.write(initialColumn)
    if initialColumn != "":
        for algo in algos: file.write("," + algo) 
    else:
        first = True
        for algo in algos:
            if first: first = False
            else: file.write(",")
            file.write(algo)

    return file

# Tiempo VS Tamano

SIZES = [2e6, 4e6, 6e6, 8e6, 10e6, 12e6, 14e6]
SIZE_FIXED_PROPORTION = 0.5
SIZE_FIXED_TEXT_TO_DICT = 2.5

resultsfile = initfile("tiempo_vs_tamano.csv", "tamano")

for size in SIZES:
    calcular_prova(int(size), SIZE_FIXED_PROPORTION, SIZE_FIXED_TEXT_TO_DICT, 
                    False, int(size), "tiempo_total", resultsfile, lambda x : int(x))

print "Time vs Tamano computed"

# Tiempo VS Proporcion

PROPORTIONS = [0.1, 0.2, 0.4, 0.6, 0.8, 0.9, 1]
PROPORTION_FIXED_SIZE = 5e6
PROPORTION_FIXED_TEXT_TO_DICT = 2.5

resultsfile = initfile("tiempo_vs_proporcion.csv", "proporcion")

for proportion in PROPORTIONS:
    calcular_prova(int(PROPORTION_FIXED_SIZE), proportion, PROPORTION_FIXED_TEXT_TO_DICT,
                   False, proportion, "tiempo_total", resultsfile, lambda x : int(x))

print "Time vs Proportion computed"

# Tiempo VS Factor

FACTORS = [2, 2.5, 3, 3.5, 4, 4.5, 5]
FACTOR_FIXED_SIZE = 5e6
FACTOR_FIXED_PROPORTION = 0.5

resultsfile = initfile("tiempo_vs_factor.csv", "factor")

for factor in FACTORS:
    calcular_prova(int(FACTOR_FIXED_SIZE), FACTOR_FIXED_PROPORTION, factor,
        False, factor, "tiempo_total", resultsfile, lambda x : int(x))

print "Time vs Factor computed"

# Para proporcion, tamano y factor fijado, 
# tiempo medio de busqueda, tiempo medio de insercion, y memoria maxima usada

FIXED_PROPORTION = 0.5
FIXED_SIZE = 5e6
FIXED_TEXT_TO_DICT = 2.5

busquedafile = initfile("mediana_busqueda.csv")
insercionfile = initfile("mediana_insercion.csv")
memoriafile = initfile("memoria.csv")
statsfile = file = open(STATS_OUTPUT_PATH + "stats.txt", "w+")

busqueda_map = {}
insercion_map = {}
tamano_memoria_map = {}
iteraciones_map = {}
stats_map = {}

for algo in algos: 
    busqueda_map[algo] = insercion_map[algo] = tamano_memoria_map[algo] = iteraciones_map = 0

for _ in range(TESTS_PER_EXPERIMENT):
    subprocess.call(["generador-archivos", str(FIXED_SIZE), str(FIXED_PROPORTION), str(FIXED_TEXT_TO_DICT)])

    for algo in algos:
        with open(devnull, "w") as fnull:
            executable = ALGO_PATH + algo
            executablestats = executable + "-stats"

            file1 = FILES_PATH + "arxiu1"
            file2 = FILES_PATH + "arxiu2"
            
            outputtiempo = OUTPUT_PATH + algo + "/tiempo.json"
            outputstats = OUTPUT_PATH + algo + "/stats.json"

            subprocess.call([executable, file1, file2, outputtiempo], stdout = fnull, stderr = fnull)
            subprocess.call([executablestats, file1, file2, outputstats], stdout = fnull, stderr = fnull)

        with open(outputtiempo) as tiempo_json: tiempo = json.load(tiempo_json)
        with open(outputstats) as stats_json: stats = json.load(stats_json)

        busqueda_map[algo] += tiempo["tiempo_busqueda"]
        insercion_map[algo] += tiempo["tiempo_insercion"]
        tamano_memoria_map[algo] += stats["memoria_maxima_kb"]

        if algo in stats_map:
            for attr, valor in stats.iteritems():
                stats_map[algo][attr] += valor
        else: stats_map[algo] = stats

for algo in algos:
    busqueda_map[algo] /= TESTS_PER_EXPERIMENT
    insercion_map[algo] /= TESTS_PER_EXPERIMENT
    tamano_memoria_map[algo] /= TESTS_PER_EXPERIMENT
    for key, value in stats_map[algo].iteritems():
        stats_map[algo][key] = value/TESTS_PER_EXPERIMENT

first = True
for algo in algos: 
    if first: 
        busquedafile.write("\n")
        insercionfile.write("\n")
        memoriafile.write("\n")
        first = False
    else: 
        busquedafile.write(",")
        insercionfile.write(",")
        memoriafile.write(",")
    
    busquedafile.write(str(busqueda_map[algo]/(FIXED_SIZE*FIXED_TEXT_TO_DICT)))
    insercionfile.write(str(insercion_map[algo]/FIXED_SIZE))   
    memoriafile.write(str(tamano_memoria_map[algo]))   

for algo in algos:
    stats_map[algo]["algoritmo"] = algo
    statsfile.write(json.dumps(stats_map[algo], indent=4, sort_keys=True))
    statsfile.write("\n\n")






