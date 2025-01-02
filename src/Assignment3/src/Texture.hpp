//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include "global.hpp"
#include <eigen-3.4.0/Eigen/Eigen>
#include <opencv2/opencv.hpp>
class Texture{
private:
    cv::Mat image_data;

public:
    Texture(const std::string& name)
    {
        image_data = cv::imread(name);
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
        width = image_data.cols;
        height = image_data.rows;
    }

    int width, height;

    Eigen::Vector3f getColor(float u, float v)
    {
        auto u_img = u * width;
        auto v_img = (1 - v) * height;
        auto color = image_data.at<cv::Vec3b>(v_img, u_img);
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }

    Eigen::Vector3f getColorBilinear(float u,float v)
    {
        float u0 = int(u * width), u1 = int(u * width) +1;
        float v0 = int(v * width), v1 = int(v * width) +1;

        float s = u * width - u0, t = v * height - v0;
        Eigen::Vector3f color1 = getColor(u0 / width, v0 / height);
        Eigen::Vector3f color2 = getColor(u1 / width, v0 / height);
        Eigen::Vector3f colorU0 = color1 + s*(color2 - color1);

        Eigen::Vector3f color3 = getColor(u0 / width, v1 / height);
        Eigen::Vector3f color4 = getColor(u1 / width, v1 / height);
        Eigen::Vector3f colorU1 = color3 + s * (color4 - color3);

        Eigen::Vector3f colorF = colorU0 + t * (colorU1 - colorU0); 
        return Eigen::Vector3f(colorF[0], colorF[1], colorF[2]);
    }

    Eigen::Vector3f getColorBilinear2(float u,float v)
    {
        float u0 = (int)(u + 0.5);
        float u1 = (int)u +1;
        float v0 = (int)(v + 0.5);
        float v1 = (int)v +1;
        float s = u - u0, t = v - v0;
        Eigen::Vector3f color1 = getColor(u0 , v0);
        Eigen::Vector3f color2 = getColor(u1 , v0);
        Eigen::Vector3f colorU0 = color1 + s *(color2 - color1);

        Eigen::Vector3f color3 = getColor(u0 , v1);
        Eigen::Vector3f color4 = getColor(u1 , v1);
        Eigen::Vector3f colorU1 = color3 + s * (color4 - color3);

        Eigen::Vector3f colorF = colorU0 + t * (colorU1 - colorU0); 
        return Eigen::Vector3f(colorF[0], colorF[1], colorF[2]);
    }
};
#endif //RASTERIZER_TEXTURE_H
