/*
 * FileRW.hpp
 *
 *  Created on: 2018年10月23日
 *      Author: wj
 */

#ifndef FILERW_HPP_
#define FILERW_HPP_
#include <iostream>
#include <string>

#include "opencv2/core/core.hpp"
using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::ostream;
using namespace cv;

int  createfile();
void writefilehead();

void writefiletail();
void writefile(int time,short xspeed,short yspeed,short zspeed);
void destoryfile();
void writefiled(int time,double xspeed,double yspeed,double zspeed);

#endif /* FILERW_HPP_ */
