/*
 * Copyright (C) 2014 Fondazione Istituto Italiano di Tecnologia - Italian Institute of Technology
 * Author: Jorhabib Eljaik
 * email:  jorhabib.eljaik@iit.it
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */

#include "nonLinearMeasurementGaussianPdf.h"

namespace BFL{
nonLinearMeasurementGaussianPdf::nonLinearMeasurementGaussianPdf ( const Gaussian& additiveNoise )
: AnalyticConditionalGaussianAdditiveNoise (additiveNoise, NUMBEROFCONDITIONALARGUMENTS)
{

}

nonLinearMeasurementGaussianPdf::~nonLinearMeasurementGaussianPdf()
{

}

MatrixWrapper::ColumnVector nonLinearMeasurementGaussianPdf::ExpectedValueGet() const
{
    // This is pretty much the measurement model (Accelerometer in this particular case)
    // State variables and input are private members of ConditionalPDF
    MatrixWrapper::ColumnVector state = ConditionalArgumentGet(0);
    MatrixWrapper::ColumnVector expectedValue(4);
    MatrixWrapper::Quaternion tmpQuat(state);
    MatrixWrapper::Matrix Q(3,3);
    Q = tmpQuat.toRotation();
    cout << "[BFL::nonLinearMeasurementGaussianPdf::ExpectedValueGet] Rotation matrxi from quaternion: " << Q ;
    MatrixWrapper::ColumnVector g0(3); g0(1) = 0; g0(2) = 0; g0(3) = 9.81;
    expectedValue = Q*g0;
    
    return expectedValue + AdditiveNoiseMuGet(); 
}

MatrixWrapper::Matrix nonLinearMeasurementGaussianPdf::dfGet ( unsigned int i ) const
{
    MatrixWrapper::ColumnVector state  = ConditionalArgumentGet(0);
    // Remapping for easier copy
    double q0 = state(1);
    double q1 = state(2);
    double q2 = state(3);
    double q3 = state(4);
    if (i == 1) { // Derivative with respect to the first conditional argument (i.e. the 4-dim state)
        // Gavity vector
        MatrixWrapper::ColumnVector g0(3); g0(1) = 0; g0(2) = 0; g0(3) = 9.81;
        // Allocating space for Jacobian
        MatrixWrapper::Matrix dq(3,12);
        // TODO Remember to multiply this matrix by 2 at the end  and move this to the quaternion wrapper
        // Partial derivative of Q with respect to q0
        dq(1,1) = 2*q0; dq(1,2) = -q3 ; dq(1,3) = -q2;
        dq(2,1) = q3  ; dq(2,2) = 2*q0; dq(2,3) = -q1;
        dq(3,1) = -q2 ; dq(3,2) = q1  ; dq(3,3) = 2*q0;
        // Partial derivative of Q with respect to q1
        dq(1,4) = 2*q1; dq(1,5) = q2  ; dq(1,6) = q3 ;
        dq(2,4) = q2  ; dq(2,5) = 0   ; dq(2,6) = -q0;
        dq(3,4) = q3  ; dq(3,5) = q0  ; dq(3,6) = 0  ;
        // Partial derivative of Q with respect to q2
        dq(1,7) = 0   ; dq(1,8) = q1  ; dq(1,9) = q0;
        dq(2,7) = q1  ; dq(2,8) = 2*q2; dq(2,9) = q3;
        dq(3,7) = -q0 ; dq(3,8) = q3  ; dq(3,9) = 0;
        // Partial derivative of Q with respect to q3
        dq(1,10) = 0  ; dq(1,11) = -q0; dq(1,12) = q1;
        dq(2,10) = q0 ; dq(2,11) = 0  ; dq(2,12) = q2;
        dq(3,10) = q1 ; dq(3,11) = q2 ; dq(3,12) = 2*q3;
        return dq;
    } else {
        if (i >= NumConditionalArgumentsGet()) {
            cout << "This pdf only has " << NumConditionalArgumentsGet() << "conditional arguments \n";
            exit(-BFL_ERRMISUSE);
        } else {
            cout << "dq is not implemented for the "<< i <<"th conditional argument\n";
            exit(-BFL_ERRMISUSE);
        }
    }
    
}


} // BFL namespace

