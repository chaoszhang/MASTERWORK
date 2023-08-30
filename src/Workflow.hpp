#include "VersionControl.hpp"
#include "OutputStream.hpp"
#include "ArgParser.hpp"

struct Workflow {
	static void initialize() {
		VERSION_CONTROL.setSharedCategory("Workflow");

		/*
		VERSION_CONTROL.addUpdate("2023-08-30", "TEST2");
		VERSION_CONTROL.addUpdate("2023-08-31", "TEST3");
		VERSION_CONTROL.addUpdate("2023-07-31", "TEST1");
		VERSION_CONTROL.setUniqueCategory("test");
		VERSION_CONTROL.addUpdate("2023-08-28", "test");
		*/
	}


} WORKFLOW;

int main(int argc, char** argv) {
	try {
		Workflow::initialize();
		ArgParser::initialize();

		LOG << CONFIG::FULL_NAME << std::endl;
		LOG << CONFIG::PROGRAM_NAME << " " << VERSION_CONTROL.getVersion() << std::endl;
		
		//LOG << VERSION_CONTROL.getHistory();
	}
	catch (...){
		ERROR << "Error in static initializations.\n";
		exit(1);
	}

	DEBUG << "Hello world!";
	return 0;
}