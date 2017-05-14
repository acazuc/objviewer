#ifndef DEBUG_H
# define DEBUG_H

# include <iostream>
# include <unistd.h>
# include <typeinfo>

# undef ERROR

# define LOG(s) {std::cout << "[LOG] " << s << " (" << __FILE__ << ':' << __LINE__ << ')' << std::endl;}
# define WARN(s) {std::cerr << "[WARNING] " << s << " (" << __FILE__ << ':' << __LINE__ << ')' << std::endl;}
# define EXCEPTION(s) {std::cout << "[EXCEPTION] " << typeid(s).name() << ": " << s.what() << " (" << __FILE__ << ':' << __LINE__ << ')' << std::endl;}
# define ERROR(s) {std::cout << "[ERROR] " << s << " (" << __FILE__ << ':' << __LINE__ << ')' << std::endl;exit(EXIT_FAILURE);}

#endif
