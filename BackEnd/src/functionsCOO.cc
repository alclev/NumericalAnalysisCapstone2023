#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <map>
#include <cmath>
#include <unordered_map>

//Rather than create three separate vectors, could store one vector with a struct that consists of the
//coordinates and the values all as one.

namespace COO {

    /**
     * @brief Compressed Coordinate Structure implementation
     * 
     * @tparam T 
     */
    template <typename T>
    class COOMatrix {

    //save position and value of non-zero elements
        public:
            std::vector<size_t> rowCoord;
            std::vector<size_t> colCoord;
            std::vector<T> values;
            size_t numRows;
            size_t numCols;
            size_t nnz; //number of non zero elements        
    };

    /**
     * @brief Get the Value COO object
     * 
     * @param compressedCoord 
     * @param row 
     * @param col 
     * @return double 
     */
    template<typename T>
        T get_matrixCOO(COOMatrix<T> compressedCoord, size_t row, size_t col) {
            if (compressedCoord.numRows < row) {
                throw std::invalid_argument("Row is not within range\n");
            }
            if (compressedCoord.numCols < col) {
                throw std::invalid_argument("Column is not within range\n");
            }

            for (size_t i = 0; i < compressedCoord.values.size(); ++i) {
                if (row == compressedCoord.rowCoord[i] && col == compressedCoord.colCoord[i]) {
                    return compressedCoord.values.at(i);
                }
            }

            return 0;
        }


    /**
     * @brief Convert a dense matrix to a sparse compressed column object. Any non-zero values are added 
     *        along with their coordinates
     * 
     * @param denseMatrix 
     * @param rows 
     * @param cols 
     * @param nonz 
     * @return COOMatrix 
     */
    template<typename T>
        COOMatrix<T> from_vector(std::vector<std::vector<T>>& denseMatrix) {
            size_t nnz_id = 0;
            COO::COOMatrix<T> coo;
            coo.numRows = denseMatrix.size();
            coo.numCols = denseMatrix.at(0).size();
            for (size_t i = 0; i < denseMatrix.size(); ++i) {
                for (size_t j = 0; j < denseMatrix[i].size(); ++j) {
                    if (denseMatrix[i][j] != 0) {
                        coo.rowCoord.push_back(i);
                        coo.colCoord.push_back(j);
                        coo.values.push_back(denseMatrix[i][j]);
                        nnz_id++;
                        coo.nnz += 1;
                    }
                }
            }

            return coo;
        }

    /**
     * @brief Convert a Compressed Coordinate Structure to a Dense Matrix
     * 
     * @param compressedCoord 
     * @return std::vector<std::vector<int>> 
     */
    template<typename T>
        std::vector<std::vector<T>> convertCOOtoDense(COOMatrix<T> compressedCoord) {
            std::vector<std::vector<int>> dense;
            int nnz_id = 0;

            for (size_t i = 0; i < compressedCoord.numRows; ++i) {
                std::vector<int> rowVector;
                for (size_t j = 0; j < compressedCoord.numCols; ++j) {
                    if (i == compressedCoord.rowCoord[nnz_id] && j == compressedCoord.colCoord[nnz_id]) {
                        rowVector.push_back(compressedCoord.values[nnz_id]);
                        nnz_id++;
                    } else {
                        rowVector.push_back(0);
                    }
                }
                dense.push_back(rowVector);
            }
            return dense;
        }

    /**
     * @brief Find the minumum, nonzero value within the COO values vector
     * 
     * @tparam T 
     * @param compressedCoord 
     * @return T 
     */
    template<typename T>
        T find_min_COO(COOMatrix<T> &compressedCoord) {
            if (compressedCoord.values.size() == 0) {
                throw std::invalid_argument("Error: no values within the COO matrix\n");
                return 0;
            }
            T min_val = compressedCoord.values[0];
            for (auto &value : compressedCoord.values) {
                if (value < min_val) {
                    min_val = value;
                }
            }

            return min_val;
        }

    /**
     * @brief Find the maximum value within the COO values vector
     * 
     * @tparam T 
     * @param compressedCoord 
     * @return T 
     */
    template<typename T>
        T find_max_COO(COOMatrix<T> &compressedCoord) {
            if (compressedCoord.values.size() == 0) {
                throw std::invalid_argument("Error: no values within the COO matrix\n");
                return 0;
            }
            T max_val = compressedCoord.values[0];
            for (auto &value : compressedCoord.values) {
                if (value > max_val) {
                    max_val = value;
                }
            }

            return max_val;
        }

