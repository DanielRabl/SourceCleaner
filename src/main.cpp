#include <qpl/qpl.hpp>

constexpr auto invalid_directories = std::array{
	".vs", "Debug", "Release", "x64", "x86"
};

void extract_directory_recursive(const qpl::filesys::paths& paths, qpl::size& removed_sum) {
	for (auto& path : paths) {

		bool valid = true;
		if (path.is_directory()) {
			auto name = path.get_full_name();
			for (auto& invalid : invalid_directories) {
				if (name == invalid) {
					valid = false;
					break;
				}
			}
		}

		if (!valid && path.exists()) {
			if (path.is_directory()) {
				removed_sum += path.file_size_recursive();
				path.remove();
			}
			else if (path.is_file()) {
				removed_sum += path.file_size();
				path.remove();
			}
		}
	}
}

void extract_directory(std::string path_string, qpl::size& removed_sum) {
	qpl::filesys::path current_path = path_string;

	if (current_path.is_file()) {
		qpl::println("rejected ", path_string, ": not a directory.");
		return;
	}

	bool valid = false;
	auto check = current_path.list_current_directory_tree();
	for (auto& i : check) {
		if (i.is_file() && i.get_file_extension() == "sln") {
			valid = true;
			break;
		}
	}
	if (!valid) {
		qpl::println("rejected ", path_string, ": no solution file found.");
		return;
	}
	qpl::print("cleaning ", path_string, " . . . ");


	auto paths = current_path.list_current_directory_tree();
	extract_directory_recursive(paths, removed_sum);

	qpl::println("done");
}

int main(int argc, char** argv) try {


	std::vector<std::string> args(argc - 1);
	for (qpl::size i = 0u; i < argc - 1; ++i) {
		args[i] = argv[i + 1];
	}

	qpl::size removed_sum = 0u;
	qpl::clock clock;
	if (argc == 1) {
		auto paths = qpl::filesys::list_current_directory();
		for (auto path : paths) {
			extract_directory(path, removed_sum);
		}
	}
	else {
		for (auto path : args) {
			extract_directory(path, removed_sum);
		}
	}
	auto elapsed = clock.elapsed_str();
	auto tree = qpl::filesys::get_current_location();

	qpl::println();
	qpl::println_repeat("- ", 50);
	qpl::println();
	qpl::println(tree, " has ", qpl::memory_size_string(tree.file_size_recursive()), " size.");
	qpl::println();
	tree.list_current_directory().print_tree();

	qpl::println();
	qpl::println_repeat("- ", 50);
	qpl::println();
	qpl::println("took ", elapsed);
	qpl::println("removed ", qpl::memory_size_string(removed_sum), ".");
	qpl::system_pause();


}
catch (std::exception& any) {
	qpl::println("exception caught: ");
	qpl::println(any.what());
	qpl::system_pause();
}