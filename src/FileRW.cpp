#include "FileRW.hpp"

FileStorage filestore;
#define FILENAME "/home/ubuntu/calib/GY.xml"



class MyTData
{
public :
  MyTData() :
    time(0), Xspeed(0), Yspeed(0),Zspeed(0)
  {
  }
  explicit MyTData(int) :
    time(97), Xspeed(0), Yspeed(0),Zspeed(0)
  {
  }
  explicit MyTData(int t,double x,double y,double z) :
    time(t), Xspeed(x), Yspeed(y),Zspeed(z)
  {
  }
  int time;
  double Xspeed;
  double Yspeed;
  double Zspeed;
 
  void write(FileStorage& fs)  //Write serialization for this class
  {
    fs << "{" << "time" << time << "Xspeed" << Xspeed << "Yspeed" << Yspeed << "Zspeed" << Zspeed << "}";
	// fs << "{"<< "time" << Xspeed  <<  "}";
  }

  void read(const FileNode& node)  //Read serialization for this class
  {

    time = (int)node["time"];
    Xspeed = (double)node["Xspeed"];
    Yspeed = (double)node["Yspeed"];
    Zspeed = (double)node["Zspeed"];
   
  }
};
int  createfile()
{
	filestore=FileStorage(FILENAME, FileStorage::WRITE);


}

void writefilehead()
{
	filestore << "gyro" << "[";


	
}
void writefiletail()
{
	filestore << "]";
}

void writefile(int time,short xspeed,short yspeed,short zspeed)
{
	
	MyTData mydate(time,xspeed,yspeed,zspeed);
	mydate.write(filestore);


}

void writefiled(int time,double xspeed,double yspeed,double zspeed)
{
	
	MyTData mydate(time,xspeed,yspeed,zspeed);
	mydate.write(filestore);


}

void destoryfile()
{
	filestore.release();

}


