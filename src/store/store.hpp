/*
 * store.hpp
 *
 *  Created on: Jan 3, 2019
 *      Author: wj
 */

#ifndef STORE_HPP_
#define STORE_HPP_
#include <iostream>   
#include <list>   
#include <numeric>   
#include <algorithm> 
#include <osa_mutex.h>
typedef struct
{
	int value;
	double ptzpan;
	double ptztitle;
	
}storepam;
using namespace std;
#define MAXSTORE_LINE (100)
class Store{
	
	private:
		Store();
		
		~Store();
		
		static Store*instance;

		list<storepam> store;

		void addstore();
		
		void erasestore(int id);

		void update();

		void load();
		void save();
		void reload();

		void erase();

		void gostore(int num);

		void registorfun();
	private:
		 FILE *filefp=NULL;
		 char buffer[MAXSTORE_LINE];
		 OSA_MutexHndl filelock;
		
	public:
		static Store*getinstance();
		void create();

	private:
		static void Storemodevent(long lParam);
		

};



#endif /* STORE_HPP_ */
