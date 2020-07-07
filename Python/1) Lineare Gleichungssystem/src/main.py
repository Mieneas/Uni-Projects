import numpy as np
import tomograph

def change_row(A: np.ndarray, b: np.ndarray, col_a: int, dis_row: int, dig_is_0: bool = False) -> (
np.ndarray, np.ndarray):
    row = np.zeros((1, col_a))
    zeile = 0
    pos = -1
    flag = 1

    if not dig_is_0:
        while zeile < col_a:
            row[0][zeile] = A[0][zeile]
            if A[zeile][0] != 0 and flag:
                pos = zeile
                flag = 0
            zeile += 1
        if pos == -1:
            raise ValueError("there is no pivot.")
        zeile = 0
        while zeile < col_a:
            A[0][zeile] = A[pos][zeile]
            A[pos][zeile] = row[0][zeile]
            zeile += 1
        b_number = b[pos] #[0]
        b[pos] = b[0]
        b[0] = b_number
        return A, b
    else:
        while zeile < col_a:
            row[0][zeile] = A[dis_row][zeile]
            if zeile+dis_row < col_a and A[zeile+dis_row][dis_row] != 0:
                pos = zeile + dis_row
            zeile += 1
        if pos == -1:
            if b[dis_row] == 0:
                raise ValueError("the matrix has infinity solutions.")
            else:
                raise ValueError("the matrix has no solutions.")
        zeile = 0
        while zeile < col_a:
            A[dis_row][zeile] = A[pos][zeile]
            A[pos][zeile] = row[0][zeile]
            zeile += 1
        b_number = b[pos]
        b[pos] = b[dis_row]
        b[dis_row] = b_number
        return A, b


def chose_pivot(A: np.ndarray, b: np.ndarray, rows_a: int, col_a: int) -> (np.ndarray, np.ndarray):
    the_bigger = A[0][0]
    pivot = 0
    chosen_row = 0

    while pivot < rows_a:
        if the_bigger < A[pivot][0]:
            the_bigger = A[pivot][0]
            chosen_row = pivot
        pivot += 1

    if A[chosen_row][0] == 0:
        pivot = 0
        while pivot < rows_a:
            if the_bigger > A[pivot][0]:
                the_bigger = A[pivot][0]
                chosen_row = pivot
                break
            pivot += 1
        while pivot < rows_a:
            if (the_bigger < A[pivot][0]) and A[pivot][0] != 0:
                the_bigger = A[pivot][0]
                chosen_row = pivot
            pivot += 1

    columns = 0
    row = np.zeros((1, col_a))
    while columns < col_a:
        row[0][columns] = A[chosen_row][columns]
        columns += 1

    columns = 0
    while columns < col_a:
        A[chosen_row][columns] = A[0][columns]
        A[0][columns] = row[0][columns]
        columns += 1
    b_number = b[chosen_row]
    b[chosen_row] = b[0]
    b[0] = b_number
    return A, b


def valid_matrix(rows_a: int, col_a: int, rows_b: int):
    if rows_a != col_a:
        raise ValueError("the Matrix are not secure")
    elif col_a != rows_b:
        raise ValueError("the Matrix are not compatible")


def solve_of_matrix(A: np.ndarray, b: np.ndarray, rows_a: int, rows_b: int = 0) -> (np.ndarray, np.ndarray):
    # for gaussian_elimination
    if rows_b == 0:
        i = 0
        j = 0
        zero_size = 0
        while i < rows_a:
            if b[i] != 0:
                while j < rows_a:
                    if A[i][j] == 0:
                        zero_size += 1
                    j += 1
                if zero_size == rows_a:
                    raise ValueError("The Matrix has no solution.")
            elif b[i] == 0:
                while j < rows_a:
                    if A[i][j] == 0:
                        zero_size += 1
                    j += 1
                if zero_size == rows_a:
                    raise ValueError("The Matrix has infinity solutions.")
            i += 1
            j = 0
            zero_size = 0
    # for back_substitution
    if rows_b != 0:
        a_rows = rows_a
        b_rows = rows_b
        zero_size = 0
        i = 0
        j = 0
        diag = 0
        while i < a_rows:
            if b[i] != 0:
                while j < a_rows:
                    if A[i][j] == 0:
                        zero_size += 1
                    j += 1
                if zero_size == a_rows:
                    raise ValueError("The Matrix has no solution.")
                else:
                    zero_size = 0
                j = 0
            if A[i][i] != 0:
                diag += 1
            i += 1

        if diag != b_rows:
            raise ValueError("The Matrix has infinite solutions.")


