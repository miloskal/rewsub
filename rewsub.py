#!/usr/bin/python3

"""  
    Purpose of this program is a movie-subtitle synchronization.
  
    Copyright (C) 2020 Miloš Kaličanin <miloskal0108997@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
"""

from ctypes import *
from PyQt5.QtWidgets import *
from pathlib import Path

class MainWindow(QWidget):

    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.pathLineEdit = QLineEdit(self)
        self.pathLineEdit.setPlaceholderText("Input File")
        browseButton = QPushButton("Choose Input", self)
        self.rewindQuantityLineEdit = QLineEdit(self)
        self.rewindQuantityLineEdit.setPlaceholderText("How Much Miliseconds (e.g. -3500)")
        okButton = QPushButton("Rewind", self)
        
        layout = QVBoxLayout()
        layout.addWidget(self.pathLineEdit)
        layout.addWidget(browseButton)
        layout.addWidget(self.rewindQuantityLineEdit)
        layout.addWidget(okButton)

        self.setLayout(layout)
        self.resize(250,250)
        self.move(QApplication.desktop().screen().rect().center() - self.rect().center())
        self.move(self.x(), self.y() - 200)

        self.setWindowTitle("rewsub")

        browseButton.released.connect(self.onBrowseButtonReleased)
        okButton.released.connect(self.onOkButtonReleased)
        self.show()


    def onBrowseButtonReleased(self):
        dialog = QFileDialog(self)
        fileName, _ = dialog.getOpenFileName(self,"Choose Input File","",
        "All Files (*)", options=QFileDialog.Options() 
                    | QFileDialog.DontUseNativeDialog)
        if fileName.endswith(".srt") or fileName.endswith(".sub"):
            self.pathLineEdit.setText(fileName)
        else:
            msg = QMessageBox(self)
            msg.setIcon(QMessageBox.Critical)
            msg.setText("Error !")
            msg.setInformativeText("File must have '.srt' or '.sub' extension.")
            self.pathLineEdit.setText("")
            msg.exec_()

    def onOkButtonReleased(self):
        shared_lib = CDLL("./core.so")
        
        original_path = Path(self.pathLineEdit.text())
        path_to_input = c_char_p(bytes(self.pathLineEdit.text(), encoding='utf-8'))
        how_much_to_rewind = c_longlong(int(self.rewindQuantityLineEdit.text()))
        path_to_output = c_char_p(bytes(f"{str(original_path.parents[0])}/"
                                        f"_{original_path.name}", encoding='utf-8'))

        rewind_sub = shared_lib.rewind_subtitle
        rewind_sub(path_to_input, how_much_to_rewind, path_to_output)
        msg = QMessageBox(self)
        msg.setIcon(QMessageBox.Information)
        msg.setText("Success !")
        msg.setInformativeText(f"File '_{original_path.name}' saved.")
        msg.setStandardButtons(QMessageBox.Ok)
        msg.exec_()


if __name__ == '__main__':
    app = QApplication([])
    gui = MainWindow()
    app.exec_()
