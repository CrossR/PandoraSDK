/**
 *  @file PandoraSDK/src/Objects/CartesianVector.cc
 * 
 *  @brief Implementation of the cartesian vector class.
 * 
 *  $Log: $
 */

#include "Objects/CartesianVector.h"

#include <limits>

namespace pandora
{

float CartesianVector::GetCosOpeningAngle(const CartesianVector &rhs) const
{
    const float magnitudesSquared(this->GetMagnitudeSquared() * rhs.GetMagnitudeSquared());

    if (magnitudesSquared < std::numeric_limits<float>::epsilon())
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    float cosTheta = this->GetDotProduct(rhs) / std::sqrt(magnitudesSquared);

    if (cosTheta > 1.f)
    {
        cosTheta = 1.f;
    }
    else if (cosTheta < -1.f)
    {
        cosTheta = -1.f;
    }

    return cosTheta;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CartesianVector::GetSphericalCoordinates(float &radius, float &phi, float &theta) const
{
    const float magnitude(this->GetMagnitude());

    if (std::fabs(magnitude) < std::numeric_limits<float>::epsilon())
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    radius = magnitude;
    phi    = std::atan2(GetY(), GetX());
    theta  = std::acos(GetZ() / radius);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CartesianVector::GetCylindricalCoordinates(float &radius, float &phi, float &z) const
{
    const float magnitude(this->GetMagnitude());

    if (std::fabs(magnitude) < std::numeric_limits<float>::epsilon())
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    radius = std::sqrt(GetY() * GetY() + GetX() * GetX());
    phi    = std::atan2(GetY(), GetX());
    z      = GetZ();
}

//------------------------------------------------------------------------------------------------------------------------------------------

CartesianVector CartesianVector::GetUnitVector() const
{
    const float magnitude(this->GetMagnitude());

    if (std::fabs(magnitude) < std::numeric_limits<float>::epsilon())
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    return CartesianVector(GetX() / magnitude, GetY() / magnitude, GetZ() / magnitude);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream & stream, const CartesianVector& cartesianVector)
{
    stream  << "  x: " << cartesianVector.GetX()
            << "  y: " << cartesianVector.GetY()
            << "  z: " << cartesianVector.GetZ()
            << " length: " << cartesianVector.GetMagnitude();

    return stream;
}

} // namespace pandora
