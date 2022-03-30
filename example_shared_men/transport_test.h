#ifndef IMAGE_TRANSPORT_TEST_H
#define IMAGE_TRANSPORT_TEST_H

#include <string>

void send_image_test(const std::string load_videopath, double send_fps);

void receive_image_test(const std::string &safe_videopath, int save_fps);

#endif