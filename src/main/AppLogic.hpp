#pragma once
#include "../downloaders/IDownloader.hpp"

#include <memory>

class AppLogic
{
private:
    std::unique_ptr<IDownloader> downloader;

public:
    void findVertexes();

public:
    AppLogic(std::unique_ptr<IDownloader> downloader);

    virtual ~AppLogic() {}

    AppLogic(const AppLogic &) = delete;
    AppLogic &operator=(const AppLogic &) = delete;
    AppLogic(const AppLogic &&) = delete;
    AppLogic &operator=(const AppLogic &&) = delete;
};