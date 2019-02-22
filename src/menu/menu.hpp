/*
 * menu.hpp
 *
 *  Created on: 2018年11月9日
 *      Author: wj
 */

#ifndef MENU_HPP_
#define MENU_HPP_
#include "osd.hpp"
typedef void ( *CALLBACKFUNC)(void *context);
typedef struct
{
	int index;
	int fathreindex;
	int level;
	int levelfather;
	int levelson;
	int x;
	int y;
	int w;
	int h;
	int colour;
	int displayvalid;
	wchar_t context[odstextlen];
	int active;
	CALLBACKFUNC fun;
	CALLBACKFUNC renderfun;


}Submenu;


typedef struct
{
	int index;
	
	int x;
	int y;
	int w;
	int h;
	int colour;
	int displayvalid;
	int active;
	CALLBACKFUNC fun;


}Panelmenu;
#define SUBMENUMAX 100
#define PANELMAX 5
enum{
	NULLMOD,
	WORKMOD,
	SINGLESHOW,
	PANOSHOW,
	SELECTSHOW,
	ZEROSHOW,
	

	MENUMAX,

};

enum{
	NOACTIVE,
	ACTIVE,

};

enum{
	NULLLEVEL,
	FIRSTLEVEL,
	SECONDLEVEL,
	LEVELMAX,

};
void workmod(void *context);
void singlemod(void *context);
void panomod(void *context);
void selectmod(void *context);
void zeromod(void *context);
class MENU{

	

public:
	Submenu *submenu;
	Panelmenu *panelmenu;
	static MENU*getinstance();

	void menureset();
	void menushow();
	void setcallback(int menu,CALLBACKFUNC rendcall);
	void menuselect(int menu,int show);
private:
	static MENU*instance;
	MENU(){};
	~MENU(){};



};







#endif /* MENU_HPP_ */
