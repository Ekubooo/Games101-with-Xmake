#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen-3.4.0/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    float cosAlpha = cos((rotation_angle/180.0f)*MY_PI);
    float sinAlpha = sin((rotation_angle/180.0f)*MY_PI);
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
    model << cosAlpha, -sinAlpha, 0, 0, 
             sinAlpha,  cosAlpha, 0, 0, 
             0, 0, 1.0f, 0,
             0, 0, 0, 1.0f;
    // Then return it.
    return model;
}
Eigen::Matrix4f get_rotation(Vector3f axis, float angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
    Eigen::Matrix3f E = Eigen::Matrix3f::Identity();
    Eigen::Matrix3f nnT = axis * axis.transpose();
    Eigen::Matrix3f N, temp;
    float Alpha = (angle/180.0f) * MY_PI;
    N << 0, -axis.z(), axis.y(),
         axis.z(), 0, -axis.x(),
        -axis.y(), axis.x(), 0;
    temp = cos(Alpha)*E + (1-cos(Alpha))*nnT + sin(Alpha)*N;
    model << temp.row(0), 0,
             temp.row(1), 0,
             temp.row(2), 0, 
              0, 0, 0, 1.0f; 
    //model.topLeftCorner(3, 3) = temp;
    //model(3,3) = 1.0f;
    return model;
}
Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    float top = abs(zNear) * tan(eye_fov/2);
    float bottom = -top;
    float right = top * aspect_ratio;
    float left = -right;
    float A = zNear+zFar;
    float B = -zNear*zFar;
    // Students will implement this function
    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f o1, o2, p2o;
    o1  << 2/(right - left), 0, 0, 0, 
           0, 2/(top - bottom), 0, 0,
           0, 0, 2/(zNear - zFar), 0,
           0, 0, 0, 1;
    o2  << 1, 0, 0, -(right+left)/2,
           0, 1, 0, -(top+bottom)/2, 
           0, 0, 1, -(zNear+zFar)/2,
           0, 0, 0,  1;
    p2o << zNear, 0, 0, 0, 
           0, zNear, 0, 0, 
           0, 0, A, B, 
           0, 0, 1, 0;
    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    projection = o1* o2 * p2o;
    // Then return it.
    return projection;
}

int main(int argc, const char** argv)
{
    Eigen::Vector3f myAxis = {1.0f, 1.0f, 1.0f};
    myAxis = myAxis.normalized();
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
        else
            return 0;
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);
        r.set_model(get_rotation(myAxis,angle));
        //r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 5.0f;
        }
        else if (key == 'd') {
            angle -= 5.0f;
        }
    }
    return 0;
}
