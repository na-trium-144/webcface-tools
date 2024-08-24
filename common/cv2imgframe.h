#include <webcface/image_frame.h>
#include <opencv2/core.hpp>

webcface::ImageFrame cv2ImgFrame(cv::Mat &img_mat) {
    assert(img_mat.depth() == CV_8U);
    webcface::ImageFrame img_frame;
    // https://stackoverflow.com/questions/26681713/convert-mat-to-array-vector-in-opencv
    if (img_mat.isContinuous()) {
        img_frame =
            webcface::ImageFrame(webcface::sizeHW(img_mat.rows, img_mat.cols),
                                 img_mat.data, webcface::ImageColorMode::bgr);
    } else {
        img_frame =
            webcface::ImageFrame(webcface::sizeHW(img_mat.rows, img_mat.cols),
                                 webcface::ImageColorMode::bgr);
        for (int i = 0; i < img_mat.rows; ++i) {
            std::memcpy(&img_frame.at(i, 0, 0), img_mat.ptr<unsigned char>(i),
                        img_mat.cols * img_mat.channels());
        }
    }
    return img_frame;
}
