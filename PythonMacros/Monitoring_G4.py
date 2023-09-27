import os
import sys
import time

import numpy as np
import matplotlib.pyplot as plt

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
    

    
def SendMessage(message_text):
    chatID = '-1001921335158'
    apiURL = 'https://api.telegram.org/bot{}/sendMessage'.format(TOKEN_global)
    
    MAX_LENGTH = 4096
    
    Messages = []
    if len(message_text) > MAX_LENGTH:
        Messages = [message_text[i:i+MAX_LENGTH] for i in range(0, len(message_text), MAX_LENGTH)]
    else:
        Messages = [message_text]
     
    for message in Messages:    
        try:
            response = requests.post(apiURL, json={'chat_id': chatID, 'text': message, 'parse_mode': 'HTML'})
            print(response.text)
        except Exception as e:
            print(e)



def SendPhoto(photo_path):
    chatID = '-1001921335158'        
    apiURL = 'https://api.telegram.org/bot{}/sendPhoto'.format(TOKEN_global)
    print(apiURL)
    
    try:
        params = {'chat_id': chatID}
        files = {'photo': open(photo_path, 'rb')}
        response = requests.post(apiURL, params=params, files=files)
        print(response.text)
    except Exception as e:
        print(e)


def Monitoring(N_Jobs, N_EventsPerJob, N_Files ,Particles, DirectoryToMonitor):
    
    SendMessage("Starting Simulation")
    
    print('Entering in Monitoring')
    start_time = time.time()
    Checkpoint_Message = start_time
    Checkpoint_Plot = start_time
    Checkpoint_Sampling = start_time
    
    Checkpoint_LastChange = start_time
    LastTotalNumberEvents_monitored = 0
    
    Delta_T_Warning = 60.0 # seconds
    Delta_T_Countdown = 10.0 # seconds
    
    
    UpdateFreq_Message = 20.0 # seconds
    UpdateFreq_Plot = 120.0 # seconds
    UpdateFreq_Sampling = 1.0 # seconds
    
    
    Particles = Particles.split(',')
    TotNumberEvents_computed = N_Jobs*N_EventsPerJob*len(Particles)*N_Files
    print('TotNumberEvents_computed', TotNumberEvents_computed)
    print(Particles)
    
    TotNumberEvents_monitored = 0
    
    SubVector_Particles = [0 for i in range(len(Particles))]
    SubVector_Jobs = [SubVector_Particles for i in range(N_Jobs)]
    SubVector_Files = [SubVector_Jobs for i in range(N_Files)]
    
    t = np.array([])
    evts = np.array([])

    
    
    TotalNumberEvents_monitored = 0
    Total_Percentage = 0.0
    
    ContinuousDump = True 
    while TotalNumberEvents_monitored < TotNumberEvents_computed and ContinuousDump:
        MessageString = "" 
        for i in range(N_Files):
            if i > 0:
                MessageString += "\n\n"
            MessageString += "F"+str(i)+": "
            for j in range(N_Jobs):
                NameFile =  os.path.join(DirectoryToMonitor, 'monitoring_f'+str(i)+'_j'+str(j)+'.txt')
                NameLock =  os.path.join(DirectoryToMonitor, 'monitoring_f'+str(i)+'_j'+str(j)+'.lock')
                
                # check if .lock file exists else wait unitil it disappear
                while os.path.exists(NameLock):
                    pass
                    # wait 1 millisecond
                    #print('Waiting for lock file to disappear')
                if os.path.exists(NameFile):
                    f = open(NameFile, 'r')
                    for line in f:
                        line_split = line.split(' ')
                        particle_name = line_split[0]
                        EventNumber = int(line_split[1])
                        # Add the percentage to the string with no decimals
                        MessageString += ' ' + str(int(100 * EventNumber/N_EventsPerJob)) + particle_name[0]+'% '
                        
                        particle_index = 0
                        for k, particle in enumerate(Particles):
                            if particle_name in particle:
                                particle_index = k
                                break
                        SubVector_Files[i][j][particle_index] = EventNumber
                        
                        for index_dummy in range(particle_index):
                            SubVector_Files[i][j][index_dummy] = N_EventsPerJob
                    f.close()
                else:

                    MessageString += ' - '
                    SubVector_Files[i][j] = [0 for i in range(len(Particles))]
                        
        #print(SubVector_Files)
        
        # Sum all the elements in the SubVector_Files
        TotalNumberEvents_monitored = np.sum(SubVector_Files)
        Total_Percentage = 100*TotalNumberEvents_monitored/TotNumberEvents_computed
        
        if TotalNumberEvents_monitored > LastTotalNumberEvents_monitored:
            Checkpoint_LastChange = time.time()
            LastTotalNumberEvents_monitored = TotalNumberEvents_monitored


        if time.time() - Checkpoint_Sampling > UpdateFreq_Sampling:
            Checkpoint_Sampling = time.time()
            t = np.append(t, time.time() - start_time)
            evts = np.append(evts, TotalNumberEvents_monitored)
            #evts_avg = np.convolve(evts, np.ones(N_avg)/N_avg, mode='valid')


        if time.time() - Checkpoint_LastChange > Delta_T_Warning:
            Message = "<b>WARNING</b>\n"
            Message += "No events processed in the last " + str(Delta_T_Warning) + " seconds\n"
            Message += "\n"*5
            Message += "<b>STOPPING</b>\n"
            ContinuousDump = False
        
        if time.time() - Checkpoint_Message > UpdateFreq_Message:
            Checkpoint_Message = time.time()
            Message = "<b>Statistics</b>\n"
            Message += "<b>Events processed</b>\n"+str(TotalNumberEvents_monitored)+" / "+str(TotNumberEvents_computed)+"\n<b>.ooo00ooo..ooo00ooo.</b>\n"
            Message += MessageString + "\n"
            Message += "\n<b>.ooo00ooo..ooo00ooo.</b>\n"
            Message += "<b>Tot Percentage</b>: " + str(100*TotalNumberEvents_monitored/TotNumberEvents_computed) + "\n"
            Message += "<b>Time elapsed</b>: " + str(time.time() - start_time) + " seconds\n"
            SendMessage(Message)
            print(Message)

            
        if time.time() - Checkpoint_Plot > UpdateFreq_Plot:
            Checkpoint_Plot = time.time()
            plt.figure(figsize=(10, 6), dpi = 200)
            plt.plot(t, evts)
            plt.xlabel('Time [s]')
            plt.ylabel('Events processed')
            plt.title('Events processed over time')
            plt.grid()
            plt.savefig(os.path.join(DirectoryToMonitor,'Monitoring.png'))
            plt.close()

            # Now I want to define Delta_n = 10 and then plot evts[i+Delta_n] - evts[i] / t[i+Delta_n] - t[i] versus the time at the middle
            # Delta_n = 5
            # t_middle = np.array([])
            # evts_middle = np.array([])
            # for i in range(len(t)-Delta_n):
            #     t_middle = np.append(t_middle, t[i+int(Delta_n/2)])
            #     evts_middle = np.append(evts_middle, (evts_avg[i+Delta_n] - evts_avg[i]) / (t[i+Delta_n] - t[i]))
            
            # # Plot only the last 50 points only if there are more than 50 points
            # if len(t_middle) > 100:
            #     t_middle = t_middle[-100:]
            #     evts_middle = evts_middle[-100:]
            # else:
            #     t_middle = t_middle[:]
            #     evts_middle = evts_middle[:]


            # plt.figure(figsize=(10, 6), dpi = 200)
            # plt.plot(t_middle, evts_middle)
            # plt.xlabel('Time [s]')
            # plt.ylabel('Events processed per second')
            # plt.title('Events processed per second over time')
            # plt.grid()
            # plt.savefig(os.path.join(DirectoryToMonitor,'Monitoring_per_second.png'))
            # plt.close()
            
            SendPhoto(os.path.join(DirectoryToMonitor,'Monitoring.png'))
            
            
            
            
    SendMessage("Finished")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Monitoring')
    parser.add_argument('-n', '--n-jobs', type=int, required=True, help='Number of jobs')
    parser.add_argument('-e', '--n-events-per-job', type=int, required=True, help='Number of events per job')
    parser.add_argument('-f', '--n-files', type=int, required=True, help='Number of files')
    parser.add_argument('-p', '--particles',  required=True, help='Array of Particle names')
    parser.add_argument('-d', '--directory', type=str, required=True, help='Directory to monitor')
    args = parser.parse_args()
    
    
    print("Ciao! I'm monitoring")
    Monitoring(args.n_jobs, args.n_events_per_job, args.n_files,args.particles, args.directory)