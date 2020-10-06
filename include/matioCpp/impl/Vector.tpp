#ifndef MATIOCPP_VECTOR_TPP
#define MATIOCPP_VECTOR_TPP

/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 */

template<typename T>
bool matioCpp::Vector<T>::initializeVector(const std::string& name, Span<T> inputVector)
{
    size_t dimensions[] = {1, static_cast<size_t>(inputVector.size())};
    return initializeVariable(name, VariableType::Vector, matioCpp::get_type<T>::valueType, dimensions, (void*)inputVector.data());
}

template<typename T>
bool matioCpp::Vector<T>::checkCompatibility(const matvar_t *inputPtr) const
{
    if (!inputPtr)
    {
        std::cerr << "[matioCpp::Vector::checkCompatibility] The input pointer is null." << std::endl;
        return false;
    }

    matioCpp::VariableType outputVariableType = matioCpp::VariableType::Unsupported;
    matioCpp::ValueType outputValueType = matioCpp::ValueType::UNSUPPORTED;
    get_types_from_matvart(inputPtr, outputVariableType, outputValueType);

    if (outputVariableType != matioCpp::VariableType::Vector)
    {
        std::cerr << "[matioCpp::Vector::checkCompatibility] The input variable is not a vector." << std::endl;
        return false;
    }

    if (inputPtr->isComplex)
    {
        std::cerr << "[matioCpp::Vector::checkCompatibility] Cannot copy a complex variable to a non-complex one." << std::endl;
        return false;
    }

    if (!matioCpp::is_convertible_to_primitive_type<T>(outputValueType))
    {
        std::string dataType = "";
        std::string classType = "";

        get_types_names_from_matvart(inputPtr, classType, dataType);

        std::cerr << "[matioCpp::Vector::checkCompatibility] The input type is not convertible to " <<
            typeid(T).name() <<"." << std::endl <<
            "                                       Input class type: " << classType << std::endl <<
            "                                       Input data type: " << dataType << std::endl;
        return false;
    }
    return true;
}

template<typename T>
matioCpp::Vector<T>::Vector()
{
    static_assert (!std::is_same<T, bool>::value, "Vector<bool> is not supported." );
    std::vector<T> empty;
    initializeVector("unnamed_vector", matioCpp::make_span(empty));
}

template<typename T>
matioCpp::Vector<T>::Vector(const std::string& name)
{
    static_assert (!std::is_same<T, bool>::value, "Vector<bool> is not supported." );
    std::vector<T> empty;

    if (std::is_same<T, char>::value) //If the type is char, the name corresponds to the content
    {
        empty.resize(name.size());
        for (size_t i = 0; i < name.size(); ++i)
        {
            empty[i] = name[i];
        }
    }

    initializeVector(name, matioCpp::make_span(empty));
}

template<typename T>
matioCpp::Vector<T>::Vector(const std::string& name, Span<T> inputVector)
{
    static_assert (!std::is_same<T, bool>::value, "Vector<bool> is not supported." );
    initializeVector(name, inputVector);
}

template <typename T>
matioCpp::Vector<T>::Vector(const std::string &name, const std::string &inputString)
{
    static_assert (std::is_same<T, char>::value,"The assignement operator from a string is available only if the type of the vector is char");
    size_t dimensions[] = {1, static_cast<size_t>(inputString.size())};
    initializeVariable(name, VariableType::Vector, matioCpp::get_type<T>::valueType, dimensions, (void*)inputString.c_str());
}

template<typename T>
matioCpp::Vector<T>::Vector(const Vector<T> &other)
{
    fromOther(other);
}

template<typename T>
matioCpp::Vector<T>::Vector(Vector<T> &&other)
{
    fromOther(std::forward<matioCpp::Vector<T>>(other));
}

template<typename T>
matioCpp::Vector<T>::Vector(const MatvarHandler &handler)
    : matioCpp::Variable(handler)
{
    static_assert (!std::is_same<T, bool>::value, "Vector<bool> is not supported." );

    if (!checkCompatibility(handler.get()))
    {
        assert(false);
        std::vector<T> empty;
        initializeVector("unnamed_vector", matioCpp::make_span(empty));
    }
}

template<typename T>
matioCpp::Vector<T>::~Vector()
{

}

template<typename T>
matioCpp::Vector<T> &matioCpp::Vector<T>::operator=(const matioCpp::Vector<T> &other)
{
    fromOther(other);
    return *this;
}

template<typename T>
matioCpp::Vector<T> &matioCpp::Vector<T>::operator=(matioCpp::Vector<T> &&other)
{
    fromOther(std::forward<matioCpp::Vector<T>>(other));
    return *this;
}

template<typename T>
matioCpp::Vector<T> &matioCpp::Vector<T>::operator=(const Span<T> &other)
{
    if (size() == other.size())
    {
        memcpy(toMatio()->data, other.data(), size() * sizeof(T));
    }
    else
    {
        initializeVector(name(), other);
    }

    return *this;
}

template<typename T>
matioCpp::Vector<T> &matioCpp::Vector<T>::operator=(const std::string &other)
{
    static_assert (std::is_same<T, char>::value,"The assignement operator from a string is available only if the type of the vector is char");
    if (size() == other.size())
    {
        memcpy(toMatio()->data, other.data(), size() * sizeof(T));
    }
    else
    {
        size_t dimensions[] = {1, static_cast<size_t>(other.size())};
        initializeVariable(name(), VariableType::Vector, matioCpp::get_type<T>::valueType, dimensions, (void*)other.c_str());
    }

    return *this;

}

