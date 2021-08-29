// zpp.cpp : Defines the entry point for the application.
//

#include "zpp.h"

using namespace std;

int main()
{
	std::cout << std::filesystem::current_path();
	zipFileWriter z("../../../outf.zip");
	z.addFolder("../../../out/");
	z.flush();

	zipFileReader r("../../../outf.zip");
	r.extractAll("../../../temp/");
	r.extractFirstToFile("../../../temp/test.txt");
	auto list = r.getFilesList();
	for (auto e = list.begin(); e != list.end(); e++) {
		std::cout << e->c_str() << "\n";
	}
	return 0;
}

