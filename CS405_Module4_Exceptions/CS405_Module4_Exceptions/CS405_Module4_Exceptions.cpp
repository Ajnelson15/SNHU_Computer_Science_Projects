//Andrew Nelson
//CS405 Exceptions

// Exceptions.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <exception> // Needed for std::exception class
#include <stdexcept> // Needed for std::runtime_error and std::invalid_argument


// Added A Custom Class that inherits the std::exception class
class CustomException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Custom Exception Occurred";
    }
};


// Throw a standard exception as required
bool do_even_more_custom_application_logic()
{
    // Throws Standard Runtime Error
    throw std::runtime_error("Runtime Error");

    return true;
}
void do_custom_application_logic()
{
 
    //Wrapped application logic in a try/catch block to handle standard exceptions
    try {
        if (do_even_more_custom_application_logic())
        {
            std::cout << "Even More Custom Application Logic Succeeded." << std::endl;
        }
    }
    // Handles standard exception and implements what() function for output
    catch (const std::exception& e) {
        std::cout << "Caught std::exception: " << e.what() << std::endl;
    }
    // Throws custom exception to be handled in main();
    throw CustomException();

}

// Check for division by zero and throws exception
float divide(float num, float den)
{
    // Division by Zero
    if (den == 0) {
        throw std::invalid_argument("Cannot Divide by Zero"); 
    }
    // Safe division
    return (num / den);
}

void do_division()
{

    float numerator = 10.0f;
    float denominator = 0;

    //Added try/catch for a specific exception
    try {
        auto result = divide(numerator, denominator);
        std::cout << "divide(" << numerator << ", " << denominator << ") = " << result << std::endl;
    }

    catch (const std::invalid_argument& e) {
        std::cerr << "Division Error Found: " << e.what() << std::endl;
    }
}

int main()
{
    // Added handlers to catch exception in specific order (Custom, Standard, and Catch-All)
    try {
        std::cout << "Exceptions Tests!" << std::endl;
        do_division();
        do_custom_application_logic();
    }
    // Catches custom exceptions
    catch (const CustomException& ce) {
        std::cerr << "Custom Exception in main: " << ce.what() << std::endl;
    }
    // Catches standard exceptions
    catch (const std::exception& e) {
        std::cerr << "std::exception in main: " << e.what() << std::endl;
    }
    // Catches all other exceptions
    catch (...) {
        std::cerr << "Unexpected exception in main" << std::endl;
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu