
#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <exception>
#include <string>
#include <cinttypes>
#include <utility>

#define BUILD_EXCEPTION(exceptionname, sourcename) \
	class exceptionname : public sourcename { \
		using sourcename::sourcename; \
        protected: \
            const std::string exceptionName = #exceptionname; \
	}
	

namespace GostCrypt {

    // TODO: consider removing 'GostCrypt' prefix since we use a namespace (ie: GostCrypt::Exception)

	class GostCryptException: public std::exception
	{
	public:

	    explicit GostCryptException(const char* message, const char* function, const char* filename, uint32_t line);

	    explicit GostCryptException(std::string message, const char* function, const char* filename, uint32_t line);

	    ~GostCryptException() noexcept override= default;

	    const char* what() const noexcept override { return compiledmessage.c_str(); };
	protected:
	    std::string msg;
		std::string func;
		std::string file;
		std::string compiledmessage;
        const std::string exceptionName = "GostCryptException";
		uint32_t l;
		
	};

	#define GOSTCRYPTEXCEPTION(msg) GostCryptException(msg, __FUNCTION__, __FILE__, __LINE__)


	BUILD_EXCEPTION(FileNotFoundException, GostCryptException);
	#define FILENOTFOUNDEXCEPTION(file) FileNotFoundException(file, __FUNCTION__, __FILE__, __LINE__)

    BUILD_EXCEPTION(InvalidParameterException, GostCryptException);
    #define INVALIDPARAMETEREXCEPTION(msg) InvalidParameterException(msg, __FUNCTION__, __FILE__, __LINE__)

    BUILD_EXCEPTION(DataNotMutableException, GostCryptException);
    #define DATANOTMUTABLEEXCEPTION() DataNotMutableException("Can't modify data of unmutable ptr", __FUNCTION__, __FILE__, __LINE__)

    BUILD_EXCEPTION(FailedMemoryAllocationException, GostCryptException);
    #define FAILEDMEMORYALLOCATIONEXCEPTION() DataNotMutableException("Can't allocate memory", __FUNCTION__, __FILE__, __LINE__)

    BUILD_EXCEPTION(BufferAlreadyFreedException, GostCryptException);
    #define BUFFERALREADYFREEDEXCEPTION() BufferAlreadyFreedException("Memory already freed", __FUNCTION__, __FILE__, __LINE__)

    BUILD_EXCEPTION(AlgorithmUnititializedException, GostCryptException);
    #define ALGORITHMUNITITILIZEDEXCEPTION() AlgorithmUnititializedException("Algorithm has not been initialized", __FUNCTION__, __FILE__, __LINE__)

    BUILD_EXCEPTION(ProgramTooOldException, GostCryptException);
    #define PROGRAMTOOOLDEXCEPTION() ProgramTooOldException("GostCrypt needs an update to perform this operation.", __FUNCTION__, __FILE__, __LINE__)

    BUILD_EXCEPTION(Unsupported64BitsException, GostCryptException);
    #define UNSUPPORED64BITSEXCEPTION() Unsupported64BitsException("This operation is supported only by the 64Bits version of Gostcrypt", __FUNCTION__, __FILE__, __LINE__)

    BUILD_EXCEPTION(VolumePasswordException, GostCryptException);
    #define VOLUMEPASSWORDEXCEPTION() VolumePasswordException("Could not open volume. Double check the password ?", __FUNCTION__, __FILE__, __LINE__)


}

#endif
