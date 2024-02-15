#ifndef A3DVECTOR_H
#define A3DVECTOR_H

#include <math.h>
#include <A3DSDK.h>
#include <A3DSDKTypes.h>
#include <A3DSDKErrorCodes.h>
#include <A3DSDKBase.h>
#include <A3DSDKRootEntities.h>
#include <A3DSDKGeometry.h>

class A3DVector3d
{
public:
	double x; 
	double y;
	double z;

	A3DVector3d() {}
	A3DVector3d(double dX, double dY, double dZ) : x(dX), y(dY), z(dZ) {}
	A3DVector3d(const A3DVector3dData& sV) : x(sV.m_dX), y(sV.m_dY), z(sV.m_dZ) {} 
	inline void set(double dX, double dY, double dZ)
	{	 x = dX; y = dY; z = dZ; }
	
	A3DVector3d(const A3DVector3d& toCopy) : x(toCopy.x), y(toCopy.y), z(toCopy.z) {}
	
	inline double Length() const { return sqrt(x*x + y*y + z*z); }

	inline double LenghtSquared() const{ return x*x + y*y + z*z; }	

	inline A3DStatus normalize()
	{
		double d = this->Length();
		if(d == 0)
			return A3D_ERROR;
		x/=d;
		y/=d;
		z/=d;
		return A3D_SUCCESS;
	}

	inline double Distance(const A3DVector3d& sOther) const 
	{
		return sqrt(this->SquareDistance(sOther));
	}

	inline double SquareDistance(const A3DVector3d& sOther) const
	{
		double d=0,dd;
		dd = x; dd -= sOther.x; dd *= dd; d += dd;
		dd = y; dd -= sOther.y; dd *= dd; d += dd;
		dd = z; dd -= sOther.z; dd *= dd; d += dd;
		return d;
	}

	inline double Dot(const A3DVector3d& sOther) const
	{
		return x*sOther.x + y*sOther.y + z*sOther.z;
	}

	inline A3DVector3d Cross(const A3DVector3d& sOther)
	{
		return A3DVector3d(y*sOther.z - z*sOther.y, z*sOther.x - x*sOther.z, x*sOther.y - y*sOther.x);
	}


	// in-place vector algebra
	inline A3DVector3d& operator += ( const A3DVector3d& sOther )
	{
		x+=sOther.x;
		y+=sOther.y;
		z+=sOther.z;
		return *this;
	}

	inline A3DVector3d& operator -= ( const A3DVector3d& sOther )
	{
		x-=sOther.x;
		y-=sOther.y;
		z-=sOther.z;
		return *this;
	}

	inline A3DVector3d operator + ( const A3DVector3d& sOther ) const
	{
		return A3DVector3d(x+sOther.x, y+sOther.y, z+sOther.z);
	}

	inline A3DVector3d operator - ( const A3DVector3d& sOther ) const
	{
	   return A3DVector3d(x-sOther.x, y-sOther.y, z-sOther.z);
	}

	inline A3DVector3d& operator *= ( double dValue )
	{	
		x*=dValue;
		y*=dValue;
		z*=dValue;
		return *this;
	}

	inline A3DVector3d& operator /= ( double dValue )
	{
		x/=dValue;
		y/=dValue;
		z/=dValue;
		return *this;
	}

	inline A3DVector3d operator * ( double dValue ) const
	{
		return	A3DVector3d(x*dValue, y*dValue, z*dValue);
	}

	inline A3DVector3d operator / ( double dValue ) const
	{
		 return A3DVector3d(x/dValue, y/dValue, z/dValue);
	}
};

#endif