def get_edited_row(value: int, row: int, dis_row: int, A: np.ndarray, rows: int, b: np.ndarray,
                   edit_b: bool = False) -> np.ndarray:
    if edit_b:
        digit = value * b[row]
        b[dis_row] -= digit
        return b
    else:
        matrix = A.copy()
        i = 0
        while i < rows:
            matrix[row][i] = value * A[row][i]
            A[dis_row][i] -= matrix[row][i]
            i += 1
        return A


def gaussian_elimination(A: np.ndarray, b: np.ndarray, use_pivoting: bool = True) -> (np.ndarray, np.ndarray):
    """
    Gaussian Elimination of Ax=b with or without pivoting.

    Arguments:
    A : matrix, representing left side of equation system of size: (m,m)
    b : vector, representing right hand side of size: (m, )
    use_pivoting : flag if pivoting should be used

    Return:
    A : reduced result matrix in row echelon form (type: np.ndarray, size: (m,m))
    b : result vector in row echelon form (type: np.ndarray, size: (m, ))

    Raised Exceptions:
    ValueError: if matrix and vector sizes are incompatible, matrix is not square or pivoting is disabled but necessary

    Side Effects:
    -

    Forbidden:
    - numpy.linalg.*
    """
    # Create copies of input matrix and vector to leave them unmodified
    mat = A.copy()
    vek = b.copy()

    # TODO: Test if shape of matrix and vector is compatible and raise ValueError if not
    rows_a, col_a = mat.shape
    rows_b = vek.shape[0]

    # check if the matrix valid:
    valid_matrix(rows_a, col_a, rows_b)

    # do need to use pivot?
    if use_pivoting:
        mat, vek = chose_pivot(mat, vek, rows_a, col_a)

    # test if the element A[0][0] = 0 then change this row
    if mat[0][0] == 0:
        if use_pivoting:
            mat, vek = change_row(mat, vek, rows_a, col_a)
        else:
            raise ValueError("there are zeros on the diagonal.")

    # TODO: Perform gaussian elimination
    # gaussian_elimination
    i = 1
    k = 0
    while k < col_a - 1:
        if mat[k][k] == 0:
            if use_pivoting:
                mat, vek = change_row(mat, vek, col_a, k, True)
            else:
                raise ValueError("there are zeros on the diagonal.")
        value = mat[k + i][k] / mat[k][k]
        while i < col_a - k:
            vek = get_edited_row(value, k, i + k, mat, rows_a, vek, True)
            mat = get_edited_row(value, k, i + k, mat, rows_a, vek)
            i += 1
            if k + i != rows_a:
                value = mat[k + i][k] / mat[k][k]
        k += 1
        i = 1

    # check if the Matrix is solvable
    solve_of_matrix(mat, vek, rows_a)

    i = 0
    while i < rows_a:
        if mat[i][i] == 0:
            raise ValueError("there is zeros on diagonal.")
        i += 1

    return mat, vek

def back_substitution(A: np.ndarray, b: np.ndarray) -> np.ndarray:
    """
    Back substitution for the solution of a linear system in row echelon form.

    Arguments:
    A : matrix in row echelon representing linear system
    b : vector, representing right hand side

    Return:
    x : solution of the linear system

    Raised Exceptions:
    ValueError: if matrix/vector sizes are incompatible or no/infinite solutions exist

    Side Effects:
    -

    Forbidden:
    - numpy.linalg.*
    """

    # TODO: Test if shape of matrix and vector is compatible and raise ValueError if not
    a_rows, a_col = A.shape
    b_rows = b.shape[0]
    if a_col != b_rows:
        raise ValueError("The matrix and vector is'n compatible")

    # TODO: Initialize solution vector with proper size
    x = np.zeros(b_rows)

    # TODO: Run backsubstitution and fill solution vector, raise ValueError if no/infinite solutions exist
    # check if the Matrix is solvable
    solve_of_matrix(A, b, a_rows, b_rows)

    j = 0
    i = a_rows - 1
    k = 0
    sum_ = 0
    while i >= 0:
        while j < a_rows - i - 1:
            if i != a_rows - 1:
                sum_ = sum_ + (x[a_rows - j - 1] * A[a_rows - k - 1][a_rows - j - 1])
            j += 1
        b[i] = b[i] - sum_
        x[i] = b[i] / A[i][i]
        sum_ = 0
        k += 1
        j = 0
        i -= 1
    return x


