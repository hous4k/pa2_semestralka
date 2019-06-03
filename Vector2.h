#pragma once


#include <iostream>

struct Vector2 
{
    public:
        double x;
        double y;
        Vector2 ();
        Vector2 ( double x, double y );
        
        Vector2 & Add ( const Vector2 & vec );
        Vector2 & Subtract ( const Vector2 & vec );
        Vector2 & Multiply ( const Vector2 & vec );
        Vector2 & Divide ( const Vector2 & vec );

        friend Vector2 operator + ( const Vector2 & v1, const Vector2 & v2 );
        friend Vector2 operator - ( const Vector2 & v1, const Vector2 & v2 );
        /*
        friend Vector2 operator * ( const Vector2 & v1, const Vector2 & v2 );
        friend Vector2 operator / ( const Vector2 & v1, const Vector2 & v2 );
        */

        Vector2 & operator += ( const Vector2 & v );
        Vector2 & operator -= ( const Vector2 & v );
        /*
        Vector2 & operator *= ( const Vector2 & v );
        Vector2 & operator /= ( const Vector2 & v );
        */

        Vector2 & operator * ( const double & i );
        Vector2 & Zero();

        friend std::ostream & operator << ( std::ostream & ostr, const Vector2 v );
};