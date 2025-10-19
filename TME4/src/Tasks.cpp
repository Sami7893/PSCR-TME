#include "Tasks.h"
#include "util/ImageUtils.h"
#include "util/thread_timer.h"
#include <thread>
#include <sstream>

namespace pr {

void treatImage(FileQueue& fileQueue, const std::filesystem::path& outputFolder) {
    // measure CPU time in this thread
    pr::thread_timer timer;
    
    while (true) {
        std::filesystem::path file = fileQueue.pop();
        if (file == pr::FILE_POISON) break; // poison pill
        QImage original = pr::loadImage(file);
        if (!original.isNull()) {
            QImage resized = pr::resizeImage(original);
            std::filesystem::path outputFile = outputFolder / file.filename();
            pr::saveImage(resized, outputFile);
        }
    }

    // trace
    std::stringstream ss;
    ss << "Thread " << std::this_thread::get_id() << " (treatImage): " << timer << " ms CPU" << std::endl;
    std::cout << ss.str();
}



void reader(FileQueue& fileQueue, ImageTaskQueue& imageQueue) {
    pr::thread_timer timer;
    while (true) {
        std::filesystem::path file = fileQueue.pop();
        if (file == pr::FILE_POISON) break;
        QImage original = pr::loadImage(file);
        if (!original.isNull()) {
            imageQueue.push(TaskData (file,original,false));
        }
    }
    std::stringstream ss;
    ss << "Thread " << std::this_thread::get_id() << " (reader): " << timer << " ms CPU" << std::endl;
    std::cout << ss.str();
}

void resizer(ImageTaskQueue& imageQueue, ImageTaskQueue& resizedQueue) {
    pr::thread_timer timer;
    while (true) {
        TaskData task = imageQueue.pop();
        if (task.poison == true) break;
        if (!task.image.isNull()) {
            QImage resized = pr::resizeImage(task.image);
            resizedQueue.push(TaskData (task.file,resized,false));
        }
    }
    std::stringstream ss;
    ss << "Thread " << std::this_thread::get_id() << " (resizer): " << timer << " ms CPU" << std::endl;
    std::cout << ss.str();
}

void saver(ImageTaskQueue& resizedQueue, const std::filesystem::path& outputFolder) {
    pr::thread_timer timer;
    while (true) {
        TaskData resized = resizedQueue.pop();
        if (resized.poison == true) break;
        if (!resized.image.isNull()) {
            std::filesystem::path outputFile = outputFolder / resized.file.filename();
            pr::saveImage(resized.image, outputFile);
        }
    }
    std::stringstream ss;
    ss << "Thread " << std::this_thread::get_id() << " (saver): " << timer << " ms CPU" << std::endl;
    std::cout << ss.str();
}



} // namespace pr