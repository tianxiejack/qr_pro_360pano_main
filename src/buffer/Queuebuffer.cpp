#include"Queuebuffer.hpp"


Queue*Queue::instance=NULL;

void Queue::create()
{
	
	for(int chId=0; chId<IMAGEQUEUE; chId++)
		image_queue_create(&bufQue[DISPALYPROCESS][chId], PROCESSQUEUENUM,
				Config::getinstance()->getcamwidth()*Config::getinstance()->getcamheight()*Config::getinstance()->getcamchannel(),
				memtype_normal);
	
	for(int chId=0; chId<IMAGEQUEUE; chId++)
		image_queue_create(&bufQue[TOPANOSTICH][chId], PROCESSQUEUENUM,
				Config::getinstance()->getcamwidth()*Config::getinstance()->getcamheight()*Config::getinstance()->getcamchannel(),
				memtype_normal);

	for(int chId=0; chId<IMAGEQUEUE; chId++)
		image_queue_create(&bufQue[FROMEPANOSTICH][chId], PROCESSQUEUENUM,
				Config::getinstance()->getcamwidth()*Config::getinstance()->getcamheight()*Config::getinstance()->getcamchannel(),
				memtype_normal);
	
	for(int chId=0; chId<IMAGEQUEUE; chId++)
		image_queue_create(&bufQue[VIDEOLOADRTSP][chId], 2,
				Config::getinstance()->getcamwidth()*Config::getinstance()->getcamheight()*Config::getinstance()->getcamchannel(),
				memtype_malloc);
	
	for(int chId=0; chId<IMAGEQUEUE; chId++)
		image_queue_create(&bufQue[DISPALYTORTP][chId], 3,
				Config::getinstance()->getdiswidth()*Config::getinstance()->getdisheight()*Config::getinstance()->getdischannel(),
				memtype_gst);
	
}

void *Queue::getfull(int index,int chid,int forever)
{
	return image_queue_getFulltime(&bufQue[index][chid],forever);



}
void  Queue::putempty(int index,int chid,void *info)
{

	image_queue_putEmpty(&bufQue[index][chid],(OSA_BufInfo* )info);

}


int Queue::getfullcount(int index,int chid)
{

	return image_queue_fullCount(&bufQue[index][chid]);



}
void *Queue::getprefull(int index,int chid,void *curentif)
{
	OSA_BufInfo *current=(OSA_BufInfo *)curentif;
	return &bufQue[index][chid].bufInfo[(current->currentbufid-1+PROCESSQUEUENUM)%PROCESSQUEUENUM];


}


void *Queue::getempty(int index,int chid ,int forever)
{
	return image_queue_getEmptytime(&bufQue[index][chid],forever);


}
void  Queue::putfull(int index,int chid ,void *info)
{

	image_queue_putFull(&bufQue[index][chid],(OSA_BufInfo* )info);
}

Queue *Queue::getinstance()
{
	if(instance==NULL)
		instance=new Queue();



	return instance;
	


}
