import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

import pandas as pd
import os
import sys
import time
import datetime
import shutil
from PIL import Image
#import tqdm

from tqdm import tqdm

import plotly.express as px

def PlotViolation(fName):
    df = pd.read_csv(fName, sep=',', header=None)
    df.columns = ['X', 'Y', 'Z', 'E']
    
    # Take numpy arrays
    X = df['X'].values
    Y = df['Y'].values
    Z = df['Z'].values
    E = df['E'].values

    
    # I wan a scatter plot with colorbar guided by the energy E
    # Figure is 3D and I want to see the scene from the polar vector (1,1,1)
    
    
    # Plot the figure high resolution
    plt.figure(figsize=(14,12), dpi=300)
    ax = plt.axes(projection='3d')
    ax.scatter(X, Y, Z, c=E, cmap='plasma', s=1.5)
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.view_init(azim=35, elev=60)
    # Retrieve the colorbar
    sm = plt.cm.ScalarMappable(cmap='plasma', norm=plt.Normalize(vmin=min(E), vmax=max(E)))
    sm._A = []
    plt.colorbar(sm)
    
    # Take the path and save the picture 
    figName = fName.replace('.csv', '.png')
    plt.savefig(figName)
    
    fig = px.scatter_3d(df, x='X', y='Y', z='Z', color='E', opacity=0.7, width=1400, height=1000, range_color=[min(E), max(E)])
    fig.update_traces(marker_size=1.5)
    fig.write_html(fName.replace('.csv', '.html'))
    
    
    # Make a clip with rotation of the figure
    # I want to rotate the figure 360 degrees in 10 seconds
    # I want to save the clip in the same folder of the picture

    # # Define the number of frames
    # nFrames = 10
    # # Define the time of the clip
    # clipTime = 4
    # # Define the time of each frame
    # frameTime = clipTime/nFrames
    # # Define the number of frames per second
    # fps = nFrames/clipTime
    
    # # make a subfolder with all the frames
    # folderName = fName.replace('.csv', '') + '_frames'
    
    # if not os.path.exists(folderName):
    #     os.makedirs(folderName)
    # else:
    #     shutil.rmtree(folderName)
    #     os.makedirs(folderName)
        
    # # Make the frames
    # # Use 

    # for i in tqdm(range(nFrames)):
    #     ax.view_init(azim=i, elev=(45 + 10*np.sin(3*i*np.pi/180)))
    #     figName = folderName + '/f' + str(i) + '.png'
    #     plt.savefig(figName)
    
    # # Make the gif with Pillow
    # # Make a list with all the frames
    # frames = []
    # for i in tqdm(range(nFrames)):
    #     figName = folderName + '/f' + str(i) + '.png'
    #     frames.append(Image.open(figName))
        
    # # Save the gif
    # gifName = fName.replace('.csv', '.gif')
    # frames[0].save(gifName, format='GIF', append_images=frames[1:], save_all=True, duration=frameTime*1000, loop=0)
    
    
    
    