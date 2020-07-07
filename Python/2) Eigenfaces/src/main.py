import numpy as np
import lib
import matplotlib as mpl
import matplotlib.image
####################################################################################################
# Exercise 1: Power Iteration

def power_iteration(M: np.ndarray, epsilon: float = -1.0) -> (np.ndarray, list):
    """
    Compute largest eigenvector of matrix M using power iteration. It is assumed that the
    largest eigenvalue of M, in magnitude, is well separated.

    Arguments:
    M: matrix, assumed to have a well separated largest eigenvalue
    epsilon: epsilon usedM.dot(vector) for convergence (default: 10 * machine precision)

    Return:
    vector: eigenvector associated with largest eigenvalue
    residuals : residual for each iteration step

    Raised Exceptions:
    ValueError: if matrix is not square

    Forbidden:
    numpy.linalg.eig, numpy.linalg.eigh, numpy.linalg.svd
    """
    if M.shape[0] != M.shape[1]:
        raise ValueError("Matrix not nxn")

    # TODO: set epsilon to default value if not set by user
    if epsilon != 10 * np.finfo(np.float64).eps:
        epsilon = np.finfo(np.float64).eps

    # TODO: random vector of proper size to initialize iteration
    vector = np.ones(M.shape[0])

    # Initialize residual list and residual of current eigenvector estimate
    residuals = []
    residual = 2.0 * epsilon
    residuals.append(residual)
    # Perform power iteration
    while residual > epsilon:
        # TODO: implement power iteration
        old_vector = vector
        vector = M.dot(vector)/np.linalg.norm(M.dot(vector))
        residual = np.linalg.norm(vector - old_vector)
        residuals.append(residual)

    return vector, residuals


####################################################################################################
# Exercise 2: Eigenfaces

def load_images(path: str, file_ending: str=".png") -> (list, int, int):
    """
    Load all images in path with matplotlib that have given file_ending

    Arguments:
    path: path of directory containing image files that can be assumed to have all the same dimensions
    file_ending: string that image files have to end with, if not->ignore file

    Return:
    images: list of images (each image as numpy.ndarray and dtype=float64)
    dimension_x: size of images in x direction
    dimension_y: size of images in y direction
    """

    images = []

    # TODO read each image in path as numpy.ndarray and append to images
    # Useful functions: lib.list_directory(), matplotlib.image.imread(), numpy.asarray()
    file_names = lib.list_directory(path)
    file_names.sort()
    for i in range(len(file_names)):
        if not str(file_names[i]).endswith(file_ending):
            continue
        _image = mpl.image.imread(path + file_names[i], "png")
        array_for_image = np.float64(np.asarray(_image))
        images.append(array_for_image)


    # TODO set dimensions according to first image in images
    dimension_y, dimension_x = images[0].shape

    return images, dimension_x, dimension_y


def setup_data_matrix(images: list) -> np.ndarray:
    """
    Create data matrix out of list of 2D data sets.

    Arguments:
    images: list of 2D images (assumed to be all homogeneous of the same size and type np.ndarray)

    Return:
    D: data matrix that contains the flattened images as rows
    """
    # TODO: initialize data matrix with proper size and data type
    x = images[0].shape[0]
    y = images[0].shape[1]

    rows_size = images[0].shape[0]
    col_size = images[0].shape[1]

    images_size = len(images)
    D = np.zeros((images_size, y * x))
    # TODO: add flattened images to data matrix
    for i in range(images_size):
        for rows in range(rows_size):
            for col in range(col_size):
                D[i][(rows * col_size) + col] = images[i][rows][col]
    return D


def calculate_pca(D: np.ndarray) -> (np.ndarray, np.ndarray, np.ndarray):
    """
    Perform principal component analysis for given data matrix.
    Arguments:
    D: data matrix of size m x n where m is the number of observations and n the number of variables

    Return:
    pcs: matrix containing principal components as rows
    svals: singular values associated with principle components
    mean_data: mean that was subtracted from data
    """

    # TODO: subtract mean from data / center data at origin
    rows_size = D.shape[0]
    col_size = D.shape[1]

    mean_data = np.zeros(col_size)

    for i in range(rows_size):
        for j in range(col_size):
            mean_data[j] += D[i][j]
    mean_data /= rows_size

    # TODO: compute left and right singular vectors and singular values
    # Useful functions: numpy.linalg.svd(..., full_matrices=False)
    for i in range(rows_size):
        for j in range(col_size):
            D[i][j] -= mean_data[j]
    svals, pcs = [np.ones((1, rows_size))] * 2
    u, svals, pcs = np.linalg.svd(D, full_matrices=False)

    return pcs, svals, mean_data