    /**
     * @brief Scale up a matrix by a certain scalar, throws error if scalar is zero
     * 
     * @param compressedCoord 
     * @param scalar 
     * @return COO 
     */
    template<typename T>
        COOMatrix<T> scalar_mult_matrixCOO(COOMatrix<T> &compressedCoord, int scalar) {
            if (scalar == 0) {
                throw std::invalid_argument("Error: cannot zero out matrix\n");
            }

            const int valueSize = static_cast<int>(compressedCoord.values.size());
            for (size_t i = 0; i < valueSize; ++i) {
                compressedCoord.values.at(i) = compressedCoord.values.at(i) * scalar;
            }

            return compressedCoord;
        }

    /**
     * @brief Scale down a matrix by a certain scalar, throws error if scalar is zero
     * 
     * @param compressedCoord 
     * @param scalar 
     * @return COO 
     */
    template<typename T>
        COOMatrix<T> scalar_div_matrixCOO(COOMatrix<T> &compressedCoord, int scalar) {
            if (scalar == 0) {
                throw std::invalid_argument("Error: cannot divide by zero\n");
            }

            for (size_t i = 0; i < compressedCoord.values.size(); ++i) {
                compressedCoord.values.at(i) = compressedCoord.values.at(i) / scalar;
            }

            return compressedCoord;
        }

    /**
     * @brief Takes a COO matrix structure and a scalar, modifies the matrix by that scalar using pass by reference
     * 
     * @tparam T (double,int,float)
     * @param compressedCoord 
     * @param scalar 
     */
    template<typename T>
        void scalar_add_matrixCOO(COOMatrix<T> &compressedCoord, int scalar) {
            //No modification, return nothing
            if (scalar == 0) {
                return;
            }

            for (size_t i = 0; i < compressedCoord.values.size(); ++i) {
                compressedCoord.values.at(i) = compressedCoord.values.at(i) + scalar;
            }
        }

    /**
     * @brief Takes a COO matrix structure and a scalar, modifies the matrix by that scalar using pass by reference
     * 
     * @tparam T (double,int,float)
     * @param compressedCoord 
     * @param scalar 
     */
    template<typename T>
        void scalar_sub_matrixCOO(COOMatrix<T> &compressedCoord, int scalar) {
            //No modification, return nothing
            if (scalar == 0) {
                return;
            }

            for (size_t i = 0; i < compressedCoord.values.size(); ++i) {
                compressedCoord.values.at(i) = compressedCoord.values.at(i) - scalar;
            }
        }

    /**
     * @brief Add two Compressed Coordinate matrices together, both matrices must be of the same size
     * 
     * @param compressedCoord1 
     * @param compressedCoord2 
     * @return COO 
     */
    template <typename T>
        COOMatrix<T> add_matrixCOO(const COOMatrix<T>& compressedCoord1, const COOMatrix<T>& compressedCoord2) {
            if (compressedCoord1.numRows != compressedCoord2.numRows) {
                throw std::invalid_argument("The number of rows in the first matrix must match the number of rows in the second matrix.");
            }
            if (compressedCoord1.numCols != compressedCoord2.numCols) {
                throw std::invalid_argument("The number of columns in the first matrix must match the number of columns in the second matrix.");
            }

            std::vector<size_t> rowCoord;
            std::vector<size_t> colCoord;
            std::vector<T> values;

            size_t i = 0, j = 0;
            while (i < compressedCoord1.values.size() && j < compressedCoord2.values.size()) {
                size_t aRow = compressedCoord1.rowCoord[i];
                size_t bRow = compressedCoord2.rowCoord[j];
                size_t aCol = compressedCoord1.colCoord[i];
                size_t bCol = compressedCoord2.colCoord[j];
                if (aRow < bRow || (aRow == bRow && aCol < bCol)) {
                    rowCoord.push_back(aRow);
                    colCoord.push_back(aCol);
                    values.push_back(compressedCoord1.values[i]);
                    i++;
                }
                else if (aRow > bRow || (aRow == bRow && aCol > bCol)) {
                    rowCoord.push_back(bRow);
                    colCoord.push_back(bCol);
                    values.push_back(compressedCoord2.values[j]);
                    j++;
                }
                else {
                    rowCoord.push_back(aRow);
                    colCoord.push_back(aCol);
                    values.push_back(compressedCoord1.values[i] + compressedCoord2.values[j]);
                    i++;
                    j++;
                }
            }
            while (i < compressedCoord1.values.size()) {
                rowCoord.push_back(compressedCoord1.rowCoord[i]);
                colCoord.push_back(compressedCoord1.colCoord[i]);
                values.push_back(compressedCoord1.values[i]);
                i++;
            }
            while (j < compressedCoord2.values.size()) {
                rowCoord.push_back(compressedCoord2.rowCoord[j]);
                colCoord.push_back(compressedCoord2.colCoord[j]);
                values.push_back(compressedCoord2.values[j]);
                j++;
            }

            COOMatrix<T> returnMatrix;
            returnMatrix.numRows = compressedCoord1.numRows;
            returnMatrix.numCols = compressedCoord1.numCols;
            returnMatrix.nnz = values.size();
            returnMatrix.rowCoord = rowCoord;
            returnMatrix.colCoord = colCoord;
            returnMatrix.values = values;

            return returnMatrix;
        }

