
#include "GostCryptException.h"

#include <sstream>
#include <cstring>

const char* GostCrypt::GostCryptException::what() const noexcept {
	std::stringstream output;
	output << exceptionName;
	output << " in " << file;
	output << "::" << func;
	output << "::" << l;
	output << " \t" << msg;

	// Note: Exception::what() should not build a message but instead should have a predefined message ready to be sent.
	// In GostCrypt, we usually need a lot of parameters and building the exception here is VERY practical.
	// The huge drawback is that the ressource is never freed.
	// TODO: move this code in the Constructor maybe, so this function returns only a pointer.

	char *message = new char[strlen(output.str().c_str())+1];
	strcpy(message, output.str().c_str());

	return message;
}
