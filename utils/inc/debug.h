#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <iostream>
#include <boost/timer/timer.hpp>

enum class DEBUG_LEVELS
{
  VERBOSE,
  HIGH,
  LOW
};

const DEBUG_LEVELS DEBUG_LEVEL = DEBUG_LEVELS::LOW;

#define LOG( string, debugLevel ) \
{ \
  if ( debugLevel >= DEBUG_LEVEL ) \
  { \
    std::cout<<__FILE__<<" "<<__LINE__<<" "<<__FUNCTION__<<" "<<string<<"\n"; \
  } \
}

#define LOG_EQUAL( expr1, expr2, string, debugLevel ) \
{ \
  if ( expr1 == expr2 ) \
  { \
    LOG( string, debugLevel ) \
  } \
}

#define LOG_NOT_EQUAL( expr1, expr2, string, debugLevel ) \
{ \
  if ( expr1 != expr2 ) \
  { \
    LOG( string, debugLevel ) \
  } \
}

inline void LOG_NO_DECORATIONS( const std::string& string, DEBUG_LEVELS debugLevel )
{ 
  if ( debugLevel >= DEBUG_LEVEL )
  {
    std::cout<<string<<"\n";
  }
}

#ifdef _DEBUG
#define _verify( x ) assert( x )
#else
#define _verify( x ) x
#endif

#define LOGPERF boost::timer::auto_cpu_timer timer

#endif//_DEBUG_H_