// #include <Wire.h>
// #include "I2Cdev.h"
// #include "MPU6050.h"
// #include "TensorFlowLite.h"
// #include "model_data.h"  // Include your TensorFlow Lite model header file

// MPU6050 mpu;

// // Define TensorFlow Lite globals
// constexpr int kTensorArenaSize = 5 * 1024;
// uint8_t tensor_arena[kTensorArenaSize];
// tflite::MicroErrorReporter micro_error_reporter;
// const tflite::Model* model;
// tflite::MicroInterpreter* interpreter;
// TfLiteTensor* model_input;
// TfLiteTensor* model_output;

// void setup() {
   
//     Serial.begin(115200);

   
//     Wire.begin();
//     mpu.initialize();

//     // Initialize TensorFlow Lite model and interpreter
//     model = tflite::GetModel(human_activity_model);
//     static tflite::MicroMutableOpResolver micro_mutable_op_resolver;
//     micro_mutable_op_resolver.AddBuiltin(
//         tflite::BuiltinOperator_FULLY_CONNECTED,
//         tflite::ops::micro::Register_FULLY_CONNECTED());
//     static tflite::MicroInterpreter static_interpreter(
//         model, micro_mutable_op_resolver, tensor_arena, kTensorArenaSize,
//         &micro_error_reporter);
//     interpreter = &static_interpreter;
//     interpreter->AllocateTensors();
//     model_input = interpreter->input(0);
//     model_output = interpreter->output(0);
// }

// void loop() {
    
//     int16_t ax, ay, az;
//     mpu.getAcceleration(&ax, &ay, &az);

//     // Preprocess accelerometer data (if necessary)

//     // Copy data to model input tensor
//     model_input->data.f[0] = static_cast<float>(ax);
//     model_input->data.f[1] = static_cast<float>(ay);
//     model_input->data.f[2] = static_cast<float>(az);

//     // Run inference
//     TfLiteStatus invoke_status = interpreter->Invoke();
//     if (invoke_status != kTfLiteOk) {
//         Serial.println("Invoke failed");
//         return;
//     }

//     // Read predicted activity from model output tensor
//     float activity_prob = model_output->data.f[0];  // Example: probability of activity

//     // Display or transmit predicted activity (e.g., via Serial)
//     Serial.print("Predicted activity probability: ");
//     Serial.println(activity_prob);

//     // Delay for a short period
//     delay(100);
// }
#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Adafruit_TFLite.h"
#include "model_data.h"  // Include your TensorFlow Lite model header file

// Define MPU6050 objects for two sensors
MPU6050 mpu1;
MPU6050 mpu2;

// Define TensorFlow Lite globals
constexpr int kTensorArenaSize = 5 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
tflite::MicroErrorReporter micro_error_reporter;
const tflite::Model* model;
tflite::MicroInterpreter* interpreter;
TfLiteTensor* model_input;
TfLiteTensor* model_output;

void setup() {
    // Initialize serial communication
    Serial.begin(115200);

    // Initialize first MPU6050 sensor
    Wire.begin();
    mpu1.initialize();

    // Initialize second MPU6050 sensor
    // Adjust the I2C address if necessary
    mpu2.initialize(0x69); // Example: I2C address 0x69

    // Initialize TensorFlow Lite model and interpreter
    model = tflite::GetModel(model_data.h); 
    static tflite::MicroMutableOpResolver micro_mutable_op_resolver;
    micro_mutable_op_resolver.AddBuiltin(
        tflite::BuiltinOperator_FULLY_CONNECTED,
        tflite::ops::micro::Register_FULLY_CONNECTED());
    static tflite::MicroInterpreter static_interpreter(
        model, micro_mutable_op_resolver, tensor_arena, kTensorArenaSize,
        &micro_error_reporter);
    interpreter = &static_interpreter;
    interpreter->AllocateTensors();
    model_input = interpreter->input(0);
    model_output = interpreter->output(0);
}

void loop() {
    // Read accelerometer data from first MPU6050 sensor
    int16_t ax1, ay1, az1;
    mpu1.getAcceleration(&ax1, &ay1, &az1);

    // Read accelerometer data from second MPU6050 sensor
    int16_t ax2, ay2, az2;
    mpu2.getAcceleration(&ax2, &ay2, &az2);

    // Copy data to model input tensor
    // Assuming the model input shape is [batch_size, num_features]
    model_input->data.f[0] = static_cast<float>(ax1);
    model_input->data.f[1] = static_cast<float>(ay1);
    model_input->data.f[2] = static_cast<float>(az1);
    model_input->data.f[3] = static_cast<float>(ax2);
    model_input->data.f[4] = static_cast<float>(ay2);
    model_input->data.f[5] = static_cast<float>(az2);

    // Run inference
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
        Serial.println("Invoke failed");
        return;
    }

    // Read predicted activity from model output tensor
    float activity_prob = model_output->data.f[0];  // Example: probability of activity

    // Display or transmit predicted activity (e.g., via Serial)
    Serial.print("Predicted activity probability: ");
    Serial.println(activity_prob);

    // Delay for a short period
    delay(100);
}
