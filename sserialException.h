#ifndef SOLAREXCEPTION_H
#define SOLAREXCEPTION_H

#include <exception>


class sserialException : public exception
{
public:
	/** Default constructor */
	sserialException() {}
	sserialException(char* error)
	{
		this->error = error;
	}
	sserialException(const sserialException& other)
	{
		this->error = other.error;
	}
	sserialException& operator=(const sserialException& other)
	{
		if(this == &other) return *this;  // handle self assignment
		this->error = other.error;
		return *this;
	}
	ostream& operator<<(ostream& os)
	{
		os << error;
		return os;
	}
	const char* error;
protected:
private:
};

#endif // SOLAREXCEPTION_H
