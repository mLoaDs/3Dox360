#ifndef _3DOX_EXCEPTIONS_H_
#define _3DOX_EXCEPTIONS_H_

class BadBiosRomException : public std::exception {};
class BadGameRomException : public std::exception {};
class BadNvRamFileException : public std::exception {};

#endif // _3DOX_EXCEPTIONS_H_