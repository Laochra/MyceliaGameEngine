#include <iostream>

#include "Application.h" // Replace with chosen application

int main(char** args)
{
	Application* application = new Application();

	return application->Run(); // Run returns when the application has closed
}