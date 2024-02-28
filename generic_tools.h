#ifndef GENERIC_TOOLS_H
#define GENERIC_TOOLS_H


//perform element-wise addition of two vectors of the same type, 
//returning a new vector containing the element-wise sum of the input vectors
template<typename T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b){
    assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());

    transform(a.begin(), a.end(), b.begin(), 
                   std::back_inserter(result), std::plus<T>());
    return result;
}


#endif