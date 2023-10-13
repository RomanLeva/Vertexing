#include <iostream>

#include "AppLogic.hpp"
#include "../downloaders/IDownloader.hpp"
#include "../vertex_search/VertexSearcher.hpp"
#include "../downloaders/FedraDownloader.hpp"

#include <memory>

using namespace std;

int main(int argc, char **argv)
{
    cout << "Starting application...\n";

    unique_ptr<IDownloader> downloader(std::make_unique<FedraDownloader>());

    unique_ptr<AppLogic> myApp(std::make_unique<AppLogic>(std::move(downloader)));
    myApp->findVertexes();

    exit(0);
}
