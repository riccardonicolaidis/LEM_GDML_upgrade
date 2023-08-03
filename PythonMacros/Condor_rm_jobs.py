import os
import sys
import re

def Condor_rm_jobs():
    """Remove all jobs from the Condor queue."""
    # Get the current user's name.
    user = "rnicolai"
    StringBeforeID = user + " ID: "
    # Get lines from command "condor_q "
    # rnicolai ID: 1902     7/14 00:45      _      _      1      1 1902.0
    
    Output = os.popen("condor_q " + user).readlines()
    
    for line in Output:
        if StringBeforeID in line:
            # Get the job ID.
            JobID = line.split(StringBeforeID)[1].split()[0]
            # Remove the job.
            os.system("condor_rm " + JobID)


        
if __name__ == '__main__':
    Condor_rm_jobs()