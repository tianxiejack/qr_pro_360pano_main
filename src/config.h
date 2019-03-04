/*
 * config.h
 *
 *  Created on: 2018年9月14日
 *      Author: fsmdn113
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define VERSION_STRING		       "v1.04"



#define TV  (1)
#define FIRWIDTH (720)
#define FIRHEIGHT (576)



#define camerafov	            (18)

#define CAPTURETVWIDTH  (1920)
#define CAPTURETVHEIGHT  (1080)


#define PANO360SRCWIDTH (1920)
#define PANO360SRCHEIGHT (1080)

#define PANO360WIDTH (1920)
#define PANO360HEIGHT (1080)



#define CYLINDERPROJECTION (0)


#define AUTOMOV (0)

//#define CAMERAFOCUSLENGTH (4670)
//#define CAMERAFOCUSLENGTH (4759.721)
#define CAMERAFOCUSLENGTH (1780)

#define IMAGEQUEUE (3)
#define QUEUESUM 3

#define ANGLESCALE (1000)

#define PANOSCALE (1)

#define PANOSHIFT (0)

#define ANGLEINTREVAL (10)

#define ANGLESHIFT (0)

#define PANOWDTEHENTEND (0)

#define FEATURESTICH (0)

#define CYLINDER (1)

#define PANODETECTNUM (2)

#define PANOEXTRAH 20

//#define PANOSRCSHIFT (870)
#define PANOSRCSHIFT (100)



#define PANODSTTAILSHIFT (100)

#define PANOCYLINDCUT (200)

#define PANOGRAYDETECT (1)

#define MOVEBLOCKNUM (20)

#define MODELINGNUM (5)

#define MOVEBLOCKENABLE (1)

#define MOVELKBLOCKNUM (20)

#define LKMOVANGLE (1.5)

#define MOVDETECTDOW (2)

#define MOVDETECTDOWENABLE (1)

#define DETECTTEST (0)

#define FILEVIDEO (0)

#define MULTICPUPANO (1)

#define MULTICPUPANOLK (1)

#define MOVDETECTSRCWIDTH (PANO360WIDTH)
#define MOVDETECTSRCHEIGHT (PANO360HEIGHT)


#define PTZBOARD (9600)
#define PTZADDR (1)
#define PTZDP (0)
#define PTZWAIT (1)

#define PTZPANINVERSE (0)
#define PTZTITLEINVERSE (1)

#define MULTICPUPANONUM (15)

#define ZEROJUEGE (15)

#define MULDETECTTRAIN (0)

#define KALMANFILTER (0)


#define QUANTER (0)


#define FEATURETEST (0)


#define IMAGEPROCESSSTICH (0)

#define BUTTERFLY 0

#define TVFOV (18)

#define PTZOK (1)


#define FASTMODE (1)


#define ZEROCALIBRATIONMODE (0)

#define MVDETECTALG (1)

#define CALIBLOSTCOUNT (3)

#define CONFIGINIT (0)
//#define PANOSRCSHIFT (0)

#endif /* CONFIG_H_ */
