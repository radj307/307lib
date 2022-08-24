#pragma once
/**
 * @example	event_example.cpp
 *			This is an example of how to use the <event.hpp> header.
 */
#include <event.hpp>

#include <string>

using namespace shared;

struct myEventArgs : EventArgs {
	bool state;
	std::string name;

	myEventArgs(const bool state, const std::string& name) : state{ state }, name{ name } {}
};

struct MeaninglessIndirection {

	// A static event with default arguments
	static Event<EventArgs> onInstanceCreated;

	// Define an event that uses the 'basic_event_args' argument type
	Event<EventArgs> onDestruction;
	Event<myEventArgs> onContainsPunctuationCalled;

	MeaninglessIndirection()
	{
		// Trigger the `onInstanceCreated` event:
		onInstanceCreated.notify(this);
	}

	~MeaninglessIndirection()
	{
		// Trigger the `onDestruction` event:
		onDestruction.notify(this);
	}


	void ContainsPunctuation(const std::string& name) const
	{
		// check if 'name' contains any punctuation characters:
		const bool nameContainsPunctation{ false };
		for (const auto& ch : name) {
			if (ispunct(ch)) {
				nameContainsPunctation = true;
				break;
			}
		}
		// notify the 'onContainsPunctuationCalled' event with the results:
		onCall.notify(nameContainsPunctation, name);
	}

};

inline void ExampleHandlerFunction(void* sender, EventArgs* const ev_args)
{
	if (auto* args = (myEventArgs* const)ev_args) {
		if (state)
			std::cout << args->name << std::endl;
		else
			std::cerr << args->name << std::endl;

		args->handled = args->state;
	}
}

void example_use()
{
	MeaninglessIndirection::onInstanceCreated.add(CallbackEventHandler([](void* sender, EventArgs* const ev_args) {	//< Create an rvalue callback handler using a lambda.
		std::cout << "New Instance of type `MeaninglessIndirection` created." << std::endl;
	}));

	MeaninglessIndirection foo;																			//< STDOUT receives: "New Instance of type `MeaninglessIndirection` created."

	foo.onContainsPunctuationCalled += CallbackEventHandler(ExampleHandlerFunction);

	foo.call("Hello");																					//< ExampleHandlerFunction is called

	foo.call("World!");
}																										//< The lambda callback handler we added to the onInstanceCreated event is removed because it goes out-of-scope.
