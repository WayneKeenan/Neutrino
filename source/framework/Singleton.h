/*
 *  CSingleton.h
 *  Bullet Point
 *
 *  Created by Korruptor on 23/03/2010.
 *  Copyright 2010 Ruffian Games. All rights reserved.
 *
 */


// Nicked from MSD :D

#pragma once
#include "Memory.h"

template <typename T> class CSingleton
{
  
private:
  static T * s_pSingleton;
  
public:
  CSingleton( void )
  {
  }
  
  ~CSingleton( void )
  {
    ASSERT( s_pSingleton );
    s_pSingleton = 0;  
  }
  
  inline static T& Instance( void )
  {
    ASSERT( s_pSingleton );
    return ( *s_pSingleton ); 
  }
  
  inline static T* InstancePtr( void )
  {  
    return ( s_pSingleton );  
  }
  
  static void Create()
  {
    ASSERT( InstancePtr() == NULL);
    s_pSingleton = NEWX T;
  }
  
  //! Destroy this singleton
  static void Destroy()
  {
    ASSERT( InstancePtr() != NULL );
    DELETEX s_pSingleton;
    s_pSingleton = NULL;
  }
};

template <typename T> T* CSingleton <T>::s_pSingleton = 0;