        /**
         * @brief Subtracting two COO matrices and returning the difference
         * 
         * @tparam T 
         * @param compressedCoord1 
         * @param compressedCoord2 
         * @return COOMatrix<T> 
         */
        template <typename T>
        COOMatrix<T> sub_matrixCOO(const COOMatrix<T>& compressedCoord1, const COOMatrix<T>& compressedCoord2) {
            if (compressedCoord1.numRows != compressedCoord2.numRows) {
                throw std::invalid_argument("The number of rows in the first matrix must match the number of rows in the second matrix.");
            }
            if (compressedCoord1.numCols != compressedCoord2.numCols) {
                throw std::invalid_argument("The number of columns in the first matrix must match the number of columns in the second matrix.");
            }

            std::vector<size_t> rowCoord;
            std::vector<size_t> colCoord;
            std::vector<T> values;

            size_t i = 0, j = 0;
            while (i < compressedCoord1.values.size() && j < compressedCoord2.values.size()) {
                size_t aRow = compressedCoord1.rowCoord[i];
                size_t bRow = compressedCoord2.rowCoord[j];
                size_t aCol = compressedCoord1.colCoord[i];
                size_t bCol = compressedCoord2.colCoord[j];
                if (aRow < bRow || (aRow == bRow && aCol < bCol)) {
                    rowCoord.push_back(aRow);
                    colCoord.push_back(aCol);
                    values.push_back(compressedCoord1.values[i]);
                    i++;
                }
                else if (aRow > bRow || (aRow == bRow && aCol > bCol)) {
                    rowCoord.push_back(bRow);
                    colCoord.push_back(bCol);
                    values.push_back(compressedCoord2.values[j]);
                    j++;
                }
                else {
                    rowCoord.push_back(aRow);
                    colCoord.push_back(aCol);
                    values.push_back(compressedCoord1.values[i] - compressedCoord2.values[j]);
                    i++;
                    j++;
                }
            }
            while (i < compressedCoord1.values.size()) {
                rowCoord.push_back(compressedCoord1.rowCoord[i]);
                colCoord.push_back(compressedCoord1.colCoord[i]);
                values.push_back(compressedCoord1.values[i]);
                i++;
            }
            while (j < compressedCoord2.values.size()) {
                rowCoord.push_back(compressedCoord2.rowCoord[j]);
                colCoord.push_back(compressedCoord2.colCoord[j]);
                values.push_back(compressedCoord2.values[j]);
                j++;
            }

            COOMatrix<T> returnMatrix;
            returnMatrix.numRows = compressedCoord1.numRows;
            returnMatrix.numCols = compressedCoord1.numCols;
            returnMatrix.nnz = values.size();
            returnMatrix.rowCoord = rowCoord;
            returnMatrix.colCoord = colCoord;
            returnMatrix.values = values;

            return returnMatrix;
        }