def accumulated_energy(singular_values: np.ndarray, threshold: float = 0.8) -> int:
    """
    Compute index k so that threshold percent of magnitude of singular values is contained in
    first k singular vectors.

    Arguments:
    singular_values: vector containing singular values
    threshold: threshold for determining k (default = 0.8)

    Return:
    k: threshold index
    """

    # TODO: Normalize singular value magnitudes
    _sum = 0
    _sum1 = 0

    rows_size = singular_values.shape[0]
    i = 0
    while i < rows_size:
        _sum += singular_values[i]
        i += 1
    i = 0
    while i < rows_size:
        _sum1 += singular_values[i]
        if _sum1 >= threshold * _sum:
            break
        i += 1
    k = i + 1
    # TODO: Determine k that first k singular values make up threshold percent of magnitude
    return k


def project_faces(pcs: np.ndarray, images: list, mean_data: np.ndarray) -> np.ndarray:
    """
    Project given image set into basis.

    Arguments:
    pcs: matrix containing principal components / eigenfunctions as rows
    images: original input images from which pcs were created
    mean_data: mean data that was subtracted before computation of SVD/PCA

    Return:
    coefficients: basis function coefficients for input images, each row contains coefficients of one image
    """

    # TODO: initialize coefficients array with proper size
    coefficients = setup_data_matrix(images)
    # TODO: iterate over images and project each normalized image into principal component basis
    rows_size = images[0].shape[0]
    col_size = images[0].shape[1]

    img_size = len(images)
    for k in range(img_size):
        for i in range(rows_size):
            for j in range(col_size):
                coefficients[k][(i * col_size) + j] = coefficients[k][(i * col_size) + j] - mean_data[(i * col_size) + j]

    # TODO: add flattened images to data matrix
    data = np.zeros((img_size, pcs.shape[0]))
    for i in range(img_size):
        data[i] = pcs.dot(coefficients[i])

    return data


def identify_faces(coeffs_train: np.ndarray, pcs: np.ndarray, mean_data: np.ndarray, path_test: str) -> (np.ndarray, list, np.ndarray):
    """
    Perform face recognition for test images assumed to contain faces.

    For each image coefficients in the test data set the closest match in the training data set is calculated.
    The distance between images is given by the angle between their coefficient vectors.

    Arguments:
    coeffs_train: coefficients for training images, each image is represented in a row
    path_test: path to test image data

    Return:
    scores: Matrix with correlation between all train and test images, train images in rows, test images in columns
    imgs_test: list of test images
    coeffs_test: Eigenface coefficient of test images
    """

    # TODO: load test data set
    imgs_test, y, x = load_images(path_test)

    # TODO: project test data set into eigenbasis
    coeffs_test = project_faces(pcs, imgs_test, mean_data)


    # TODO: Initialize scores matrix with proper size
    scores = np.zeros((coeffs_train.shape[0], coeffs_test.shape[0]))
    # TODO: Iterate over all images and calculate pairwise correlation
    for i in range(coeffs_train.shape[0]):
        for j in range(coeffs_test.shape[0]):
            norm_v2 = np.linalg.norm(coeffs_test[j])
            norm_v1 = np.linalg.norm(coeffs_train[i])
            mul_v = coeffs_train[i].dot(coeffs_test[j])
            mul_n = norm_v2 * norm_v1
            scores[i][j] = np.arccos(mul_v/mul_n)
    return scores, imgs_test, coeffs_test


if __name__ == '__main__':

    print("All requested functions for the assignment have to be implemented in this file and uploaded to the "
          "server for the grading.\nTo test your implemented functions you can "
          "implement/run tests in the file tests.py (> python3 -v test.py [Tests.<test_function>]).")
