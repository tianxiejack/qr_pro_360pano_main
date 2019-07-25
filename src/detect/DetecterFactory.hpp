/*
 * DetecterFactory.hpp
 *
 *  Created on: Feb 18, 2019
 *      Author: wj
 */

#ifndef DETECTERFACTORY_HPP_
#define DETECTERFACTORY_HPP_

class Detector;
class DetectorFactory{
public:
	typedef enum
	{
		HOGDETECTOR,
		SALIENCYDETECTOR,
		DEEPLEARNDETECTOR,
		MVLEARNDETECTOR,
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


#endif /* DETECTERFACTORY_HPP_ */
