import numpy as np
####################################################################################################
# Exercise 1: Interpolation

def lagrange_interpolation(x: np.ndarray, y: np.ndarray) -> (np.poly1d, list):
    """
    Generate Lagrange interpolation polynomial.

    Arguments:
    x: x-values of interpolation points
    y: y-values of interpolation points

    Return:
    polynomial: polynomial as np.poly1d object
    base_functions: list of base polynomials
    """

    assert (x.size == y.size)

    base_functions = []

    # TODO: Generate Lagrange base polynomials and interpolation polynomial
    i = j = 0
    poly = np.poly1d(1)
    polynomial = np.poly1d(0)
    denominator = 1
    while i < x.size:
        while j < x.size:
            if i != j:
                numerator = np.poly1d([1, -x[j]])
                denominator *= x[i] - x[j]
                poly *= np.poly1d(numerator)
            j += 1
        poly /= denominator
        base_functions.append(poly)
        poly *= y[i]
        denominator = 1
        polynomial += poly
        poly = 1
        i += 1
        j = 0
    return polynomial, base_functions


def hermite_cubic_interpolation(x: np.ndarray, y: np.ndarray, yp: np.ndarray) -> list:
    """
    Compute hermite cubic interpolation spline

    Arguments:
    x: x-values of interpolation points
    y: y-values of interpolation points
    yp: derivative values of interpolation points

    Returns:
    spline: list of np.poly1d objects, each interpolating the function between two adjacent points
    """

    assert (x.size == y.size == yp.size)

    spline = []
    f_f = np.zeros(4)
    # TODO compute piecewise interpolating cubic polynomials
    v = i = 0
    while i < x.size - 1:
        hermite = np.zeros((4, 4))
        pair_points = [x[i], x[i + 1]]
        vanda = np.vander(pair_points, 4)
        x_derivative = np.vander(pair_points, 3)
        hermite[0] = vanda[0]
        hermite[1] = vanda[1]
        while v < 3:
            hermite[2][v] = x_derivative[0][v]
            hermite[3][v] = x_derivative[1][v]
            if v < 2:
                hermite[2][v] *= 3 - v
                hermite[3][v] *= 3 - v
            v += 1
        f_f[0] = y[i]
        f_f[1] = y[i + 1]
        f_f[2] = yp[i]
        f_f[3] = yp[i + 1]
        m = np.dot(np.linalg.inv(hermite), f_f)
        spline.append(np.poly1d(list(m)))
        v = 0
        i += 1
    return spline


####################################################################################################
# Exercise 2: Animation

def natural_cubic_interpolation(x: np.ndarray, y: np.ndarray) -> list:
    """
    Intepolate the given function using a spline with natural boundary conditions.

    Arguments:
    x: x-values of interpolation points
    y: y-values of interpolation points

    Return:
    spline: list of np.poly1d objects, each interpolating the function between two adjacent points
    """

    assert (x.size == y.size)
    # TODO construct linear system with natural boundary conditions
    big_matrix = np.zeros((4 * (x.size - 1), 4 * (x.size - 1)))
    b = np.zeros(4 * (x.size-1))

    big_matrix[0][3] = 6*x[0]
    big_matrix[0][2] = 2

    big_matrix[big_matrix.shape[0] - 1][big_matrix.shape[1] - 2] = 2
    big_matrix[big_matrix.shape[0] - 1][big_matrix.shape[1] - 1] = 6 * x[x.size - 1]

    i = 0
    while i < x.size - 1:
        help_matrix = np.zeros((4, 4 * (x.size-1)))
        pair_points = [x[i], x[i + 1]]
        vanda = np.vander(pair_points, 4)
        x_derivative = np.vander(pair_points, 3)
        x_derivative2 = np.vander(pair_points, 2)
        operand = 6
        help_matrix[0][(4*i)] = vanda[0][3]
        help_matrix[1][(4*i)] = vanda[1][3]
        v = 0
        while v < 3:
            help_matrix[0][(3 - v) + (4*i)] = vanda[0][v]
            help_matrix[1][(3 - v) + (4*i)] = vanda[1][v]
            help_matrix[2][(3 - v) + (4*i)] = x_derivative[1][v]
            if v < 2:
                help_matrix[3][(3 - v) + (4*i)] = x_derivative2[1][v]
            if v < 2:
                help_matrix[2][(3 - v) + (4*i)] *= 3 - v
                help_matrix[3][(3 - v) + (4*i)] *= operand
                operand = 2
            v += 1
        k = 0
        while k < 4:
            if i != x.size - 2:
                help_matrix[2][k+4 + (i*4)] = -help_matrix[2][k + (i*4)]
                help_matrix[3][k+4 + (i*4)] = -help_matrix[3][k + (i*4)]
            if k < 2:
                big_matrix[(i*4) + k+1] = help_matrix[k]
            elif i != x.size - 2:
                if k == 3:
                    big_matrix[(i*4) + k] = help_matrix[k-1]
                big_matrix[(i*4) + k+1] = help_matrix[k]
            k += 1
        b[(i * 4) + 1] = y[i]
        b[(i * 4) + 2] = y[i + 1]
        i += 1
    # TODO solve linear system for the coefficients of the spline
    k = l = 0
    while k < y.size - 1:
        if y[i] != 0:
          l = 1
        k += 1
    if l == 0:
        u, s, v = np.linalg.svd(big_matrix)
        coffs = v[4 * (x.size - 1)-1]
    else:
        invert_matrix = np.linalg.inv(big_matrix)
        coffs = np.dot(invert_matrix, b)
    spline = []
    # TODO extract local interpolation coefficients from solution
    i = 0
    while i < len(coffs):
        poly_list = [coffs[i+3], coffs[i+2], coffs[i+1], coffs[i]]
        spline.append(np.poly1d(poly_list))
        i += 4
    return spline


