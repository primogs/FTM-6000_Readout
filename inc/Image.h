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
 
#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <stdint.h>
#include "ftm.h"

class Image
{
public:
	Image();
	~Image();
	
	int OutputContent();
	int ReadFromDevice(const char*serialPortName);
	int WriteToDevice(const char*serialPortName);
	int LoadFromFile(const char*filename);
	void SaveToFile(const char*filename);
private:
	bool DecodeFTM6000Channel(Channel * chn,const unsigned char * c,const unsigned char * s,bool home = false);
	void PrintChannel(size_t i, Channel * chn);
	
	int _kbhit();
	void SerialSendACK(int &serial_port);
	void SerialSendNACK(int &serial_port);
	unsigned char Sum(unsigned char *data, size_t len);
	long int DiffInMs(const struct timespec *time1,const struct timespec *time0);

	uint8_t *mImage;
	size_t mImageSize;
};

#endif // IMAGE_HPP
