#ifndef PREPROCESSOR_HPP
#define PREPROCESSOR_HPP

/* Interface with pipeline structure intent 
 * Each preprocessing step should be able to be loaded in the corresponding signal source. 
 * However the API should be flexible to allow various pre-processors without additional details. 
*/

namespace PROCESSING {

class IPreprocessor {
public:
    virtual ~IPreprocessor() = default;
    virtual void process(const uint16_t* input, uint16_t* output, size_t count) const = 0;
};

} // Namespace PROCESSING 
#endif /* PREPROCESSOR_HPP */