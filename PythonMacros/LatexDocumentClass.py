import os
import re
import sys
import time
import shutil
import subprocess


class LatexDocumentClass:
    def __init__(self):
        self.NameOfTheDocument = "Default Name of the Document"
        self.Title = "Default Title"
        self.Author = "Default Author"
        self.Email = "default.email@default.com"
        self.OutputDirectory = "./Output/"
        self.LatexDocument = ""
        self.AmIOnASlide = False
        self.AmIInAnItemize = False
        self.Body = ""
    
    def SetNameOfTheDocument(self, name):
        self.NameOfTheDocument = name

    def SetEmail(self, email):
        self.Email = email

    def SetTitle(self, title):
        self.Title = title

    def SetAuthor(self, author):
        self.Author = author
    
    def SetOutputDirectory(self, outputDirectory):
        self.OutputDirectory = outputDirectory
    
    def BuildPreamble(self):
        Text = r'''
            \documentclass[8pt]{beamer} 
            \usetheme{CambridgeUS} 
            \usepackage{textpos} 
            \usepackage[latin1]{inputenc} 
            \usepackage{amsmath} 
            \usepackage{mathtools} 
            \usepackage{color} 
            \usepackage{mathabx} 
            \usepackage{graphicx} 
            \usepackage{tikz} 
            \usepackage{esvect} 
            \usetikzlibrary{arrows,shapes} 
            \usecolortheme{beaver} 
            \usepackage{graphicx} 
            \usepackage{changepage} 
            \setbeamertemplate{navigation symbols}{} 
            \setbeamertemplate{navigation symbols}{} 
        '''

        return Text
    
    def BuildTitleSetting(self):
        Text = r'''
        \title{''' + self.Title + r'''}
        \author{''' + self.Author + r''' \footnote{''' + self.Email + r'''}}
        \date{\today}
        '''
        return Text


    def BuildBeginDocument(self):
        Text = r'''
        \begin{document}
        '''
        return Text
    
    def BuildTitlePage(self):
        Text = r'''
        \begin{frame}
            \titlepage
        \end{frame}
        '''
        return Text 

    def BuildEndDocument(self):
        Text = r'''
        \end{document}
        '''
        return Text


    # Dummy Slide
    def BuildDummySlide(self):
        Text = r'''
        \begin{frame}
            \frametitle{Dummy Slide}
            \begin{itemize}
                \item Item 1
                \item Item 2
                \item Item 3
            \end{itemize}
        \end{frame}
        '''
        self.Body += Text
        return Text
    # Begin Slide
    def BeginSlide(self, title):
        if self.AmIOnASlide:
            print("Error: You are already on a slide")
            sys.exit()
        
        self.AmIOnASlide = True
        Text = r'''
        \begin{frame}
            \frametitle{''' + title + r'''}
        '''
        self.Body += Text
        return Text
    
    # End Slide
    def EndSlide(self):
        Text = r'''
        \end{frame}
        '''
        self.AmIOnASlide = False
        self.Body += Text
        return Text
    
    # Begin Itemize
    def BeginItemize(self):
        if self.AmIInAnItemize:
            print("Error: You are already in an itemize")
            sys.exit()
        
        self.AmIInAnItemize = True
        Text = r'''
        \begin{itemize}
        '''
        self.Body += Text
        return Text
    
    # End Itemize
    def EndItemize(self):
        Text = r'''
        \end{itemize}
        '''
        self.AmIInAnItemize = False
        self.Body += Text
        return Text

    # Item
    def Item(self, text):
        if not self.AmIInAnItemize:
            print("Error: You are not in an itemize")
            sys.exit()
        
        Text = r'''
        \item ''' + text + r'''
        '''
        self.Body += Text
        return Text


    # Figure
     
    def InsertFigure(self, path, caption, width):
        width_str = str(width)
        Text = r'''
        \begin{figure}[h]
            \centering
            \includegraphics[width='''+ width_str + r'''\textwidth]{''' + path + r'''}
            \caption{''' + caption + r'''}
        \end{figure}
        '''
        self.Body += Text
        return Text




    def BuildLatexDocument(self):
        self.LatexDocument = self.BuildPreamble()
        self.LatexDocument += self.BuildTitleSetting()
        self.LatexDocument += self.BuildBeginDocument()
        self.LatexDocument += self.BuildTitlePage()
        self.LatexDocument += self.Body
        self.LatexDocument += self.BuildEndDocument()

    def WriteLatexDocument(self):
        if not os.path.exists(self.OutputDirectory):
            os.makedirs(self.OutputDirectory)
        with open(os.path.join(self.OutputDirectory,self.NameOfTheDocument + ".tex"), "w") as f:
            f.write(self.LatexDocument)

    def CompileLatexDocument(self):
        os.chdir(self.OutputDirectory)
        subprocess.call(["pdflatex", self.NameOfTheDocument + ".tex"])
        os.chdir("..")

    def CleanFromGarbage(self):
        os.chdir(self.OutputDirectory)
        subprocess.call(["rm", "-rf", self.NameOfTheDocument + ".aux"])
        subprocess.call(["rm", "-rf", self.NameOfTheDocument + ".log"])
        subprocess.call(["rm", "-rf", self.NameOfTheDocument + ".nav"])
        subprocess.call(["rm", "-rf", self.NameOfTheDocument + ".out"])
        subprocess.call(["rm", "-rf", self.NameOfTheDocument + ".snm"])
        subprocess.call(["rm", "-rf", self.NameOfTheDocument + ".toc"])
        os.chdir("..")

    def Compile(self):
        self.BuildLatexDocument()
        self.WriteLatexDocument()
        self.CompileLatexDocument()
        self.CleanFromGarbage()




