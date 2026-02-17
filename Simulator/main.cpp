#include "Simulator.h"

#include <error.h>
#include <iostream>

int main()
{
	// Create an instance of the Simulator
	Simulator simulator;

	// Initialize the simulator
	if (!simulator.init())
	{
		std::cerr << "Failed to initialize the simulator." << std::endl;
		return 1;
	}

	// Start the simulator
	simulator.start();

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::hours(24));
	}
	return 0;
}