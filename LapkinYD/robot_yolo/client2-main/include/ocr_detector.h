#ifndef OCR_DETECTOR_H
#define OCR_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <string>
#include <vector>

struct OcrResult
{
    std::string text;
    cv::Rect region;
    float confidence;
};

class OcrDetector
{
public:
    OcrDetector();
    ~OcrDetector();

    bool isLoaded() const { return m_ready; }

    std::vector<OcrResult> detect(const cv::Mat &frameRGB);
    void drawResults(cv::Mat &frameRGB, const std::vector<OcrResult> &results);

private:
    tesseract::TessBaseAPI *m_tess = nullptr;
    bool m_ready = false;

    cv::Mat preprocess(const cv::Mat &src);
};

#endif // OCR_DETECTOR_H
