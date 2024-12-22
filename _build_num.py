#!/bin/python3

import datetime

d = datetime.datetime.now(tz=datetime.UTC)

with open("build_num.hpp", "w") as f:
    f.write(
f'''\
#ifndef WAYROUND_I2P_20241208_155643_188113
#define WAYROUND_I2P_20241208_155643_188113

namespace wayround_i2p::ccedit {{

const unsigned int ccedit_version[] = {{ 
{d.year},
{d.month},
{d.day},
{d.hour},
{d.minute},
{d.second}
}};

const std::string ccedit_version_string = 
	"{d.year}.{d.month}.{d.day}."
	"{d.hour}.{d.minute}.{d.second}"
	;

}}


#endif
'''
)  


