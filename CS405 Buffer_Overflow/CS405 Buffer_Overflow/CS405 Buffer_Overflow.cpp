// Andrew Nelson
//CS405 Buffer Overflow

#include <iostream>
#include <string>
#include <stdexcept>  // Include the standard exception library for runtime_error


int main()
{
	std::cout << "Buffer Overflow Example" << std::endl;

    // Constant account number
    const std::string account_number = "CharlieBrown42";

    // Input buffer (max 19 chars + null terminator)
    char user_input[20];

    // Loop until valid input
    while (true) { 
        try {
            std::cout << "\nEnter a value (max 19 characters): ";
            std::cin.getline(user_input, sizeof(user_input)); // Read input safely -- Built in overflow protection and handling



            if (std::cin.fail()) { // Check for buffer overflow
                throw std::runtime_error("BUFFER OVERFLOW: Too many input characters!");
            }

            if (user_input[0] == '\0') { // Check if input is empty
                throw std::runtime_error("INPUT ERROR: No input provided!");
            }

            break; // Exit loop if input is valid
        }
        catch (const std::runtime_error& e) { // Handle overflow error
            std::cerr << "    \nERROR: " << e.what() << std::endl;
            std::cin.clear(); // Reset input state
            std::cin.ignore(10000, '\n'); // Discard excess input
            std::cout << "Please enter a shorter value.\n";
        }
    }

    std::cout << "You entered: " << user_input << std::endl;
    std::cout << "Account Number = " << account_number << std::endl;

    return 0; // Program exits successfully
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
