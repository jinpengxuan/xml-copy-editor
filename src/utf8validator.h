#ifndef UTF8_VALIDATOR_H
#define UTF8_VALIDATOR_H

inline bool is8Bit();

inline bool isUtf8TupelIndicator( unsigned char ch );

inline bool isUtf8OverlongTupel( unsigned char ch );

inline bool isUtf8TripleIndicator( unsigned char ch );

inline bool isUtf8OverlongTriple( unsigned char ch1, unsigned char ch2 );

inline bool isUtf8QuartetIndicator( unsigned char ch );

inline bool isUtf8OverlongQuartet( unsigned char ch1, unsigned char ch2 );

inline bool isUtf8QuintetIndicator( unsigned char ch );

inline bool isUtf8OverlongQuintet( unsigned char ch1, unsigned char ch2 );

inline bool isUtf8SextetIndicator( unsigned char ch );

inline bool isUtf8OverlongSextet( unsigned char ch1, unsigned char ch2 );

inline bool isUtf8Continuation( unsigned char ch );

bool isValidUtf8( const char * s, unsigned int len );

#endif
