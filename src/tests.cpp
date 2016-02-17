#include <vector>
#include <exception>
#include <gmock/gmock.h>
#include <opencv2/core.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <mar_algo.h>
#include "helper.h"
#include "capture.h"
#include "imageBuffer.h"
#include "properties.h"
#include "projection.h"

// ============================= Compile Constants ============================== 
// TEST(compile_constants, face_cascade_name_correctly_constructed) {
//   ASSERT_STREQ(FACE_CASCADE, "./assets/lbpcascade_frontalface.xml" );
// }

// TEST(compile_constants, eyes_cascade_name_correctly_constructed) {
//   ASSERT_STREQ(EYES_CASCADE, "./assets/haarcascade_eye_tree_eyeglasses.xml" );
// }

// ============================= mar_algo ==============================

TEST(binary_fold_rec, operate_on_non_balanced_containers) {
  std::vector<int> v = {10,10,10,10,10};
  auto result = mar::binary_fold_rec(v.cbegin(), v.cend(), std::minus<int>());
  ASSERT_EQ(result, 10);
}

TEST(binary_fold_rec, operate_on_non_balanced_containers2) {
  std::vector<int> v = {1,3,5,6,1};
  auto result = mar::binary_fold_rec(v.cbegin(), v.cend(), std::minus<int>());
  ASSERT_EQ(result, 0);
}

TEST(binary_fold_rec, operate_on_balanced_containers) {
  std::vector<int> v = {5,4,3,2};
  auto result = mar::binary_fold_rec(v.cbegin(), v.cend(), std::minus<int>());
  ASSERT_EQ(result, 0);
}

TEST(binary_fold_rec, operate_on_balanced_containers2) {
  std::vector<int> v = {7,4,9,2,6,8};
  auto result = mar::binary_fold_rec(v.cbegin(), v.cend(), std::minus<int>());
  ASSERT_EQ(result,-2);
}


// ============================= helper ============================== 
TEST(helper_parametrise, correctly_parse_command_line_arguments) {
  constexpr unsigned short argc {13};
  const char* commands_to_parse[argc] {"command", "--animation_speed", "30",
      "--camera", "2", "--max_images_in_loop", "50", "--new_image_fadein_time", "5.8",
      "--quit_after_minutes", "23", "--anti_alliasing", "8"};
  helper::parametrise(argc, const_cast<char**>(commands_to_parse));
  ASSERT_EQ(properties::animation_speed, 30);
  ASSERT_EQ(properties::camera, 2);
  ASSERT_EQ(properties::max_images_in_loop, 50);
  ASSERT_EQ(properties::quit_after_minutes, 23);
  ASSERT_EQ(properties::anti_alliasing, 8);
  ASSERT_FLOAT_EQ(properties::new_image_fadein_time, 5.8);
}

TEST(helper_parametrise, throw_exception_on_help) {
  constexpr unsigned short argc {2};
  const char* commands_to_parse[argc] {"command", "--help"};
  ASSERT_THROW(helper::parametrise
	       (argc,const_cast<char**>(commands_to_parse)),
	       helper::param_help_exception); 
}

TEST(helper_loadSampleImages, vector_size_is_correct) {
    std::vector<cv::Mat> images {helper::loadSampleImages()}; // should be 10 images
    ASSERT_EQ(images.size(),9);
}

// ============================= ImageBuffer =============================
class ImageBufferTest : public testing::Test {
public:
  ImageBuffer buffer{}; // default
  ImageBuffer bufferCap{100}; // with given capacity
  ImageBuffer bufferVec{ std::vector<cv::Mat>{10,cv::Mat(10,10,CV_32FC1,5.f)}};
};

TEST_F(ImageBufferTest,test_ctor_with_default_capacity) {
  ASSERT_EQ(buffer.capacity(),properties::max_images_in_loop);
}

TEST_F(ImageBufferTest,test_ctor_with_given_capacity) {
  ASSERT_EQ(bufferCap.capacity(),100);
}

TEST_F(ImageBufferTest,test_ctor_with_vector) {
  ASSERT_EQ(bufferVec.size(),10);
  ASSERT_EQ(bufferVec.capacity(),properties::max_images_in_loop);
}

