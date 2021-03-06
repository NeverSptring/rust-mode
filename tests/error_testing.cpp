#include "error.hpp"

#include <sstream>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(error_library_result_concept) {
	rust::Result<char, const char*> someReturnValue =
	                                    rust::Ok<char, const char*>('y'),
	                                someReturnError =
	                                    rust::Err<char, const char*>("error");

	BOOST_CHECK_EQUAL(someReturnValue, (rust::Ok('y')));
	BOOST_CHECK_NE(someReturnValue, (rust::Err("error")));

	BOOST_CHECK_EQUAL(someReturnError, (rust::Err("error")));
	BOOST_CHECK_NE(someReturnError, (rust::Ok('y')));
}

BOOST_AUTO_TEST_CASE(error_library_result_printing) {
	rust::Result<char, const char*> someReturnValue =
	                                    rust::Ok<char, const char*>('y'),
	                                someReturnError =
	                                    rust::Err<char, const char*>("error");

	std::ostringstream osOk, osErr;

	osOk << someReturnValue;
	osErr << someReturnError;

	BOOST_CHECK_EQUAL(osOk.str(), "y");
	BOOST_CHECK_EQUAL(osErr.str(), "Err: error");
}

BOOST_AUTO_TEST_CASE(error_library_option_concept) {
	rust::Option<char> someReturnValue = rust::Some('y'),
	                   someReturnNone = rust::None<char>();

	BOOST_CHECK_EQUAL(someReturnValue, rust::Some('y'));
	BOOST_CHECK_NE(someReturnValue, rust::None<char>());

	BOOST_CHECK_EQUAL(someReturnNone, rust::None());
	BOOST_CHECK_NE(someReturnNone, rust::Some('y'));
}

BOOST_AUTO_TEST_CASE(error_library_result_casting) {
	rust::Result<char, const char*> someReturnValue =
	                                    rust::Ok<char, const char*>('y'),
	                                someReturnError =
	                                    rust::Err<char, const char*>("error");

	char myResult = someReturnValue;
	BOOST_CHECK_EQUAL(myResult, 'y');
	BOOST_CHECK_THROW(char myFailedResult = someReturnError, ErrCastException);
}

BOOST_AUTO_TEST_CASE(error_library_option_casting) {
	rust::Option<char> someReturnValue = rust::Some('y'),
		someReturnNone = rust::None<char>();

	char myResult = someReturnValue;
	BOOST_CHECK_EQUAL(myResult, 'y');
	BOOST_CHECK_THROW(char myFailedResult = someReturnNone, NoneCastException);
}

BOOST_AUTO_TEST_CASE(error_library_result_unwrapping) {
	rust::Result<char, const char*> someReturnValue =
	    rust::Ok<char, const char*>('y');
	BOOST_CHECK_EQUAL(someReturnValue.unwrap(), 'y');

	// Fails:
	//rust::Err<char, const char*>("Error").unwrap();
}

BOOST_AUTO_TEST_CASE(error_library_option_unwrapping) {
	rust::Option<char> someReturnValue = rust::Some('y');
	BOOST_CHECK_EQUAL(someReturnValue.unwrap(), 'y');

	// Fails:
	//rust::None<char>().unwrap();
}

rust::Result<char, char> rTryMacroTest(rust::Result<char, char> arg) {
	rTry(arg);
	return rust::Ok<char, char>('z');
}

rust::Result<char, char> rTryMacroSecondTest(rust::Result<char, char> arg) {
	rTry(rTryMacroTest(arg));
	return rust::Ok<char, char>('x');
}

BOOST_AUTO_TEST_CASE(error_library_try_macro) {
	// Compilation failure
	//rTry(rust::Some('y'));

	BOOST_CHECK_EQUAL((rTryMacroTest(rust::Ok<char, char>('a'))),
	                  (rust::Ok<char, char>('z')));
	BOOST_CHECK_EQUAL((rTryMacroTest(rust::Err<char, char>('n'))),
	                  (rust::Err<char, char>('n')));
	BOOST_CHECK_EQUAL((rTryMacroSecondTest(rust::Ok<char, char>('a'))),
	                  (rust::Ok<char, char>('x')));
	BOOST_CHECK_EQUAL((rTryMacroSecondTest(rust::Err<char, char>('n'))),
	                  (rust::Err<char, char>('n')));
}
