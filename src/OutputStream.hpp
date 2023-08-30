#define DEBUG if(DEBUG_MODE) VERBOSE
#include <iostream>

struct OutputStream {
	bool enabled = true, needDeletion = false;
	std::ostream* pout = nullptr;

	OutputStream(std::ostream& fout = std::cerr) : pout(&fout) {}

	OutputStream(std::ostream* pout) : pout(pout), needDeletion(true) {}

	template<class T> OutputStream& operator<< (const T& v) {
		if (enabled) (*pout) << v;
		return *this;
	}

	OutputStream& operator<< (std::basic_ostream<char, std::char_traits<char> >& (*endl)(std::basic_ostream<char, std::char_traits<char> >&)) {
		if (enabled) (*pout) << endl;
		return *this;
	}

	~OutputStream() {
		if (needDeletion && pout != nullptr) delete pout;
	}

} OUTPUT(nullptr), ERROR, WARNING, LOG, VERBOSE;