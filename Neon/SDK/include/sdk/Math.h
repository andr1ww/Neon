#pragma once
#define CONSTEXPR constexpr
#define INV_PI			(0.31830988618f)
#define HALF_PI			(1.57079632679f)
#define PI 					(3.1415926535897932f)

namespace SDK
{
	class FMath {
      public:
            template <class T>
            static CONSTEXPR FORCEINLINE T Min( const T A, const T B ) {
                    return ( A <= B ) ? A : B;
            }

	        template< class T >
static constexpr FORCEINLINE T Max(const T A, const T B)
            {
                    return (A >= B) ? A : B;
            }


	        template< class T >
static FORCEINLINE T Clamp(const T X, const T Min, const T Max)
            {
                    return X < Min ? Min : X < Max ? X : Max;
            }
	        
	        template< class T >
                static FORCEINLINE T Square(const T A)
            {
                    return A * A;
            }

	        template< class T >
static constexpr FORCEINLINE T Abs(const T A)
            {
                    return (A >= (T)0) ? A : -A;
            }

	        static FORCEINLINE float Atan(float Value) { return atanf(Value); }

#define FASTASIN_HALF_PI (1.5707963050f)
	        /**
                * Computes the ASin of a scalar value.
                *
                * @param Value  input angle
                * @return ASin of Value
                */
	        static FORCEINLINE float Sqrt(float Value) { return sqrtf(Value); }
	        static FORCEINLINE float FastAsin(float Value)
            {
                    // Clamp input to [-1,1].
                    bool nonnegative = (Value >= 0.0f);
                    float x = FMath::Abs(Value);
                    float omx = 1.0f - x;
                    if (omx < 0.0f)
                    {
                            omx = 0.0f;
                    }
                    float root = FMath::Sqrt(omx);
                    // 7-degree minimax approximation
                    float result = ((((((-0.0012624911f * x + 0.0066700901f) * x - 0.0170881256f) * x + 0.0308918810f) * x - 0.0501743046f) * x + 0.0889789874f) * x - 0.2145988016f) * x + FASTASIN_HALF_PI;
                    result *= root;  // acos(|x|)
                    // acos(x) = pi - acos(-x) when x < 0, asin(x) = pi/2 - acos(x)
                    return (nonnegative ? FASTASIN_HALF_PI - result : result - FASTASIN_HALF_PI);
            }
#undef FASTASIN_HALF_PI
	        
	        static float Atan2(float Y, float X)
            {
                    //return atan2f(Y,X);
                    // atan2f occasionally returns NaN with perfectly valid input (possibly due to a compiler or library bug).
                    // We are replacing it with a minimax approximation with a max relative error of 7.15255737e-007 compared to the C library function.
                    // On PC this has been measured to be 2x faster than the std C version.

                    const float absX = FMath::Abs(X);
                    const float absY = FMath::Abs(Y);
                    const bool yAbsBigger = (absY > absX);
                    float t0 = yAbsBigger ? absY : absX; // Max(absY, absX)
                    float t1 = yAbsBigger ? absX : absY; // Min(absX, absY)

                    if (t0 == 0.f)
                            return 0.f;

                    float t3 = t1 / t0;
                    float t4 = t3 * t3;

                    static const float c[7] = {
                            +7.2128853633444123e-03f,
                            -3.5059680836411644e-02f,
                            +8.1675882859940430e-02f,
                            -1.3374657325451267e-01f,
                            +1.9856563505717162e-01f,
                            -3.3324998579202170e-01f,
                            +1.0f
                    };

                    t0 = c[0];
                    t0 = t0 * t4 + c[1];
                    t0 = t0 * t4 + c[2];
                    t0 = t0 * t4 + c[3];
                    t0 = t0 * t4 + c[4];
                    t0 = t0 * t4 + c[5];
                    t0 = t0 * t4 + c[6];
                    t3 = t0 * t3;

                    t3 = yAbsBigger ? (0.5f * PI) - t3 : t3;
                    t3 = (X < 0.0f) ? PI - t3 : t3;
                    t3 = (Y < 0.0f) ? -t3 : t3;

                    return t3;
            }

            template <class T>
            static constexpr FORCEINLINE T
            DivideAndRoundUp( T Dividend, T Divisor ) {
                    return ( Dividend + Divisor - 1 ) / Divisor;
            }

            static FORCEINLINE UINT32 FloorLog2( UINT32 Value ) {
                    // Use BSR to return the log2 of the integer
                    unsigned long Log2;
                    if ( _BitScanReverse( &Log2, Value ) != 0 ) {
                            return Log2;
                    }

                    return 0;
            }
    };
}