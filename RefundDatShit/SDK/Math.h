#pragma once

#define square( x ) ( x * x )

namespace M
{
	extern inline void SinCos( float radians, float *sine, float *cosine );
	extern void CorrectMovement(QAngle vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove);
	extern void VectorAngles( const Vector& forward, QAngle &angles );
	//void VectorToAngles(const Vector & forward, const Vector & up, Vector & angles);
	extern void VectorToAngles(const Vector & forward, const Vector & up, QAngle & angles);
	extern void AngleVectors( const QAngle& angles, Vector *forward );
	extern void AngleVectors( const QAngle &angles, Vector *forward, Vector *right, Vector *up );
	extern QAngle CalcAngle( Vector v1, Vector v2 );
	extern void CalcAngle3(const Vector & vecSource, const Vector & vecDestination, QAngle & qAngles);
	extern float GetFov( const QAngle& viewAngle, const QAngle& aimAngle );
	extern float VectorDistance( Vector v1, Vector v2 );
	extern void VectorTransform( Vector& in1, matrix3x4a_t& in2, Vector &out );
	extern float GetDelta( float hspeed, float maxspeed, float airaccelerate );
	extern Vector ExtrapolateTick( Vector p0, Vector v0 );

	extern inline float RandFloat( float M, float N );
	
	// sperg cried about the previous method, 
	//here's not only a faster one but inaccurate as well to trigger more people
	inline float FASTSQRT( float x )
 	{
   		unsigned int i = *(unsigned int*) &x;

   		i  += 127 << 23;
   		// approximation of square root
   		i >>= 1; 
   		return *(float*) &i;
	}
}
