/*
 * IpCameraStroe.h
 *
 *  Created on: 2019年3月7日
 *      Author: fsmdn121
 */

#ifndef IPCAMERASTROE_H_
#define IPCAMERASTROE_H_

#include <iostream>
#include <list>
using namespace std;
char IPDIRRECTDIR[128]="IPCamerastore.xml";
#define MAXIPSTORE_LINE (100)
typedef struct
{
	int value;
	double az;//fangwei
	double el;//fuyang

}Structipstore;

class IpCameraStroe
{
public:
	IpCameraStroe(char *dir);
	void Load();
	void AddPos(float az,float el);
	void AddPic();
	Structipstore GetStore(int idx);
	void Erase(int idx);
	void Save();
private:
	 FILE *filefp=NULL;
	  char *filename;
	  char buffer[MAXIPSTORE_LINE];
	list<Structipstore> IpCamearastore;
};







#endif /* IPCAMERASTROE_H_ */
