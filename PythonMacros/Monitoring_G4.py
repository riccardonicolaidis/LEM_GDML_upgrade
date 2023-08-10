import os
import sys
import time

import numpy as np

import argparse


import requests
import telegram

def GetToken(TokenFile):
    FilePath = os.path.dirname(os.path.abspath(__file__))
    if os.path.exists(os.path.join(FilePath,'token',TokenFile)):
        f_token = open(os.path.join(FilePath,'token',TokenFile), 'r')
        TOKEN = f_token.read()
        TOKEN = TOKEN.replace('\n', '')
        f_token.close()
        return TOKEN
    else:
        print('Token file does not exist')
        return None
    
    
TOKEN_global = GetToken('token.token')    
    

    
def SendMessage(message):
    chatID = '-1001921335158'
    apiURL = 'https://api.telegram.org/bot{}/sendMessage'.format(TOKEN_global)
    try:
        response = requests.post(apiURL, json={'chat_id': chatID, 'text': message})
        print(response.text)
    except Exception as e:
        print(e)


def Monitoring(N_Jobs, N_EventsPerJob, N_Files ,Particles, DirectoryToMonitor):
    
    print('Entering in Monitoring')
    Particles = Particles.split(',')
    TotNumberEvents_computed = N_Jobs*N_EventsPerJob*len(Particles)*N_Files
    print('TotNumberEvents_computed', TotNumberEvents_computed)
    print(Particles)
    
    TotNumberEvents_monitored = 0
    
    SubVector_Particles = [0 for i in range(len(Particles))]
    SubVector_Jobs = [SubVector_Particles for i in range(N_Jobs)]
    SubVector_Files = [SubVector_Jobs for i in range(N_Files)]
    
    
    Total_Percentage = 0.0
    while Total_Percentage < (100.0-1e-6):
        print('TotNumberEvents_monitored', TotNumberEvents_monitored)
        for i in range(N_Files):
            for j in range(N_Jobs):
                NameFile =  os.path.join(DirectoryToMonitor, 'monitoring_f'+str(i)+'_j'+str(j)+'.txt')
                NameLock =  os.path.join(DirectoryToMonitor, 'monitoring_f'+str(i)+'_j'+str(j)+'.lock')
                # check if .lock file exists else wait unitil it disappear
                while os.path.exists(NameLock):
                    # wait 1 millisecond
                    time.sleep(0.001)
                if os.path.exists(NameFile):
                    f = open(NameFile, 'r')
                    for line in f:
                        line_split = line.split(' ')
                        particle_name = line_split[0]
                        EventNumber = int(line_split[1])
                        # Find the index of the particle in the Particles vector of strings
                        # use enumerate
                        particle_index = 0
                        for k, particle in enumerate(Particles):
                            if particle_name in particle:
                                particle_index = k
                                break
                        SubVector_Files[i][j][particle_index] = EventNumber
                        for k in range(particle_index):
                            SubVector_Files[i][j][k] = N_EventsPerJob
    
        # Sum all the elements in the SubVector_Files
        TotalNumberEvents_monitored = np.sum(SubVector_Files)
        Total_Percentage = 100*TotalNumberEvents_monitored/TotNumberEvents_computed
        Message = "Events processed "+str(TotalNumberEvents_monitored)+" / "+str(TotNumberEvents_computed)+"\n\n"
        
        for i in range(N_Files):
            for j in range(N_Jobs):
                Message += "File "+str(i)+" Job "+str(j)+"\n"
                for k in range(len(Particles)):
                    Message += "\t" + Particles[k]+" \t"+str(SubVector_Files[i][j][k])+" : \t"+ str(100*SubVector_Files[i][j][k]/N_EventsPerJob)+"\n"
                Message += "\n"
            Message += "\n"
        
        Message += "\n"
        Message += "Tot Percentage: " + str(100*TotalNumberEvents_monitored/TotNumberEvents_computed)
        
        SendMessage(Message)
        print(Message)
        print(TOKEN_global)
        # Sleep 5 seconds
        time.sleep(20)

    SendMessage("Finished")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Monitoring')
    parser.add_argument('-n', '--n-jobs', type=int, required=True, help='Number of jobs')
    parser.add_argument('-e', '--n-events-per-job', type=int, required=True, help='Number of events per job')
    parser.add_argument('-f', '--n-files', type=int, required=True, help='Number of files')
    parser.add_argument('-p', '--particles',  required=True, help='Array of Particle names')
    parser.add_argument('-d', '--directory', type=str, required=True, help='Directory to monitor')
    args = parser.parse_args()
    
    Monitoring(args.n_jobs, args.n_events_per_job, args.n_files,args.particles, args.directory)