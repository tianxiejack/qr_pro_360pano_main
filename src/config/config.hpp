/*
 * #
 *
 *  Created on: 2018年11月6日
 *      Author: wj
 */

#ifndef __CONFIG_HPP_
#define __CONFIG_HPP_
class Config{

	public:
	/*camera */
     int cam_width;
     int cam_height;
     int cam_channel;
     double cam_fov;

     double cam_fx; 
     double cam_fy; 
     double cam_ox;
     double cam_oy;
     double cam_k1;
     double cam_k2;
     double cam_k3;
     double cam_k4;

     int cam_readfromfile;
     int cam_fixcamereafov;


    int camsource;

     inline int getcamsource(){return camsource;};
     void setcamsource(int flag){camsource=flag;};
     inline int getcam_fixcamereafov(){return cam_fixcamereafov;};
     inline int getcam_readfromfile(){return cam_readfromfile;};
     void setcam_readfromfile(int flag){cam_readfromfile=flag;};

     inline int getcamwidth(){return cam_width;};
     inline int getcamheight(){return cam_height;};
     inline int getcamchannel(){return cam_channel;};
     inline double getcamfx(){return cam_fx;};

     void setcamfx(int fx){ cam_fx=fx;};

    /*display*/
	 int display_width;
	 int display_height;
	 int display_channel;
	 inline int getdiswidth(){return display_width;};
	 inline  int getdisheight(){return display_height;};
	 inline int getdischannel(){return display_channel;};


	 /*pano*/
	 double panozeroptz;
	 double panozeroptztitle;
	 int panoprocesswidth;
	 int panoprocessheight;
	 int panocylinder;
	 int panofusion;

	 double angleinterval;
	 int panocalibration;
	 
	 int panoprocessshift;
	 int panoprocesstailcut;


	 inline int getpanocalibration(){return panocalibration;};
	 inline int getpanoprocessshift(){return panoprocessshift;};
	 void setpanoprocessshift(int shift){panoprocessshift=shift;}

	  inline int getpanoprocesstailcut(){return panoprocesstailcut;};
	 void setpanoprocesstailcut(int shift){panoprocesstailcut=shift;}

	 inline int  getpanocylinder(){return panocylinder;};
	 inline int  getpanofusion(){return panofusion;};
	 inline double  getangleinterval(){return angleinterval;};
	 
	 inline double getpanozeroptz(){return panozeroptz;};
	 void setpanozeroptz(double angle){panozeroptz=angle;}


	  inline double getpanozeroptztitle(){return panozeroptztitle;};
	 void setpanozeroptztitle(double angle){panozeroptztitle=angle;}

	 inline int getpanoprocesswidth(){return panoprocesswidth;};
	 inline  int getpanoprocessheight(){return panoprocessheight;};

	 /*mvdetect*/
	 int mvprocesswidth;
	 int mvprocessheight;
	 int mvdownup;
	 int minarea;
	 int maxarea;
	 int detectthread;

	 inline int getminarea(){return minarea;};
	 inline int getmaxarea(){return maxarea;};
	 inline int getdetectthread(){return detectthread;};
	 inline int getmvprocesswidth(){return mvprocesswidth;};
	 inline  int getmvprocessheight(){return mvprocessheight;};
	 inline  int getmvdownup(){return mvdownup;};

	 int ptzwait;
	 int ptzbroad;
	 int ptzaddres;
	 int ptzdp;

	 int ptzpaninverse;
	 int ptztitleinverse;
	 int ptzspeed;

	 inline int getptzpaninverse(){return ptzpaninverse;};
	 inline  int getptztitleinverse(){return ptztitleinverse;};
	 

	 inline int getptzwait(){return ptzwait;};
	 inline  int getptzbroad(){return ptzbroad;};
	 inline  int getptzaddres(){return ptzaddres;};
	 inline  int getptzdp(){return ptzdp;};

	 inline int getptzspeed(){return ptzspeed;};
	 void setptzspeed(int speed){ ptzspeed=speed;};





	 int intergralenable;
	 inline int getintergralenable(){return intergralenable;};
	 void setintergralenable(int enable){ intergralenable=enable;};


	 

	static Config *getinstance();
	void saveconfig();
	void loadconfig();
	void SaveConfig();
	private:



	Config();
	~Config();
	static Config *instance;
	//final static Config configinstance=new Config();

};




#endif /* CONFIG_HPP_ */
