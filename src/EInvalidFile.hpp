#include <stdexcept>

/**
 * @author Lukáš Rynt <ryntluka@fit.cvut.cz>
 * @date 25.4.2020
 */

class invalid_file : public std::runtime_error
{
public:
	explicit invalid_file(const char * message)
			: runtime_error(std::string("Invalid file: ") + message)
	{}
};