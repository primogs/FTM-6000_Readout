 //    This file is part of FTM6000-Serial.
 //
 //    Copyright (C) 2023 Matthias Hund
 //    
 //    This program is free software: you can redistribute it and/or modify
 //    it under the terms of the GNU General Public License as published by
 //    the Free Software Foundation, either version 3 of the License, or
 //    (at your option) any later version.
 //    
 //    This program is distributed in the hope that it will be useful,
 //    but WITHOUT ANY WARRANTY; without even the implied warranty of
 //    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 //    GNU General Public License for more details.
 //    
 //    You should have received a copy of the GNU General Public License
 //    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
#include <iostream>
#include <fstream>
#include "Image.h"

std::string gFileName;
std::string gComPort;
	
void print_binary(unsigned int number)
{
    if (number >> 1) {
        print_binary(number >> 1);
    }
    putc((number & 1) ? '1' : '0', stdout);
}

inline bool FileExists (const std::string& name) 
{
    std::ifstream f(name.c_str());
    return f.good();
}

void AskForNewFileName()
{
	std::cout << "please enter file name: ";
	std::cin >> gFileName;
}

int GetUserInput()
{
	int action=0;
	std::cout << "image: " << gFileName << std::endl;
	std::cout << "device: " << gComPort << '\n' << std::endl;
	
	std::cout << "select an action:" << std::endl;
	std::cout << "\t1 read from FTM-6000" << std::endl;
	std::cout << "\t2 write to FTM-6000" << std::endl;
	std::cout << "\t3 load image from file" << std::endl;
	std::cout << "\t4 save image as file" << std::endl;
	std::cout << "\t5 print image content" << std::endl;
	std::cout << "\t6 exit program" << std::endl;
	std::cout << "enter number: ";
	std::cin >> action;
	std::cout << std::endl;
	return action;
}

int main(int argc, char* argv[]) 
{
	if(argc>1)
		gFileName = argv[1];
	if(argc>2)
		gComPort = argv[2];
	
	std::cout << "Welcome to FTM-6000_Readout." << std::endl;
	Image FTM6000Image;
	bool run = true;
	while(run)
	{
		switch(GetUserInput())
		{
			case 1:
			{
				const int res = FTM6000Image.ReadFromDevice(gComPort.c_str());
				if(res!=0)
				{
					if(res==-1)
					{
						std::cout << "can not open " << gComPort.c_str() << std::endl;
						std::cout << "please enter device: ";
						std::cin >> gComPort;
					}
					else
					{
						std::cout << "error during read occured!" << std::endl;
					}
				}
			}
			break;
			case 2:
				FTM6000Image.WriteToDevice(gComPort.c_str());
			break;
			case 3:
			{
				if(FTM6000Image.LoadFromFile(gFileName.c_str())<0)
				{
					std::cout << "can not open " << gFileName.c_str() << std::endl;
					AskForNewFileName();
				}
			}
			break;
			case 4:
			{
				while(gFileName.length()==0)
				{
					std::cout << "no file name specified" << std::endl;
					AskForNewFileName();
				}
				
				if(FileExists(gFileName.c_str()))
				{
					std::cout << "file already exist. do you want to override? (y/n)" << std::endl;
					char answer;
					std::cin >> answer ;
					std::cout << std::endl;
					if(answer != 'y' and answer != 'Y')
						AskForNewFileName();
				}
				
				FTM6000Image.SaveToFile(gFileName.c_str());
			}
			break;
			case 5:
				FTM6000Image.OutputContent();
			break;
			case 6:
				run=false;
			break;
		}
	}
	return 0;
};
