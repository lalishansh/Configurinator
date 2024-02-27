#include <cstdlib>

import Configurinator.Model;
import Configurinator.ParseArguments;
import Configurinator.Frontend;
import Configurinator.Backend;

int main(const int argc, const char* const argv[]) {
	Configurinator::Model model_data;
	if (! Configurinator::ParseArguments(std::span{argv, static_cast<size_t>(argc)}, model_data)) {
		return EXIT_FAILURE;
	}

	std::thread ui_thread = Configurinator::SpinFrontendProcess(model_data);
	std::thread exec_thread = Configurinator::SpinBackendProcess(model_data);

	ui_thread.join();
	exec_thread.join();

	return 0;
}
