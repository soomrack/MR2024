#include "ocr_detector.h"
#include <iostream>

OcrDetector::OcrDetector()
{
    m_tess = new tesseract::TessBaseAPI();

    if (m_tess->Init(nullptr, "eng") != 0)
    {
        std::cerr << "OCR log: Failed to init Tesseract\n";
        delete m_tess;
        m_tess = nullptr;
        return;
    }

    m_tess->SetVariable("tessedit_char_whitelist", "0123456789");
    m_tess->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);

    m_ready = true;
    std::cout << "OCR log: Tesseract ready (yellow sticker digit mode)\n";
}

OcrDetector::~OcrDetector()
{
    if (m_tess)
    {
        m_tess->End();
        delete m_tess;
    }
}

static std::vector<cv::Rect> findYellowRegions(const cv::Mat &frameRGB)
{
    cv::Mat hsv;
    cv::cvtColor(frameRGB, hsv, cv::COLOR_RGB2HSV);

    cv::Mat mask;
    cv::inRange(hsv,
                cv::Scalar(20, 100, 100),
                cv::Scalar(35, 255, 255),
                mask);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<cv::Rect> regions;
    for (const auto &c : contours)
    {
        double area = cv::contourArea(c);
        if (area < 500 || area > 100000)
            continue;

        cv::Rect r = cv::boundingRect(c);
        int pad = 5;
        r.x = std::max(0, r.x - pad);
        r.y = std::max(0, r.y - pad);
        r.width = std::min(frameRGB.cols - r.x, r.width + 2 * pad);
        r.height = std::min(frameRGB.rows - r.y, r.height + 2 * pad);

        regions.push_back(r);
    }

    return regions;
}

cv::Mat OcrDetector::preprocess(const cv::Mat &src)
{
    cv::Mat gray, thresh;

    cv::cvtColor(src, gray, cv::COLOR_RGB2GRAY);
    cv::resize(gray, gray, cv::Size(), 3.0, 3.0, cv::INTER_CUBIC);
    cv::threshold(gray, thresh, 0, 255,
                  cv::THRESH_BINARY_INV + cv::THRESH_OTSU);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
    cv::dilate(thresh, thresh, kernel);

    return thresh;
}

std::vector<OcrResult> OcrDetector::detect(const cv::Mat &frameRGB)
{
    std::vector<OcrResult> results;
    if (!m_ready)
    {
        std::cout << "OCR log: Not ready!\n"
                  << std::flush;
        return results;
    }

    auto regions = findYellowRegions(frameRGB);
    std::cout << "OCR log: Yellow regions found: " << regions.size() << "\n"
              << std::flush;

    for (const auto &reg : regions)
    {
        cv::Mat roi = frameRGB(reg);
        cv::Mat processed = preprocess(roi);

        m_tess->SetImage(processed.data,
                         processed.cols,
                         processed.rows,
                         1,
                         processed.cols);
        m_tess->Recognize(nullptr);

        const char *text = m_tess->GetUTF8Text();
        float conf = m_tess->MeanTextConf();

        std::cout << "[OCR] Region " << reg << " raw text: '"
                  << (text ? text : "NULL") << "' conf=" << conf << "\n"
                  << std::flush;

        if (text && conf > 20.0f)
        {
            std::string t(text);
            t.erase(remove_if(t.begin(), t.end(),
                              [](char c)
                              { return c == '\n' || c == ' ' || c == '\r'; }),
                    t.end());

            if (!t.empty())
            {
                OcrResult r;
                r.text = t;
                r.confidence = conf;
                r.region = reg;
                results.push_back(r);
            }
        }
        delete[] text;
    }

    return results;
}

void OcrDetector::drawResults(cv::Mat &frameRGB, const std::vector<OcrResult> &results)
{
    for (const auto &r : results)
    {
        cv::rectangle(frameRGB, r.region, cv::Scalar(255, 140, 0), 3);

        std::string label = "NUM: " + r.text +
                            " (" + std::to_string((int)r.confidence) + "%)";

        int baseLine = 0;
        cv::Size ts = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.7, 2, &baseLine);

        cv::Point textPos(r.region.x, r.region.y - 8);
        if (textPos.y < 20)
            textPos.y = r.region.y + r.region.height + 25;

        cv::rectangle(frameRGB,
                      cv::Point(textPos.x - 2, textPos.y - ts.height - 4),
                      cv::Point(textPos.x + ts.width + 2, textPos.y + baseLine),
                      cv::Scalar(255, 140, 0), cv::FILLED);

        cv::putText(frameRGB, label, textPos,
                    cv::FONT_HERSHEY_SIMPLEX, 0.7,
                    cv::Scalar(0, 0, 0), 2);
    }
}