####################################################################################################
# Exercise 2: Cholesky decomposition

def compute_cholesky(M: np.ndarray) -> np.ndarray:
    """
    Compute Cholesky decomposition of a matrix

    Arguments:
    M : matrix, symmetric and positive (semi-)definite

    Raised Exceptions:
    ValueError: L is not symmetric and psd

    Return:
    L :  Cholesky factor of M

    Forbidden:
    - numpy.linalg.*
    """

    # TODO check for symmetry and raise an exception of type ValueError
    (n, m) = M.shape
    i = 0
    j = 1
    while i < n:
        while j < m - i:
            if M[i][j].dtype != float and M[i][j] != M[j][i]:
                raise ValueError("the matrix is not symmetry, or the values are not float.")
            j += 1
        i += 1
        j = 1

    # TODO build the factorization and raise a ValueError in case of a non-positive definite input matrix
    i = 0
    test_b = np.zeros(n)
    positive_test, test_b = gaussian_elimination(M, test_b, False)
    while i < n:
        if positive_test[i][i] < 0:
            raise ValueError("the matrix is not positive semi_definite.")
        i += 1

    L = np.zeros((n, n))
    check_l = np.zeros((n, n))
    check_u = np.zeros((n, n))
    i = 0
    j = 0
    while i < n:
        while j < n - i:
            check_l[j+i][i] = -1
            check_u[i][j+i] = -1
            j += 1
        i += 1
        j = 0

    rows = 0
    columns = 0
    l_and_u = 0
    temp = np.sqrt(M[rows][columns])
    while rows < n:
        while columns < n:
            while l_and_u < n:
                if rows == columns == 0 and l_and_u == 0:
                    check_l[rows][columns] = temp
                    check_u[rows][columns] = temp
                    break
                elif check_l[rows][l_and_u] == -1 and check_u[l_and_u][columns] != -1:
                    check_l[rows][l_and_u] = (M[rows][columns] - temp)/check_u[l_and_u][columns]
                    break
                elif check_u[l_and_u][columns] == -1 and check_l[rows][l_and_u] != -1:
                    check_u[l_and_u][columns] = (M[rows][columns] - temp)/check_l[rows][l_and_u]
                    break
                elif check_l[rows][l_and_u] == check_u[l_and_u][columns] == -1:
                    if M[rows][columns] - temp <= 0:
                        check_l[rows][l_and_u] = -np.sqrt(temp - M[rows][columns])
                        check_u[l_and_u][columns] = -np.sqrt(temp - M[rows][columns])
                    else:
                        check_l[rows][l_and_u] = np.sqrt(M[rows][columns] - temp)
                        check_u[l_and_u][columns] = np.sqrt(M[rows][columns] - temp)
                    break
                elif check_l[rows][l_and_u] != 0 or check_u[l_and_u][columns] != 0:
                    temp += check_l[rows][l_and_u]*check_u[l_and_u][columns]
                l_and_u += 1
            columns += 1
            l_and_u = 0
            temp = 0
        rows += 1
        columns = 0
        l_and_u = 0
        temp = 0

    return check_l


