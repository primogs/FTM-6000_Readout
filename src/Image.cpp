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


 
#include "Image.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <time.h>
#include <cstring>
#include <fcntl.h> 
#include <errno.h> 
#include <termios.h> 
#include <unistd.h> 
#include <sys/ioctl.h>

#define MINIMAGESIZE (0x180D0)

#define BLOCKSIZE (0x83)
#define PAYLOADSIZE (0x80)
#define BUFFERSIZE (0x400)


#define ADDR_HEADER (0x00)
#define ADDR_IMAGE (0xD0)
#define ADDR_TAIL  (0x180D0)

size_t CHANNEL_SIZE = 0x10;
size_t TAG_SIZE     = 0x10;
size_t PMS_SIZE     = 0x10;
size_t BT_NAME_SIZE = 0x10;
size_t BT_ADDR_SIZE = 0xC;

size_t VFO_FLAGS_OFFSET 		= 0x1B3;
size_t CHANNEL_FLAGS_OFFSET 	= 0x1BA;
size_t VFO_TOP_OFFSET 			= 0x1D0; // 0x220 
size_t HOME_TOP_OFFSET 			= 0x280; // 0x290
size_t HOME_TOP_TAG_OFFSET 		= 0x560;
size_t CHANNEL_TOP_OFFSET 		= 0x8D0; // 0x4740
size_t PMS_TOP_OFFSET 			= 0x4740; // 0x4800
size_t BT_NAME_OFFSET 			= 0x74D0;
size_t BT_ADDR_OFFSET 			= 0x74E0;
size_t CHANNEL_TOP_TAG_OFFSET 	= 0x100D0;
size_t PMS_TOP_TAG_OFFSET 		= 0x13F40;
size_t CHANNEL_COMMENT_OFFSET 	= 0x180E0;

Image::Image():mImage(nullptr),mImageSize(0)
{
}

Image::~Image()
{
	if( mImage != nullptr)
	{
		delete[] mImage;
		mImage = nullptr;
		mImageSize = 0;
	}
}

int Image::OutputContent()
{
	if(mImage==nullptr)
		return -1;
		
	std::cout << "MEMORY channels:" << std::endl;
	bool useTags = ((mImage[CHANNEL_FLAGS_OFFSET]&0x20U)==0);
	if(useTags)
		std::cout << "                                           active" << std::endl;
	std::cout << "ch  |band|rx      |tx      |duplex|offset|mode|tag   |tMode   |CTCSS   |TONE|pwr |scan|user tone|step    |clk shift" << std::endl;
	
	Channel ch;
	for(size_t i=0;i<999;i++)
	{
		
		const uint8_t * d = &mImage[CHANNEL_TOP_OFFSET + (i * CHANNEL_SIZE)];
		const uint8_t * s = &mImage[CHANNEL_TOP_TAG_OFFSET + (i * TAG_SIZE)];
		ch.Clear();
		if(DecodeFTM6000Channel(&ch, d, s))
		{
			PrintChannel(i,&ch);
		}
	}
	
	std::cout << "\nPMS channels:"<< std::endl;
	for(size_t i=0;i<50;i++)
	{
		const uint8_t * d = &mImage[PMS_TOP_OFFSET + (i * CHANNEL_SIZE)];
		const uint8_t * s = &mImage[PMS_TOP_TAG_OFFSET + (i * TAG_SIZE)];
		ch.Clear();
		if(DecodeFTM6000Channel(&ch, d, s))
		{
			PrintChannel(i,&ch);
		}
	}
	
	std::cout << "\nVFO channels:"<< std::endl;
	const bool VfoAutoMode = (mImage[VFO_FLAGS_OFFSET]&0x10u)==0;
	const bool VfoAutoStep = (mImage[VFO_FLAGS_OFFSET]&0x08u)==0;
	if(VfoAutoMode)
		std::cout << "                                       AUTO";
	if(VfoAutoStep)
		std::cout << "                                                    A_STEP";
	std::cout << std::endl;
	
	for(size_t i=0;i<5;i++)
	{
		const uint8_t * d = &mImage[VFO_TOP_OFFSET + (i * CHANNEL_SIZE)];
		const uint8_t * s = nullptr;
		ch.Clear();
		if(DecodeFTM6000Channel(&ch, d, s,true))
		{
			PrintChannel(i,&ch);
		}
	}
	
	std::cout << "\nHOME channel:"<< std::endl;
	const uint8_t * d = &mImage[HOME_TOP_OFFSET];
	const uint8_t * s = &mImage[HOME_TOP_TAG_OFFSET];
	ch.Clear();
	if(DecodeFTM6000Channel(&ch, d, s,true))
	{
		PrintChannel(0,&ch);
	}
	
	char bluetooth_name[BT_NAME_SIZE+1];
	char bluetooth_addr[18];
	
	for(size_t k=0;k<5;k++)
	{
		memset(bluetooth_name,'\0',sizeof(bluetooth_name));
		memset(bluetooth_addr,'\0',sizeof(bluetooth_addr));
		for(size_t i=0;i<BT_NAME_SIZE and mImage[BT_NAME_OFFSET+i+(k*(BT_NAME_SIZE+BT_ADDR_SIZE))]!=0xFF;++i)
			bluetooth_name[i]=mImage[BT_NAME_OFFSET+i+(k*(BT_NAME_SIZE+BT_ADDR_SIZE))];
			
		size_t j = 0;
		for(size_t i=0;i<BT_ADDR_SIZE and mImage[BT_ADDR_OFFSET+i+(k*(BT_NAME_SIZE+BT_ADDR_SIZE))]!=0xFF;++i)
		{
			if(i%2==0 and i>0 )
			{
				bluetooth_addr[i+j] = ':';
				j++;
			}
			bluetooth_addr[i+j] = mImage[BT_ADDR_OFFSET+i+(k*(BT_NAME_SIZE+BT_ADDR_SIZE))];
		}
		
		std::cout << "\n" << bluetooth_name << " " << bluetooth_addr << std::endl;
	}
	return 0;
}


