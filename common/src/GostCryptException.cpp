
#include "GostCryptException.h"

#include <sstream>

const char* GostCrypt::GostCryptException::what() const noexcept {
	std::stringstream output;
	output << exceptionName;
	output << " in " << file;
	output << "::" << func;
	output << "::" << l;
	output << " \t" << msg;
	return output.str().c_str();
}
