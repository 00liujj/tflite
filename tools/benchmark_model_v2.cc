/* Copyright 2017 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include <cstdarg>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "tensorflow/contrib/lite/kernels/register.h"
#include "tensorflow/contrib/lite/model.h"
#include "tensorflow/contrib/lite/string_util.h"
#include "tensorflow/contrib/lite/tools/mutable_op_resolver.h"

#include <opencv2/opencv.hpp>

#ifdef TFLITE_CUSTOM_OPS_HEADER
void RegisterSelectedOps(::tflite::MutableOpResolver* resolver);
#endif

#define LOG(x) std::cerr
#define CHECK(x) if (!(x)) { LOG(ERROR) << #x << "failed"; exit(1); }

namespace tensorflow {
namespace benchmark_tflite_model {

std::unique_ptr<tflite::FlatBufferModel> model;
std::unique_ptr<tflite::Interpreter> interpreter;

int Main(int argc, char* argv[]) {
    std::string graph = argv[1];
    std::string image_fn = argv[2];
    int num_threads = 1;
  CHECK(graph.c_str());

  model = tflite::FlatBufferModel::BuildFromFile(graph.c_str());
  if (!model) {
    LOG(FATAL) << "Failed to mmap model " << graph;
  }
  LOG(INFO) << "Loaded model " << graph << "\n";
  model->error_reporter();
  LOG(INFO) << "resolved reporter\n";

#ifdef TFLITE_CUSTOM_OPS_HEADER
  tflite::MutableOpResolver resolver;
  RegisterSelectedOps(&resolver);
#else
  tflite::ops::builtin::BuiltinOpResolver resolver;
#endif

  tflite::InterpreterBuilder(*model, resolver)(&interpreter);
  if (!interpreter) {
    LOG(FATAL) << "Failed to construct interpreter\n";
  }

  if (num_threads != -1) {
    interpreter->SetNumThreads(num_threads);
  }

  interpreter->UseNNAPI(false);

  int input = interpreter->inputs()[0];
  std::cout << "the input size is " << interpreter->inputs().size()
            << ", input names " << interpreter->GetInputName(0) << std::endl;
  std::cout << "the output size is " << interpreter->outputs().size()
            << ", output names " << interpreter->GetOutputName(0) << std::endl;

  //interpreter->ResizeInputTensor(input, sizes);

  if (interpreter->AllocateTensors() != kTfLiteOk) {
    LOG(FATAL) << "Failed to allocate tensors!";
  }

  TfLiteTensor* input_tensor = interpreter->tensor(input);

  std::vector<int>  sizes(input_tensor->dims->size);
  std::cout << "input size is\n";
  for (int i=0; i<input_tensor->dims->size; i++) {
      sizes[i] = input_tensor->dims->data[i];
      std::cout << input_tensor->dims->data[i] << std::endl;
  }


  cv::Mat image = cv::imread(image_fn);
  cv::cvtColor(image, image, CV_BGR2RGB);
  cv::resize(image, image, cv::Size(sizes[2], sizes[1]));
  memcpy(input_tensor->data.raw, image.data, sizeof(uchar)*sizes[1]*sizes[2]*sizes[3]);
  clock_t start = clock();
  interpreter->Invoke();
  clock_t stop = clock();
  printf("invoke cost %f ms\n", 1e3*float(stop - start)/CLOCKS_PER_SEC);

  int output = interpreter->outputs()[0];
  TfLiteTensor* output_tensor = interpreter->tensor(output);

  for (int i=0; i<output_tensor->dims->size; i++) {
      //sizes[i] = input_tensor->dims->data[i];
      std::cout << output_tensor->dims->data[i] << std::endl;
  }
}


}  // namespace benchmark_tflite_model
}  // namespace tensorflow

int main(int argc, char** argv) {
  return tensorflow::benchmark_tflite_model::Main(argc, argv);
}
