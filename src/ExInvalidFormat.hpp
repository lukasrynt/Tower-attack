/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 26/05/2020.
*/

#pragma once

#include <stdexcept>

/**
 * Exception when user has entered invalid input
 */
class invalid_format : public std::runtime_error
{
public:
	explicit invalid_format(const char * message)
			: runtime_error(std::string("Invalid file format: ") + message)
	{}
};