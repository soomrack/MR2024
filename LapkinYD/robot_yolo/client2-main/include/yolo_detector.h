#ifndef YOLO_DETECTOR_H
#define YOLO_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <string>
#include <vector>

struct Detection
{
    int classId;
    float confidence;
    cv::Rect bbox;
};

class YoloDetector
{
public:
    YoloDetector(const std::string &modelPath,
                 float confThreshold = 0.25f,
                 float nmsThreshold = 0.45f);

    bool isLoaded() const { return m_loaded; }

    std::vector<Detection> detect(const cv::Mat &frameRGB);
    void drawDetections(cv::Mat &frameRGB,
                        const std::vector<Detection> &detections);

    const std::string &className(int id) const;

private:
    cv::dnn::Net m_net;
    bool m_loaded = false;
    float m_confThresh;
    float m_nmsThresh;
    static constexpr int INPUT_W = 640;
    static constexpr int INPUT_H = 640;
    std::vector<std::string> m_classNames;

    void loadClassNames();
    std::vector<Detection> postprocess(const cv::Mat &frameRGB,
                                       const std::vector<cv::Mat> &outputs);
    cv::Scalar classColor(int classId) const;
};

#endif // YOLO_DETECTOR_H