int Image::WriteToDevice(const char*serialPortName)
{
	std::cout << "not yet implemented!" << std::endl;
	return 0;
}

int Image::ReadFromDevice(const char*serialPortName)
{
	// source https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/
	if(mImage==nullptr)
	{
		mImage = new (std::nothrow) uint8_t[MINIMAGESIZE];
		mImageSize = MINIMAGESIZE;
	}
	if(mImage==nullptr)
	{
		mImageSize=0;
		return -2;
	}
	
	if(serialPortName == nullptr)
	{
		return -1;
	}
	// Open the serial port. Change device path as needed 
	int serial_port = open(serialPortName, O_RDWR);
	
	if(serial_port < 0)
	{
		return -1;
	}
	
	// Create new termios struct for convention
	struct termios tty;
	
	// Read in existing settings
	if(tcgetattr(serial_port, &tty) != 0) 
	{
		std::cout << "error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
		return 1;
	}
	
	tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
	tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
	tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
	tty.c_cflag |= CS8; // 8 bits per byte (most common)
	tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
	tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
	
	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ECHO; // Disable echo
	tty.c_lflag &= ~ECHOE; // Disable erasure
	tty.c_lflag &= ~ECHONL; // Disable new-line echo
	tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
	
	tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
	tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
	
	tty.c_cc[VTIME] = 0;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
	tty.c_cc[VMIN] = 0;
	
	
	// Set in/out baud rate to be 38400
	cfsetispeed(&tty, B38400);
	cfsetospeed(&tty, B38400);
	
	// Save tty settings, also checking for error
	if (tcsetattr(serial_port, TCSANOW, &tty) != 0) 
	{
		std::cout << "error " << errno << " from tcsetattr: " << strerror(errno) << std::endl;
		return 1;
	}
	
	unsigned char block_buffer [BUFFERSIZE];
	char read_buf[BLOCKSIZE];
	
	unsigned int block_buffer_index = 0;
	struct timespec lastRecv;
	bool blockReceived = false;
	bool secondMemory = false;
	unsigned short lastMemAddr=0;
	
	std::cout << "Waiting for data from FTM 6000. Please, start TX!" << std::endl;
	
	while(! _kbhit())
	{
		// Read bytes.
		int num_bytes = read(serial_port, &read_buf, sizeof(read_buf));
		
		// num_bytes is the number of bytes read. -1  signals an error.
		if (num_bytes < 0) 
		{
			std::cout << "error reading: " << strerror(errno) << std::endl;
			return 1;
		}
		else if(num_bytes>0)
		{
			clock_gettime(CLOCK_MONOTONIC,&lastRecv);
			for(int i=0;i<num_bytes;i++)
			{
				block_buffer[block_buffer_index] = (unsigned char)read_buf[i];		
				block_buffer_index++;
				if(block_buffer_index >= BUFFERSIZE)	// we received more than 131 byte (one block)
				{
					block_buffer_index = 0;
					std::cout << "error: buffer overflow" << std::endl;
				}
			}	
		}
		else
		{
			struct timespec now;
			clock_gettime(CLOCK_MONOTONIC,&now);
			long int delta = DiffInMs(&now,&lastRecv);
			if(delta>2)	// if no data was received in last 2 ms, we assume block is complete
			{
				unsigned char* lpBlock = NULL;
				if(block_buffer_index>=BLOCKSIZE)	// is there rubbish at the beginning
				{
					lpBlock = &(block_buffer[block_buffer_index-BLOCKSIZE]);	// pointer to begin of block
					
					const unsigned short memAddr = (lpBlock[0] << 8) | lpBlock[1];	// extract memory addres that is stored in the first two bytes
					std::cout << "received block " << memAddr << std::endl;
					if(memAddr!=0 and memAddr!=lastMemAddr+PAYLOADSIZE)
						std::cout << "\texpected block "<< lastMemAddr+PAYLOADSIZE << std::endl;
					lastMemAddr = memAddr;
					if(blockReceived and (memAddr==0)) // memory addres is not unique and will overflow, so we have to check if second chunk is transmitted
						secondMemory=true;
					blockReceived=true;
					
					bool sumOk = (Sum(lpBlock,BLOCKSIZE-1) == lpBlock[BLOCKSIZE-1]);	// check if sum at end of block is okay
					if(sumOk)
					{
						uint32_t pos = ADDR_IMAGE+memAddr;	// calculate position within empty mImage
						if(secondMemory)
							pos += 0xFFFF;
						if((pos+(PAYLOADSIZE))<mImageSize)
						{
							memcpy(&(mImage[pos]),&(lpBlock[2]),PAYLOADSIZE);	// copy received block to mImage
						}
					}
					else
					{
						SerialSendNACK(serial_port);
						std::cout << "error sum" << std::endl;
					}
				}
				if(block_buffer_index>0)
				{
					SerialSendACK(serial_port);
					block_buffer_index = 0;
				}
				else if(blockReceived and delta>100)
				{
					std::cout << "finished" << std::endl;
					break;
				}
			}
		}
	}
	
	close(serial_port);
	return 0;
}

