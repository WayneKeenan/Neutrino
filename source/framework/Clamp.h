/*
 *  clamp.h
 *  BallZup
 *
 *  Created by Korruptor on 20/03/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef __CLAMP
#define __CLAMP

template<class T>
T clamp(T v, T min, T max)
{
	if ( v > max)
		v = max;
  
	if ( v < min)
		v = min;
  
	return (v);
}

#endif
