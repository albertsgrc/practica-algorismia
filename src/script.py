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

algos = [ splitext(basename(f))[0] for f in listdir(ALGO_PATH) if f.endswith(".cc") and isfile(join(ALGO_PATH, f)) ]

MIN_SIZE = 5
MAX_SIZE = 15
POWER = 1e6

TESTS_PER_EXPERIMENT = 5

PROPORTION = 0.5
TEXT_TO_DICT = 2

subprocess.call("make")

for algo in algos:
    directory = OUTPUT_PATH + algo + "/"
    if not path.exists(directory): os.makedirs(directory)

if not path.exists(STATS_OUTPUT_PATH): os.makedirs(STATS_OUTPUT_PATH)

resultsfile = open(STATS_OUTPUT_PATH + "results_time.csv", "w+") 

resultsfile.write("tamano")
for algo in algos: resultsfile.write("," + algo) 

TOTAL_ITERATIONS = (MAX_SIZE - MIN_SIZE + 1)*len(algos)*TESTS_PER_EXPERIMENT
iters = 0

def write_percentage(perc):
    equals = int(perc*20)
    perc = int(100*perc)
    sys.stdout.write('\r')
    sys.stdout.write("[%-20s] %d%%" % ('='*equals, perc))
    sys.stdout.flush()

for sizewopower in xrange(MIN_SIZE, MAX_SIZE + 1):
    size = POWER*sizewopower
    times = {}

    for algo in algos: times[algo] = 0

    for _ in range(TESTS_PER_EXPERIMENT):
        subprocess.call(["generador-archivos", str(size), str(PROPORTION), str(TEXT_TO_DICT)])

        for algo in algos:
            write_percentage(iters/float(TOTAL_ITERATIONS))
            iters += 1

            with open(devnull, "w") as fnull:
                executable = ALGO_PATH + algo
                file1 = FILES_PATH + "arxiu1"
                file2 = FILES_PATH + "arxiu2"
                output = OUTPUT_PATH + algo + "/tiempo.json"

                subprocess.call([executable, file1, file2, output], stdout = fnull, stderr = fnull)

            with open(output) as tiempo_json: 
                tiempo = json.load(tiempo_json)

            times[algo] += tiempo["tiempo_total"]

    for algo in algos: times[algo] /= TESTS_PER_EXPERIMENT

    resultsfile.write("\n" + str(int(size)))
    for algo in algos: resultsfile.write("," + str(times[algo]))


resultsfile.close()






