#include <memory>
#include <string>
#include <iostream>
#include <cstdlib> // for system()
#include <cstdint> // for uint32_t
#include <chrono>  // for currentYear() implementation

//------------------- User includes ----------------------------

// A class that represents a form.
// As you will see below: many fields and many validators can be added to the form
#include "Form.h"

// A class that represents a field. 
// As you will see below: (at least) one validator can be added to each field 
#include "Field.h"

// A class that represents a range validator.
// It validates if a field value is in some range.
// The type used as template parameter must implement < and > operators.
#include "RangeValidator.h"

// A class that represents a no-digit-characters validator.
// It validates that the field value contains no digits.
// Works only on std::string.
#include "NoDigitValidator.h"

// A class that represents an ID validator.
// ID validation is done using the control digit.
// Works only on the type 'uint32_t'.
#include "IdValidator.h"

// A class that represents a value and his name.
// The class must override the << and >> operators.
// >> operator reads an int and << operator prints destination name if the value is
// valid and if not - the number.
#include "ValuesToNames.h"

// A class that keeps the names of the different destinations
#include "DestinationNames.h"

// A class that keeps the names of the different classes
#include "FlightTimes.h"

// A class that keeps the names of the different currencies
#include "WifiBundle.h"

// A class that represents a destination-vs.-class validator.
// It checks if the destination and class supplied matches each other.
// The types used as template parameter must be Field.
#include "DestinationToFlightTimeValidator.h"

// A class that represents a destination-vs.-WIFI-bundle validator.
// It checks if the destination and WIFI-bundle supplied matches each other.
// The types used as template parameter must be Field.
#include "DestinationToWifiBundleValidator.h"

//------------------- Function declarations ----------------------------

// Gets the welcome message
std::string getWelcomeMessage();

// Gets the goodbye message
std::string getGoodbyeMessage();

// Gets the error message
std::string getErrorMessage();

// Utility function for clearing the terminal screen
void clearScreen();

// Utility function for getting current year from computer clock
int currentYear();

//------------------- consts --------------------------

constexpr int MIN_AGE = 15;
constexpr int MAX_AGE = 120;

//------------------- main ----------------------------

int main()
{
	// Creating the form fields
	auto nameField = std::make_unique<Field<std::string>>("What is your name?");
	auto idField = std::make_unique<Field<uint32_t>>("What is your ID?");
	auto yearOfBirthField = std::make_unique<Field<int>>("What is your year of birth?");
	auto destinationNames = ValuesToNames<DestinationNames>();
	auto destinationField = std::make_unique<Field<ValuesToNames<DestinationNames>>> ("What is your flight destination?\n" + destinationNames.valuesAndNames());
	auto flightTimes = ValuesToNames<FlightTimes>();
	auto flightTimeField = std::make_unique<Field<ValuesToNames<FlightTimes>>>("What is your desired flight time range?\n" + flightTimes.valuesAndNames());
	auto wifiBundleNames = ValuesToNames<WifiBundle>();
	auto wifiBundleField = std::make_unique<Field<ValuesToNames<WifiBundle>>>("What is your desired WIFI bundle?\n" + wifiBundleNames.valuesAndNames());

	// Creating the field validators
	auto nameValidator = std::make_unique<NoDigitValidator>();
	auto idValidator = std::make_unique<IdValidator>();
	auto ageValidator = std::make_unique<RangeValidator<int>>(currentYear() - MAX_AGE, currentYear() - MIN_AGE);
	auto destinationValidator = std::make_unique<RangeValidator<ValuesToNames<DestinationNames>>>(1, 5);
	auto classValidator = std::make_unique<RangeValidator<ValuesToNames<FlightTimes>>>(1,3);
	auto wifiBundleValidator = std::make_unique<RangeValidator<ValuesToNames<WifiBundle>>>(1,3);

	// Adding the validators to the fields
	nameField->addValidator(nameValidator.get());
	idField->addValidator(idValidator.get());
	yearOfBirthField->addValidator(ageValidator.get());
	destinationField->addValidator(destinationValidator.get());
	flightTimeField->addValidator(classValidator.get());
	wifiBundleField->addValidator(wifiBundleValidator.get());

	// Creating form validators
	auto destinationToClassValidator
		= std::make_unique<DestinationToFlightTimeValidator<Field<ValuesToNames<DestinationNames>>, Field<ValuesToNames<FlightTimes>>>>(destinationField.get(), flightTimeField.get());
	auto destinationToWifiBundleValidator
		= std::make_unique<DestinationToWifiBundleValidator<Field<ValuesToNames<DestinationNames>>, Field<ValuesToNames<WifiBundle>>>>(destinationField.get(), wifiBundleField.get());

	// Creating the form and adding the fields to it
	Form myForm;
	myForm.addField(nameField.get());
	myForm.addField(idField.get());
	myForm.addField(yearOfBirthField.get());
	myForm.addField(destinationField.get());
	myForm.addField(flightTimeField.get());
	myForm.addField(wifiBundleField.get());

	// Adding form validators
	myForm.addValidator(destinationToClassValidator.get());
	myForm.addValidator(destinationToWifiBundleValidator.get());

	// Getting the information from the user
	clearScreen();
	std::cout << getWelcomeMessage();

	// Get the input only for empty or not valid fields
	myForm.fillForm();

	// Validation loop
	for (auto formIsCorrect = myForm.validateForm(); !formIsCorrect; formIsCorrect = myForm.validateForm())
	{
		// Displays all form fields with value (and error if not valid)
		clearScreen();
		std::cout << getErrorMessage();
		std::cout << myForm << '\n';

		myForm.fillForm();
	}

	clearScreen();
	std::cout << getGoodbyeMessage();
	std::cout << myForm << '\n';
	return 0;
}

std::string getWelcomeMessage()
{
	return "+----------------------------------------------------------+\n"
		"|                  Hello and welcome!                      |\n"
		"|  In order to register please fill in the fields below    |\n"
		"+----------------------------------------------------------+\n";
}

std::string getErrorMessage()
{
	return "+----------------------------------------------------------+\n"
		"|     There was an error in at least one of the fields!    |\n"
		"|                Please correct the error(s)               |\n"
		"+----------------------------------------------------------+\n";
}

std::string getGoodbyeMessage()
{
	return "+----------------------------------------------------------+\n"
		"|                      Thank you!                          |\n"
		"|               This is the data you sent:                 |\n"
		"+----------------------------------------------------------+\n";
}

void clearScreen()
{
	system("cls");
}

int currentYear()
{
	namespace ch = std::chrono;
	auto ymd = ch::year_month_day(ch::floor<ch::days>(ch::system_clock::now()));
	return static_cast<int>(ymd.year());
}
