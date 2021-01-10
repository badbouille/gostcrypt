
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
	    const char* name() const noexcept { return exceptionName.c_str(); };
	protected:
	    std::string msg;
		std::string func;
		std::string file;
		std::string compiledmessage;
        const std::string exceptionName = "GostCryptException";
		uint32_t l;
		
	};

	#define GOSTCRYPTEXCEPTION(msg) GostCrypt::GostCryptException(msg, __FUNCTION__, __FILE__, __LINE__)


	BUILD_EXCEPTION(FileNotFoundException, GostCryptException);
	#define FILENOTFOUNDEXCEPTION(file) GostCrypt::FileNotFoundException("File " + std::string(file) + " not found", __FUNCTION__, __FILE__, __LINE__)

    BUILD_EXCEPTION(MountpointNotFoundException, GostCryptException);
    #define MOUNTPOINTNOTFOUNDEXCEPTION(folder) GostCrypt::MountpointNotFoundException("Mountpoint " + folder + " not found. Is the volume mounted?", __FUNCTION__, __FILE__, __LINE__)

    BUILD_EXCEPTION(UmountFailedException, GostCryptException);
    #define UMOUNTFAILEDEXCEPTION(folder) GostCrypt::UmountFailedException("Mountpoint " + folder + " could not be unmounted.", __FUNCTION__, __FILE__, __LINE__)

    BUILD_EXCEPTION(CantCreateFileException, GostCryptException);
    #define CANTCREATEFILEEXCEPTION(file) GostCrypt::CantCreateFileException("File " + std::string(file) + " could not be created", __FUNCTION__, __FILE__, __LINE__)

    BUILD_EXCEPTION(CantOpenFileException, GostCryptException);
    #define CANTOPENFILEEXCEPTION(file) GostCrypt::CantOpenFileException("File " + std::string(file) + " could not be opened", __FUNCTION__, __FILE__, __LINE__)

    BUILD_EXCEPTION(InvalidParameterException, GostCryptException);
    #define INVALIDPARAMETEREXCEPTION(msg) GostCrypt::InvalidParameterException(msg, __FUNCTION__, __FILE__, __LINE__)

    BUILD_EXCEPTION(DataNotMutableException, GostCryptException);
    #define DATANOTMUTABLEEXCEPTION() GostCrypt::DataNotMutableException("Can't modify data of unmutable ptr", __FUNCTION__, __FILE__, __LINE__)

    BUILD_EXCEPTION(FailedMemoryAllocationException, GostCryptException);
    #define FAILEDMEMORYALLOCATIONEXCEPTION() GostCrypt::DataNotMutableException("Can't allocate memory", __FUNCTION__, __FILE__, __LINE__)

    BUILD_EXCEPTION(BufferAlreadyFreedException, GostCryptException);
    #define BUFFERALREADYFREEDEXCEPTION() GostCrypt::BufferAlreadyFreedException("Memory already freed", __FUNCTION__, __FILE__, __LINE__)

    BUILD_EXCEPTION(AlgorithmUnititializedException, GostCryptException);
    #define ALGORITHMUNITITILIZEDEXCEPTION() GostCrypt::AlgorithmUnititializedException("Algorithm has not been initialized", __FUNCTION__, __FILE__, __LINE__)

    BUILD_EXCEPTION(ContainerUninitializedException, GostCryptException);
    #define CONTAINERUNINITIALIZEDEXCEPTION() GostCrypt::ContainerUninitializedException("Container has not been opened", __FUNCTION__, __FILE__, __LINE__)

    BUILD_EXCEPTION(ProgramTooOldException, GostCryptException);
    #define PROGRAMTOOOLDEXCEPTION() GostCrypt::ProgramTooOldException("GostCrypt needs an update to perform this operation.", __FUNCTION__, __FILE__, __LINE__)

    BUILD_EXCEPTION(Unsupported64BitsException, GostCryptException);
    #define UNSUPPORED64BITSEXCEPTION() GostCrypt::Unsupported64BitsException("This operation is supported only by the 64Bits version of Gostcrypt", __FUNCTION__, __FILE__, __LINE__)

    BUILD_EXCEPTION(VolumePasswordException, GostCryptException);
    #define VOLUMEPASSWORDEXCEPTION() GostCrypt::VolumePasswordException("Could not open volume. Double check the password ?", __FUNCTION__, __FILE__, __LINE__)


}

#endif
