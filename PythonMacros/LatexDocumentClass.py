import os
import re
import sys
import time
import shutil
import subprocess


sys.path.append("~rnicolai/PYTHON_PKG")

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
        self.AmIInTable = False
        self.Body = ""
        self.DocumentClass = "beamer"
        self.IsThisLongTable = False
        
        self.AllowedDocumentClasses = ["beamer", "article"]
    
    def SetNameOfTheDocument(self, name):
        self.NameOfTheDocument = name
            
        
    def SetDocumentClass(self, classInput):
        self.DocumentClass = str.lower(classInput)
        if not str.lower(classInput) in self.AllowedDocumentClasses:
            os.error()
        
        

    def SetEmail(self, email):
        self.Email = email

    def SetTitle(self, title):
        self.Title = title

    def SetAuthor(self, author):
        self.Author = author
    
    def SetOutputDirectory(self, outputDirectory):
        self.OutputDirectory = outputDirectory
    
    def BuildPreamble(self):
        Text = ""
        if self.DocumentClass == "beamer":        
            Text = r'''
                \documentclass[8pt]{beamer}
                \usepackage{beamerthemesplit}
                %\usepackage[orientation=portrait,size=custom,width=60,height=80, scale = 1.4]{beamerposter} 
                \geometry{papersize={16cm,22cm}}
                \usetheme{CambridgeUS} 
                \usepackage{textpos} 
                \usepackage[latin1]{inputenc} 
                \usepackage{amsmath} 
                \usepackage{mathtools} 
                \usepackage{color} 
                \usepackage{mathabx} 
                \usepackage{microtype} % Miglioramento dell'allineamento del testo
                \usepackage{ragged2e} % Miglioramento dell'allineamento del testo
                \usepackage{graphicx}
                \usepackage{longtable} 
                \usepackage{tikz} 
                \usepackage{esvect} 
                \usetikzlibrary{arrows,shapes} 
                \usecolortheme{beaver} 
                \usepackage{graphicx} 
                \usepackage{booktabs}
                \usepackage{changepage} 
                \setbeamertemplate{navigation symbols}{} 
                \setbeamertemplate{navigation symbols}{} 
            '''
        elif self.DocumentClass == "article":
            Text = r'''
                \documentclass[a3paper,7pt]{article}
                \usepackage[a3paper, margin=2cm]{geometry}
                \usepackage{microtype} % Miglioramento dell'allineamento del testo
                \usepackage{ragged2e} % Miglioramento dell'allineamento del testo
                \usepackage{longtable}


                \usepackage[english]{babel}
                \usepackage{graphicx}
                \usepackage{booktabs}
                \usepackage[hidelinks,backref]{hyperref}
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
    
    def InsertClearPage(self):
        Text = r'''
        \clearpage
        '''
        self.Body += Text
        return Text
    
    
    def BuildTitlePage(self):
        Text = ""
        if self.DocumentClass == "beamer":
            Text = r'''
            \begin{frame}
                \titlepage
            \end{frame}
            '''
        elif self.DocumentClass == "article":
            Text = r'''
            \maketitle
            '''
        return Text 

    def BuildEndDocument(self):
        Text = r'''
        \end{document}
        '''
        return Text


    # Dummy Slide
    def BuildDummySlide(self):
        Text = ""
        if self.DocumentClass == "beamer":
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
        Text = ""
        if self.DocumentClass == "beamer":
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
        Text = ""
        if self.DocumentClass == "beamer":
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
        print("Compiling Latex Document")
        print(os.getcwd())
        os.system("pdflatex "+self.NameOfTheDocument+".tex")
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
        
        
        
    def RemoveUnderscore(self):
        # Find all the underscores. If _ is not preceeded by a \, replace it with \_
        # This is to avoid the latex compiler to interpret the underscore as a subscript
        pattern = r'(?<!\\)_'
        substitution = r'\\_'
        results = re.sub(pattern, substitution, self.LatexDocument)
        self.LatexDocument = results
        

    def Compile(self):
        self.BuildLatexDocument()
        self.RemoveUnderscore()
        self.WriteLatexDocument()
        self.CompileLatexDocument()
        self.CleanFromGarbage()



    def BeginTable(self, Header, IsLongTable = False):
        self.IsThisLongTable = IsLongTable
        
        self.AmIInTable = True
        
        TableFormatting = ""
        
        for i in range(len(Header)):
            TableFormatting += r'''p{2 cm}'''

        Text = ""
        
        if IsLongTable:
            Text = r'''
            \begin{longtable}{'''+ TableFormatting +r'''}
            '''   
        else:
            Text = r'''
            \begin{table}
            \begin{tabular}{''' + len(Header)*"l" + '''}
            '''
        
        
        TextHeader = ""
        for index, elements in enumerate(Header):
            if index == (len(Header)-1):
                TextHeader += str(elements) + r'''\\
                    '''
            else:
                TextHeader += str(elements) + r'''&'''
        
        
        
        if IsLongTable:
            Text+=r'''
            \toprule
            '''
                
            Text += TextHeader + r'''
            \midrule
            \endfirsthead
            \toprule
            '''
            
            Text += TextHeader + r'''
            \midrule
            \endhead
            '''
        else: 
            Text += TextHeader + r'''
            \midrule
            '''
             
        self.Body += Text    
        return Text
        
        
    def AddRow(self, Line):
        Text = ""
        if self.AmIInTable == True:
            for index, elements in enumerate(Line):
                if index ==(len(Line)-1):
                    Text += str(elements) + r'''\\
                        '''
                else:
                    Text += str(elements) + r'''&'''
            self.Body += Text
        else:
            os.error()
        
        
    def EndTable(self):
        self.AmIInTable = False

        if self.IsThisLongTable:
            Text = r'''
            \bottomrule
            \end{longtable}
            '''
        else:
            Text = r'''
            \bottomrule
            \end{tabular}
            \end{table}
            '''    
        
        self.Body += Text        
        return Text
    
    def AddSection(self, title):
        Text = r'''
        \section{''' + title + r'''}
        '''
        self.Body += Text
        return Text
    
    
    def AddTwoFigures(self, Path1,Path2,Caption1,Caption2,Width):
        Text = r'''
        \begin{columns}[b]
        \begin{column}{.5\textwidth}
        \begin{figure}
        \centering
        \includegraphics[width='''+ str(Width) +r'''\textwidth]{'''+ str(Path1) +r'''}
        \caption{'''+ str(Caption1) +r'''}
        \end{figure}      
        \end{column}
        % create the column with the second image, that also
        % occupies half of the slide
        \begin{column}{.5\textwidth}
        \begin{figure}
        \centering
        \includegraphics[width='''+ str(Width) +r'''\textwidth]{'''+ str(Path2) +r'''}
        \caption{'''+ str(Caption2) +r'''}
        \end{figure}
        \end{column}
        \end{columns}
        '''
    
        self.Body += Text    
        return Text
        
    

