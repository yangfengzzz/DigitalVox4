//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef math_functions_hpp
#define math_functions_hpp

#include "matrix3x3.h"
#include "quaternion.h"

namespace vox {
namespace pbd {

class MathFunctions {
private:
    static void jacobiRotate(Matrix3x3F &A, Matrix3x3F &R,
                             int p, int q);
    
public:
    /*
     * Return the inf norm of the matrix.
     */
    static float infNorm(const Matrix3x3F &A);
    
    /*
     * Return the one norm of the matrix.
     */
    static float oneNorm(const Matrix3x3F &A);
    
    static void eigenDecomposition(const Matrix3x3F &A,
                                   Matrix3x3F &eigenVecs,
                                   Vector3F &eigenVals);
    
    /*
     * Perform polar decomposition A = (U D U^T) R
     */
    static void polarDecomposition(const Matrix3x3F &A,
                                   Matrix3x3F &R,
                                   Matrix3x3F &U,
                                   Matrix3x3F &D);
    
    /*
     * Perform a polar decomposition of matrix M and return the rotation matrix R.
     * This method handles the degenerated cases.
     */
    static void polarDecompositionStable(const Matrix3x3F &M,
                                         const float tolerance,
                                         Matrix3x3F &R);
    
    /*
     * Perform a singular value decomposition of matrix A: A = U * sigma * V^T.
     * This function returns two proper rotation matrices U and V^T which do not
     * contain a reflection. Reflections are corrected by the inversion handling
     * proposed by Irving et al. 2004.
     */
    static void svdWithInversionHandling(const Matrix3x3F &A,
                                         Vector3F &sigma,
                                         Matrix3x3F &U,
                                         Matrix3x3F &VT);
    
    static float cotTheta(const Vector3F &v, const Vector3F &w);
    
    /** Computes the cross product matrix of a vector.
     * @param  v        input vector
     * @param  v_hat    resulting cross product matrix
     */
    static void crossProductMatrix(const Vector3F &v, Matrix3x3F &v_hat);
    
    /** Implementation of the paper: \n
     * Matthias Muller, Jan Bender, Nuttapong Chentanez and Miles Macklin,
     * "A Robust Method to Extract the Rotational Part of Deformations",
     * ACM SIGGRAPH Motion in Games, 2016
     */
    static void extractRotation(const Matrix3x3F &A, QuaternionF &q, const unsigned int maxIter);
};

}
}
#endif /* math_functions_hpp */