int Image::LoadFromFile(const char*filename)
{
	if(filename==nullptr or strlen(filename)==0)
		return -1;
	
	std::ifstream file;
	file.open(filename,std::ios::binary);
	if(file.is_open()==false)
		return -1;
	
	file.seekg (0, file.end);
    int fileSize = file.tellg();
    file.seekg (0, file.beg);
	if(mImage!=nullptr)
	{
		mImageSize = 0;
		delete[] mImage;
	}
	mImage = new (std::nothrow) uint8_t[fileSize];
	
	if(mImage!=nullptr)
		file.read(reinterpret_cast<char*>(mImage),fileSize);
	mImageSize = fileSize;
	file.close();
	return 0;
}

void Image::SaveToFile(const char*filename)
{
	if(mImage==nullptr)
		return;
		
	std::ofstream file;
	file.open(filename,std::ios::binary);
	file.write(reinterpret_cast<const char*>(mImage),mImageSize);
	file.close();
}

int Image::_kbhit() 
{
    static const int STDIN = 0;
    static bool initialized = false;
 
    if (! initialized) 
	{
        // turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }
 
    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}

void Image::SerialSendACK(int &serial_port)
{
	unsigned char ack[] = { 0x06 };
	write(serial_port, ack, sizeof(ack));
}

void Image::SerialSendNACK(int &serial_port)
{
	std::cout << "send NACK" << std::endl;
	unsigned char nack[] = { 0x15 };
	write(serial_port, nack, sizeof(nack));
}

unsigned char Image::Sum(unsigned char *data, size_t len)
{
    unsigned char res = 0;
    for (size_t i = 0; i < len; i++) 
	{
        res += data[i];
    }
    return res;
}

long int Image::DiffInMs(const struct timespec *time1,const struct timespec *time0) 
{
	struct timespec diff = {.tv_sec = time1->tv_sec - time0->tv_sec, .tv_nsec = time1->tv_nsec - time0->tv_nsec};
	if (diff.tv_nsec < 0) 
	{
		diff.tv_nsec += 1000000000; // nsec/sec
		diff.tv_sec--;
	}
	return (diff.tv_sec*1000)+(diff.tv_nsec/1000000);
}

bool Image::DecodeFTM6000Channel(Channel * chn,const unsigned char * c,const unsigned char * s,bool home)
{
	/* byte
	 * 0  : valid (b1000 0000)
	 *      scan  (b0110 0000)
	 *      band  (b0000 0111)
	 * 1  : mode  (b0111 0000)
	 *            (b0000 0000)
	 *      duplex(b0000 0111)
	 * 2  : rx    (b1110 1111)
	 * 3  : rx	  (b1111 1111)
	 * 4  : rx    (b1111 1111)
	 * 5  : sql   (b1111 0000)
	 *      step  (b0000 1111)
	 * 6  : tx    (b1110 1111)
	 * 7  : tx    (b1111 1111)
	 * 8  : tx    (b1111 1111)
	 * 9  : power (b1100 0000)
	 *            (b00X0 0000) active channel?
	 *      tone  (b0001 1111)
	 * 10 : clock (b1000 0000) 
	 *            (b0XX0 0000) tone?
	 *      dcs   (b0000 1111)
	 * 11 : usr ct(b0001 1111)
	 * 12 : 
	 * 13 : offset(b1111 1111)
	 * 14 :
	 * 15 :
	 */
		
	if (!home && !(c[0] & 0x80U))
		return false; /* not programmed */
	
	for(size_t i=0;s!=nullptr and i<6 and s[i]!=0xFF;++i)
		chn->tag[i]=s[i];
		
	chn->scan = (c[0] & 0x60U) >> 5;
	chn->band = (c[0] & 0x07U) + 1;
	chn->mode = (c[1] & 0x70U) >> 4;

	chn->rx += 100 * 100 * 1000 * (c[2] & 0x0fU);
	chn->rx += 100 * 10 * 1000 * ((c[3] & 0xf0U)>>4);
	chn->rx += 100 * 1000 * 		(c[3] & 0x0fU);
	chn->rx += 100 * 100 * 	   ((c[4] & 0xf0U)>>4);
	chn->rx += 100 * 10 *         (c[4] & 0x0fU);
	chn->rx += 500 *		((c[2] & 0x80U)>>7);
	chn->rx += 250 *		((c[2] & 0x40U)>>6);
	chn->rx += 125 *		((c[2] & 0x20U)>>5);

	switch (c[1] & 0x07U) 
	{
	case 0x02U: chn->duplex = -1; break;
	case 0x03U: chn->duplex = +1; break;
	case 0x04U: // separate transmit frequency
		chn->tx += 100 * 100 * 1000 * (c[6] & 0x0fU);
		chn->tx += 100 * 10 * 1000 * ((c[7] & 0xf0U)>>4);
		chn->tx += 100 * 1000 * (c[7] & 0x0fU);
		chn->tx += 100 * 100 * ((c[8] & 0xf0U)>>4);
		chn->tx += 100 * 10 * (c[8] & 0x0fU);
		chn->tx += 500 *		((c[6] & 0x80U)>>7);
		chn->tx += 250 *		((c[6] & 0x40U)>>6);
		chn->tx += 125 *		((c[6] & 0x20U)>>5);
	}
	
	chn->step = (c[5] & 0x0fU);
	chn->sql  = (c[5] & 0xf0U) >> 4;
	chn->tone = c[9] & 0x1fU; 
	
	chn->dcs = c[10] & 0x0fU;
	
	chn->clk = c[10]>>7; // clock shift
	
	chn->user = (c[11]& 0x1fU)*100;
	if (chn->duplex) 
	{
		chn->offset += c[13] * 50;
	}

	chn->power = (c[9] & 0xc0U) >> 6;
	
	chn->active  = (c[9]&0b00100000) != 0;
	chn->tone_app = (c[10]&0b01100000)>>5;

	return true;
}

void Image::PrintChannel(size_t i, Channel * chn)
{
	std::cout << std::setw(4) << i+1 << " "; 				// Channel No
	std::cout << std::setw(4) << bands[chn->band] << " ";
	std::cout << std::setw(8) << chn->rx << " ";				// Receive Frequency
	std::cout << std::setw(8) << chn->tx << " "; 			// Transmit Frequency
	std::cout << std::setw(6) << chn->duplex << " "; 		// Offset Direction
	std::cout << std::setw(6) << chn->offset << " ";			// Offset Frequency
	std::cout << std::setw(4) << modes[chn->mode] << " "; 	// Operating Mode
	std::cout << std::setw(6) << chn->tag << " ";			// Name
	std::cout << std::setw(8) << toneModes[chn->sql] << " ";	// Tone Mode
	std::cout << std::setw(5) << tones[chn->tone] << " Hz ";	// CTCSS Frequency
	std::cout << std::setw(4) << dcsCodes[chn->dcs] << " ";
	std::cout << std::setw(4) << powers[chn->power] << " ";	// Tx Power
	std::cout << std::setw(4) << scans[chn->scan] << " ";	// SCAN
	std::cout << std::setw(6) << chn->user << " Hz ";		// user CTCSS
	std::cout << std::setw(4) << steps[chn->step] << " kHz ";// step
	std::cout << chn->clk << " ";			// clock shift
	std::cout << chn->active << " ";
	std::cout << chn->tone_app << std::endl;
}