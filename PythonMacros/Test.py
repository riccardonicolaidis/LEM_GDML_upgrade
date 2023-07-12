from LatexDocumentClass import LatexDocumentClass
import os
import re
import sys
import time
import shutil
import subprocess
import numpy as np
import matplotlib.pyplot as plt

def mandelbrot(c, max_iter):
    z = c
    for i in range(max_iter):
        if abs(z) > 2:
            return i
        z = z * z + c
    return max_iter+1



# Instantiation of the class
LatexDocument = LatexDocumentClass()

# Settings
LatexDocument.SetNameOfTheDocument("ExampleDocument")
LatexDocument.SetTitle("Example Document: How to use the LatexDocumentClass")
LatexDocument.SetAuthor("Riccardo Nicolaidis")
LatexDocument.SetEmail("mail@mail.mail")
LatexDocument.SetOutputDirectory("./OutputDirectory/")

# Body of the document
LatexDocument.BeginSlide("Introduction")
LatexDocument.Body += r'''
This is an example of how to use the LatexDocumentClass.

The LatexDocumentClass is a Python class that allows to create a Latex document in a very easy way.

It is built on top of the Latex Beamer class, and it is designed to create slides for presentations.

I created this class because it may be useful for managing output Report data from simulations, data analysis, etc.

You only need to do the following steps:
'''

LatexDocument.BeginItemize()
LatexDocument.Item(r"Import the \texttt{LatexDocumentClass} class from the \texttt{LatexDocumentClass.py} file")
LatexDocument.Item(r"Instantiate the class")
LatexDocument.Item(r"Set name, author, title, email, output directory")
LatexDocument.Item(r"Build the body of the document by adding the desired content")
LatexDocument.Item(r"Compile the document")
LatexDocument.EndItemize()
LatexDocument.EndSlide()



LatexDocument.BeginSlide("Commands")
LatexDocument.Body += r'''
The following commands are available:
'''

LatexDocument.BeginItemize()
LatexDocument.Item(r"\texttt{LatexDocument.BeginSlide(<Title of the slide>)}")
LatexDocument.Item(r"\texttt{LatexDocument.EndSlide()} (to be used at the end of the slide, if not the slide will not be closed and you'll get an error)")
LatexDocument.Item(r"\texttt{LatexDocument.BeginItemize()} (to be used at the beginning of an itemize environment)")
LatexDocument.Item(r"\texttt{LatexDocument.EndItemize()} (to be used at the end of an itemize environment, if not the itemize environment will not be closed and you'll get an error)")
LatexDocument.Item(r"\texttt{LatexDocument.Item(<Text of the item>)} (to be used inside an itemize environment, if not you'll get an error)")
LatexDocument.Item(r"\texttt{LatexDocument.Body} (to be used for adding text to the body of the document)")
LatexDocument.Item(r"\texttt{LatexDocument.InsertFigure(<Path of the image>, <Caption of the image>)} (to be used for adding a figure to the body of the document)")
LatexDocument.Item(r"\texttt{LatexDocument.Compile()} (For compiling and generating the PDF file)")

LatexDocument.EndItemize()
LatexDocument.EndSlide()


LatexDocument.BeginSlide("Example of a figure")

# Creation of the desired plots
# Dimensions of the fractal
width, height = 2600, 800
xmin, xmax = -1.5, -1.4
ymin, ymax = -0.05, 0.05
max_iter = 500

# Creazione di un array di coordinate complesse
x = np.linspace(xmin, xmax, width)
y = np.linspace(ymin, ymax, height)
X, Y = np.meshgrid(x, y)
Z = X + 1j * Y

# Applicazione della funzione del frattale di Mandelbrot
fractal = np.zeros(Z.shape, dtype=int)
for i in range(width):
    for j in range(height):
        fractal[j, i] = mandelbrot(Z[j, i], max_iter)

# Creazione del plot colorato del frattale di Mandelbrot
plt.figure(figsize=(10, 5))
plt.imshow(np.log10(fractal), extent=(xmin, xmax, ymin, ymax), cmap='hot', aspect = 'auto')
plt.colorbar()
plt.title("Mandelbrot Set")
plt.xlabel("Re(c)")
plt.ylabel("Im(c)")

img_name = "Test.png"
# Global path 
img_dir = os.getcwd() + "/Images/"

if not os.path.exists(img_dir):
    os.makedirs(img_dir)

plt.savefig(img_dir + "Test.png")




LatexDocument.InsertFigure(img_dir + "Test.png", "A beautiful Mandelbrot Set", 1.)
LatexDocument.EndSlide()


LatexDocument.BeginSlide("Another interesting plot")

import numpy as np
import matplotlib.pyplot as plt

def hough_transform(points, angle_res=0.5, dist_res=0.5):
    max_dist = int(np.ceil(np.sqrt(np.square(points[:, 0].max()) + np.square(points[:, 1].max()))))
    num_thetas = int(180 / angle_res)
    thetas = np.deg2rad(np.arange(0, 180, angle_res))
    rhos = np.arange(-max_dist, max_dist + 1, dist_res)
    accumulator = np.zeros((len(rhos), len(thetas)), dtype=np.uint8)
    
    sin_thetas = np.sin(thetas)
    cos_thetas = np.cos(thetas)
    
    for x, y in points:
        for theta_idx in range(len(thetas)):
            rho = int(x * cos_thetas[theta_idx] + y * sin_thetas[theta_idx])
            rho_idx = np.abs(rhos - rho).argmin()
            accumulator[rho_idx, theta_idx] += 1
    
    return accumulator, rhos, thetas

def plot_hough_transform(accumulator, rhos, thetas):
    plt.figure(figsize=(10, 6))
    plt.imshow(accumulator, cmap='hot', extent=[np.rad2deg(thetas.min()), np.rad2deg(thetas.max()), rhos.min(), rhos.max()], aspect='auto')
    plt.xlabel('Theta (degrees)')
    plt.ylabel('Rho')
    plt.title('Hough Transform')
    plt.colorbar()
    #plt.show()

# Esempio di utilizzo
# Genera alcuni punti in linea
# Genera tanti punti centrati intorno a (10, 10) con deviazione standard 1
points0 = np.random.randn(1000, 2)*0.5 + np.array([10, 10])
points1 = np.random.randn(1000, 2)*0.5 + np.array([20, 20])
points2 = np.random.randn(1000, 2)*0.5 + np.array([30, 30])
points3 = np.random.randn(1000, 2)*0.5 + np.array([50, 50])


#Unisci i punti
points = np.vstack([points0, points1, points2, points3])

# Esegui la trasformata di Hough
accumulator, rhos, thetas = hough_transform(points)

# Visualizza il risultato
plot_hough_transform(accumulator, rhos, thetas)

plt.savefig(img_dir + "Test2.png")

LatexDocument.InsertFigure(img_dir + "Test2.png", "A beautiful Hough Transform", 1.)
LatexDocument.EndSlide()




LatexDocument.BeginSlide("End of the document")
LatexDocument.Body += r'''
This is the end of the document.
'''
LatexDocument.EndSlide()



LatexDocument.Compile()