def periodic_cubic_interpolation(x: np.ndarray, y: np.ndarray) -> list:
    """
    Interpolate the given function with a cubic spline and periodic boundary conditions.

    Arguments:
    x: x-values of interpolation points
    y: y-values of interpolation points

    Return:
    spline: list of np.poly1d objects, each interpolating the function between two adjacent points
    """

    assert (x.size == y.size)
    # TODO: construct linear system with periodic boundary conditions
    big_matrix = np.zeros((4 * (x.size - 1), 4 * (x.size - 1)))
    b = np.zeros(4 * (x.size - 1))

    big_matrix[0][3] = 3 * x[0] * x[0]
    big_matrix[0][2] = 2 * x[0]
    big_matrix[0][1] = 1

    big_matrix[1][3] = 6 * x[0]
    big_matrix[1][2] = 2

    big_matrix[0][4 * (x.size - 1) - 1] = -3 * x[x.size - 1] * x[x.size - 1]
    big_matrix[0][4 * (x.size - 1) - 2] = -2 * x[x.size - 1]
    big_matrix[0][4 * (x.size - 1) - 3] = -1

    big_matrix[1][4 * (x.size - 1) - 1] = -6 * x[x.size - 1]
    big_matrix[1][4 * (x.size - 1) - 2] = -2

    i = 0
    while i < x.size - 1:
        help_matrix = np.zeros((4, 4 * (x.size - 1)))
        pair_points = [x[i], x[i + 1]]
        vanda = np.vander(pair_points, 4)
        x_derivative = np.vander(pair_points, 3)
        x_derivative2 = np.vander(pair_points, 2)
        operand = 6
        help_matrix[0][(4 * i)] = vanda[0][3]
        help_matrix[1][(4 * i)] = vanda[1][3]
        v = 0
        while v < 3:
            help_matrix[0][(3 - v) + (4 * i)] = vanda[0][v]
            help_matrix[1][(3 - v) + (4 * i)] = vanda[1][v]
            help_matrix[2][(3 - v) + (4 * i)] = x_derivative[1][v]
            if v < 2:
                help_matrix[3][(3 - v) + (4 * i)] = x_derivative2[1][v]
            if v < 2:
                help_matrix[2][(3 - v) + (4 * i)] *= 3 - v
                help_matrix[3][(3 - v) + (4 * i)] *= operand
                operand = 2
            v += 1
        k = 0
        while k < 4:
            if i != x.size - 2:
                help_matrix[2][k + 4 + (i * 4)] = -help_matrix[2][k + (i * 4)]
                help_matrix[3][k + 4 + (i * 4)] = -help_matrix[3][k + (i * 4)]
            if k < 2:
                big_matrix[(i * 4) + k + 2] = help_matrix[k]
            elif i != x.size - 2:
                if k == 3:
                    big_matrix[(i * 4) + k+1] = help_matrix[k - 1]
                big_matrix[(i * 4) + k + 2] = help_matrix[k]
            k += 1

        b[(i * 4) + 2] = y[i]
        b[(i * 4) + 3] = y[i + 1]
        i += 1
    # TODO solve linear system for the coefficients of the spline
    k = l = 0
    while k < y.size - 1:
        if y[i] != 0:
            l = 1
            break
        k += 1
    if l == 0:
        u, s, v = np.linalg.svd(big_matrix)
        coffs = v[4 * (x.size - 1) - 1]
    else:
        invert_matrix = np.linalg.inv(big_matrix)
        coffs = np.dot(invert_matrix, b)
    spline = []
    # TODO extract local interpolation coefficients from solution
    i = 0
    while i < len(coffs):
        poly_list = [coffs[i + 3], coffs[i + 2], coffs[i + 1], coffs[i]]
        spline.append(np.poly1d(poly_list))
        i += 4
    return spline


if __name__ == '__main__':
    print("All requested functions for the assignment have to be implemented in this file and uploaded to the "
          "server for the grading.\nTo test your implemented functions you can "
          "implement/run tests in the file tests.py (> python3 -v test.py [Tests.<test_function>]).")
