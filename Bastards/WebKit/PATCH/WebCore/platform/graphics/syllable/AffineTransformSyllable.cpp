/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "AffineTransform.h"

#include "IntRect.h"
#include "FloatRect.h"

namespace WebCore {

AffineTransform::AffineTransform()
   /* : m_transform()*/
{
}

AffineTransform::AffineTransform(double a, double b, double c, double d, double tx, double ty)
    /*: m_transform(a, b, c, d, tx, ty)*/
{
}

#if 0
AffineTransform::AffineTransform(const QMatrix& matrix)
    /*: m_transform(matrix)*/
{
}
#endif

void AffineTransform::setMatrix(double a, double b, double c, double d, double tx, double ty)
{
    //m_transform.setMatrix(a, b, c, d, tx, ty);
}

void AffineTransform::map(double x, double y, double* x2, double* y2) const
{
   // m_transform.map(x, y, x2, y2);
}

IntRect AffineTransform::mapRect(const IntRect& rect) const
{
    //return m_transform.mapRect(rect);
    return IntRect();
}

FloatRect AffineTransform::mapRect(const FloatRect& rect) const
{
    //return m_transform.mapRect(rect);
    return FloatRect();
}

bool AffineTransform::isIdentity() const
{
   // return m_transform.isIdentity();
   return( false );
}

double AffineTransform::a() const
{
    //return m_transform.m11();
    return( 0 );
}

void AffineTransform::setA(double a)
{
//    m_transform.setMatrix(a, b(), c(), d(), e(), f());
}

double AffineTransform::b() const
{
    //return m_transform.m12();
     return( 0 );
}

void AffineTransform::setB(double a)
{
//    m_transform.setMatrix(a, b(), c(), d(), e(), f());
}

double AffineTransform::c() const
{
    //return m_transform.m21();
     return( 0 );
}

void AffineTransform::setC(double a)
{
//    m_transform.setMatrix(a, b(), c(), d(), e(), f());
}

double AffineTransform::d() const
{
    //return m_transform.m22();
     return( 0 );
}

void AffineTransform::setD(double a)
{
//    m_transform.setMatrix(a, b(), c(), d(), e(), f());
}

double AffineTransform::e() const
{
    //return m_transform.dx();
     return( 0 );
}


void AffineTransform::setE(double a)
{
//    m_transform.setMatrix(a, b(), c(), d(), e(), f());
}

double AffineTransform::f() const
{
    //return m_transform.dy();
     return( 0 );
}


void AffineTransform::setF(double a)
{
//    m_transform.setMatrix(a, b(), c(), d(), e(), f());
}

void AffineTransform::reset()
{
//    m_transform.reset();
}

AffineTransform& AffineTransform::scale(double sx, double sy)
{
    //m_transform.scale(sx, sy);
    return *this;
}

AffineTransform& AffineTransform::rotate(double d)
{
   // m_transform.rotate(d);
    return *this;
}

AffineTransform& AffineTransform::translate(double tx, double ty)
{
    //m_transform.translate(tx, ty);
    return *this;
}

AffineTransform& AffineTransform::shear(double sx, double sy)
{
   // m_transform.shear(sx, sy);
    return *this;
}

double AffineTransform::det() const
{
    //return m_transform.det();
    return( 0 );
}

AffineTransform AffineTransform::inverse() const
{
    //if(!isInvertible())
        return AffineTransform();

    //return m_transform.inverted();
}
/*
AffineTransform::operator QMatrix() const
{
    return m_transform;
}
*/
bool AffineTransform::operator==(const AffineTransform& other) const
{
    //return m_transform == other.m_transform;
    return( true );
}

AffineTransform& AffineTransform::operator*=(const AffineTransform& other)
{
//    m_transform *= other.m_transform;
    return *this;
}

AffineTransform AffineTransform::operator*(const AffineTransform& other)
{
//    return m_transform * other.m_transform;
return( AffineTransform() );
}

}

// vim: ts=4 sw=4 et





