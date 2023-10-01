// Copyright 2014-2016, Kurt Zeilenga. All rights reserved.
//
// Permission to use, copy, modify, and/or distribute this software
// for any purpose with or without fee is hereby granted, provided
// that the above copyright notice and this permission notice appear
// in all copies.
//
// Provided "as is" without warranty of any kind.

// source: https://github.com/kz7zzz/ftm-400d

#include "ftm.h"


const char * sqls[] = {
	"noise", "T-TX", "T-TRX", "T-REV",
	"D-TRX", "program", "pager", "D-TX",
	"TT/DR", "DT/TR", NULL, NULL,
	NULL, NULL, NULL, NULL,
};

const char * scans[] = {"", "skip", "select", "unknown", NULL};


const char * tones[] = {
	"67.0", "69.3", "71.9", "74.4", "77.0", "79.7", "82.5", "85.4",
	"88.5", "91.5", "94.8", "97.4", "100.0", "103.5", "107.2", "110.9",

	"114.8", "118.8", "123.0", "127.3", "131.8", "136.5", "141.3", "146.2",
	"151.4", "156.7", "162.2", "167.9", "173.8", "179.9", "186.2", "192.8",

	"203.5", "206.5", "210.7", "218.1", "225.7", "229.1", "233.6", "241.8", 
	"250.3", "254.1"
};

const char * dcsCodes[] = {
	"023", "025", "026", "031", "032", "043", "047", "051",
	"054", "065", "071", "072", "073", "074", "114", "115",
	"116", "125", "131", "132", "134", "143", "152", "155",
	"156", "162", "165", "172", "174", "205", "223", "226",
	"243", "244", "245", "251", "261", "263", "265", "271",
	"306", "311", "315", "331", "343", "345", "351", "364",
	"365", "371", "411", "412", "413", "423", "431", "432",
	"445", "464", "465", "466", "503", "506", "516", "532",
	"546", "565", "606", "612", "624", "627", "631", "632",
	"654", "662", "664", "703", "712", "723", "731", "732",
	"734", "743", "754", NULL
};

const char * modes[] = {"FM", "AM", "NFM"};

const char * toneModes[] = {
   "OFF",
   "TONE",
   "TONE SQL",
   "REV TONE",
   "DCS",
   "PR FREQ",
   "PAGER"};

const char * powers[] = {"high", "med", "low", "unknown"};

const char * steps[] = {"5", "6.25", "8.33", "10", "12.5", "15", "20", "25", "50", "100"};

const char * bands[] = {"", "Air", "VHF", "RG1", "UHF", "RG2", "Unknown", "Unknown", "Unknown", NULL};