TEST_F(ImageBufferTest,test_iterator) {
  bufferVec.iterate([](cv::Mat mat){
      ASSERT_FLOAT_EQ(mat.at<float>(3,3),5.f);
      ASSERT_EQ(mat.size(),cv::Size(10,10));
    });
}

TEST_F(ImageBufferTest,test_image_add_and_iterator) {
  for (int i =0; i<10; ++i) buffer.add_image(cv::Mat(10,10,CV_32FC1,20.f));
  buffer.iterate([](cv::Mat mat){
      ASSERT_FLOAT_EQ(mat.at<float>(6,2),20.f);
      ASSERT_EQ(mat.size(),cv::Size(10,10));
    });
}

#ifndef UNSAFE_OPTIMISATIONS
TEST_F(ImageBufferTest,operator_at_throws) {
  ASSERT_THROW(buffer[100],std::out_of_range);
}

TEST_F(ImageBufferTest,operator_at_correctly_throws) {
  try { buffer[100]; }
  catch (const std::out_of_range& e) {
    ASSERT_STREQ(e.what(), "ImageBuffer: item's index is out of bounds");
  }
}
#endif

TEST_F(ImageBufferTest,operator_at_correctly_returns) {
  cv::Mat mat = bufferVec[1];
  ASSERT_FLOAT_EQ(mat.at<float>(3,3),5.f);
}

// =============================== Capture ===============================

// TEST(Capture_Ctor, correctly_emit_exception) {
//   const unsigned short argc {3};
//   const char* commands_to_parse[argc] {"command", "--camera", "15"};
//   helper::parametrise(argc, const_cast<char**>(commands_to_parse));
//   Projection proj{};
//   try { Capture cap {&proj}; }
//   catch (const std::exception& e) {
//     ASSERT_STREQ(e.what(), "Did not manage to create Capture Window");
//   }
// }

TEST(Face_struct, correctly_construct_face) {
  helper::opencv::Face face {cv::Rect{1,2,30,40},cv::Rect{2,3,5,2},cv::Rect{10,20,100,200}};
  ASSERT_EQ(face.left_eye.x,1);
  ASSERT_EQ(face.left_eye.width,30);
  ASSERT_EQ(face.right_eye.y,3);
  ASSERT_EQ(face.right_eye.height,2);
  ASSERT_EQ(face.face.x,10);
  ASSERT_EQ(face.face.y,20);
}

TEST(Capture_Ctor, getters_function_correctly) {
  const char* commands_to_parse[1] {"command"};
  helper::parametrise(1, const_cast<char**>(commands_to_parse));
  helper::gl::setup();
  Projection proj{};
  Capture cap {&proj}; 
  ASSERT_EQ(cap.get_window_width(),properties::capture_screen_width);
  ASSERT_EQ(cap.get_window_height(),properties::capture_screen_height);
}

// ============================= Projection ============================== 
TEST(Projector_Ctor, getters_function_correctly) {
  const char* commands_to_parse[1] {"command"};
  helper::parametrise(1, const_cast<char**>(commands_to_parse));
  helper::gl::setup();
  Projection proj {};
  ASSERT_EQ(proj.get_window_width(),properties::projection_monitor_width);
  ASSERT_EQ(proj.get_window_height(),properties::projection_monitor_height);
}

// =============================== Use Cases  ===============================
// TEST(use_case_debug_display, load_and_debug_display_images) {
//     std::vector<cv::Mat> sampleImages {helper::loadSampleImages()}; 
//     ImageBuffer images{sampleImages, properties::max_images_in_loop};
//     images.iterate(std::bind(&helper::displayMat,std::placeholders::_1,0));
//     cv::waitKey(5000);
//     EXPECT_TRUE(false) << "As long as you did see some GUI windows with images, it's normal for this test to fail. This is just a quick hack to avoid more complex testing code herein....";
// }

// ================================ main ================================
int main(int argc, char** argv) {
  helper::logging::supress(); // suppress logging
  testing::InitGoogleMock(&argc, argv);
  std::cout.rdbuf(nullptr); // supress cout
  return RUN_ALL_TESTS();
}


