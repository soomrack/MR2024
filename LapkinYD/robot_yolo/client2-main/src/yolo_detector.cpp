#include "yolo_detector.h"
#include <iostream>

// COCO 80 classes
static const std::vector<std::string> COCO_CLASSES = {
    "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat",
    "traffic light", "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat",
    "dog", "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "backpack",
    "umbrella", "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard", "sports ball",
    "kite", "baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket",
    "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple",
    "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair",
    "couch", "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse",
    "remote", "keyboard", "cell phone", "microwave", "oven", "toaster", "sink",
    "refrigerator", "book", "clock", "vase", "scissors", "teddy bear", "hair drier",
    "toothbrush"};

YoloDetector::YoloDetector(const std::string &modelPath,
                           float confThreshold,
                           float nmsThreshold)
    : m_confThresh(confThreshold), m_nmsThresh(nmsThreshold)
{
    m_classNames = COCO_CLASSES;

    try
    {
        m_net = cv::dnn::readNetFromONNX(modelPath);
        if (m_net.empty())
        {
            std::cerr << "Yolo log: Failed to load model: " << modelPath << std::endl;
            return;
        }

        // check cuda
        m_net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        m_net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

        m_loaded = true;
        std::cout << "Yolo log: Model loaded: " << modelPath << std::endl;
    }
    catch (const cv::Exception &e)
    {
        std::cerr << "Yolo log: OpenCV exception: " << e.what() << std::endl;
    }
}

const std::string &YoloDetector::className(int id) const
{
    static const std::string unknown = "unknown";
    if (id < 0 || id >= (int)m_classNames.size())
        return unknown;
    return m_classNames[id];
}

cv::Scalar YoloDetector::classColor(int classId) const
{
    static const std::vector<cv::Scalar> palette = {
        {255, 56, 56}, {255, 157, 151}, {255, 112, 31}, {255, 178, 29}, {207, 210, 49}, {72, 249, 10}, {146, 204, 23}, {61, 219, 134}, {26, 147, 52}, {0, 212, 187}, {44, 153, 168}, {0, 194, 255}, {52, 69, 147}, {100, 115, 255}, {0, 24, 236}, {132, 56, 255}, {82, 0, 133}, {203, 56, 255}, {255, 149, 200}, {255, 55, 199}};
    return palette[classId % (int)palette.size()];
}

std::vector<Detection> YoloDetector::detect(const cv::Mat &frameRGB)
{
    if (!m_loaded)
        return {};

    cv::Mat blob;
    cv::dnn::blobFromImage(frameRGB, blob,
                           1.0 / 255.0,
                           cv::Size(INPUT_W, INPUT_H),
                           cv::Scalar(0, 0, 0),
                           /*swapRB=*/false,
                           /*crop=*/false);

    m_net.setInput(blob);

    std::vector<cv::Mat> outputs;
    m_net.forward(outputs, m_net.getUnconnectedOutLayersNames());

    return postprocess(frameRGB, outputs);
}

std::vector<Detection> YoloDetector::postprocess(const cv::Mat &frameRGB,
                                                 const std::vector<cv::Mat> &outputs)
{
    cv::Mat raw = outputs[0];
    raw = raw.reshape(1, raw.size[1]);
    cv::Mat data;
    cv::transpose(raw, data);

    const float scaleX = (float)frameRGB.cols / INPUT_W;
    const float scaleY = (float)frameRGB.rows / INPUT_H;

    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    for (int i = 0; i < data.rows; ++i)
    {
        const float *row = data.ptr<float>(i);

        cv::Mat scores(1, (int)m_classNames.size(), CV_32F,
                       const_cast<float *>(row + 4));
        cv::Point classIdPoint;
        double maxScore;
        cv::minMaxLoc(scores, nullptr, &maxScore, nullptr, &classIdPoint);

        if (maxScore < m_confThresh)
            continue;

        float cx = row[0] * scaleX;
        float cy = row[1] * scaleY;
        float w = row[2] * scaleX;
        float h = row[3] * scaleY;

        int x = (int)(cx - w / 2.0f);
        int y = (int)(cy - h / 2.0f);

        classIds.push_back(classIdPoint.x);
        confidences.push_back((float)maxScore);
        boxes.push_back(cv::Rect(x, y, (int)w, (int)h));
    }

    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, m_confThresh, m_nmsThresh, indices);

    std::vector<Detection> result;
    result.reserve(indices.size());
    for (int idx : indices)
    {
        Detection d;
        d.classId = classIds[idx];
        d.confidence = confidences[idx];
        d.bbox = boxes[idx];
        result.push_back(d);
    }
    return result;
}

void YoloDetector::drawDetections(cv::Mat &frameRGB,
                                  const std::vector<Detection> &detections)
{
    for (const auto &det : detections)
    {
        const cv::Scalar color = classColor(det.classId);
        const cv::Rect &box = det.bbox;

        cv::Rect clipped = box & cv::Rect(0, 0, frameRGB.cols, frameRGB.rows);
        if (clipped.empty())
            continue;

        cv::rectangle(frameRGB, clipped, color, 2);
        std::string label = className(det.classId) +
                            " " +
                            std::to_string((int)(det.confidence * 100)) + "%";

        int baseLine = 0;
        cv::Size textSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX,
                                            0.6, 1, &baseLine);

        int labelY = std::max(clipped.y, textSize.height + 4);
        cv::Rect labelRect(clipped.x, labelY - textSize.height - 4,
                           textSize.width + 4, textSize.height + baseLine + 4);
        labelRect &= cv::Rect(0, 0, frameRGB.cols, frameRGB.rows);

        cv::rectangle(frameRGB, labelRect, color, cv::FILLED);

        cv::putText(frameRGB, label,
                    cv::Point(clipped.x + 2, labelY - 2),
                    cv::FONT_HERSHEY_SIMPLEX, 0.6,
                    cv::Scalar(0, 0, 0), 1, cv::LINE_AA);
    }
}
