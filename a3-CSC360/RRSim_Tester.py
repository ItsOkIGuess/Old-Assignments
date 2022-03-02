import matplotlib.pyplot as plt
import os
import sys
import numpy as np

'''
Tests, and analyzes the output of rrsim.c
For CSC 360 Assignment 3
By Romney Duncan
V#00895553


Runs 480 Simulations on rrsim.c (please note that this will not only delete and past files created by rrsim,
but also will create 480 simulation output files) see readme for use instructions
only outputs one graph, so if you want to get both you have to go in here and mix up some things
pretty easy to make cmd line options for that, but I'm lazy
'''

def main():
	#used to have it so you could manually enter the seed, 
	#but apparently we needed 20 and I didn't feel like messing around with a RNG
	seed = [1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 1010, 1011, 1012, 1013, 1014, 1015, 1016, 1017, 1018, 1019]
	quantum = [50,100,250,500]
	dispatch = [0,5,10,15,20,25]
	tasks = -1
	filenum = 1
	styles = ['o','*','H','p']
	
	if(len(sys.argv) <4):
		print("Insufficient Arguments Exiting")
		return
		
	for i in range(0,len(sys.argv)):
		if(sys.argv[i] == "-seed"):
			beet = (sys.argv[i+1])
		if(sys.argv[i] == "-tasks"):
			tasks = sys.argv[i+1]
	#Deletes all previous saves		
	os.system("rm SimSave.txt")
	os.system("rm Sim_*")
	

	avgW = []
	avgTA = []
	waittimes = []
	turnaroundtimes = []
	label = "q=" + str(quantum[0])
	#runs and graphs the simulations just some nested loops for the seeds/quantums/dispatches
	for i in range(0,len(quantum)):
		
		if(len(avgW) >0):
			label = "q=" + str(quantum[i-1])
			plt.plot(dispatch,avgW, label = label, marker = styles[i])
			
		avgTA.clear()
		avgW.clear()
		waittimes.clear()
		turnaroundtimes.clear()
		
		for j in range(0,len(dispatch)):
			for k in range(0,len(seed)):
				cmd = "./simgen " + tasks + " " + str(seed[k]) + " | ./rrsim --quantum " + str(quantum[i]) + " --dispatch " + str(dispatch[j])
				os.system(cmd)
				filename = "Sim_" + str(filenum) + ".txt"
				f = open(filename, "r")
				data = f.read()
				data = data.split('\n')
				f.close()
				filenum = filenum + 1
				for line in data:
					line = line.split()
				
					if(len(line) == 5):
						waittimes.append(float(line[3][2:]))
						turnaroundtimes.append(float(line[4][3:]))
			avgW.append(np.mean((waittimes)))
			avgTA.append(np.mean(turnaroundtimes))
			
	#finallizing the graph
	label = "q=" + str(quantum[len(quantum) - 1])
	plt.title("Round Robin Scheduler -- Average Wait-- 1000 Tasks")
	plt.xlabel("Dispatch Overhead (Ticks)")
	plt.ylabel("Average Wait Time (Ticks)")
	plt.plot(dispatch,avgW, label = label, marker = 'p')
	plt.legend()
	plt.savefig("averageSleepTime")
#End of Main










































if __name__ == "__main__":
	main()