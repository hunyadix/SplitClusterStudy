#include <iostream>
#include <exception>

int main(int argc, char** argv)
{
	try
	{
		std::cout << "PlotEventClusters started." << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cout << "An error occured: " << e.what() << std::endl;
	}
	return 0; 
}