template<typename T>
matioCpp::Span<T> matioCpp::Vector<T>::toSpan()
{
    return matioCpp::make_span(*this);
}

template<typename T>
const matioCpp::Span<const T> matioCpp::Vector<T>::toSpan() const
{
    return matioCpp::make_span(*this);
}

template<typename T>
bool matioCpp::Vector<T>::setName(const std::string &newName)
{
    return initializeVector(newName, toSpan());
}

template<typename T>
typename matioCpp::Vector<T>::index_type matioCpp::Vector<T>::size() const
{
    //A vector should have the size of dimensions equal to 2
    assert(this->dimensions().size() == 2);

    return std::min(this->dimensions()[0], this->dimensions()[1]) > 0 ? std::max(this->dimensions()[0], this->dimensions()[1]) : 0;
}

template<typename T>
void matioCpp::Vector<T>::resize(typename matioCpp::Vector<T>::index_type newSize)
{
    if (newSize != size())
    {
        std::vector<T> newVector(newSize);
        memcpy(newVector.data(), data(), std::min(newSize, size()) * sizeof(T));
        this->operator=(newVector);
    }
}

template<typename T>
typename matioCpp::Vector<T>::pointer matioCpp::Vector<T>::data()
{
    return static_cast<T*>(toMatio()->data);
}

template<typename T>
typename matioCpp::Vector<T>::const_pointer matioCpp::Vector<T>::data() const
{
    return static_cast<const T*>(toMatio()->data);
}

template<typename T>
typename matioCpp::Vector<T>::reference matioCpp::Vector<T>::operator()(typename matioCpp::Vector<T>::index_type el)
{
    return toSpan()(el);
}

template<typename T>
typename matioCpp::Vector<T>::value_type matioCpp::Vector<T>::operator()(typename matioCpp::Vector<T>::index_type el) const
{
    return toSpan()(el);
}

template<typename T>
std::string matioCpp::Vector<T>::operator()() const
{
    static_assert (std::is_same<T, char>::value,"The operator () to convert to a string is available only if the type of the vector is char");
    return std::string(data(), size());
}

template<typename T>
typename matioCpp::Vector<T>::reference matioCpp::Vector<T>::operator[](typename matioCpp::Vector<T>::index_type el)
{
    return this->operator()(el);
}

template<typename T>
typename matioCpp::Vector<T>::value_type matioCpp::Vector<T>::operator[](typename matioCpp::Vector<T>::index_type el) const
{
    return this->operator()(el);
}

template<typename T>
typename matioCpp::Vector<T>::iterator matioCpp::Vector<T>::begin()
{
    return iterator(this, 0);
}

template<typename T>
typename matioCpp::Vector<T>::iterator matioCpp::Vector<T>::end()
{
    return iterator(this, size());
}

template<typename T>
typename matioCpp::Vector<T>::const_iterator matioCpp::Vector<T>::begin() const
{
    return const_iterator(this, 0);
}

template<typename T>
typename matioCpp::Vector<T>::const_iterator matioCpp::Vector<T>::end() const
{
    return const_iterator(this, size());
}

template<typename T>
typename matioCpp::Vector<T>::const_iterator matioCpp::Vector<T>::cbegin() const
{
    return const_iterator(this, 0);
}

template<typename T>
typename matioCpp::Vector<T>::const_iterator matioCpp::Vector<T>::cend() const
{
    return const_iterator(this, size());
}

template<typename T>
typename matioCpp::Vector<T>::reverse_iterator matioCpp::Vector<T>::rbegin()
{
    return reverse_iterator{end()};
}

template<typename T>
typename matioCpp::Vector<T>::reverse_iterator matioCpp::Vector<T>::rend()
{
    return reverse_iterator{begin()};
}

template<typename T>
typename matioCpp::Vector<T>::const_reverse_iterator matioCpp::Vector<T>::rbegin() const
{
    return const_reverse_iterator{cend()};
}

template<typename T>
typename matioCpp::Vector<T>::const_reverse_iterator matioCpp::Vector<T>::rend() const
{
    return const_reverse_iterator{cbegin()};
}

template<typename T>
typename matioCpp::Vector<T>::const_reverse_iterator matioCpp::Vector<T>::crbegin() const
{
    return const_reverse_iterator{cend()};
}

template<typename T>
typename matioCpp::Vector<T>::const_reverse_iterator matioCpp::Vector<T>::crend() const
{
    return const_reverse_iterator{cbegin()};
}

template<typename T>
matioCpp::Vector<T> matioCpp::Variable::asVector()
{
    return matioCpp::Vector<T>(*m_handler);
}

template<typename T>
const matioCpp::Vector<T> matioCpp::Variable::asVector() const
{
    return matioCpp::Vector<T>(*m_handler);
}

matioCpp::String matioCpp::Variable::asString()
{
    return matioCpp::Vector<char>(*m_handler);
}

const matioCpp::String matioCpp::Variable::asString() const
{
    return matioCpp::Vector<char>(*m_handler);
}

#endif // MATIOCPP_VECTOR_TPP
