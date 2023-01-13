/**
 *  @file   PandoraSDK/include/Objects/CartesianVector.h
 * 
 *  @brief  Header file for the cartesian vector class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_CARTESIAN_VECTOR_H
#define PANDORA_CARTESIAN_VECTOR_H 1

#include "Pandora/StatusCodes.h"

#include <Eigen/Dense>

#include <cmath>
#include <iostream>
#include <limits>

namespace pandora
{

/**
 *  @brief  CartesianVector class
 */
class CartesianVector
{
public:
    /**
     *  @brief  Constructor, create a vector from the cartesian coordinates of the end point,
     *          origin at (0,0,0,)
     * 
     *  @param  x the end point x coordinate
     *  @param  y the end point y coordinate
     *  @param  z the end point z coordinate
     */
    CartesianVector(float x, float y, float z);

    /**
     *  @brief  Constructor, create a vector from an Eigen vector.
     * 
     *  @param  v the Eigen::Vector3f.
     */
    CartesianVector(const Eigen::Vector3f &rhs);

    /**
     *  @brief  Copy constructor
     * 
     *  @param  rhs the cartesian vector to copy
     */
    CartesianVector(const CartesianVector &rhs);

    /**
     *  @brief  Set the values of cartesian vector components
     * 
     *  @param  x the x coordinate
     *  @param  y the y coordinate
     *  @param  z the z coordinate
     */
    void SetValues(float x, float y, float z);

    /**
     *  @brief  Get the underlying Eigen vector.
     * 
     *  @return The Eigen::Vector3f
     */
    Eigen::Vector3f GetV() const;

    /**
     *  @brief  Get the cartesian x coordinate
     * 
     *  @return The cartesian x coordinate
     */
    float GetX() const;

    /**
     *  @brief  Get the cartesian y coordinate
     * 
     *  @return The cartesian y coordinate
     */
    float GetY() const;

    /**
     *  @brief  Get the cartesian z coordinate
     * 
     *  @return The cartesian z coordinate
     */
    float GetZ() const;

    /**
     *  @brief  Get the magnitude
     * 
     *  @return The magnitude
     */
    float GetMagnitude() const;

    /**
     *  @brief  Get the magnitude squared
     * 
     *  @return The magnitude squared
     */
    float GetMagnitudeSquared() const;

    /**
     *  @brief  Get the dot product of the cartesian vector with a second cartesian vector
     * 
     *  @param  rhs the second cartesian vector
     * 
     *  @return The dot product
     */
    float GetDotProduct(const CartesianVector &rhs) const;

    /**
     *  @brief  Get the cross product of the cartesian vector with a second cartesian vector
     * 
     *  @param  rhs the second cartesian vector
     * 
     *  @return The cross product
     */
    CartesianVector GetCrossProduct(const CartesianVector &rhs) const;

    /**
     *  @brief  Get the cosine of the opening angle of the cartesian vector with respect to a second cartesian vector
     * 
     *  @param  rhs the second cartesian vector
     * 
     *  @return The cosine of the opening angle
     */
    float GetCosOpeningAngle(const CartesianVector &rhs) const;

    /**
     *  @brief  Get the opening angle of the cartesian vector with respect to a second cartesian vector
     * 
     *  @param  rhs the second cartesian vector
     * 
     *  @return The opening angle
     */
    float GetOpeningAngle(const CartesianVector &rhs) const;

    /**
     *  @brief  Get the distance squared of a cartesian vector with respect to a second cartesian vector
     *
     *  @param  rhs the second cartesian vector
     *
     *  @return The distance squared
     */
    float GetDistanceSquared(const CartesianVector &rhs) const;

    /**
     *  @brief  Get the spherical coordinates of the cartesian vector
     * 
     *  @param  radius the magnitude of the vector
     *  @param  phi the azimuth of the vector
     *  @param  theta the inclination of the vector
     */
    void GetSphericalCoordinates(float &radius, float &phi, float &theta) const;

    /**
     *  @brief  Get the cylindrical coordinates of the cartesian vector (x/y .. radius, z .. z)
     * 
     *  @param  radius the radius (x,y-plane) of the vector
     *  @param  phi the azimuth of the vector
     *  @param  z the z position of the vector
     */
    void GetCylindricalCoordinates(float &radius, float &phi, float &z) const;

    /**
     *  @brief  Get a unit vector in the direction of the cartesian vector
     * 
     *  @return The unit vector
     */
    CartesianVector GetUnitVector() const;

    /**
     *  @brief  Cartesian vector assignment operator
     * 
     *  @param  rhs the cartesian vector to assign
     */
    CartesianVector &operator=(const CartesianVector &rhs);

    /**
     *  @brief  Cartesian vector += operator
     * 
     *  @param  rhs the cartesian vector to add
     */
    CartesianVector &operator+=(const CartesianVector &rhs);

