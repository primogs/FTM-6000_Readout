// Copyright 2014-2016, Kurt Zeilenga. All rights reserved.
//
// Permission to use, copy, modify, and/or distribute this software
// for any purpose with or without fee is hereby granted, provided
// that the above copyright notice and this permission notice appear
// in all copies.
//
// Provided "as is" without warranty of any kind.

// source: https://github.com/kz7zzz/ftm-400d

#ifndef __FTM_H__
#define __FTM_H__

#include <string>
#include <cstring>

class Channel
{
public:
	Channel()
	{
		Clear();
	}
	
	void Clear()
	{
		bank=0;
		slot=0;
		band=0;
		rx=0;
		tx=0;
		duplex=0;
		offset=0;
		sql=0;
		tone=0;
		dcs=0;
		mode=0;
		power=0;
		scan=0;
		clk=0;
		step=0;
		user=0;
		active=0;
		tone_app=0;
	
		memset(tag,'\0',sizeof(tag));
	}
	
	std::string memname;
	int bank;
	int slot;

	int band;
	int rx;		// in khz
	int tx;		// in khz
	int duplex; // 0, -1, +1
	int offset; // in khz
	int sql;
	int tone;
	int dcs;
	int mode;
	int power;
	char tag[7];
	int scan;
	int clk;
	int step;
	int user;
	int active;
	int tone_app;
};

extern const char * scans[];
extern const char * modes[];
extern const char * powers[];
extern const char * sqls[];
extern const char * tones[];
extern const char * dcsCodes[];
extern const char * toneModes[];
extern const char * bands[];
extern const char * pchannels[];
extern const char * steps[];

#endif