        /**
         * @brief 
         * 
         * @tparam T 
         * @param compressedCoord1 
         * @param compressedCoord2 
         * @return COOMatrix<T> 
         */
        template <typename T>
            COOMatrix<T> multiply_matrixCOO(const COOMatrix<T>& compressedCoord1, const COOMatrix<T>& compressedCoord2) {
                if (compressedCoord1.numCols != compressedCoord2.numRows) {
                    throw std::invalid_argument("The number of columns in the first matrix must match the number of rows in the second matrix.");
                }
 

                COOMatrix<T> returnMatrix;
                returnMatrix.numRows = compressedCoord1.numRows;
                returnMatrix.numCols = compressedCoord2.numCols;
                
                
                // compute dot products of rows of A and columns of B
                for (size_t i = 0; i < compressedCoord1.rowCoord.size(); i++) {
                    for (size_t j = 0; j < compressedCoord2.rowCoord.size(); j++) {
                        if (compressedCoord1.colCoord[i] == compressedCoord2.rowCoord[j]) {
                            size_t row = compressedCoord1.rowCoord[i];
                            size_t col = compressedCoord2.colCoord[j];
                            T val = compressedCoord1.values[i] * compressedCoord2.values[j];
                            size_t k = 0;
                            while (k < returnMatrix.rowCoord.size() && (returnMatrix.rowCoord[k] < row || (returnMatrix.rowCoord[k] == row && returnMatrix.colCoord[k] < col))) {
                                k++;
                            }
                            if (k == returnMatrix.rowCoord.size() || (returnMatrix.rowCoord[k] != row || returnMatrix.colCoord[k] != col)) {
                                returnMatrix.rowCoord.insert(returnMatrix.rowCoord.begin() + k, row);
                                returnMatrix.colCoord.insert(returnMatrix.colCoord.begin() + k, col);
                                returnMatrix.values.insert(returnMatrix.values.begin() + k, val);
                            } else {
                                returnMatrix.values[k] += val;
                            }
                        }
                    }
                }

                return returnMatrix;
                
            }



        /**
         * @brief 
         * 
         * @tparam T 
         * @param compressedCoord 
         * @return COOMatrix<T> 
         */
        template <typename T>
            COOMatrix<T> transpose_matrixCOO(const COOMatrix<T>& compressedCoord) {
                // Create a new instance of the COOMatrix class to hold the transposed matrix
                COOMatrix<T> compressedCoordT;

                // Set the number of rows, columns, and non-zero elements in the transposed matrix to be the same as the original matrix
                compressedCoordT.numRows = compressedCoord.numCols;
                compressedCoordT.numCols = compressedCoord.numRows;
                compressedCoordT.nnz = compressedCoord.nnz;

                for (size_t i = 0; i < compressedCoord.colCoord.size(); ++i) {
                    compressedCoordT.colCoord.push_back(compressedCoord.rowCoord.at(i));
                    compressedCoordT.rowCoord.push_back(compressedCoord.colCoord.at(i));
                    compressedCoordT.values.push_back(compressedCoord.values.at(i));
                }

                for (size_t i = 0; i < compressedCoord.values.size(); ++i) {
                    size_t min_index = i;
                    for (size_t j = i+1; j < compressedCoord.values.size(); ++j) {
                        if (compressedCoordT.rowCoord[j] < compressedCoordT.rowCoord[min_index]) {
                            min_index = j;
                        }
                    }

                    if (min_index != i) {
                        std::swap(compressedCoordT.colCoord[min_index], compressedCoordT.colCoord[i]);
                        std::swap(compressedCoordT.rowCoord[min_index], compressedCoordT.rowCoord[i]);
                        std::swap(compressedCoordT.values[min_index], compressedCoordT.values[i]);
                    }
                }

                return compressedCoordT;

            }





        template<typename T>
            void guassian_jordan_elimination(COOMatrix<T> &compressedCoord) {
                
            }
    
    std::vector<std::vector<double>> load_fileCOO(std::string fileName) {
        std::ifstream file(fileName);
        int num_row, num_col, num_lines;

        file >> num_row >> num_col >> num_lines;

        std::vector<std::vector<double>> matrix = std::vector<std::vector<double>>(num_row, std::vector<double>(num_col, 0.0));

        for (int i = 0; i < num_lines; ++i) {
            double data;
            int row, col;
            file >> row >> col >> data;
            matrix[(row-1)][col-1] = data;
        }
        file.close();
        return matrix;
    }
}