    /**
     *  @brief  Cartesian vector -= operator
     * 
     *  @param  rhs the cartesian vector to subtract
     */
    CartesianVector &operator-=(const CartesianVector &rhs);

    /**
     *  @brief  Cartesian vector *= operator
     * 
     *  @param  scalar the scalar to multiply
     */
    CartesianVector &operator*=(const double scalar);

    /**
     *  @brief  Cartesian vector == operator
     * 
     *  @param  rhs the cartesian vector to compare
     */
    bool operator==(const CartesianVector &rhs) const;

private:
    Eigen::Vector3f m_v;
};

/**
 *  @brief  Cartesian vector addition operator
 * 
 *  @param  lhs first cartesian vector, to which the second is added
 *  @param  rhs second cartesian vector, which is added to the first
 */
CartesianVector operator+(const CartesianVector &lhs, const CartesianVector &rhs);

/**
 *  @brief  Cartesian vector subtraction operator
 * 
 *  @param  lhs first cartesian vector, from which the second is subtracted
 *  @param  rhs second cartesian vector, which is subtracted from the first
 */
CartesianVector operator-(const CartesianVector &lhs, const CartesianVector &rhs);

/**
 *  @brief  Cartesian vector multiplication with scalar operator
 * 
 *  @param  lhs the cartesian vector to be multiplied by the scalar
 *  @param  scalar the value of the scalar
 */
CartesianVector operator*(const CartesianVector &lhs, const double scalar);

/**
 *  @brief  Operator to dump cartesian vector properties to an ostream
 *
 *  @param  stream the target ostream
 *  @param  cartesianVector the cartesian vector
 */
std::ostream &operator<<(std::ostream & stream, const CartesianVector& cartesianVector);

//------------------------------------------------------------------------------------------------------------------------------------------

inline CartesianVector::CartesianVector(float x, float y, float z) :
    m_v(x, y, z)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline CartesianVector::CartesianVector(const CartesianVector &rhs) :
    m_v(rhs.m_v)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline CartesianVector::CartesianVector(const Eigen::Vector3f &rhs) :
    m_v(rhs)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void CartesianVector::SetValues(float x, float y, float z)
{
    m_v = Eigen::Vector3f(x, y, z);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Eigen::Vector3f CartesianVector::GetV() const
{
    return m_v;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CartesianVector::GetX() const
{
    return m_v.x();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CartesianVector::GetY() const
{
    return m_v.y();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CartesianVector::GetZ() const
{
    return m_v.z();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CartesianVector::GetMagnitude() const
{
    return m_v.norm();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CartesianVector::GetMagnitudeSquared() const
{
    return m_v.squaredNorm();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CartesianVector::GetDotProduct(const CartesianVector &rhs) const
{
    return m_v.dot(rhs.m_v);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline CartesianVector CartesianVector::GetCrossProduct(const CartesianVector &rhs) const
{
    return CartesianVector(m_v.cross(rhs.m_v));
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CartesianVector::GetOpeningAngle(const CartesianVector &rhs) const
{
    return std::acos(this->GetCosOpeningAngle(rhs));
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CartesianVector::GetDistanceSquared(const CartesianVector &rhs) const
{
    return (  (GetX() - rhs.GetX()) * (GetX() - rhs.GetX())
            + (GetY() - rhs.GetY()) * (GetY() - rhs.GetY())
            + (GetZ() - rhs.GetZ()) * (GetZ() - rhs.GetZ()));
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline CartesianVector &CartesianVector::operator=(const CartesianVector &rhs)
{
    this->SetValues(rhs.GetX(), rhs.GetY(), rhs.GetZ());
    return *this;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline CartesianVector &CartesianVector::operator+=(const CartesianVector &rhs)
{
    m_v += rhs.m_v;
    return *this;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline CartesianVector &CartesianVector::operator-=(const CartesianVector &rhs)
{
    m_v -= rhs.m_v;
    return *this;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline CartesianVector &CartesianVector::operator*=(const double scalar)
{
    m_v *= scalar;
    return *this;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool CartesianVector::operator==(const CartesianVector &rhs) const
{
    return m_v == rhs.m_v;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline CartesianVector operator+(const CartesianVector &lhs, const CartesianVector &rhs)
{
    return CartesianVector(lhs.GetV() + rhs.GetV());
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline CartesianVector operator-(const CartesianVector &lhs, const CartesianVector &rhs)
{
    return CartesianVector(lhs.GetV() - rhs.GetV());
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline CartesianVector operator*(const CartesianVector &lhs, const double scalar)
{
    return CartesianVector(lhs.GetV() * scalar);
}

} // namespace pandora

#endif // #ifndef PANDORA_CARTESIAN_VECTOR_H
