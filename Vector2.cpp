#include "Vector2.h"

Vector2::Vector2 () 
{
    x = 0;
    y = 0;
}

Vector2::Vector2 ( double x, double y ) : x ( x ), y (y)
{}

Vector2 & Vector2::Add ( const Vector2 & vec ) 
{
    this -> x += vec . x;
    this -> y += vec . y;
    return * this;
}

Vector2 & Vector2::Subtract ( const Vector2 & vec )
{

    this -> x -= vec . x;
    this -> y -= vec . y;
    return * this;
}

Vector2 operator + ( const Vector2 & v1, const Vector2 & v2 )
{
    return Vector2 ( v1 . x + v2 . x , v1 . y + v2 . y);
}

Vector2 operator - ( const Vector2 & v1, const Vector2 & v2 )
{
    return Vector2 ( v1 . x - v2 . x , v1 . y - v2 . y);
}


Vector2 & Vector2::operator += ( const Vector2 & v ) 
{ 
    return ( this -> Add ( v ) );
}

Vector2 & Vector2::operator -= ( const Vector2 & v ) 
{ 
    return ( this -> Subtract ( v ) );
}

Vector2 & Vector2::operator * ( const double & i ) 
{
    this -> x *= i;
    this -> y *= i;

    return * this;
}

Vector2 & Vector2::Zero () 
{
    this -> x = 0;
    this -> y = 0;

    return * this;
}

std::ostream & operator << ( std::ostream & ostr, const Vector2 v )
{
    ostr << "(" << v . x << "," << v . y << ")";
    return ostr;
}
