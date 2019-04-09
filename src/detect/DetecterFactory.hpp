/*
 * DetecterFactory.hpp
 *
 *  Created on: Feb 18, 2019
 *      Author: wj
 */

#ifndef DETECTERFACTORY_HPP_
#define DETECTERFACTORY_HPP_
#if USE_DETECTV2
class Detector;
class DetectorFactory{
public:
	typedef enum
	{
		HOGDETECTOR,
		SALIENCYDETECTOR,
		DEEPLEARNDETECTOR,
		COMPOSITEDETECTOR,
	}DetectroName;
public:
	static DetectorFactory*getinstance();
	Detector *createDetector(DetectroName name);
private:
	static DetectorFactory *instance_;
	DetectorFactory(){};
	~DetectorFactory();
};

#endif

#endif /* DETECTERFACTORY_HPP_ */