def solve_cholesky(L: np.ndarray, b: np.ndarray) -> np.ndarray:
    """
    Solve the system L L^T x = b where L is a lower triangular matrix

    Arguments:
    L : matrix representing the Cholesky factor
    b : right hand side of the linear system

    Raised Exceptions:
    ValueError: sizes of L, b do not match
    ValueError: L is not lower triangular matrix

    Return:
    x : solution of the linear system

    Forbidden:
    - numpy.linalg.*
    """

    # TODO Check the input for validity, raising a ValueError if this is not the case
    (n, m) = L.shape
    b_len = b.shape[0]

    if n != m:
        raise ValueError("the matrix is not square.")
    if n != b_len:
        raise ValueError("the matrix and the vector are not compatible.")

    for i in range(0, n):
        for j in range(1+i, n):
            if L[i][j] != 0:
                raise ValueError("the matrix is not lower triangular.")

    # TODO Solve the system by forward- and backsubstitution
    l_t = np.zeros((n, n))
    for i in range(n):
        for j in range(m):
            l_t[i][j] = L[j][i]

    x_vector = np.zeros(n)

    j = 0
    i = 0
    k = 0
    sum_ = 0
    while i < n:
        while j < i:
            if i != 0:
                sum_ = sum_ + (x_vector[j] * L[i][i-k+j])
            j += 1
        b[i] = b[i] - sum_
        x_vector[i] = b[i]/L[i][i]
        sum_ = 0
        k += 1
        j = 0
        i += 1

    x_vector = back_substitution(l_t, x_vector)

    return x_vector

####################################################################################################
# Exercise 3: Tomography


def setup_system_tomograph(n_shots: np.int, n_rays: np.int, n_grid: np.int) -> (np.ndarray, np.ndarray):
    """
    Set up the linear system describing the tomographic reconstruction

    Arguments:
    n_shots  : number of different shot directions
    n_rays   : number of parallel rays per direction
    n_grid   : number of cells of grid in each direction, in total n_grid*n_grid cells

    Return:
    L : system matrix
    g : measured intensities

    Raised Exceptions:
    -

    Side Effects:
    -

    Forbidden:
    -
    """

    # TODO: Initialize system matrix with proper size
    L = np.zeros((n_rays*n_shots, n_grid*n_grid))
    # TODO: Initialize intensity vector
    g = np.zeros(n_shots*n_rays)

    # TODO: Iterate over equispaced angles, take measurements, and update system matrix and sinogram
    theta = 0
    # Take a measurement with the tomograph from direction r_theta.
    # intensities: measured intensities for all <n_rays> rays of the measurement. intensities[n] contains the intensity for the n-th ray
    # ray_indices: indices of rays that intersect a cell
    # isect_indices: indices of intersected cells
    # lengths: lengths of segments in intersected cells
    # The tuple (ray_indices[n], isect_indices[n], lengths[n]) stores which ray has intersected which cell with which length. n runs from 0 to the amount of ray/cell intersections (-1) of this measurement.
    shots = 0
    g_index = 0
    while shots < n_shots:
        intensities, ray_indices, isect_indices, lengths = tomograph.take_measurement(n_grid, n_rays, theta)
        #while g_index < len(intensities):
            #g[(n_rays*shots) + ray_indices[shots]] = intensities[shots]
        for ray_index in range(len(ray_indices)):
            L[(n_rays * shots) + ray_indices[ray_index]][isect_indices[ray_index]] = lengths[ray_index]
            g[(n_rays * shots) + ray_indices[ray_index]] = intensities[ray_indices[ray_index]]
            #g_index += 1
        theta = theta + np.pi/n_shots
        shots += 1
    return [L, g]


def compute_tomograph(n_shots: np.int, n_rays: np.int, n_grid: np.int) -> np.ndarray:
    """
    Compute tomographic image

    Arguments:
    n_shots  : number of different shot directions
    n_rays   : number of parallel rays per direction
    n_grid   : number of cells of grid in each direction, in total n_grid*n_grid cells

    Return:
    tim : tomographic image

    Raised Exceptions:
    -

    Side Effects:
    -

    Forbidden:
    """

    # Setup the system describing the image reconstruction
    L, g = setup_system_tomograph(n_shots, n_rays, n_grid)

    # TODO: Solve for tomographic image using your Cholesky solver
    # (alternatively use Numpy's Cholesky implementation)
    matrix = L.T
    u_matrix = np.linalg.solve(matrix.dot(L), matrix.dot(g))

    # TODO: Convert solution of linear system to 2D image
    tim = np.zeros((n_grid, n_grid))

    vector_index = 0
    for row in range(n_grid):
        for col in range(n_grid):
            tim[row][col] = u_matrix[vector_index]
            vector_index += 1
    return tim


if __name__ == '__main__':
    print("All requested functions for the assignment have to be implemented in this file and uploaded to the "
          "server for the grading.\nTo test your implemented functions you can "
          "implement/run tests in the file tests.py (> python3 -v test.py [Tests.<test_function>]).")
