#ifndef MATIOCPP_MULTIDIMENSIONALARRAY_H
#define MATIOCPP_MULTIDIMENSIONALARRAY_H

/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 */

#include <matioCpp/ForwardDeclarations.h>
#include <matioCpp/ConversionUtilities.h>
#include <matioCpp/Span.h>
#include <matioCpp/Variable.h>

/**
 * @brief MultiDimensionalArray is a particular type of Variable specialized for multidimensional arrays of a generic type T.
 * @note The underlying array is in column-major format.
 */
template<typename T>
class matioCpp::MultiDimensionalArray : public matioCpp::Variable
{
public:

    using element_type = T; /** Defines the type of an element of the Vector. Needed to use the iterator. **/

    using value_type = std::remove_cv_t<T>; /** Defines the type of an element of the Vector without "const". Useful to use make_span. **/

    using allocator_type = std::allocator<T>; /** Defines how to allocate T. **/

    using index_type = size_t; /** The type used for indices. **/

    using reference = element_type&; /** The reference type. **/

    using pointer = typename std::allocator_traits<std::allocator<T>>::pointer; /** The pointer type. **/

    using const_pointer = typename std::allocator_traits<std::allocator<T>>::const_pointer; /** The const pointer type. **/

    //TODO. Definire un iteratore stile map, dove hai value e indices.

    /**
     * @brief Default Constructor
     * @note The name is set to "unnamed_multidimensional_array".
     */
    MultiDimensionalArray();

    /**
     * @brief Constructor
     * @param name The name of the MultiDimensionalArray
     */
    MultiDimensionalArray(const std::string& name);

    /**
     * @brief Constructor
     * @param name The name of the MultiDimensionalArray
     * @param dimensions The dimensions of the MultiDimensionalArray
     */
    MultiDimensionalArray(const std::string& name, const std::vector<index_type>& dimensions);

    /**
     * @brief Constructor
     * @param name The name of the MultiDimensionalArray
     * @param dimensions The dimensions of the MultiDimensionalArray
     * @param inputVector The raw pointer to the data stored in column-major order
     */
    MultiDimensionalArray(const std::string& name, const std::vector<index_type>& dimensions, const_pointer inputVector);

    /**
     * @brief Copy constructor
     */
    MultiDimensionalArray(const Vector<T>& other);

    /**
     * @brief Move constructor
     */
    MultiDimensionalArray(Vector<T>&& other);

    /**
    * Destructor.
    */
    ~MultiDimensionalArray();

    /**
     * @brief Assignement operator (copy) from another MultiDimensionalArray.
     * @param other The other MultiDimensionalArray.
     * @return A reference to this MultiDimensionalArray.
     */
    MultiDimensionalArray<T>& operator=(const MultiDimensionalArray<T>& other);

    /**
     * @brief Assignement operator (move) from another MultiDimensionalArray.
     * @param other The other MultiDimensionalArray.
     * @return A reference to this MultiDimensionalArray.
     */
    MultiDimensionalArray<T>& operator=(MultiDimensionalArray<T>&& other);

    /**
     * @brief Set from a vectorized array
     * @note The pointer is supposed to be in column-major format.
     * @param dimensions The input dimensions
     * @param inputVector The input pointer.
     * @return True if successfull.
     */
    bool fromVectorizedArray(const std::vector<index_type>& dimensions, const_pointer inputVector);

    /**
     * Inherited from matioCpp::Variable
     */
    virtual bool fromOther(const Variable& other) final;

    /**
     * Inherited from matioCpp::Variable
     */
    virtual bool fromOther(Variable&& other) final;

    /**
     * Inherited from matioCpp::Variable
     */
    virtual bool fromMatio(const matvar_t * inputVar) final;

    /**
     * @brief Get this MultiDimensionalArray as a Span
     */
    matioCpp::Span<T> toSpan();

    /**
     * @brief Get this MultiDimensionalArray as a Span (const version)
     */
    const matioCpp::Span<const T> toSpan() const;

    /**
     * @brief Change the name of the Variable
     * @param newName The new name
     * @return True if successfull.
     *
     * @warning This requires the MultiDimensionalArray to be reallocated. It performs memory allocation.
     */
    bool setName(const std::string& newName);

    /**
     * @brief Resize the vector.
     * @param newSize The new size.
     *
     * @warning This requires to allocate memory for twice the new size.
     * @warning Previous data is lost.
     */
    void resize(const std::vector<index_type>& newDimensions);

    /**
     * @brief Direct access to the underlying array.
     * @note The underlying array is in column-major format.
     * @return A pointer to the internal data.
     */
    pointer data();

    /**
     * @brief Direct access to the underlying array.
     * @note The underlying array is in column-major format.
     * @return A pointer to the internal data.
     */
    const_pointer data() const;

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @warning Each element of el has to be strictly smaller than the corresponding dimension.
     * @return A reference to the element.
     */
    reference operator()(const std::vector<index_type>& el);

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @warning Each element of el has to be strictly smaller than the corresponding dimension.
     * @return A copy to the element.
     */
    value_type operator()(const std::vector<index_type>& el) const;

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @warning Each element of el has to be strictly smaller than the corresponding dimension.
     * @return A reference to the element.
     */
    reference operator[](const std::vector<index_type>& el);

    /**
     * @brief Access specified element.
     * @param el The element to be accessed.
     * @warning Each element of el has to be strictly smaller than the corresponding dimension.
     * @return A copy to the element.
     */
    value_type operator[](const std::vector<index_type>& el) const;
};

#endif // MATIOCPP_MULTIDIMENSIONALARRAY_H
