#pragma once

struct VerifyException : std::runtime_error
{
   VerifyException(const char * msg) : std::runtime_error(msg) {}
};

#define Verify(x) do { if (!(x)) { std::cerr << "Assertion " << #x << " failed at function " << __FUNCTION__ << " at file " << __FILE__ << ":" << __LINE__ << std::endl << std::flush; throw VerifyException(#x); } } while ( 0 )
