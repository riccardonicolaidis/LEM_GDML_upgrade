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
    df.columns = ['ID','X', 'Y', 'Z', 'E']
    
    # Exclude all the points where E == 0
    df = df[df['E'] != 0]
    
    
    
    # Take numpy arrays
    X = df['X'].values
    Y = df['Y'].values
    Z = df['Z'].values
    E = df['E'].values
    ID = df['ID'].values
    
    MaxID =  500
    
    # ID may not be consecutive
    # Build an ID2 that is consecutive
    
    ID2 = np.zeros(len(ID))
    
    currentID = 0
    for i in range(len(ID)):
        if i == 0:
            ID2[i] = currentID
        else:
            if ID[i] != ID[i-1]:
                currentID += 1
            ID2[i] = currentID
            
    # Add a new column to the dataframe
    df['ID2'] = ID2
    
            
    # reduce the sample
    df2 = df[df['ID2'] < MaxID]

    
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
    
    
    fig2 = px.scatter_3d(df2, x='X', y='Y', z='Z', color='ID2', opacity=0.7, width=1400, height=1000, range_color=[0, MaxID])
    fig2.update_traces(marker_size=1.5)
    fig2.write_html(fName.replace('.csv', '_ID.html'))
    
    