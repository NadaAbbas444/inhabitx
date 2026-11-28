/*
* DEEPCRAFT Studio 5.16.7527.0+cda5e80bc6d2ff8f9dd9f30a82f9ad44f49da0e3
* Copyright © 2023- Imagimob AB, All Rights Reserved.
* 
* Generated at 11/27/2026 22:42:48 UTC. Any changes will be lost.
* 
* Model ID  e48a1b9d-548a-41c8-860a-86ac08e95c34
* 
* Memory    Size                      Efficiency
* Buffers   76800 bytes (RAM)         100 %
* State     9992 bytes (RAM)          100 %
* Readonly  34308 bytes (Flash)       100 %
* 
* Backend              tensorflow
* Keras Version        2.15.0
* Backend Model Type   Sequential
* Backend Model Name   conv2d-small-balanced-0
* 
* Class Index | Symbol Label
* 0           | unlabeled
* 1           | background
* 2           | human_activity
* 
* Layer                          Shape           Type       Function
* Sliding Window (data points)   [512]           float      dequeue
*    window_shape = [512]
*    stride = 320
*    buffer_multiplier = 1
* Hamming smoothing              [512]           float      dequeue
*    sym = True
* Real Discrete Fourier Transform [257,2]         float      dequeue
*    axis = 0
* Frobenius norm                 [257]           float      dequeue
*    axis = 0
* Mel Filterbank                 [32]            float      dequeue
*    num_filters = 32
*    sample_rate = 16000
*    f_low = 100
*    f_high = 8000
* Add Constant                   [32]            float      dequeue
*    A = 1
* Logarithm                      [32]            float      dequeue
* Clip                           [32]            float      dequeue
*    min = 0
*    max = 4
* Imagimob Speech Features       [32]            float      dequeue
*    output_freq = 50
*    output_features = 32
*    low_cut_freq = 100
*    high_cut_freq = 8000
* Sliding Window (data points)   [50,32]         float      dequeue
*    window_shape = [50,32]
*    stride = 800
*    buffer_multiplier = 1
* Contextual Window (Sliding Window) [50,32]         float      dequeue
*    contextual_length_sec = 1
*    prediction_freq = 2
* Input Layer                    [50,32]         float      dequeue
*    shape = [50,32]
* Reshape                        [50,32,1]       float      dequeue
*    shape = [50,32,1]
*    trainable = True
* Convolution 2D                 [25,16,12]      float      dequeue
*    filters = 12
*    kernel_size = [3,3]
*    strides = [2,2]
*    padding = same
*    activation = linear
*    use_bias = False
*    trainable = True
*    weight = float[3,3,1,12]
* Batch Normalization            [25,16,12]      float      dequeue
*    epsilon = 0.001
*    trainable = True
*    scale = True
*    center = True
*    axis = 3
*    gamma = float[12]
*    beta = float[12]
*    mean = float[12]
*    variance = float[12]
* Activation                     [25,16,12]      float      dequeue
*    activation = relu
*    trainable = True
* Convolution 2D                 [25,16,24]      float      dequeue
*    filters = 24
*    kernel_size = [3,3]
*    strides = [1,1]
*    padding = same
*    activation = linear
*    use_bias = False
*    trainable = True
*    weight = float[3,3,12,24]
* Activation                     [25,16,24]      float      dequeue
*    activation = relu
*    trainable = True
* Convolution 2D                 [25,16,24]      float      dequeue
*    filters = 24
*    kernel_size = [3,3]
*    strides = [1,1]
*    padding = same
*    activation = linear
*    use_bias = False
*    trainable = True
*    weight = float[3,3,24,24]
* Batch Normalization            [25,16,24]      float      dequeue
*    epsilon = 0.001
*    trainable = True
*    scale = True
*    center = True
*    axis = 3
*    gamma = float[24]
*    beta = float[24]
*    mean = float[24]
*    variance = float[24]
* Activation                     [25,16,24]      float      dequeue
*    activation = relu
*    trainable = True
* Max pooling 2D                 [12,8,24]       float      dequeue
*    pool_size = [2,2]
*    strides = [2,2]
*    padding = valid
*    trainable = True
* Dropout                        [12,8,24]       float      dequeue
*    rate = 0.05
*    trainable = True
* Global average pooling 2D      [24]            float      dequeue
* Dense                          [3]             float      dequeue
*    units = 3
*    use_bias = True
*    activation = linear
*    trainable = True
*    weight = float[24,3]
*    bias = float[3]
* Activation                     [3]             float      dequeue
*    activation = softmax
*    trainable = True
* 
* Exported functions:
* 
* int IMAI_dequeue(float *restrict data_out)
*    Description: Dequeue features. RET_SUCCESS (0) on success, RET_NODATA (-1) if no data is available, RET_NOMEM (-2) on internal memory error
*    Parameter data_out is Output of size float[3].
* 
* int IMAI_enqueue(const float *restrict data_in)
*    Description: Enqueue features. Returns SUCCESS (0) on success, else RET_NOMEM (-2) when low on memory.
*    Parameter data_in is Input of size float[1].
* 
* void IMAI_init(void)
*    Description: Initializes buffers to initial state. This function also works as a reset function.
* 
* 
* Disclaimer:
*   The generated code relies on the optimizations done by the C compiler.
*   For example many for-loops of length 1 must be removed by the optimizer.
*   This can only be done if the functions are inlined and simplified.
*   Check disassembly if unsure.
*   tl;dr Compile using gcc with -O3 or -Ofast
*/

/*
* Tensorflow Test Set
* 
* (ACC) Accuracy 94.318 %
* (F1S) F1 Score 94.313 %
* 
* Name of class                                               unlabeled       background   human_activity
* (TP) True Positive or Correct Positive Prediction                   0              214              201
* (FN) False Negative or Incorrect Negative Prediction                0                6               19
* (FP) False Positive or Incorrect Positive Prediction                0               19                6
* (TN) True Negative or Correct Negative Prediction                 440              201              214
* (TPR) True Positive Rate or Sensitivity, Recall              100.00 %          97.27 %          91.36 %
* (TNR) True Negative Rate or Specificity, Selectivity         100.00 %          91.36 %          97.27 %
* (PPV) Positive Predictive Value or Precision                 100.00 %          91.85 %          97.10 %
* (NPV) Negative Predictive Value                              100.00 %          97.10 %          91.85 %
* (FNR) False Negative Rate or Miss Rate                       100.00 %           2.73 %           8.64 %
* (FPR) False Positive Rate or Fall-Out                          0.00 %           8.64 %           2.73 %
* (FDR) False Discovery Rate                                   100.00 %           8.15 %           2.90 %
* (FOR) False Omission Rate                                      0.00 %           2.90 %           8.15 %
* (F1S) F1 Score                                               100.00 %          94.48 %          94.15 %
*/

#include <float.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#include "model.h"

// Working memory
static int8_t _buffer[76800];
static int8_t _state[9992];

// Parameters
static const uint32_t _k5[] = {
    0x3da3d70a, 0x3da3e945, 0x3da41ff6, 0x3da47b1a, 0x3da4faae, 0x3da59ead, 0x3da66711, 0x3da753d1, 
    0x3da864e6, 0x3da99a43, 0x3daaf3de, 0x3dac71a8, 0x3dae1393, 0x3dafd990, 0x3db1c38b, 0x3db3d173, 
    0x3db60333, 0x3db858b5, 0x3dbad1e2, 0x3dbd6ea1, 0x3dc02eda, 0x3dc3126f, 0x3dc61946, 0x3dc9433f, 
    0x3dcc903c, 0x3dd0001c, 0x3dd392bc, 0x3dd747fa, 0x3ddb1fb0, 0x3ddf19b9, 0x3de335ed, 0x3de77423, 
    0x3debd432, 0x3df055ed, 0x3df4f929, 0x3df9bdb7, 0x3dfea369, 0x3e01d506, 0x3e0468b9, 0x3e070cb2, 
    0x3e09c0d8, 0x3e0c8510, 0x3e0f593f, 0x3e123d48, 0x3e15310f, 0x3e183476, 0x3e1b4760, 0x3e1e69af, 
    0x3e219b42, 0x3e24dbfc, 0x3e282bba, 0x3e2b8a5d, 0x3e2ef7c4, 0x3e3273cb, 0x3e35fe51, 0x3e399733, 
    0x3e3d3e4d, 0x3e40f37b, 0x3e44b697, 0x3e48877d, 0x3e4c6608, 0x3e50520f, 0x3e544b6e, 0x3e5851fc, 
    0x3e5c6591, 0x3e608606, 0x3e64b330, 0x3e68ece8, 0x3e6d3302, 0x3e718555, 0x3e75e3b6, 0x3e7a4df9, 
    0x3e7ec3f3, 0x3e81a2bc, 0x3e83e92d, 0x3e863537, 0x3e8886c2, 0x3e8addb8, 0x3e8d3a02, 0x3e8f9b87, 
    0x3e920232, 0x3e946de9, 0x3e96de94, 0x3e99541c, 0x3e9bce69, 0x3e9e4d61, 0x3ea0d0ec, 0x3ea358f1, 
    0x3ea5e557, 0x3ea87604, 0x3eab0ae0, 0x3eada3d1, 0x3eb040bc, 0x3eb2e189, 0x3eb5861c, 0x3eb82e5d, 
    0x3ebada30, 0x3ebd897b, 0x3ec03c23, 0x3ec2f20e, 0x3ec5ab21, 0x3ec86741, 0x3ecb2653, 0x3ecde83b, 
    0x3ed0acdf, 0x3ed37422, 0x3ed63de9, 0x3ed90a1a, 0x3edbd897, 0x3edea945, 0x3ee17c09, 0x3ee450c6, 
    0x3ee72760, 0x3ee9ffbb, 0x3eecd9bb, 0x3eefb544, 0x3ef29239, 0x3ef5707e, 0x3ef84ff6, 0x3efb3086, 
    0x3efe1210, 0x3f007a3c, 0x3f01ebd1, 0x3f035db8, 0x3f04cfe4, 0x3f064245, 0x3f07b4ce, 0x3f09276f, 
    0x3f0a9a1c, 0x3f0c0cc5, 0x3f0d7f5c, 0x3f0ef1d3, 0x3f10641b, 0x3f11d626, 0x3f1347e6, 0x3f14b94c, 
    0x3f162a4a, 0x3f179ad3, 0x3f190ad7, 0x3f1a7a48, 0x3f1be918, 0x3f1d5739, 0x3f1ec49d, 0x3f203136, 
    0x3f219cf5, 0x3f2307cc, 0x3f2471ae, 0x3f25da8c, 0x3f274259, 0x3f28a906, 0x3f2a0e86, 0x3f2b72ca, 
    0x3f2cd5c6, 0x3f2e376a, 0x3f2f97ab, 0x3f30f679, 0x3f3253c7, 0x3f33af88, 0x3f3509af, 0x3f36622d, 
    0x3f37b8f7, 0x3f390dfd, 0x3f3a6134, 0x3f3bb28d, 0x3f3d01fd, 0x3f3e4f76, 0x3f3f9aea, 0x3f40e44e, 
    0x3f422b95, 0x3f4370b1, 0x3f44b397, 0x3f45f439, 0x3f47328c, 0x3f486e82, 0x3f49a811, 0x3f4adf2b, 
    0x3f4c13c5, 0x3f4d45d2, 0x3f4e7547, 0x3f4fa219, 0x3f50cc3a, 0x3f51f3a1, 0x3f531841, 0x3f543a0f, 
    0x3f555901, 0x3f56750a, 0x3f578e20, 0x3f58a437, 0x3f59b746, 0x3f5ac742, 0x3f5bd420, 0x3f5cddd5, 
    0x3f5de457, 0x3f5ee79d, 0x3f5fe79c, 0x3f60e44a, 0x3f61dd9d, 0x3f62d38c, 0x3f63c60e, 0x3f64b518, 
    0x3f65a0a2, 0x3f6688a3, 0x3f676d11, 0x3f684de4, 0x3f692b13, 0x3f6a0495, 0x3f6ada62, 0x3f6bac72, 
    0x3f6c7abd, 0x3f6d453a, 0x3f6e0be2, 0x3f6ecead, 0x3f6f8d94, 0x3f70488f, 0x3f70ff97, 0x3f71b2a4, 
    0x3f7261b1, 0x3f730cb6, 0x3f73b3ac, 0x3f74568d, 0x3f74f553, 0x3f758ff8, 0x3f762676, 0x3f76b8c6, 
    0x3f7746e3, 0x3f77d0c8, 0x3f78566f, 0x3f78d7d4, 0x3f7954f0, 0x3f79cdc0, 0x3f7a423f, 0x3f7ab267, 
    0x3f7b1e35, 0x3f7b85a5, 0x3f7be8b3, 0x3f7c475a, 0x3f7ca197, 0x3f7cf767, 0x3f7d48c6, 0x3f7d95b2, 
    0x3f7dde26, 0x3f7e2221, 0x3f7e619f, 0x3f7e9c9f, 0x3f7ed31e, 0x3f7f051a, 0x3f7f3290, 0x3f7f5b80, 
    0x3f7f7fe7, 0x3f7f9fc5, 0x3f7fbb17, 0x3f7fd1dd, 0x3f7fe416, 0x3f7ff1c2, 0x3f7ffadf, 0x3f7fff6e, 
    0x3f7fff6e, 0x3f7ffadf, 0x3f7ff1c2, 0x3f7fe416, 0x3f7fd1dd, 0x3f7fbb17, 0x3f7f9fc5, 0x3f7f7fe7, 
    0x3f7f5b80, 0x3f7f3290, 0x3f7f051a, 0x3f7ed31e, 0x3f7e9c9f, 0x3f7e619f, 0x3f7e2221, 0x3f7dde26, 
    0x3f7d95b2, 0x3f7d48c6, 0x3f7cf767, 0x3f7ca197, 0x3f7c475a, 0x3f7be8b3, 0x3f7b85a5, 0x3f7b1e35, 
    0x3f7ab267, 0x3f7a423f, 0x3f79cdc0, 0x3f7954f0, 0x3f78d7d4, 0x3f78566f, 0x3f77d0c8, 0x3f7746e3, 
    0x3f76b8c6, 0x3f762676, 0x3f758ff8, 0x3f74f553, 0x3f74568d, 0x3f73b3ac, 0x3f730cb6, 0x3f7261b1, 
    0x3f71b2a4, 0x3f70ff97, 0x3f70488f, 0x3f6f8d94, 0x3f6ecead, 0x3f6e0be2, 0x3f6d453a, 0x3f6c7abd, 
    0x3f6bac72, 0x3f6ada62, 0x3f6a0495, 0x3f692b13, 0x3f684de4, 0x3f676d11, 0x3f6688a3, 0x3f65a0a2, 
    0x3f64b518, 0x3f63c60e, 0x3f62d38c, 0x3f61dd9d, 0x3f60e44a, 0x3f5fe79c, 0x3f5ee79d, 0x3f5de457, 
    0x3f5cddd5, 0x3f5bd420, 0x3f5ac742, 0x3f59b746, 0x3f58a437, 0x3f578e20, 0x3f56750a, 0x3f555901, 
    0x3f543a0f, 0x3f531841, 0x3f51f3a1, 0x3f50cc3a, 0x3f4fa219, 0x3f4e7547, 0x3f4d45d2, 0x3f4c13c5, 
    0x3f4adf2b, 0x3f49a811, 0x3f486e82, 0x3f47328c, 0x3f45f439, 0x3f44b397, 0x3f4370b1, 0x3f422b95, 
    0x3f40e44e, 0x3f3f9aea, 0x3f3e4f76, 0x3f3d01fd, 0x3f3bb28d, 0x3f3a6134, 0x3f390dfd, 0x3f37b8f7, 
    0x3f36622d, 0x3f3509af, 0x3f33af88, 0x3f3253c7, 0x3f30f679, 0x3f2f97ab, 0x3f2e376a, 0x3f2cd5c6, 
    0x3f2b72ca, 0x3f2a0e86, 0x3f28a906, 0x3f274259, 0x3f25da8c, 0x3f2471ae, 0x3f2307cc, 0x3f219cf5, 
    0x3f203136, 0x3f1ec49d, 0x3f1d5739, 0x3f1be918, 0x3f1a7a48, 0x3f190ad7, 0x3f179ad3, 0x3f162a4a, 
    0x3f14b94c, 0x3f1347e6, 0x3f11d626, 0x3f10641b, 0x3f0ef1d3, 0x3f0d7f5c, 0x3f0c0cc5, 0x3f0a9a1c, 
    0x3f09276f, 0x3f07b4ce, 0x3f064245, 0x3f04cfe4, 0x3f035db8, 0x3f01ebd1, 0x3f007a3c, 0x3efe1210, 
    0x3efb3086, 0x3ef84ff6, 0x3ef5707e, 0x3ef29239, 0x3eefb544, 0x3eecd9bb, 0x3ee9ffbb, 0x3ee72760, 
    0x3ee450c6, 0x3ee17c09, 0x3edea945, 0x3edbd897, 0x3ed90a1a, 0x3ed63de9, 0x3ed37422, 0x3ed0acdf, 
    0x3ecde83b, 0x3ecb2653, 0x3ec86741, 0x3ec5ab21, 0x3ec2f20e, 0x3ec03c23, 0x3ebd897b, 0x3ebada30, 
    0x3eb82e5d, 0x3eb5861c, 0x3eb2e189, 0x3eb040bc, 0x3eada3d1, 0x3eab0ae0, 0x3ea87604, 0x3ea5e557, 
    0x3ea358f1, 0x3ea0d0ec, 0x3e9e4d61, 0x3e9bce69, 0x3e99541c, 0x3e96de94, 0x3e946de9, 0x3e920232, 
    0x3e8f9b87, 0x3e8d3a02, 0x3e8addb8, 0x3e8886c2, 0x3e863537, 0x3e83e92d, 0x3e81a2bc, 0x3e7ec3f3, 
    0x3e7a4df9, 0x3e75e3b6, 0x3e718555, 0x3e6d3302, 0x3e68ece8, 0x3e64b330, 0x3e608606, 0x3e5c6591, 
    0x3e5851fc, 0x3e544b6e, 0x3e50520f, 0x3e4c6608, 0x3e48877d, 0x3e44b697, 0x3e40f37b, 0x3e3d3e4d, 
    0x3e399733, 0x3e35fe51, 0x3e3273cb, 0x3e2ef7c4, 0x3e2b8a5d, 0x3e282bba, 0x3e24dbfc, 0x3e219b42, 
    0x3e1e69af, 0x3e1b4760, 0x3e183476, 0x3e15310f, 0x3e123d48, 0x3e0f593f, 0x3e0c8510, 0x3e09c0d8, 
    0x3e070cb2, 0x3e0468b9, 0x3e01d506, 0x3dfea369, 0x3df9bdb7, 0x3df4f929, 0x3df055ed, 0x3debd432, 
    0x3de77423, 0x3de335ed, 0x3ddf19b9, 0x3ddb1fb0, 0x3dd747fa, 0x3dd392bc, 0x3dd0001c, 0x3dcc903c, 
    0x3dc9433f, 0x3dc61946, 0x3dc3126f, 0x3dc02eda, 0x3dbd6ea1, 0x3dbad1e2, 0x3db858b5, 0x3db60333, 
    0x3db3d173, 0x3db1c38b, 0x3dafd990, 0x3dae1393, 0x3dac71a8, 0x3daaf3de, 0x3da99a43, 0x3da864e6, 
    0x3da753d1, 0x3da66711, 0x3da59ead, 0x3da4faae, 0x3da47b1a, 0x3da41ff6, 0x3da3e945, 0x3da3d70a
};

static const uint32_t _k12[] = {
    0x00000003, 0x00000005, 0x00000007, 0x00000009, 0x0000000b, 0x0000000e, 0x00000011, 0x00000014, 
    0x00000017, 0x0000001a, 0x0000001e, 0x00000022, 0x00000026, 0x0000002b, 0x00000030, 0x00000035, 
    0x0000003b, 0x00000041, 0x00000047, 0x0000004e, 0x00000056, 0x0000005e, 0x00000067, 0x00000070, 
    0x0000007b, 0x00000085, 0x00000091, 0x0000009e, 0x000000ab, 0x000000ba, 0x000000ca, 0x000000da, 
    0x000000ed, 0x00000100
};

static const uint32_t _k20[] = {
    0xbd6bd7e0, 0x3e49caef, 0xbdd13341, 0x3e17aa95, 0xbd44d9f6, 0x3e9f0ee2, 0xbeb0134d, 0xbe6834bd, 
    0xbde44545, 0xbe2e3fc2, 0x3e90caba, 0xbe9303cd, 0x3b23ae1d, 0x3e8729af, 0x3d919836, 0xbe771480, 
    0x3e867f9e, 0xbe645fcd, 0x3e965eca, 0x3e012963, 0x3d8aaf06, 0x3df02911, 0xbe2c3da5, 0x3d3081a7, 
    0x3e1f82bc, 0xbe898d0f, 0xbe47e719, 0x3e2eab7f, 0xbdc8c496, 0xbdb85035, 0xbe889785, 0xbd9d2144, 
    0xbe0f971f, 0xbe3c3743, 0x3d87e6d3, 0xbeccff9f, 0xbe04daac, 0xbe712db3, 0x3de545f3, 0xbe62098e, 
    0xbe62cdff, 0xbd5c60dd, 0x3e536b80, 0x3dd602fa, 0x3e451003, 0x3e43d5be, 0x3d9c2152, 0xbdb70d7a, 
    0x3e477ec7, 0xbafb02d2, 0xbe074807, 0xbe535431, 0x3d8055c5, 0xbe5537a5, 0xbd947049, 0xbe5c982e, 
    0xbe0ec585, 0xbdf4adeb, 0x3e1e0d8f, 0x3cc3e01a, 0x3dfb0068, 0x3ecce1e4, 0x3e222514, 0xbd98a9d1, 
    0xbe906c9f, 0xbe52147e, 0xbdfac373, 0x3dd4422d, 0x3de359e2, 0x3e2fb533, 0xbd4334ef, 0x3d8a738c, 
    0xbe8956d6, 0xbdd4068a, 0xbe1d46f9, 0x3da0e757, 0x3e097e2b, 0x3e59cc2a, 0x3e558b45, 0x3da76250, 
    0x3e272f75, 0xbe8d72c8, 0xbea45658, 0xbe4c3d82, 0x3dd6cbe8, 0x3e17255c, 0x3df3b835, 0x3d911f77, 
    0x3e14fcb3, 0xbe4e91fa, 0x3e38e3a7, 0x3e71cb7a, 0xbcec358f, 0xbc93fe6d, 0x3c62dcde, 0x3d75a163, 
    0xbe8dd0ef, 0xbe85e0af, 0xbdb29e04, 0x3e7796f1, 0xbd75100f, 0x3d8ac06c, 0x3edf6ad5, 0x3e5f4148, 
    0x3d7e1ad9, 0xbc9ec52d, 0x3e4dbef3, 0xbe5791a3
};

static const uint32_t _k22[] = {
    0x40691a13, 0x40a8c1e5, 0x40669252, 0x3fbdaaf5, 0x407aadad, 0x40a5915c, 0x401de16c, 0x4059c56b, 
    0x4011da56, 0x40431218, 0x408e5e4d, 0x3fab142f
};

static const uint32_t _k23[] = {
    0x3e83076e, 0xbeb26744, 0xbe9e4832, 0x3f636697, 0x3f2ccd5f, 0x3e28e368, 0xbf417998, 0x3f0322a8, 
    0xbf7c9205, 0x3f01bcab, 0x3f294b43, 0xbf6233fd
};

static const uint32_t _k27[] = {
    0x3df0ca3c, 0xbc0f7266, 0xbe42facc, 0x3e952a3f, 0x3e3da3f6, 0x3bf8bb48, 0xbe314c09, 0x3d80a2f5, 
    0xbc937416, 0x3d8fa80e, 0x3d2095e4, 0xbe8bbea6, 0x3dcd81c1, 0x3dd20bb5, 0xbda53999, 0x3da3e79d, 
    0xbd254eea, 0x3e00ccdf, 0xbe275b75, 0x3d8a8789, 0xbdceea8f, 0xbe230b86, 0x3e576131, 0xbd1469d6, 
    0x3e8355a7, 0x3c3a3b8e, 0xbc591cfc, 0x3e07d3b3, 0x3c97ad0f, 0x3dc95a07, 0xbe04a053, 0xbc68b591, 
    0x3db7212e, 0xbce631a4, 0x3dd8585b, 0x3d768e5e, 0xbead7795, 0xbc690b9e, 0xbd812930, 0xbdefa6de, 
    0x3e6fba1e, 0xbd0c07a6, 0xbe4e35bf, 0x3e874385, 0xbdaaba54, 0x3eb41d09, 0xbef447f0, 0xbe0aa487, 
    0xbe2dc477, 0xbe046ff7, 0xbdabb083, 0xbd6878ce, 0x3e5edae8, 0xbda1e982, 0xbe8dd036, 0x3e2153f3, 
    0xbde82714, 0x3dded0ad, 0xbea7dbc0, 0xbda75e68, 0xbe2efae7, 0xbe380d58, 0xbccbd623, 0x3e2db58e, 
    0xbca919fa, 0xbe9be9aa, 0xbe96c1d6, 0x3a99d519, 0xbd64000f, 0x3c548c0c, 0xbeb38202, 0xbd842e96, 
    0xbd4d8e50, 0x3dbcc90b, 0x3db3b686, 0xbe80e7e8, 0xbe5f16d0, 0x3e072b50, 0xbe372f63, 0xbdd58511, 
    0xbe33eee7, 0x3db74810, 0xbdb63a32, 0xbd6c03e6, 0x3d5b3020, 0xbe68be82, 0xbdee0b66, 0xbcedaa11, 
    0xbdd45ab6, 0xbe8224ad, 0xbc8fa878, 0xbd182f9f, 0xbe6d5257, 0xbe1ca65d, 0xbec494cb, 0xbea4d1a4, 
    0xbe75cf70, 0xbeb93a1f, 0xbdefe1bf, 0x3b2c9d21, 0xbe248903, 0x3d3f77e4, 0xbe5d7373, 0xbd0cff62, 
    0xbe8abc41, 0x3d7d5552, 0xbe8d462e, 0xbe19aee1, 0x3e166144, 0x3ebda7eb, 0xbdf9d23e, 0xbe3c2215, 
    0xbe83174b, 0x3e826b77, 0x3dfa2d1c, 0xbd9bef0c, 0x3e86a23a, 0x3e536ddf, 0x3de58ff8, 0x3e18f3ac, 
    0xbd651f52, 0xbe66fcdc, 0x3e34d0a2, 0xbe4b9a19, 0xbe325f14, 0x3e62034d, 0xbd018319, 0xb80d11c0, 
    0x3e17d82a, 0x3e905d00, 0x3d9f3f2b, 0x3dda5dae, 0xbde49e6b, 0x3d10f9cb, 0xbde5494c, 0xbf133cf7, 
    0xbe5e03c6, 0xbe1d0577, 0x3de120f5, 0xbdc4379c, 0x3e022622, 0xbccd0f43, 0xbf29d96f, 0xbe5d9d72, 
    0x3e176ee5, 0x3d756b5b, 0xbe8e4f97, 0xbe068554, 0x3d874128, 0xbe050b56, 0x3b8e855c, 0xbd4fc8b5, 
    0xbe1590ca, 0xbd8c002a, 0x3b89e85a, 0xbc5c6b49, 0xbdd5d21b, 0xbf0d1ea2, 0x3d7ed1e0, 0xbe1a4420, 
    0x3d470d1f, 0x3d74d451, 0x3c09ab0b, 0xbe3a529e, 0xbd978ade, 0xbc2f3add, 0xbd7e9953, 0xbe1c9a07, 
    0xbed4973c, 0xbe953ef9, 0xbe409997, 0xbef51424, 0xbce81e94, 0xbef0c67c, 0xbe82470c, 0xbebefdd5, 
    0xbe5a3026, 0xbecc8e51, 0xbf02bd67, 0xbe6593f8, 0x3d8d7f01, 0x3cce93ec, 0x3de437a6, 0x3e10b4b0, 
    0x3d561228, 0x3e6c30e6, 0x3d42c1ba, 0xbe066f5d, 0xbd1a652a, 0xbcf8240f, 0x3e32b557, 0xbe5b31be, 
    0x3dee845b, 0xbef1fb59, 0x3e3723e5, 0x3de17f2a, 0xbe743aab, 0x3e657902, 0xbd3555e1, 0x3e279f18, 
    0xbdb38f3c, 0x3d36f1bd, 0x3eaa5c45, 0xbe96b134, 0xbdeeec9d, 0xbeb8da4e, 0x3db4f496, 0xbe37697a, 
    0xbeef819c, 0x3d83b1a2, 0xbe8ef44f, 0xbeb996c1, 0xbe4ccd9c, 0xbe6f1019, 0x3ddea86f, 0xbe99bc81, 
    0x3b51b112, 0x3dcf6983, 0xbd73e2cc, 0xbd279ace, 0xbb1f5f39, 0x3e0ef5cb, 0x3d4cd5e9, 0xbcc80e7f, 
    0xbd881e90, 0x3da02c6b, 0xbe43e407, 0x3d291e23, 0xbdd7e50d, 0x3ded3164, 0xbd39b7a3, 0x3d18bb33, 
    0xbd83324f, 0xbe848589, 0x3d1d4328, 0xbe0b25ab, 0xbad68f35, 0x3d16454c, 0xbd74ae4c, 0x3dc1fbe2, 
    0xbe4b4f8f, 0xbc93a5c1, 0x3e353218, 0xbda83d9d, 0x3d1a2865, 0xbdb9e658, 0x3ddc8167, 0xbe458bb7, 
    0x3dbd229f, 0xbdb26963, 0xbe00ab79, 0x3e70ecbb, 0x3d80a5f3, 0xbd960a1a, 0xbd7b3aad, 0x3dda3256, 
    0x3e04d2f8, 0x3d12dae6, 0x3d73b22c, 0x3d929f83, 0x3d38366c, 0xbd17aefd, 0xbd427fbf, 0xbe2ac0bf, 
    0x3d9e2d52, 0xbd8cfe5a, 0xbd34a3db, 0xbdb9aeee, 0xbe055bf0, 0xbdaae43d, 0x3bdcd447, 0xbd808a01, 
    0xbd4be4d1, 0x3dee5ff6, 0xbd9add15, 0xbdf685ee, 0x3cc67047, 0xbdb88cca, 0x3dd3128b, 0x3dece5d4, 
    0xbd744ba8, 0xbdc15eb2, 0x3d4b6d8e, 0xbe2101a2, 0x3af2a87e, 0xbd59ab0e, 0xbd98bb17, 0x3e368b48, 
    0xbd73e2ca, 0xbd92eed1, 0xbe131c47, 0x3be1c6ef, 0x3ebe4f35, 0xbde9c872, 0xbe03782d, 0x3d7c4a51, 
    0xbda81849, 0x3de04be9, 0xbd0a0ff0, 0xbe37f1d8, 0xbd37466f, 0xbdf312dd, 0x3d385f39, 0x3d922958, 
    0x3dc1dd4d, 0x3da44ac1, 0xbe1076a8, 0xbc8980f0, 0x3d6f07e5, 0x3d93b2d9, 0xbdbfe146, 0xbda7a454, 
    0xbd40e64a, 0xbe492903, 0xbca54fd0, 0x3d6257ff, 0xbdc825c5, 0xbe22587f, 0xbdbdf947, 0x3d32ae55, 
    0xbe116cb7, 0xbe15e27e, 0x3ddecfbd, 0xbdcc43e4, 0x3dde99e7, 0xbd95347d, 0x3df4321e, 0xbcd8b044, 
    0xbeaf3991, 0x3da00001, 0xbe0203a1, 0xbe153130, 0xbdd67fe9, 0xbe348d73, 0xbdcd6aa1, 0x3c5c58ff, 
    0x3dac8b98, 0xbd95cebe, 0xbbbeaec2, 0x3d51aa97, 0xbe7a22a8, 0x3db76b15, 0xbe027a2e, 0xbdb91506, 
    0xbdd941da, 0xbe228fb4, 0x3e0d8335, 0xbd8b6bc3, 0xbd89b5e8, 0xbdcc3bfc, 0x3dda9e0e, 0xbd7ac323, 
    0xbeaf109b, 0x3d20f5aa, 0xbc767356, 0xbdf8b3fe, 0x3e249de1, 0xbd8fe7c8, 0x3c8a97c9, 0x3e234e84, 
    0x3cd24ccc, 0xbe5c4abf, 0xbe00e431, 0x3d95273f, 0x3d3e956d, 0x3d467f7b, 0xbe094769, 0xbe4162b3, 
    0xbe4041e5, 0xbe0dea23, 0x3e4c495f, 0xbda1fee2, 0x3c347c7b, 0xbdb4588f, 0x3cc9db0b, 0xbd88307e, 
    0xbe740a18, 0xbdc055a3, 0xbe093b51, 0xbe6a8039, 0xbdcc2330, 0xbd5ff562, 0x3e4d4858, 0xbd7b0d95, 
    0xbe1296b6, 0x3d1af3a1, 0x3ca8d8be, 0xbd441676, 0xbe40c6cf, 0xbc780997, 0x3db2bd53, 0xbe75517b, 
    0x3d8e8a0f, 0xbd8a7d30, 0x3e090f0c, 0x3db7a054, 0xbe03c2e8, 0xbd4ff5a3, 0xbd5b0b51, 0x3dc70a71, 
    0x3e107ae3, 0xbd511949, 0xbcaaf9d5, 0xbd8619b0, 0x3d126b1c, 0xbcb06aee, 0x3db542d2, 0xbe33a15b, 
    0xbe091bb9, 0x3de69f79, 0xbd40227a, 0x3e5fee7c, 0x3e0102dc, 0xbd064ae9, 0xbdc7c251, 0x3def98c0, 
    0xbe35679a, 0x3c92f157, 0x3d9bd05f, 0xbde0e0fe, 0xbd3250c9, 0xbe079501, 0xb9d20942, 0x3da7d7a7, 
    0x3d6a059a, 0xbe2925dd, 0xbe042d33, 0xbd6e82f8, 0xbd9f4a6d, 0x3d2ee2d4, 0x3e0f4972, 0xbd612353, 
    0xbdf5c995, 0xbdbf3eb3, 0x3e7375cb, 0xbd63a781, 0xbe45155d, 0x3d5e7758, 0x3cf1d5b1, 0xbea74cd4, 
    0xbd5e7db2, 0xbdfd6215, 0xbc7533d0, 0x3e2571e7, 0xbe8ae500, 0xbdb9b0a4, 0xbdc43a3d, 0x3d7fdf8c, 
    0xbd3e2fdd, 0xbe647830, 0x3ca160f7, 0xbe0d214b, 0x3e05aa32, 0xbe94a109, 0xbdcd7e1e, 0x3d13e157, 
    0xbe27fb5f, 0xbc9c883a, 0x3e027ea7, 0x3bbedfba, 0xbe43914c, 0xbd8da347, 0x3de54c20, 0xbea28a5f, 
    0xbde905bb, 0xbe6cc007, 0xbdd91889, 0x3e01ce00, 0xbde6352a, 0x3cb40554, 0x3e2c41c3, 0x3dd91fad, 
    0xbecb3046, 0x3e6d6241, 0xbe12b30a, 0xbd980ea7, 0x3d08c211, 0x3cb7687c, 0x3e1f65b1, 0x3bce3ce3, 
    0xbe486ce3, 0x3e5b3786, 0xbdbe9c9d, 0xbd318bca, 0xbe2117d3, 0xbdbe196c, 0xbbed5908, 0xbe09b6ae, 
    0x3d266768, 0xbe8decc7, 0xbcbd1b4c, 0x3dcb58a2, 0xbbfb3b99, 0xbda63522, 0x3dc9b2b0, 0xbd5981b2, 
    0xbe8f8c7e, 0xbcf64e75, 0xbd0e4c02, 0xbd802645, 0x3d90be5c, 0xbe15beae, 0x3ca4cbc7, 0x3d84fb55, 
    0x3e4d2f05, 0xbe5a7f56, 0xbe0379af, 0x3ea243f2, 0x3e209c1d, 0x3e92eb37, 0xbe91fd0d, 0xbd8e60a0, 
    0xbe10f8a8, 0xbd732d2a, 0x3ea10952, 0x3dbd5127, 0x3e6b9730, 0xbd1b4579, 0xbe14b1ba, 0x3e88814b, 
    0x3e3a72aa, 0xbcfe2a68, 0xbe690c7b, 0xbb3986fe, 0xbe501730, 0xbe60e816, 0x3ec0717b, 0x3dc4ef9b, 
    0x3e214ae5, 0xbd6be945, 0xbde8b874, 0x3d80be55, 0x3e0c5b9f, 0x3dd878b0, 0xbe6bef5f, 0x3c782d50, 
    0xbe8d6863, 0xbe7a83a1, 0x3eb0410c, 0xbd03285d, 0xbe1a5095, 0x3d9420b6, 0x3d3bbb9f, 0x3da10477, 
    0xbe45513e, 0xbe72f2fe, 0x3b94e103, 0x3ccff0bc, 0xbdc2c3b9, 0x3cbb414e, 0xbdfb7970, 0xbcef1314, 
    0xbbe19ada, 0xbe011e95, 0x3d433c91, 0xbd194339, 0x3cdbc1da, 0xbd0f655a, 0x3d51af6e, 0x3cd86fd2, 
    0xbcb0a04d, 0x3dc9fe8a, 0xbcac5837, 0xbc80c3af, 0x3e0fe70a, 0x3e254c58, 0xbdfdc297, 0xbe1d4960, 
    0xbd20a325, 0x3d0efe20, 0xbd83f0e4, 0xbe22c589, 0xbd24996e, 0xbdd3390d, 0xbe3fa6d1, 0x3e030c01, 
    0x3d58ff64, 0xbe1856dc, 0xbd97c889, 0x3da54c3d, 0xbe42e0c2, 0xbda9cc8d, 0x3cf125db, 0xbdf7803f, 
    0x3dd18385, 0x3d2eeb7e, 0xbd3b084e, 0x3c90111d, 0xbd2ea2cf, 0xbd1b6985, 0x3d9b6338, 0x3d84d480, 
    0x3c29abb1, 0xbdc25a41, 0xbbb9c798, 0x3a430007, 0x3d1333b1, 0xbd0da53b, 0xbe03ee6f, 0xbce840fd, 
    0xbe0432df, 0x3d29093d, 0x3d94a80f, 0xbd3b9bcc, 0xbd6e1b9c, 0x3a675f1b, 0xbdb7379b, 0x3db0c6b8, 
    0xbe0cfc1b, 0xbd20c64c, 0xbe3a7b2b, 0xbd39a781, 0xbdfa67b1, 0xbcbec395, 0xbdb919b2, 0x3e132e58, 
    0xbe2809e6, 0xbe463e15, 0x3d6becfd, 0xbd82a6fc, 0x390643ba, 0xbe0cd272, 0xbe13089c, 0x3ce00f37, 
    0x3dfaa99e, 0xbd35c788, 0x3d4435a2, 0x3b8af50a, 0xbb46f84e, 0xbe3feaea, 0xbcb6a97e, 0xbd837a02, 
    0x3d4f979d, 0x3ceacf73, 0xbdcb3b6d, 0xbd022a10, 0xbdba2f82, 0xbc385179, 0xbe0ab67e, 0xbd29a74d, 
    0xbe16904b, 0x3df5fc52, 0xbe197d45, 0x3dc733a0, 0xbc393850, 0xbe13ffc5, 0xbe3f5189, 0x3d962819, 
    0xbde569ef, 0xbcc6a525, 0x3ce1cfb5, 0xbd44212b, 0xbdba6967, 0xbbced23c, 0xbe4315b7, 0xbdfa3eb2, 
    0xbe6b98da, 0xbc85f630, 0xbc055d9b, 0xbd60944e, 0xbd812dda, 0xbb7b7f6a, 0x3d378e49, 0xbcc56e2c, 
    0x3cd7ac3b, 0xbc9d362b, 0xbdeb8bae, 0xbe386190, 0x3bfa6d7c, 0xbe645980, 0xbddda77b, 0xbe25e113, 
    0xbda87113, 0x3e0cb336, 0xbd8ce804, 0xbe045135, 0xbe527a64, 0x3cbbcbf0, 0xbe07421b, 0xbd63abc0, 
    0xbe7a8e14, 0xbde6085c, 0xbc538c28, 0xbddc4d51, 0x3b168a21, 0x3d7cd3bb, 0xbccf0e38, 0x3dadab4c, 
    0xbe27437b, 0x3e18d5fd, 0xbe072957, 0xbdade147, 0xbddebd78, 0xbe5c6a92, 0x3e207c95, 0x3cd72a78, 
    0x3d0a345b, 0xbdb6eadd, 0xbe999567, 0xbcfa8360, 0xbdd22af7, 0xbe5f8b90, 0x3ddafd23, 0xbe7ff2f4, 
    0xbdb6efc4, 0xbe4126d3, 0x3dd5148f, 0xbde27781, 0xbda090f1, 0xbd94801a, 0xbdd4c9e1, 0xbd702626, 
    0xbda8f84c, 0x3dbf3fe5, 0xbb474de5, 0xbde73536, 0xbdcbf831, 0xbde4fb3a, 0x3da1f9f7, 0xbd8b0604, 
    0x3dec59f2, 0xbe1e67cb, 0xbde67933, 0x3db36105, 0x3de54ce8, 0xbc508fb6, 0x3df44d97, 0x3d4b4545, 
    0x3d70d420, 0xbd7a7ccb, 0x3e9af431, 0xbe3b6386, 0xbd8de9c1, 0xbe0bfe77, 0xbe479169, 0x3e4db02d, 
    0xbcb021d4, 0xbdb989e0, 0xbd15d90a, 0xbd207b71, 0x3d08a882, 0x3ddd5ca0, 0x3d4fddf6, 0x3c2ff9ee, 
    0xbe155e13, 0xbc4f02fb, 0xbd122dd6, 0x3e44d2a2, 0xbcaecd75, 0x3e386221, 0x3c830f97, 0xbd98228a, 
    0x3cbbf1ce, 0x3e3d3b40, 0x3daf5178, 0x3db96c64, 0xbe4010cb, 0xbe13f1b3, 0xbd7ad18e, 0xbe3d81f2, 
    0x3ed435dc, 0xbe087a0c, 0x3bda9cae, 0x3e90ae60, 0xbe57fb52, 0x3bc869b3, 0x3d87285a, 0xbef92307, 
    0x3da17da2, 0xbea8dd24, 0xbe04761f, 0xbdae5196, 0x3ee17c7d, 0xbea10296, 0xbe128cd3, 0x3d999b0f, 
    0xbe124ca2, 0x3deec0c5, 0x3dcbfbca, 0xbeddb0a9, 0x3de4df64, 0x3eaa0df6, 0xbe46be3e, 0xbeebe67a, 
    0x3ead97ed, 0xbda6cc9d, 0x3d8cdd75, 0x3e52b0eb, 0x3db942be, 0x3dc33546, 0x3ed4e4b0, 0xbe3b7ddc, 
    0xbd68b5d1, 0xbe25c1e3, 0xbe246af4, 0xbe9e312e, 0xbdaa73e5, 0xbdd413ed, 0x3de6c288, 0x3e8ec43b, 
    0x3e10b7c7, 0x3e4162e3, 0x3d168668, 0xbe69ec1d, 0x3b747ade, 0xbe1a6e4e, 0xbd1220f5, 0xbebc4d5e, 
    0xbdc11805, 0xba7a844a, 0xbe902e24, 0xbd44f718, 0xbdb39a9c, 0xbd5d9352, 0xbb59fc8c, 0xbe845c02, 
    0x3e3633f2, 0x3ebfb8c1, 0xbe00944b, 0xbf15b8ee, 0xbefb9c29, 0xbe39da66, 0x3e35312a, 0x3d0332a0, 
    0x3e8c7391, 0x3b730cb4, 0xbd0d822f, 0x3cab3331, 0xbe259e31, 0x3cf32e88, 0xbd767fa6, 0x3d277b2b, 
    0xbe89fd60, 0xbd456964, 0x3e906413, 0x3e3fadc5, 0x3e8a87e1, 0x3e3a04a1, 0x3b819cdc, 0xbdf900c0, 
    0xbddd4663, 0xbea4ac9e, 0x3e31120c, 0xbd1a1419, 0xbe494f94, 0x3d26033c, 0xbe8ebb93, 0xbdfcd297, 
    0xbe60918b, 0xbcac145a, 0xbd3ff0f0, 0xbe813cbf, 0xbde1c1e3, 0x3e1343ce, 0xbe9588d3, 0x3d0f406a, 
    0xbf23c012, 0xbd92d03b, 0xbe295271, 0xbf006e4e, 0xbddccbe9, 0xbe1bff63, 0x3dc5bb5e, 0xbdc651ea, 
    0xbbedee3b, 0xbdc8c775, 0xbd7f3d9c, 0x3d33bb85, 0xbd1609d6, 0xbd6a361c, 0x3c9e47a2, 0xbdeae27b, 
    0x3d9e7070, 0xbe16f146, 0x3d021c1c, 0xbc8657a4, 0x3b8465d4, 0xbcdeebe9, 0xbe0bb233, 0x3d2b9151, 
    0xbd3201bd, 0xbdd64290, 0xbdfce5f5, 0x3c4b95dd, 0xbe105bb3, 0xbb146b7c, 0xbcea5edc, 0xbd9167b1, 
    0xbd0341d8, 0xbdc10975, 0x3dbe1e3c, 0xbde49417, 0xbdb6a4f5, 0xbe18710c, 0xbe0da123, 0xbdfa7cfe, 
    0x3db4202d, 0x3d6f4f28, 0xbd1bc6cb, 0x3da6a05d, 0x3bb7f242, 0xbe22e43a, 0xbdcc9e5c, 0x3d16b79e, 
    0xbdf9a3fb, 0x3dba202c, 0x3c0c540e, 0xbe06c1e5, 0xbda2dc98, 0x3c7baa93, 0xbe0d5d7a, 0xbba77b95, 
    0x3da7a589, 0x3d864a98, 0xbe374ee8, 0xbd4fde90, 0xbc70fe65, 0x3d230660, 0xbe4500a5, 0xbdb40518, 
    0xbdd92e11, 0xbd8b5581, 0xbd94d9db, 0xbc783137, 0xbd646d9d, 0xbd808e6e, 0x3d081abb, 0xbc9d041b, 
    0xbe06eb61, 0xbe11189a, 0xbd3b03ec, 0xbda1fdcc, 0xbdc7fcde, 0x3d2ecb2e, 0xbd476fb6, 0xbdf9d5ae, 
    0xbd162c15, 0xbd8ba2ba, 0xbe30af4b, 0xbd3d94d3, 0x3c4e7dc1, 0xbbd471d5, 0x3d0f30b2, 0xbd821484, 
    0x3cb2b711, 0xbcc3a316, 0xbcb00f7d, 0xbd87a6a8, 0xbd98db35, 0xbddd13ee, 0x3c3718a5, 0x3cfdae55, 
    0xbdfb90f8, 0x3d48eec7, 0xbbbd3500, 0xbde8b5da, 0x3cc598c7, 0x3c41aed4, 0xbde1477a, 0xbd8bcb14, 
    0x3c8da26a, 0x3d4bd851, 0xbc19c8f0, 0xbdd9a10b, 0xbdb51307, 0x3d1436f7, 0xbd600fd4, 0x3c75ab36, 
    0xbcc3fd45, 0xbd6be850, 0xbbd28e69, 0xbc92db2a, 0xbda465e3, 0xbdc6f4c1, 0xbc95a9b4, 0x3e1e37d0, 
    0x3d6e3f02, 0xbbafd805, 0xbd2624cb, 0x3df93dd3, 0xbdd4c405, 0x3d12eaf9, 0x3e2141e5, 0xbd82a1be, 
    0xbd9fe525, 0x3dfb3474, 0x3e269e94, 0x3dcaf988, 0x3daaa861, 0x3e99015c, 0xbecc0c18, 0x3d63af2d, 
    0xbe3f9262, 0x3c99207c, 0x3eaae089, 0xbdb17996, 0x3c8e67be, 0xbe60bac4, 0x3c1be620, 0x3d8baac2, 
    0xbdc82381, 0x3cdee69d, 0xbd72c100, 0x3e10f296, 0x3d010868, 0xbe39e415, 0x3d6079ba, 0xbebb80b7, 
    0xbd6e9732, 0xbddd770a, 0xbb00ba2f, 0x3db18d57, 0x3e034f33, 0xbddc774d, 0xbe7a3c43, 0x3e6f83bf, 
    0xbe1d1e49, 0x3e4bcc9a, 0xbe75c132, 0xbde67e26, 0x3cca525f, 0x3d3b75b6, 0xbca052aa, 0x3daa6f59, 
    0x3dfb9477, 0x3d0952b6, 0xbee073e5, 0x3dc74bb7, 0xbe629b22, 0x3e47a724, 0xbd4a40ca, 0xbddfda08, 
    0xbd99d62b, 0xbda069f2, 0x3da14b33, 0xbd371509, 0xbdd5cc7c, 0xbe3515e6, 0xbe3c4322, 0x3dbeee0b, 
    0xbe4d8e77, 0x3d4c3236, 0xbe466ade, 0xbe8ff3cf, 0xbe60ec68, 0x3dcf42ed, 0x3d8893ba, 0xbe6e7bd1, 
    0xbdd6447c, 0xbe1a4bc2, 0xbe48cee4, 0x3e10e12e, 0xbe6adc51, 0xbd9f0a87, 0xbeb4e687, 0xbdae1a43, 
    0xbe259333, 0x3d606e30, 0xbd8ccc6e, 0xbe186caf, 0xbd049444, 0x3d4d1e0b, 0xbeaa3805, 0xbdf6c326, 
    0xbe926de9, 0x3e212e1f, 0xbec65b22, 0x3d254851, 0xbe420cd4, 0xbdc62a74, 0xbdf9aa0f, 0x3bc2924a, 
    0xbda53279, 0xbd6003dd, 0xbe788454, 0x3d02d0eb, 0xbef3de2f, 0xbc2bc14e, 0xbea7e725, 0xbe964e19, 
    0x3dc0d70e, 0xbd444d5a, 0x3e1f9b32, 0x3defb9e5, 0x3dac53ca, 0x3d4ec87b, 0x3dcdcfae, 0x3e17e564, 
    0x3d8ae7fc, 0x3e023f71, 0x3dd0f0de, 0x3e1f267b, 0xbba65e88, 0xbdcaa3df, 0x3ce7dcef, 0xbc80e0fc, 
    0x3ea24a1d, 0xbcb9e68b, 0x3d051d2e, 0xbd0adeb4, 0xbbea9033, 0xbc85d76a, 0x3d580166, 0x3d4825c9, 
    0x3c81ca2e, 0x3e56fd64, 0x3e395ddf, 0x3bb11e04, 0x3e0d4567, 0x3d38af24, 0xbdce0f30, 0x3c76d080, 
    0x3d8f6eae, 0xbe244052, 0xbd32e10a, 0x3dfcc50d, 0x3d465ed1, 0x3e1f816a, 0xbd91fc5d, 0x3e0f67b8, 
    0x3e55ed98, 0x3d5bf713, 0x3c76721b, 0x3e14bea2, 0x3db3f251, 0x3df02ba4, 0x3d7c28c8, 0x3d9e3f83, 
    0xbd6be8fe, 0x3d3b7534, 0xbce16c41, 0x3d9d7d11, 0x3e84595e, 0xbdf4e99c, 0xba75fcec, 0xbdfdd5c4, 
    0x3b4d2bb0, 0xbdad7d3b, 0xbd45ec29, 0xbe191888, 0xbd1f5f83, 0x3e1957fe, 0xbc363fe0, 0x3d434975, 
    0xbcfd090f, 0xbdaad0bf, 0xbcd9712e, 0xbc1d014b, 0xbdd78798, 0xbd9c44f9, 0xbb9aa47d, 0x3d3610c3, 
    0xbdbe18bd, 0xbd8030b3, 0x3d83fa62, 0x3de4ba79, 0xbc868372, 0x3d66069c, 0xbd10ebc5, 0x3e10fcaa, 
    0x3dfdec4c, 0x3c14aa05, 0xbe4cd281, 0xbd5d7c12, 0x3d26543c, 0xbe3783c6, 0xbbc09393, 0x3e035f49, 
    0x3d1b4ac7, 0xbe4a1888, 0xbd624319, 0xbd1e58da, 0xbcfbe278, 0xbe0408a4, 0xbe167d32, 0xbe4ff997, 
    0xbd22715a, 0x3e3121ab, 0xbc899ed1, 0xbd95df42, 0xbe116b02, 0x3cb215af, 0x3d0c8aa2, 0xbba7bf48, 
    0xbdc9f857, 0x3db2c51c, 0xbdffbb7d, 0xbd446d0b, 0xbded2580, 0x3e0b0b14, 0xbdc46eca, 0xbe9ac73e, 
    0xbebda1ad, 0x3e175d9a, 0x3db51178, 0x3c8e7d73, 0xbba75869, 0x3e748fc6, 0x3d30aae7, 0x3dabfdd2, 
    0x3e696331, 0xbe0085d9, 0x3dd91d94, 0xbdba625e, 0xbe68f339, 0x3de4bbe4, 0xbd2bacdc, 0x3cd84b2b, 
    0xbda0563b, 0x3e5b1339, 0x3e935b4e, 0x3c064bdb, 0x3e4d6dca, 0xbb3dd91a, 0x3e516740, 0xbe54775d, 
    0xbeccb8cc, 0x3e7ed2b0, 0xbdc94088, 0xbe6af41b, 0xbcadd503, 0xbe2e5bbb, 0x3d9df488, 0x3bf41e3a, 
    0xbcac6773, 0x3d96989d, 0xbe59c5b9, 0xbe55acb9, 0x3e0108f5, 0x3df85851, 0x3e01996b, 0xbe03deb8, 
    0x3d5aabf2, 0xbd5c0a99, 0xbe0eaee6, 0xbd2a082b, 0xbe48de2e, 0xbe9f0d88, 0xbd02d92a, 0x3deb795c, 
    0x3e2b81dd, 0x3e28d1ad, 0xbc8a365d, 0x3d5d6f35, 0xbd602e89, 0xbdd105fc, 0x3c87376e, 0xbe78a3e9, 
    0xbdbf529c, 0xbe675827, 0x3d3c834a, 0xbda024dc, 0x3ddc118e, 0x3e102a88, 0x3cc3f06c, 0x3d35f0e5, 
    0xbbda2c75, 0xbd396f11, 0x3e12c1ea, 0xbe84d85b, 0xbd550cad, 0xbd03ce91, 0xbc6714c7, 0x3d1b7dd0, 
    0xbdedf08d, 0x3c8ebd7c, 0x3d6efa39, 0xbe5809e4, 0x3dfff271, 0x3d929889, 0xbce5f516, 0x3d62415d, 
    0xbd1a5a2f, 0xbe64ce84, 0xbcd6696d, 0x3d87c1c9, 0x3e0aa241, 0xbdb98e82, 0xbd81c650, 0x3def205a, 
    0xbde3fdc9, 0x3d04298b, 0x3d832d3b, 0xbdb315b9, 0xbd354e22, 0xbe747ea8, 0x3d472345, 0xbe0200b1, 
    0xbe2e2abd, 0xbe80305e, 0x3d8c0c78, 0xbd072473, 0xbd198af0, 0xbde0acb0, 0xbe55924f, 0xbe4f311d, 
    0x3dba25dd, 0xbdccd3a4, 0xbe20d25b, 0xbcee4c25, 0xbd5c6b81, 0xbcc5fb86, 0x3d792fa5, 0xbdf04d85, 
    0xbe1f79e5, 0xbd8706bd, 0xbd598c64, 0xbd028ec2, 0x3db959bc, 0xbdebc252, 0xbca4d7ea, 0xbd20704d, 
    0x3d3ad608, 0xbe0dd918, 0xbd1228c2, 0x3d42b1e5, 0xbddb0ba6, 0xbda7d581, 0xbd961be4, 0x3deb7029, 
    0x3ca51a51, 0xbd27eb92, 0xbdebee79, 0x3b10ce2b, 0x3d0c8933, 0xbd84da31, 0xbb095170, 0x3d882084, 
    0xbdbd2bd1, 0xbd06da33, 0xbdca4838, 0xbdcfacc8, 0x3d19c2b2, 0xbe05296b, 0x3d4eb0ba, 0x3d7d220b, 
    0xbc935bec, 0xbe35465e, 0xbc3edd56, 0x3e1cf9ca, 0x3d8890b8, 0xbd278465, 0xbb8518a6, 0xbdac5351, 
    0x3db1888a, 0xbe15396e, 0x3d513011, 0x3e2092fe, 0x3c7605e7, 0x3d0c2ae1, 0xbdb6ffb6, 0x3d95a12c, 
    0xbe1a6522, 0xbe01552f, 0x3be3bcc3, 0x3d6797fb, 0xbcaa7f0b, 0xbd7afe1d, 0xbe3f5688, 0x3dd0c151, 
    0xbda360ae, 0x3db4ea2a, 0x3da4498c, 0x3cffcad0, 0xbd2b4272, 0xbc8f21f2, 0x3c4fef92, 0x3d997fa5, 
    0x3c0ac27a, 0x3c6aecd4, 0xbd7fee1a, 0x3d879242, 0xbe367968, 0xbc069260, 0xbd5f1899, 0x3d3ddd13, 
    0x3caf56c0, 0xbe12bb45, 0xbdf6019d, 0x3c4bdca2, 0x3ca6ff57, 0x3d43b75b, 0xbcf02d58, 0x3e02708f, 
    0xbb8f781f, 0x3c99f467, 0xba3e8615, 0xbddc635f, 0xbbb4f4af, 0x3d44120d, 0x3cdba78c, 0xbd836471, 
    0x3d4d7696, 0xbd8da0ef, 0xbe2a1f19, 0x3cecd1fc, 0xbd278c54, 0x3d6192b3, 0xbde5111e, 0xbc881a2c, 
    0x3d79f7f0, 0xbcca5e5f, 0xbd7dc45c, 0xbd50f8cf, 0x3dc13e14, 0xbd62fe8c, 0xbdf7815a, 0xbe4bfe5b, 
    0x3e5cd194, 0x3d9888a5, 0x3c894082, 0x3e08c613, 0xbe4626a2, 0x3ea347ae, 0xbe35ab6f, 0xbec84263, 
    0x3d3570b1, 0x3da05632, 0x3d2e4bc2, 0xbddbe33f, 0x3d5ccd05, 0x3c8f7ce9, 0x3de97054, 0x3dcd0629, 
    0xbd7bb6e1, 0x3de8a452, 0xbe5a88a7, 0xbe238ccf, 0x3e152857, 0x3c574cb7, 0x3e022a53, 0xbde5247d, 
    0xbe2f3a83, 0xbdb56b80, 0x3d1dd739, 0x3e406034, 0x3ccdc0fe, 0x3e5e3cec, 0xbc9df0c7, 0x3e38a608, 
    0x3d910487, 0xbd8f3b73, 0x3da8a558, 0x3d9d988b, 0x3e28783c, 0x3e666f8c, 0xbcccb077, 0xbdb38e44, 
    0xbe948374, 0x3de2dc18, 0x3c522114, 0xbeb15efb, 0x3c51cadc, 0x3cab6844, 0xbdf5b89c, 0xbe4a77bf, 
    0x3d99a98a, 0xbe112033, 0xbd3bc678, 0xbd8848e9, 0xbe23fdec, 0x3d564f84, 0xbe39c88a, 0xbdbab371, 
    0xbe40eb67, 0xbea26262, 0xbd8aaab9, 0xbdeae656, 0x3dcc0f5d, 0xbe322aa4, 0x3d8815b2, 0x3d9ad975, 
    0x3d0bc392, 0x3cd1c448, 0x3b9a0921, 0x3ca9b56a, 0x3d708032, 0x3e32c93f, 0xbcebc8fa, 0x3cbbfed7, 
    0x3e7e205e, 0x3e7fbcfc, 0xbe51f9b0, 0x3dd52c64, 0xbd92d820, 0x3d5057e0, 0xbe22cea5, 0xbdb8cb8a, 
    0x3ced5d1b, 0x3db9f0fa, 0xbd416396, 0xbe806d44, 0xbe20df4e, 0xbbf7357a, 0xbda7d36c, 0xbb195acc, 
    0x3cdaf40e, 0xbd9538e4, 0xbe087de3, 0x3e011416, 0xbcfd08b4, 0x3e00e3b1, 0x3e1c0a7d, 0xbdc8e5f6, 
    0x3d3f41aa, 0x3e643a51, 0x3dcce7bd, 0xbce87276, 0xbdd8877d, 0x3d288e3c, 0xbe3a23e3, 0x3e6b69fb, 
    0xbd63c0fe, 0xbd65e9f1, 0x3e067e2e, 0x3e137045, 0xbec5828f, 0x3dfa61b3, 0xbca4d2ec, 0xbe94c819, 
    0x3c07dfd8, 0xbe74594a, 0x3daa3d24, 0x3dd96ace, 0xbe80ad53, 0x3d997433, 0xbe21a3f5, 0x3d88a897, 
    0xbd9e144d, 0xbe1cc7ec, 0x3e1705ab, 0xbad021e0, 0x3e387058, 0xbd1e2813, 0xbe34a7c8, 0x3e026925, 
    0xbe7da051, 0xbbf20ba4, 0x3e216c18, 0x3b622d65, 0xbdda6d8a, 0x3e33e9be, 0x3cc2ee79, 0xbc1f7933, 
    0x3d67f10e, 0xbdf3f09c, 0xbe8020c3, 0x3dfdfa8f, 0xbd9e40a6, 0xbd74d9ef, 0xbc172243, 0x3df29d60, 
    0xbe4b10de, 0xbdbaf173, 0xbded64a0, 0xbe87029a, 0xbda556c7, 0xbe92dbeb, 0x3e1a5433, 0x3e17093b, 
    0x3df34ded, 0x3e7f9c39, 0x3d9a15b7, 0xbd2b8d40, 0xbe84fcf0, 0x3d402959, 0x3d5af573, 0xbe80859a, 
    0x3df75fd6, 0xbe578437, 0x3d867d17, 0xbd0d74b2, 0x3cf4d9f8, 0xbb995922, 0x3cd46684, 0x3dd802e9, 
    0xbe7cc4ae, 0xbdf4839c, 0xbd63bd07, 0xbe8e98f3, 0xbdbb1f4d, 0xbe225d3e, 0x3dd85774, 0x3d635a51, 
    0xbe4743c8, 0xbe88967b, 0x3da32c2e, 0x3e609a6b, 0x3d2bf995, 0x3db04a86, 0xbe487a14, 0xbdd1eba1, 
    0xbe839f8b, 0xbc9d4903, 0xbc5bf1c8, 0x3d01911d, 0xbcf2c365, 0xbd59e308, 0x3dd39e50, 0x3e38c3e8, 
    0x3e8f532d, 0xbe7f6a68, 0xbe981ec0, 0xbde5d2f7, 0xbe833564, 0xbc8ccb38, 0x3e8acf13, 0xbe0e9dcf, 
    0xbcb2fe0e, 0x3d1e0afa, 0x3e0b9847, 0x3dffe3dd, 0x3d504b27, 0xbe15ef36, 0xbe1b950d, 0xbd8bd027, 
    0xbe5b66cf, 0x3c79684b, 0x3e349e0f, 0x38bd4c88, 0xbdd55ae0, 0xbdf8420a, 0xbc9ce6de, 0xbe437a57, 
    0xbd2e5151, 0xba726f3f, 0x3d8419d7, 0xbe54287e, 0x3dec1cd0, 0xbcc88edd, 0xbd3c0291, 0xbbfe09c4, 
    0xbe045b34, 0xbd575598, 0x3d9bc8fd, 0x3e1d6c41, 0x3cbd0e0e, 0xbdc03680, 0xbd88b6ed, 0xb99f2a4e, 
    0x3defde8c, 0xbcd49dac, 0xbd87a315, 0xbddece0d, 0xbcd1e125, 0x3dc1a360, 0x3df22624, 0x3e82f1b1, 
    0xbd372e82, 0x3e891348, 0xbc63467a, 0x3c6bc677, 0x3de0a34b, 0xbd73e830, 0xbe356bff, 0x3ce2b0e0, 
    0xbe183dff, 0xbe5a68f9, 0xbca76f08, 0xbdff78be, 0x3da17924, 0xbe906c65, 0x3d0917d7, 0xbdf4ce83, 
    0x3a643c10, 0xbe415618, 0xbe384ab0, 0xbd5ca756, 0x3dc2ba3f, 0xbcbaf9ed, 0x3d96212d, 0x3e229833, 
    0x3d1d235f, 0xbd0d02b3, 0xbe49a495, 0xbd09bc77, 0xbd4c2370, 0xbcee75b0, 0x3db8c27b, 0xbc4e9389, 
    0x3dd6142c, 0xbd0d7408, 0xbd8a0391, 0x3e2a82b2, 0xbe156401, 0x3e4033bc, 0xbe8481dc, 0xbe1be3c3, 
    0xba83eeb4, 0xbd5e8363, 0xbb1e311a, 0x3dc6fee5, 0x3c599be8, 0x3c3bd4a0, 0x3d0cafec, 0xbe2740b7, 
    0xbeb60b5d, 0xbc6d7920, 0xbd250712, 0xbe84edd7, 0xbe643d76, 0xbc51e7c4, 0x3e3b2d1a, 0xbe625d48, 
    0x3dec496b, 0x3bab9cf1, 0x3dda2a35, 0xbb34af50, 0xbde3de54, 0x3dfe0c13, 0xbe507676, 0xbdf8d77d, 
    0xbe4be3cf, 0xbb37931c, 0x3e4161f9, 0xbe69353e, 0xbe2710d2, 0xbd6020a1, 0xbcff40ad, 0x3e743a6a, 
    0x3d675f38, 0xbe22e2a3, 0xbe006bef, 0xbdf5d5fb, 0xbdad1b6f, 0x3d8692d8, 0xbd64baf4, 0xbd09ce14, 
    0x3d1a74bd, 0x3cb6347d, 0x3d75f637, 0x3dab1faa, 0x3dc22670, 0x3cdea7d7, 0xbd74380d, 0xbd746988, 
    0xbe3933e6, 0x3dd4b870, 0xbdcf020a, 0xbe561e7b, 0x3d93b901, 0xbcd4f813, 0x3a189b18, 0xbc959cb9, 
    0x3dc031f7, 0x3ddc2d0f, 0xbe5e3a12, 0xbdefb865, 0xbe119fc3, 0x3d7575ac, 0xbdb26790, 0x3b06f6da, 
    0xbe01c8f7, 0x3e425b59, 0xbd9e1c2c, 0x3c9baad1, 0xbe75a9b9, 0x3e4408b6, 0xbe81d389, 0xbd93a8f3, 
    0xbe1d1472, 0x3d13d540, 0xbc408b9a, 0xbe3ed278, 0x3e3ac1ca, 0xbe6cc47e, 0xbe1ec1de, 0x3e566c74, 
    0x3d255cdf, 0xbdd8f533, 0xbe5cd200, 0x3d155520, 0x3bf30804, 0x3e002ceb, 0x3de51345, 0xbe58e0d8, 
    0xbcf2dd25, 0xbe091f77, 0xbe02a8fa, 0xbdd09bf2, 0xbd923bb4, 0x3d3053a5, 0x3bcd0805, 0xbdddd007, 
    0x3cd5b72c, 0xbda69bae, 0xbd6a8d20, 0xbc7d5529, 0xbe37caee, 0x3e829827, 0xbc43546e, 0x3ddb37b3, 
    0xbe71090e, 0x3dd8278f, 0xbe775cbe, 0x3c743053, 0x3b83db42, 0x3d8e564d, 0x3d062271, 0xbe2e89a0, 
    0x3d8f85ba, 0xbda5e09e, 0xbe113d06, 0x3e36c546, 0x3d71c960, 0x3d990d91, 0xbd64571a, 0xbd14f72b, 
    0xbe170aef, 0xbd177e41, 0x3e2d53f4, 0xbbcd39ab, 0xbcb34f41, 0x3c31c72e, 0xbe1865cf, 0x3debac51, 
    0x3b417225, 0x3d00d65c, 0x3d0e0b72, 0x3cc4317e, 0xbd9c0120, 0x3d8ef249, 0x3dead770, 0xbdb817d7, 
    0xbd0a454a, 0x3e5dc45c, 0x3ddd454b, 0x3dd93f60, 0xbe3de41b, 0x3d9181aa, 0xbe1f2018, 0xbc3f0cbf, 
    0x3bdc5575, 0xbe094c13, 0x3d0c082e, 0xbd906182, 0xbe3f140e, 0x3da7040e, 0xbe6ff753, 0x3c49334e, 
    0xbe1fee1f, 0xbda05ea7, 0x3d023bee, 0xbe2585f4, 0xbe0d304b, 0xbe1510e8, 0xbe80e439, 0xbe29efc3, 
    0x3e49c533, 0x3e1bef81, 0xbc14988c, 0x3e16e742, 0xbe5bfcec, 0x3ecb1ec2, 0xbeaccc22, 0xbe52be95, 
    0xbdc385b9, 0xbe21be9a, 0x3eaed9fa, 0xbe0793bd, 0x3e5fc990, 0x3dd4f764, 0xbe0375f2, 0x3ca4c5b6, 
    0xbe30ab86, 0xbc72025c, 0xbd0ca421, 0xbe3a1aa5, 0xbcf13901, 0xbdded48d, 0x3e975804, 0xbd37c410, 
    0xbebfc104, 0xbebf71fa, 0x3b9a14e2, 0xbeaa716c, 0xbe861342, 0xbdf93c16, 0xbe73cfa2, 0xbe367dde, 
    0xbe961b24, 0xbd063cb5, 0xbf086d47, 0xbe7d3319, 0x3dc46e4d, 0xbe1af137, 0x3dce93e9, 0x3e239971, 
    0x3d9134a5, 0x3e693aed, 0xbe3ff43e, 0x3ceba90c, 0xbe3fede8, 0x3de968df, 0x3d74ba62, 0xbeab2621, 
    0x3d1fbb02, 0x3def270f, 0xbd5ef914, 0xbdcac4d7, 0x3e00b8b2, 0xbeab1129, 0x3cd19351, 0xbde150f0, 
    0x3e0bf949, 0xbde5064e, 0xbe4b07ea, 0xbe093cb3, 0xbe4b39b5, 0xbd5a9415, 0xbe23975d, 0xbe8f465d, 
    0xbef249c3, 0xbd275875, 0xbe511fd7, 0xbe834383, 0xbeac4c51, 0xbe741f0a, 0xbe539b00, 0xbea8e95f, 
    0xbd38f1c6, 0x3e28b7c6, 0xbdae22ec, 0x3d77a31d, 0x3d60447f, 0x3da83fa9, 0x3d12f660, 0x3ca0674b, 
    0xbdb8be8f, 0x3e374559, 0xbeaeda6b, 0xbdf2b397, 0x3dc681b4, 0x3cf152f7, 0x3e07a178, 0xbe5a444f, 
    0xbcfea49e, 0xbe864911, 0x3e4740a6, 0x3e15017d, 0x3caac7ac, 0xbd0cecdb, 0xbec4a5c2, 0xbd39d609, 
    0xbdb20f44, 0xbdfb24b4, 0xbd0b5fea, 0xbd0f98d6, 0xbe8a10a2, 0x3c72bb65, 0xbdef00c9, 0xbda3375c, 
    0x3db0efd7, 0xbe0c48ac, 0xbdd2dd23, 0x3deae6de, 0xbcff49f7, 0x3cedc426, 0x3d9507e4, 0xbf33448e, 
    0x3cf4eb03, 0x3e011cf6, 0x3d8a3008, 0xbe10c5ef, 0xbe269396, 0xbe0d6a59, 0xbe55a962, 0x3c92b03e, 
    0xbd6e8c5d, 0xbd990344, 0x3d1ee41f, 0xbd8ca28e, 0x3c43d38b, 0x3e16d6cd, 0xbe09310d, 0xbe218585, 
    0xbd9e9c0b, 0x3c956f23, 0xbdd4a6e0, 0xbde65d6f, 0x3d84e525, 0x3b15cba2, 0x3d7e8343, 0x3dc46452, 
    0xbe57b98f, 0xbdf6c1b5, 0xbe0a404e, 0xbda25863, 0x3dd52b4d, 0xbe236247, 0xbde788eb, 0x3e1b6cb4, 
    0xbd80b8b4, 0xbda0200d, 0x3da565fc, 0xbf43ee10, 0xbdf0942e, 0x3e8ae05b, 0x3d250554, 0xbd97bd67, 
    0xbe0e1a55, 0xbd559830, 0xbe9bffcb, 0x3c508286, 0xbd74448e, 0xbe1db4dd, 0xbc964731, 0xbd9a5f1b, 
    0xbe07e8bf, 0xbd60db73, 0xbe234b08, 0xbe273c3c, 0xbe063a3e, 0x3d925855, 0x3d3280a1, 0xbe28a77b, 
    0x3d172701, 0xbd7c7614, 0x3dc0ee70, 0xbc99fc65, 0xbdb6855b, 0xbd8c861b, 0xbd42f612, 0xbd3c3d89, 
    0x3c909054, 0x3d8ed2db, 0xbe5dbe92, 0xbd116ddb, 0x3d425677, 0x3cb0f648, 0x3e447e3e, 0xbee6988e, 
    0xbd6cca87, 0x3e3db004, 0x3cd6486a, 0x3d3e6ac4, 0xbe091f28, 0xbe53cca5, 0xbec57b53, 0xbb52f2fc, 
    0x3defe6b0, 0xbda94ab5, 0xbe1f27ce, 0xbc674860, 0xbe100cd7, 0x3de9cb1d, 0xbe5dc395, 0xbe427489, 
    0xbcfe6d5b, 0xbd7363b0, 0xbde9060a, 0xbe5f7cad, 0xbdf84ae5, 0xbdd33ad2, 0x3dfea2de, 0x3e757855, 
    0x3e28e63d, 0x3df8af4e, 0x3e1fba04, 0x3dbb962b, 0xbd9eb989, 0xbe111bd5, 0x3d9a10bb, 0x3d49ceb1, 
    0xbe5f63f4, 0xbe37c2bb, 0x3cbf6cc2, 0x3d5994c5, 0x3e610925, 0x3cdf1754, 0x3d2bc041, 0x3dfe48da, 
    0xbd90a348, 0xbe197771, 0x3dee99b7, 0x3d1005fd, 0xbe26f7f1, 0xbe5ca306, 0x3d824149, 0x3df3472f, 
    0x3e7749b5, 0x3db983a5, 0x3bc5567b, 0xbc7e09ab, 0xbd794a71, 0xbdf77603, 0x3d9e2042, 0xbd7d72be, 
    0xbdb24767, 0x3d49c238, 0x3cca809c, 0x3e1d6114, 0xbde1822f, 0x3c92f03e, 0xbd043e44, 0x3dbed19f, 
    0x3d6576d9, 0x3da8a171, 0xbe0c43d3, 0x3e62a18c, 0x3e0501eb, 0xbbf37660, 0x3c83c04e, 0xbd46fdf4, 
    0x3dc38ac0, 0x3e1e77a0, 0x3d654d44, 0x3d9bccbe, 0xbe44b278, 0xbc9b5b85, 0xbe07ab32, 0xbd6cafb8, 
    0xbdc03192, 0x3d3f641b, 0x3d6f2e16, 0x3d8d7229, 0xbae9de8d, 0xbe7c510c, 0xbda113ba, 0xbe3a0f60, 
    0xbe5ce6b1, 0x3cdc65e8, 0xbe323dea, 0xbb71ef85, 0x3d2d8460, 0x3d130887, 0x3d24de34, 0x3e41cb9d, 
    0xbe2a831f, 0x3ccf1a72, 0x3d54cd02, 0xbd0d99cc, 0x3e4c4c26, 0x3e31250f, 0x3d8f94bb, 0x3c8776b6, 
    0xbe19205f, 0xbd4ecab6, 0xbd07a775, 0x3da5816f, 0xbea03e94, 0x3da0fb72, 0x3da17317, 0x3cd9ec7f, 
    0xbe035eb9, 0xbe21df62, 0xbd8b2247, 0xbd7e3e90, 0xbe340d26, 0xbd7f2bf0, 0x3c95b073, 0x3dbca021, 
    0xbe821e35, 0xbe738c1c, 0xbe058928, 0xbda90ef1, 0xbdc76ef5, 0xbe5d2da3, 0xbd989a45, 0xbda3a860, 
    0xbe3bdaf6, 0x3e2a3e3e, 0xbea54a36, 0x3e24ee72, 0x3ee75c48, 0xbd9dbcaa, 0xbdba9971, 0x3e64de0b, 
    0xbbd4682f, 0x3ee94eca, 0xbe823449, 0xbea0d99f, 0x3ce91b57, 0xbc96933a, 0xbcc02115, 0x3e8b178d, 
    0x3ecb08b2, 0xbea9c349, 0xbe0a3984, 0x3e80a61f, 0xbe11b485, 0x3d742106, 0xbeab632d, 0xbe69ae25, 
    0xbd26e19c, 0xbb048b93, 0xbcdf8ccb, 0x3dd9680e, 0x3d7e5658, 0xbe890f7c, 0x3c3a414f, 0x3dd94179, 
    0xbd4e7faf, 0x3d9bffdd, 0xbead04c0, 0x3deaf1b4, 0xbde0f95e, 0x3ebf6c6c, 0xbb8e1f50, 0x3da8c7ad, 
    0xbddc8697, 0x3d043efc, 0x3db9059e, 0xbdc216a9, 0x3cd05a9e, 0x3e5d057f, 0x3daa54c9, 0xbe829b64, 
    0x3d1524c1, 0x3d97219a, 0xbdb042e1, 0xbdf6262f, 0x3d77bc44, 0xbe8b8561, 0xbdce1d30, 0xbcdef9b6, 
    0xbd923d4d, 0x3d1e6118, 0xbe176365, 0xbe8c2c0a, 0xbdc91c87, 0xbd96ab2d, 0x3e01ec81, 0x3c536c16, 
    0xbe8a2730, 0xbe14b303, 0x3d66ddd6, 0xbdb428e1, 0x3dc0e196, 0x3dcf53e0, 0xbeb68f20, 0xbe08bfa2, 
    0xbf05b655, 0x3ea71126, 0xbe1f6fd8, 0xbe685275, 0xbd26951a, 0x3df53c1f, 0x3c4dbab0, 0xbdc0fff7, 
    0xbe52abde, 0xbdb55106, 0xbecf5fe7, 0xbc044d95, 0xbe0df138, 0xbdefda5d, 0xbea99c1a, 0xbe1afa2d, 
    0xbd573a8b, 0xbeb4cdf1, 0x3dd11a80, 0xbd51de84, 0xbd7a045e, 0xbe1fbac1, 0xbf184428, 0x3cb98ce6, 
    0xbeac79b8, 0xbea544ab, 0xbd8cd145, 0x3ccb83d3, 0xbe1d5911, 0x3e6acb62, 0x3d5ef022, 0xbe4f99d3, 
    0x3e4ea616, 0xbdb49512, 0xbf076f52, 0x3ddf28ce, 0x3e24b609, 0x3e2b45c9, 0x3cdfa769, 0x3d0396af, 
    0x3d5642a6, 0x3e286ade, 0x3ddf0218, 0xbd2e6fe8, 0xbce62049, 0x3d9310ed, 0x3dfb81e2, 0x3e34481a, 
    0x3e324589, 0x3e2e8f2d, 0x3ca83e8f, 0x3d9bcc6e, 0x3e27ed4b, 0xbe4777d7, 0xbcbc81d7, 0x3d4938e9, 
    0xbdef2a34, 0x3e26a1c4, 0x3e4cc042, 0x3d4646bb, 0x3e26995d, 0xbd2bf4a2, 0x3cbe0c7c, 0x3d5ba2eb, 
    0xba291ed4, 0xbe8e97e4, 0x3d823677, 0xbbd8c8e8, 0x3db8fd12, 0xbd952955, 0x3d677925, 0xbda59e42, 
    0xbdd521c4, 0xbc57d682, 0x3d6eeadc, 0x3d861504, 0x3c8342b7, 0x3cbcde3c, 0x3dc253c7, 0xbd48656d, 
    0x3ca4b9c0, 0x3de50e04, 0x3cbabc79, 0xbcba07bd, 0xbd5b0b9d, 0xbd4325cf, 0xbe213691, 0x3e62006b, 
    0x3e49041c, 0x3dd3efb1, 0x3cf27c38, 0x3cb351f7, 0x3e184c0d, 0x3d099dc3, 0x3e143490, 0xbdc6e293, 
    0x3c079cad, 0xbde599bd, 0x3d6c7dbb, 0xbd36cfb7, 0x3c86f276, 0xbe957df5, 0x3db2483c, 0xbcb960e1, 
    0xbd41a8bc, 0xbd37d6d1, 0x3d8e8c63, 0x3e1c40cc, 0xbdc6053b, 0xbc8063b6, 0xbb073e90, 0x3e537515, 
    0x3da787fe, 0x3e010480, 0x3e0fc44f, 0x3df9972f, 0x3b1f1007, 0xbc7a45fd, 0x3dd04110, 0x3e440178, 
    0x3d9e36b1, 0xbc8731e0, 0xbdf80d32, 0x3db4a291, 0x3de9829e, 0xbdc809e4, 0x3dce5a3e, 0xbd6aa62c, 
    0x3dea18c0, 0x3da297e6, 0xbad2e417, 0xbc1626ec, 0xbc28c43e, 0x3e283cd9, 0x3e01864d, 0x3e256ebc, 
    0x3c38c8e9, 0xbdfa7b96, 0x3d8c4cf7, 0xbd81e581, 0x3df0532f, 0xbe09b611, 0xbdfc8a34, 0x3cbd0a98, 
    0x3dd4a7f7, 0xbdc1e850, 0xbea44835, 0xbda124a0, 0x3dfd7756, 0xbe8ab1cc, 0xbe9abc04, 0xbe248b29, 
    0xbda20c0b, 0xbc9fd817, 0x3d8af924, 0xbec8dd3d, 0xbde83d87, 0xbea30d34, 0x3e983de0, 0xbdf4f1c9, 
    0xbeaccc75, 0x3d087808, 0xbd9f9f04, 0xbe24cd75, 0xbe3f563d, 0xbf097744, 0xbe7c6d26, 0x3e42973b, 
    0x3e7d8aff, 0xbe9d886d, 0x3de5e15b, 0x3e522138, 0x3d900b8f, 0x3baf8bb7, 0x3d3f37d7, 0x3daa0cf4, 
    0x3e0523e9, 0xbc273e91, 0x3de060cd, 0x3d9d0161, 0xbc78b31b, 0xbe8095a9, 0xbe884527, 0xbd60ad0f, 
    0x3bb4530e, 0xbea2414f, 0xbe9b8f39, 0x3e1f0a9e, 0xbe7515da, 0xbdc16fe3, 0xbc91776e, 0xbf132c4c, 
    0xbe7537a8, 0x3db554c7, 0x3e50873c, 0xbdb8a374, 0xbeea2ede, 0x3e7c3c4a, 0x3e299ee9, 0xbda884bd, 
    0xbdba1e74, 0xbe5e351f, 0xbe0042ea, 0x3ea42071, 0x3e07374a, 0x3d722ec3, 0x3d682c65, 0xbc421972, 
    0x3d3f53fc, 0x3cd73d54, 0x3d9fe030, 0x3e0c1956, 0x3d30d1d7, 0x3ce27757, 0x3e18e8b9, 0x3c178396, 
    0x3c841992, 0xbeabf4ae, 0xbe355c7d, 0x3d674c6f, 0x3ccefb02, 0xbe5b00bf, 0xbef18926, 0xbd21b1e1, 
    0xbeccbee2, 0xbd6614b7, 0xbe91e455, 0xbeee5e9d, 0xbe5f5926, 0xbe08d7ca, 0x3e463e87, 0xbdf53d75, 
    0xbee5f847, 0xbe6042f5, 0x3e678f27, 0xbd42747a, 0x3e1a70d7, 0xbdb5b384, 0xbeb8b005, 0x3cef7933, 
    0x3d9a30a6, 0xbe2f5ea1, 0xbd323827, 0x3e02a259, 0x3db2268f, 0xbe3fd23c, 0xbd770af3, 0x3c86cac2, 
    0x3daf6b9c, 0x3d9f995f, 0x3d069ae3, 0xbde3a8af, 0x3c416daa, 0xbd988118, 0x3dfb041c, 0xbdf5e83c, 
    0xbddd987b, 0xbde8362d, 0xbd28d675, 0xbce1962b, 0xbdaf5600, 0xbe3099b1, 0x3dcaf010, 0xbdbbdb37, 
    0xbb0fb364, 0xbe2104e7, 0x3d28ec05, 0xbdd88ed9, 0xbdfe9fcf, 0xbd63a890, 0xbd70f4a6, 0x3d2efcf9, 
    0xbe16d560, 0x3db85430, 0xbde81735, 0xbb80981c, 0xbe479954, 0x3e18c24a, 0x3dcffd65, 0xbc954203, 
    0xbd70600f, 0x3ab72d54, 0x3d27c59c, 0xbe1b6dfc, 0xbd9d97e2, 0xbe5f0a78, 0x3dd09a6e, 0xbdb3944b, 
    0xbd481d3d, 0xbd94906a, 0xbdedb20d, 0xbddbeb81, 0xbdee872a, 0xbc85054d, 0x3ba361a8, 0xbd7e18af, 
    0x3d242164, 0xbdc1d035, 0xbbb52b5d, 0xbdc059fe, 0x3be5f5cf, 0xbdbc5158, 0xbd9abd0d, 0xbe1ac763, 
    0x3be56734, 0x3db3200e, 0xbb3a0b2e, 0xbde87e40, 0xbd2c0496, 0xbdfdafd0, 0xbdb045f2, 0xbe2cd247, 
    0xbe409511, 0x3e089916, 0xbe0d8a6a, 0xbe0f587a, 0xbdc6a1fe, 0x3bd09371, 0xbbabf35b, 0x3d1617cd, 
    0xbe2ad47e, 0xbe4882e6, 0xbe520b8f, 0x3d8820cb, 0x3dbe3437, 0xbdc15c06, 0xbe1fd2a3, 0xbe1d4b10, 
    0x3d43bc7f, 0xbdafdc77, 0xbdf04b3d, 0xbc1479ff, 0xbe368297, 0xbd142337, 0xbcffe389, 0xbd6ae773, 
    0x3d101135, 0xbe24d063, 0x3c48d738, 0x3cbeff64, 0xbd930d5a, 0x3de2ceaa, 0xba9f93a6, 0x3d2e3a67, 
    0xbe0d8a7e, 0x3d3f6599, 0xbdca7743, 0xbca2cee4, 0x3dabb954, 0xbd7c85e4, 0xbcb967af, 0x3d90d79b, 
    0xbe008947, 0xba88bd86, 0xbe04de90, 0x3d21ec37, 0x3d91152e, 0xbb88e950, 0x3d53a8fa, 0x3c0c62de
};

static const uint32_t _k30[] = {
    0xbe3d6fd3, 0x3c5209b9, 0x3cca9595, 0x3d586166, 0x3dd6ecc3, 0x3cb7ee4f, 0xbdf28a09, 0xbd93bc3e, 
    0x3c9c71ac, 0xbd5e2f5a, 0xbe472ec7, 0xbd42bfea, 0xbd4a5539, 0x3dab4f68, 0xbdadc717, 0x3ccde244, 
    0xbd4a530c, 0xbe18385b, 0xbe1c19e7, 0xbe0289af, 0xbe1ad6ee, 0xbce4e0a9, 0x3dd3f84f, 0x3da319a0, 
    0xbe0f193c, 0x3d971873, 0xbd11c780, 0xbd697c43, 0xbc83eb89, 0xbd1d3e41, 0xbdcd0d00, 0x3e49a7c6, 
    0xbd1d8ed0, 0xbcbb34c2, 0x3ca799d3, 0x3b12a03c, 0xbd6dd69f, 0x3db5d041, 0xbc83812d, 0x3e2eed7a, 
    0xbe72d4b8, 0xbe917643, 0x3d97d86f, 0xbd97812b, 0xbe8f3aa8, 0xbc9bff4d, 0x3f04831d, 0x3c87e915, 
    0xbe43660c, 0xbdb66a1e, 0xbe1bdc84, 0xbdbcca57, 0xbdae903b, 0x3d164171, 0xbe0b13e8, 0x3edb9b23, 
    0xbce5eee1, 0x3d7398cc, 0x3e21aa12, 0xbd2035c1, 0x3cd76f77, 0xbdd3f81d, 0x3b84c024, 0xbcc4ae3d, 
    0xbd861dd6, 0xbe2711dd, 0x3e67f0e5, 0xbb9764f0, 0xbee04865, 0xbced9d52, 0x3eac94cc, 0xbddcdb57, 
    0xbdf1fbe0, 0x3df75acd, 0xbce16fed, 0xbd8bc8ab, 0xbd80b8b0, 0xbc4c4c9d, 0xbd6e0038, 0x3e47210f, 
    0xbdefc6a1, 0x3d99c7ed, 0xbdc4b502, 0x3c8fd99b, 0xbd135c13, 0xbd76968c, 0xbca116d7, 0x3d26c944, 
    0xbde5a99d, 0xbd8c9284, 0xbd8ccc4c, 0xbe0a572b, 0xbc95b3ba, 0xbaf87a34, 0x3e1d6918, 0x3bf7601e, 
    0xbd8e07d4, 0xbd66e144, 0x3d88edbf, 0xbde14337, 0xbc9779e2, 0x3d7dea34, 0x3d9c8801, 0x3e53e12e, 
    0xbc2bb5c2, 0xbda5cf30, 0x3d37f0f7, 0xbdb4d401, 0xbb8cd2ee, 0x3e3cec7d, 0x3db19be5, 0x3dd80c90, 
    0xbda00d90, 0xbe4cea61, 0xb9592463, 0xbdae309b, 0xbe0c7a7c, 0xbde0e653, 0x3e9400fc, 0x38ddf537, 
    0xbd3a0825, 0x3d9def2c, 0xbe03c770, 0xbdb0dff0, 0xbd9070e8, 0x3d7391c2, 0x3d8233df, 0x3ec6e8ce, 
    0xbcae04ad, 0xbdf05ec4, 0x3d79fa40, 0xbde48ce2, 0x3dad7639, 0x3e460b85, 0xbd9876e6, 0xbd65b357, 
    0xbd897e8f, 0xbde8dc8a, 0x3e6403e2, 0x3da09bed, 0xbe78d61a, 0xbd5e6eaf, 0x3ed7d020, 0xbe17ac0f, 
    0xbe0ad71d, 0xbd734c39, 0x3c5a8cbf, 0x3c6b6686, 0xbd173d51, 0x3ce40a7e, 0xbe03ee1a, 0x3d8a80fb, 
    0xbdee10dd, 0x3d9974eb, 0xbe3d366f, 0xbdb75ee0, 0xbe219754, 0xbd1ff2f4, 0xbcede4b5, 0xbd522300, 
    0x3d3ebc72, 0xbe152b3c, 0xbdbbf77a, 0x3da6b579, 0xbd896aef, 0xbc8d5b06, 0x3e88354a, 0xbc25d4cf, 
    0xbb1d6d3e, 0x3e97db11, 0xbdc3f584, 0xbdab4132, 0xbdae4aa4, 0xba573e7e, 0xbd2e85d6, 0x3ea1585a, 
    0x3d9d6401, 0xbd294d23, 0xba3c80ba, 0x3969f8f4, 0xbd1070d0, 0xbd5d5a2b, 0xbe02e42a, 0x3d0eba5d, 
    0xbea81512, 0xbeaf7f50, 0x3ddf0b0b, 0xbdb5213a, 0x3c52fac2, 0xbda55efe, 0x3f33ef0a, 0xbbcf7909, 
    0xbddd935b, 0xbcd5c374, 0xbe27bb38, 0xbd88b9a7, 0xbd9c2a7a, 0x3e2130a6, 0x3cd2ee0a, 0x3e71960e, 
    0x3cc731bc, 0xbe446f5a, 0x3e430349, 0xbdc9c8f9, 0xbc12d75e, 0x3e324df4, 0xbd2f05bd, 0xbe0c1b41, 
    0x3d5367fd, 0xbd467266, 0x3e0aa2c1, 0x3c846bc6, 0xbe6a5f75, 0xbe1ec517, 0x3ef338eb, 0xbb92833c, 
    0xbe52c550, 0xbcc51964, 0x3cb314cf, 0x3baa00c8, 0xbcb2e497, 0x3e08cf02, 0xbe0e41d1, 0xbd54a707, 
    0xbdcf5573, 0xbb44d5ae, 0xbaf131c8, 0x3dc57242, 0x3caa4f85, 0xbdb3d5dc, 0x3ca73fda, 0x3d503a06, 
    0xbe968b58, 0xbe2549fc, 0xbe16baa6, 0xbe5924eb, 0xbe0b9566, 0xbe9de04d, 0x3b901c63, 0x3dbfc4c1, 
    0xbe8df1d5, 0x3e22a061, 0x3dd4f87f, 0xbe334561, 0x3d7269e7, 0xbc0965e8, 0x3e7e4fa3, 0x3e08580d, 
    0xbd53e04e, 0xbd5cc4e2, 0xbe04008f, 0x3d953e3a, 0x3cc61ebe, 0xbd4a0724, 0xbe0a56ee, 0x3deda32c, 
    0x3d2b5959, 0xbeacb7d6, 0x3e247dc9, 0x3cfc9e18, 0xbdcd861b, 0x3d19fd18, 0x3ec47d3e, 0xbdb1dcc3, 
    0xbd162f0d, 0xbd6a0876, 0xbe2e1515, 0xbe5247bd, 0xbdf19cdd, 0xbc7ff8b8, 0x3dbeeee6, 0x3e10611b, 
    0xbe127ada, 0x3d64037a, 0xbe71440c, 0xbe35e293, 0x3d564a61, 0xbe930468, 0xbe221aa1, 0x3d2403f4, 
    0x3e75f8f9, 0xbe584069, 0xbdb07f5e, 0xbd61d823, 0xbe38d96f, 0x3da2eb3a, 0x3ccc49fc, 0x3d0b514a, 
    0xbea5c5fd, 0x3d861cb8, 0xbd27c3d7, 0xbe0a378d, 0x3dc8b4b1, 0x3e31bf74, 0xbe0efcca, 0x3d7a1d1f, 
    0xbdc49377, 0xbd4faef4, 0x3dc605c4, 0x3dc0c183, 0xbcfa6506, 0x3db401fb, 0x3ddef4f1, 0xbd9214ef, 
    0xbe656821, 0xbddb7a83, 0xbdf939ed, 0xbe4e4a6e, 0xbd4f1448, 0xbe6897e0, 0xbe1dd129, 0x3d86e59e, 
    0x3d0a55ce, 0x3ed444bc, 0x3da53c6d, 0xbe6e890a, 0xbd3793df, 0x3dc36f13, 0xbdb01d49, 0x3ccd92ef, 
    0xbb87e2d3, 0x3c5e567b, 0x3d08e95f, 0x3c1e82a4, 0x3db84e13, 0x3d858cea, 0xbdbc5a01, 0x3d8cbe0a, 
    0x3ccbec63, 0xbe940384, 0x3e80bc54, 0x3d9ef2f2, 0xbd302dbc, 0xbdba3c0b, 0x3e067fd0, 0xbc828351, 
    0x3e157442, 0x3e31b1d4, 0xbd042c69, 0xbe70bed6, 0xbdebe281, 0x3cb20405, 0x3d0195fa, 0x3da27cd1, 
    0xbd3f7465, 0x3d948935, 0x3d4b6720, 0xbe3b13c2, 0x3dafffb8, 0xbed08f5e, 0xbc4c9bbe, 0xbce0d9f4, 
    0x3d5db225, 0xbe0ac695, 0xbd2d3407, 0x3e20d2a8, 0xbc9f2d45, 0x3d4830c5, 0xbdb4fd45, 0x3c89afe9, 
    0xbdc12fea, 0x3c79d29b, 0xbdabaa44, 0xbd9a1751, 0xbdf7fe03, 0x3e24e1f3, 0xbde2dd5d, 0x3da81670, 
    0x3d75777f, 0xbd9aabbb, 0xbcfe9271, 0x3da6cf4b, 0xbe2fe2f2, 0xbceafed8, 0xbdce1de5, 0xbd2f8cda, 
    0xbea1a8e8, 0xbe67f625, 0xbbb9bda3, 0xbd96f0e0, 0xbd3506c3, 0xbe8929be, 0x3d64214c, 0xbcdfbcbe, 
    0xbe1e73d9, 0x3e852bc8, 0xbdc84161, 0xbdbc3071, 0xbe0dc165, 0xbe12293b, 0xbdbd14d0, 0x3e1ba814, 
    0xbbfb8822, 0xbd728c57, 0xbda8c3ad, 0x3d1a3d8e, 0xbd9443f3, 0x3bbfd3da, 0xbde03db6, 0x3de12ede, 
    0xbe0f4d41, 0xbec5726c, 0x3db1183f, 0xbc31852b, 0xbd825675, 0xbdca5ed3, 0x3e99a9e5, 0xbc83590a, 
    0xbe0c1352, 0xb98b2701, 0xbe5bb353, 0xbe1dec47, 0xbdaf8ed3, 0xbd90f7d7, 0xbe56fcf4, 0xbb1eb91a, 
    0xbd8f5144, 0x3e44410c, 0xbe4fc127, 0xbdeb77ae, 0xbd8cb377, 0xbeab4072, 0xbe11d11c, 0x3d6364be, 
    0x3dcf5f54, 0xbe824127, 0x3c3a8935, 0xbd5559f3, 0xbaff95ad, 0xbe37450d, 0xbe3f1e37, 0xbb0fa84e, 
    0xbe80809d, 0xbe27241a, 0x3e3a9f46, 0x3dbf9fa8, 0xbbed77e2, 0x3d474e8b, 0xbd036289, 0x3dc0412d, 
    0xbdc4711a, 0x3e3ddb0d, 0x3e3266f8, 0xbe29c6b5, 0xbc54dc5a, 0xbd088515, 0xbe0740d6, 0xbdb023f5, 
    0x3cd744a8, 0xbe838acc, 0x3d61e6e0, 0xbe09bc6f, 0xbda2e239, 0xbd01d932, 0x3ea8b3b0, 0xbd7ee007, 
    0xbe126a34, 0x3e622ae8, 0x3cab9406, 0x3d6d043f, 0x3d064b69, 0x3da643e9, 0xbdad553a, 0x3e347f73, 
    0x3db6dc13, 0xbc9c6959, 0x3e093070, 0x3e46422e, 0xbc4133b2, 0x3dd39084, 0xbde993f5, 0x3c7e1702, 
    0x3d84508c, 0x3c249a0b, 0x3e818bc2, 0x3e09009c, 0xbe00d78f, 0x3d8e87b7, 0xbce8c730, 0xbd9884ce, 
    0x3ba27bf1, 0xbdf668c4, 0x3d25c140, 0x3d9d33f3, 0xbe1dde22, 0xbd92491b, 0xbe4d07b0, 0x3eb1a621, 
    0x3cdc7f4e, 0x3e28310c, 0xbdd206ee, 0xbda033d9, 0xbdc49644, 0x3d23a7ab, 0x3db292d6, 0x3e49fa06, 
    0xbd7eacf2, 0xbe62b7f5, 0xbde1e1c4, 0xbb77cbb8, 0xbe551378, 0x3dec4ae3, 0x3f19e68e, 0x3d1cdf95, 
    0xbe223a8d, 0x3dca3237, 0x3ddd4338, 0x3dd40366, 0xbde23a62, 0xbdac83a6, 0x38b4e2a0, 0x3cc56caa, 
    0x3d866dc8, 0x3e3f6c1b, 0x3da4395c, 0xbe9e2308, 0xbcfb7304, 0xbd94c747, 0x3dc56359, 0xbd0999cd, 
    0x3d7d04f5, 0xbec2d23d, 0xbcf60ac2, 0xbe1079d3, 0xbd2ae8a1, 0xbc3f9a16, 0xbd0e59e9, 0x3c7cba32, 
    0xbe02c3eb, 0x3e566598, 0x3e580c73, 0x3e5bb6e6, 0x3d01b2e0, 0xbe14537b, 0x3d8dc686, 0xbcedfa20, 
    0x3cfbc8bd, 0x3e0071b4, 0x3e8d6d54, 0x3e346e71, 0x3df53875, 0x3dc2104a, 0x3da73944, 0x3dbb6057, 
    0x3e349bc2, 0xbe473966, 0x3dd3147f, 0x3d641733, 0xbbbef254, 0x3eaa16eb, 0x3d913295, 0xbd789c74, 
    0xbe597d61, 0xbc5c0ee4, 0x3de8fc64, 0x3ece8511, 0xbda6f8f4, 0x3d7cdeae, 0x3db08861, 0x3e1ac0d9, 
    0xbe036ab3, 0x3e841df5, 0xbd3b4a5b, 0xbe142fe9, 0x3d118487, 0x3dfeae2e, 0x3df08e3d, 0x3da60953, 
    0x3c042831, 0xbdc5d4f1, 0xbbabdb87, 0xbe2e882f, 0xbe203d3d, 0x3e1208dc, 0x3ec44450, 0xbd92db21, 
    0xbe38e8ec, 0xbd0479d1, 0x3d225f93, 0x3dea8fc6, 0xbd8a9a5a, 0x3e1a04be, 0x3e620c89, 0xbe297605, 
    0x3d2a18db, 0xbdd32aa2, 0x3d7183c6, 0xbeb14fdd, 0xbdbe39d0, 0xbddfb2ef, 0xbd1e0746, 0x3d5453ac, 
    0x3e8e672a, 0xbf1470bc, 0x3d8e4386, 0xbe716aee, 0x3bbf9262, 0x3c8f24d7, 0x3e4ba60d, 0xbe1917f3, 
    0xbdf18af8, 0x3ed3e315, 0x3e2d5d5a, 0x3d294bb3, 0x3d81a2d3, 0xbdbc4ef5, 0x3de41b53, 0x3d87e2f5, 
    0xb9cbeace, 0xbd2f8b48, 0x3d2475a9, 0x3d55a17d, 0x3ce919a1, 0xbe0a5b24, 0xbaae465a, 0x3d8160e2, 
    0x3e4c06a7, 0xbde1e127, 0x3e75be8d, 0x3ebaa303, 0xbd236f6c, 0x3d231da6, 0x3c7c9408, 0x3d838a7a, 
    0xbe40dd59, 0x3d7c0f3c, 0x3ca7fdc1, 0x3e51e059, 0xbe88a9e1, 0xbd87c1bf, 0x3e84f655, 0x3e97079a, 
    0x3d57e375, 0xbd608027, 0xbdf990e8, 0xbdc24c5e, 0x3cc36746, 0x3dd5a1d3, 0x3c9a7e2d, 0x3cbaa65c, 
    0x3e23a174, 0xbe69fed8, 0x3d102092, 0x3ce72bd4, 0xbc391262, 0x3e0068e1, 0x3eee8f82, 0x3cc2dd7b, 
    0x3f04a001, 0xbcb25164, 0xbd827048, 0xbe76a8b1, 0x3eaaa36d, 0x3dda3748, 0x3e067573, 0xbe19db21, 
    0xbdabf7c3, 0x3e321690, 0xbd3a640c, 0x3de29d8a, 0xbd9bf0e8, 0xbe52786c, 0x3ce8b006, 0x3e2c6943, 
    0x3d01b52e, 0x3f3e1525, 0x3d6eda55, 0x3dab4278, 0x3dd01b54, 0xbda17688, 0xbdf1e7c4, 0xbd78acf7, 
    0x3eb2ad30, 0xbd0b8279, 0xbd5900c0, 0xbe4ed87f, 0x3e06f4d4, 0x3dbdcb48, 0xbce3b9d2, 0xbe61c442, 
    0x3da922ad, 0x3e0901fc, 0x3ce25fad, 0x3ba50594, 0xbc0e18c3, 0x3d2a026d, 0xbd6afc3f, 0x3d5f0ec6, 
    0x3b44f683, 0x3e013d1b, 0xbd549af4, 0x3db8a87e, 0x3e19a92d, 0xbd0c0430, 0xbdfbcce3, 0xbca5f4f8, 
    0x3eb37ad7, 0xbd9603f7, 0x3d6edabb, 0xbe0abd42, 0x3e087ecd, 0x3d39785e, 0x3cea1ecb, 0xbd28e651, 
    0x3d879687, 0x3e3c703d, 0x3dbdfd5a, 0xbccd88bd, 0x3cb32c71, 0xbe442fff, 0x3da507b5, 0x3dc6b58c, 
    0x3dd90ecc, 0x3e59d44b, 0xbd7e5b1c, 0x3c86858b, 0xbce651b8, 0x3d550c2e, 0x3c92bedc, 0xbd047f50, 
    0x3d901eda, 0xbdf16355, 0xbd94df78, 0x3cff7878, 0x3db949e9, 0xbd5bf061, 0x3e4e8698, 0xbd861948, 
    0x3d473508, 0x3d6d1492, 0x3d42b83e, 0xbde61e9f, 0xbd8f4055, 0xbde96adb, 0x3e310775, 0x3d35b3c4, 
    0x3cbe974b, 0x3e8fb383, 0x3d54cef0, 0x3d20807b, 0x3f162fac, 0x3b1a7033, 0xbe1a1d0e, 0x3ce1bfed, 
    0x3e489346, 0xbdb73e3f, 0x3daf6559, 0x3e002b47, 0xbbdb91e0, 0x3c9aa92b, 0x3e46bea0, 0xbe4ce8f0, 
    0x3dad136e, 0x3d812ed7, 0x3d21a6b4, 0xbdc687c0, 0xbda2bc20, 0x3d06542b, 0x3d51a80c, 0xbd090f0a, 
    0x3ca23a70, 0xbc3af437, 0xbd725033, 0xbe1badc9, 0x3f1ee57e, 0xbe405fa6, 0xbe5b4cd0, 0xbc699123, 
    0x3d9350ff, 0xbe55c8e5, 0xbd45e531, 0xbd8a1532, 0x3dde96fe, 0xbd3a86e3, 0x3e0c9adf, 0xbe5d6e63, 
    0xbc850a06, 0x3e3d9e95, 0x3cd41c28, 0xbe480430, 0xbcc83aac, 0xbdae5f2a, 0x3cb6d429, 0xbd951074, 
    0xbd178114, 0x3d83ff74, 0xbd75cdae, 0xbcfcfcf5, 0x3e615ff0, 0xbb85a0e8, 0x3cba50a6, 0xbcbd2eac, 
    0xbd05e24c, 0xbcedafdd, 0xbdca14d0, 0x3e78d358, 0x3dc47782, 0x3def2cad, 0x3c5215d1, 0xbdfc1c9a, 
    0x3ccf7f58, 0xbcfe347b, 0xbd8060a8, 0x3d4efc90, 0x3d9ad65c, 0x3df18b26, 0xbd2d37d9, 0xbd77b418, 
    0x3d6e7e53, 0x3f0de5fe, 0x3e0faf37, 0x3d4553a9, 0x3ec2f1a5, 0x3d698402, 0xbe0d6448, 0x3db92180, 
    0x3c4c72cc, 0xbdf36b9e, 0xbd51d826, 0x3d7ba435, 0x3c9c19d5, 0xbd9d592f, 0x3ad7e176, 0xbd1f27ed, 
    0xbd5a6958, 0xbe341edf, 0xbd9f6e20, 0xbd091469, 0xbc9e655d, 0x3e040d80, 0xbde944e3, 0xbddfb9cb, 
    0xbdcba1e3, 0xbd4d35a0, 0xbda143ab, 0x3c626852, 0x3e795a8a, 0x3dc3367c, 0xbd60ac49, 0xbd6a3cf3, 
    0xbd0a6ab7, 0xbe82aeb7, 0xbbd2a11b, 0xbd583711, 0x3e2daa29, 0x3d155b6e, 0x3e08f611, 0xbde8cdf6, 
    0xbc51b9af, 0xbca01184, 0xbe1d493e, 0xbe5a0a15, 0xbd36c4c6, 0x3da54e70, 0xbcc9beff, 0x3d8d7c36, 
    0x3d7e23cb, 0x3da13d31, 0x3d8af694, 0x3c980729, 0xbdd9913d, 0xbd85dc02, 0xbd675a99, 0x3d8228bb, 
    0xbe197644, 0x3ca0127b, 0x3bf82da6, 0x3d50aebc, 0xbc64b908, 0x3dcec7bd, 0xbcec7650, 0x3d7ced1c, 
    0x3d9bccbb, 0xbddcf7af, 0xbd8b4514, 0x3d3fe158, 0xbdd42557, 0xbe1ea6e4, 0x3c23da47, 0x3e0048fd, 
    0xbe5534a8, 0xbe8d94d4, 0xbd454f06, 0xbe615489, 0xbdc487f8, 0xbe1cf14c, 0x3d2d5160, 0x3e1157c2, 
    0xbe1eb7be, 0x3e1d30b7, 0xbd24a668, 0xbd45daac, 0xbc96229f, 0xbc5d7008, 0x3e31160c, 0x3e2c4f36, 
    0x3d64d63c, 0xbe0f5169, 0xbd2a8f8f, 0x3da9fe09, 0x3ddb79e5, 0x3e333e21, 0xbd13a643, 0x3e459328, 
    0xbd0a45c9, 0xbe844845, 0x3e872179, 0x3d824e69, 0xbe8a7a8e, 0x3dc057e2, 0x3e1f0ff2, 0x3e44026f, 
    0xbe192a79, 0xbd641c8c, 0xbe1ecae2, 0xbd4b6671, 0xbd8525de, 0x3ca593d4, 0xbd9b82f8, 0x3d599877, 
    0xba91ddbf, 0x3d310937, 0xbe194199, 0xbdefd1c3, 0x3d571d57, 0xbe05842f, 0xbe253301, 0xbdb8be05, 
    0x3e6a2ca4, 0xbe9e910f, 0x3d69d5d0, 0xbd625e66, 0xbe5fd080, 0xbc5c75b3, 0x3ee339c0, 0xbca97b12, 
    0xbe3750bb, 0x3dced300, 0x3cb57aad, 0xbcd4c96b, 0x3cc5832f, 0x3d463c89, 0xbe53947c, 0x3e040e35, 
    0xbba8e453, 0xbe4357b1, 0x3dfc7e3e, 0x3e1a3b72, 0xbdfdb476, 0x3d70d37b, 0x3d13842c, 0xbd7d0077, 
    0xbe6ac45a, 0xbe35aaf9, 0x3c000f88, 0xbd2bfacc, 0xbd69cdde, 0xbdfaf754, 0x3c287c8c, 0x3dba3818, 
    0x3dad3cf2, 0x3eaf31f9, 0xbc52998f, 0xbe41395b, 0xbd71dac7, 0x3d582533, 0xbdceaf6c, 0x3e25d397, 
    0xbc49910f, 0xbdbbe40f, 0xbd59ca17, 0x3cd1c47c, 0x3dbd540b, 0x3e10d4a4, 0x3cb28cf4, 0x3d07fe08, 
    0xbd1a04b5, 0xbdc8e7dc, 0x3e2ad81a, 0xbdc767a7, 0xbe211dad, 0xbd153ccf, 0x3f0032b9, 0x3dbb83ee, 
    0xbd7e5051, 0x3d4df079, 0x3acf6217, 0xbe1fe034, 0xbdf94e88, 0xbc8496af, 0xbcf6d95f, 0x3d256163, 
    0x3dc6dfdc, 0x3e134ed2, 0x3bf79884, 0xbde47979, 0xbb5d6d29, 0xbd802c69, 0xbd32fd07, 0xbb81da27, 
    0x3df86198, 0xbe261a50, 0xbdbc01a4, 0x3d805752, 0xbe740da7, 0xbd39f5d8, 0x3e1fe576, 0x3d87fc80, 
    0xbca1f1c4, 0xbd6c3e50, 0x3d6e37e4, 0xbc02e4d4, 0x3d7f0ba3, 0x3df2ea2a, 0xbd019b76, 0xbcd462ff, 
    0x3d14dce5, 0xbdb42d50, 0xbd09f6f3, 0xbc4687ca, 0xbe1eca86, 0xbd89a16e, 0xbd8f0af7, 0x3d0df2c4, 
    0xbebc82e7, 0xbe280c1e, 0x3b1c73dd, 0xbe7440e0, 0xbd1198c4, 0xbe47ea41, 0x3ec44698, 0xbd36701c, 
    0x3ceb25d5, 0x3ea00065, 0x3c018fb6, 0xbd9931d6, 0xbde81c6b, 0xbcc4248c, 0xbc12f16a, 0x3e5d8646, 
    0x3dca0181, 0xbda94f18, 0xbcce498f, 0xbaeea76c, 0xbdb83204, 0x3e76d108, 0xbd931c63, 0xbcdbc7c1, 
    0xbe784139, 0xbe6a39a9, 0x3e43875d, 0x3d82ce9a, 0xbe20fa93, 0xbd1fec99, 0x3d0cd164, 0xbc992ee5, 
    0xbe0409e4, 0xbc1952c4, 0xbde587df, 0xbc307ec9, 0xbe27a53f, 0xbbccf6fa, 0xbdf84800, 0xbe23cf98, 
    0x3e04aba8, 0x3d0200b9, 0xbd926bf6, 0xbdc980b6, 0xbc15f09e, 0x3c6ecb0c, 0xbc2dfa5a, 0xbcbaf57e, 
    0x3de167e4, 0x3c693f36, 0x3d0e8786, 0xbe0a79b1, 0xbe5aeb26, 0x3b3ad144, 0x3eb353bd, 0xbc6d253a, 
    0x3f1a1035, 0xbca6d90c, 0xbda0de54, 0xbe8462ca, 0x3ebef91c, 0xbbad611a, 0x3e750cc7, 0xbe6efea3, 
    0xbe394de2, 0x3e4a5ce5, 0xbde6b76d, 0xbb2b449a, 0xbd3f700c, 0xbe8ad64c, 0x3d05a4a7, 0xbc1daf8d, 
    0x3d411487, 0x3f8c3f1e, 0xbdbc8fae, 0x3e4c38ed, 0x3e825b65, 0x3cca3d03, 0xbdab1739, 0x3d929670, 
    0x3f125ef3, 0xbe008db6, 0xbd235211, 0xbe0adcfb, 0x3e29c74a, 0xbd1efbbc, 0x3d30268d, 0xbe848349, 
    0xbded5c17, 0x3e56c546, 0x3d480db8, 0xbd9f7d55, 0xbdda2f1c, 0x3d7153cd, 0xbe67312d, 0x3d317834, 
    0x3df2eaa6, 0x3e71a744, 0xbd281891, 0x3e5ed3ad, 0x3e856cef, 0xbceb53ca, 0xbd8c0538, 0x3da9828c, 
    0x3f21b0a3, 0xbe5310c7, 0xbd8d894a, 0xbe20dd99, 0x3e8163cc, 0xbce31396, 0x3ccd3a23, 0xbdb5059b, 
    0xbe0b3480, 0x3e85a091, 0x3d358f41, 0xbdb4ae2c, 0xbc8da41f, 0xbe0a254f, 0xbdd03ba2, 0x3d2b2cb9, 
    0x3de1b3c6, 0x3da03a4c, 0xbdabb3b9, 0x3e51bca2, 0xbd42f80e, 0x3b155175, 0xbd67e994, 0xbe23e544, 
    0x3e0d8f7f, 0xbdf9a608, 0x3dd0e2b1, 0x3ea02ebe, 0x3e142e52, 0x3db64074, 0x3ecc711e, 0xbd1e8256, 
    0xbd4f994d, 0x3e4d58d0, 0xbcb67912, 0xbe29d451, 0xbba029f6, 0xbd404a87, 0x3e8d24fb, 0x3e5d2233, 
    0x3d0611ca, 0x3f5a1458, 0xbbd3b451, 0xbe31033d, 0x3f8023e5, 0xbca36383, 0xbe65d084, 0x3d922ee3, 
    0x3e4e9a9a, 0xbe53179e, 0xbd22f928, 0x3d961a7c, 0xbc1a939b, 0x3dc12eb0, 0x3e433149, 0xbe723e98, 
    0xbe0bc3c7, 0x3d374a4a, 0xbc0cd539, 0xbe5a94dc, 0x3d189678, 0x3dc87548, 0x3d8ac2ae, 0x3e34bfca, 
    0xbdcdce41, 0x3da250a9, 0xbe009368, 0xbcb94322, 0x3f5e5171, 0xbe2c317b, 0xbe8fc48d, 0x3d8f5bc7, 
    0x3e8f3677, 0xbea56998, 0xbdd5062d, 0x3de4ecb2, 0x3d93993c, 0x3dea4a90, 0x3e4614a7, 0xbe19de92, 
    0xbd5873e0, 0x3daa7aed, 0x3a8c7a59, 0xbe990b54, 0xbdcafd4f, 0xbe1243cf, 0x3e0ecee0, 0x3e06539c, 
    0x3cb0f3fc, 0x3e4db10c, 0x3c304855, 0xbe1b8e18, 0x3ec163bc, 0xbcb80c37, 0x3dcafbe8, 0xbba1cbcc, 
    0xbd72f018, 0xbe9e2d66, 0xbe0719bd, 0x3d2c080a, 0x3e1c89d9, 0x3ce1ff70, 0x3d9f976e, 0xbe8388a6, 
    0xbd7f0fcf, 0x3da1fda9, 0xbd459cbe, 0x3e30221a, 0xbd0f612f, 0x3cc7c55d, 0xbe1be174, 0xbe3c57fc, 
    0x3acd51fa, 0x3f46b64e, 0xbdcdfff5, 0x3cc085d3, 0x3ed7942a, 0x3d85d323, 0xbd337e58, 0x3d0ae4eb, 
    0xbd87bdd2, 0xbe633b47, 0x3bd77554, 0xbc14bec4, 0xbd553e82, 0x3dc021b5, 0x3d2e681b, 0xbe09d429, 
    0xbde3ae8d, 0xbe3bcf11, 0xbdb3243f, 0x3c9fef2e, 0xbcb12f04, 0x3e62b954, 0xbe3ffbc5, 0xbde3b5cd, 
    0xbcfbd99d, 0xbe15af3f, 0x3d33e2f0, 0xbd525ee9, 0x3ec9d94c, 0xbcb3ff06, 0xbe3f1012, 0x3a855bfa, 
    0xbddca67b, 0xbe96b223, 0xbdcfc69c, 0xbe1bfc7d, 0x3dc16849, 0x3d09ccfc, 0x3d815266, 0xbe042c80, 
    0xbd7302f5, 0xbd0d95c5, 0xbe39650a, 0xbde52b1f, 0xbc237c9e, 0x3dcc522d, 0xbdd0f999, 0xbe2785ec, 
    0x3db2ce51, 0x3cc87a7e, 0xbdb1bc29, 0x3d15f6bc, 0xbe1bca58, 0x3b7d18c6, 0x3c8c25f9, 0x3d443a2b, 
    0x3e72036b, 0xbd35aa4c, 0x3b28529d, 0xbe16448a, 0x3efb0ddf, 0x3d9a5912, 0x3dc46bbf, 0xbe3c1118, 
    0x3d818365, 0x3e5f864e, 0xbdc8cb9b, 0x3cf22156, 0xbd8452da, 0xbe424f9f, 0x3da0a085, 0x3d8b0025, 
    0x3e020732, 0x3f41ca72, 0x3d7cfdf3, 0x3d93254d, 0x3e370e31, 0xbb68e7d2, 0xbe02634b, 0xbd566d20, 
    0x3eac0d03, 0x3d297457, 0xbcdfa2a8, 0xbe98901c, 0x3d87e308, 0x3bd1a86f, 0x3dd53089, 0xbebeb522, 
    0xbd81ee6b, 0x3c43bf7c, 0x3da4789d, 0xbdd3afff, 0x3c532e27, 0xbcea1f26, 0x3c0eeb65, 0x3d944c28, 
    0xbd320122, 0xbd723622, 0x3d5be812, 0x3e05bce8, 0x3ddd63cf, 0x3c475a14, 0xbdd1a5c4, 0x3db86285, 
    0x3e6d5a35, 0xbcfea8e7, 0x3d97fc6b, 0xbe575748, 0x3df3d4e5, 0x3d7902e6, 0x3d6398db, 0x3ce691dc, 
    0x3ca30737, 0x3e793913, 0x3cf00908, 0xbdbbafc0, 0x3dac7bb1, 0xbe3919fa, 0x3dbdeae4, 0x3daa5b0d, 
    0x3c9e0976, 0x3e1ce9da, 0xbd02d90d, 0x3db17a09, 0xbdbd2a67, 0x3da3de5f, 0x3cdbd785, 0xbc8f3e49, 
    0x3e58c7eb, 0xbe4e7530, 0x3cffb9ee, 0x3dd3166b, 0x3ea4f4cf, 0x3c1dceaf, 0x3e7592c9, 0xbdcc6d56, 
    0x3c56850e, 0x3e59b5b3, 0xbc954cf1, 0xbda59884, 0x3cc0489f, 0xbdd7b9db, 0x3e5e1417, 0x3e5ceec0, 
    0x3d0df0e4, 0x3ef5a97b, 0xbca11422, 0xbd5ffd40, 0x3efb2d1b, 0xbc92a706, 0xbe1a34c0, 0x3dd0c470, 
    0x3ec617ab, 0xbe86f938, 0x3cb6f220, 0xbbb15dac, 0xbdc6688b, 0x3e0be020, 0xbd043c92, 0xbe1ba463, 
    0xbddfe443, 0x3c265794, 0x3cd34d41, 0xbe8b626c, 0xbc1e9677, 0x3d4c1aa8, 0xbd08fd50, 0x3ce3be6a, 
    0xbc756dcb, 0xbd2a12ca, 0xbe070a84, 0xbe170f97, 0x3f03db17, 0xbdef7fb5, 0xbe733495, 0xbbd01fa6, 
    0x3e866260, 0xbe71580f, 0x3d7ea907, 0x3bed5048, 0x3db332aa, 0xbce59ec9, 0x3dabc0ae, 0xbe1d1b60, 
    0xbd4dcc3a, 0x3e85c52d, 0xbd4e6e08, 0xbe8e4d77, 0xbd8a0b01, 0xbe3897ae, 0xbbecb992, 0x3d93edce, 
    0xbd04dbe3, 0x3e0f094f, 0xbd2de1ac, 0xbda0a10c, 0x3d21aa59, 0xbd8a1d0e, 0x3db58b25, 0xbafe043d, 
    0xbe08271c, 0xbe829863, 0x3d5ca7c8, 0x3e1b43a0, 0x3e3f3747, 0xbd79f220, 0x3e1d709e, 0xbd29f189, 
    0x3dc49fa2, 0xbdb70a32, 0xbd00ddfc, 0x3e1145ec, 0x3b783c6d, 0x3e0937cc, 0x3e25828e, 0xbd062786, 
    0x3da0f8eb, 0x3ed84b2e, 0x3e064177, 0xbe0a8e57, 0x3f0a82d8, 0x3b88a1de, 0xbe1a5c66, 0x3e006087, 
    0x3b9c8a34, 0xbe9a8a74, 0x3d33230c, 0x3ddf70b2, 0x3e03c0a7, 0xbd399dd5, 0xbc6b8349, 0xbc78dc0c, 
    0x3bf3bbd5, 0xbe769e10, 0x3d8a3d09, 0xbe1c1e64, 0xbd954236, 0x3e101a5b, 0xbe2e2516, 0xbceddc2c, 
    0xbd9d3567, 0x3cd73bbe, 0xbd165500, 0xbe14d3db, 0x3f06e706, 0xbd68c464, 0xbe45d6d1, 0xbcafbc2d, 
    0xbd5269a7, 0xbe8abc37, 0xbd95fe8d, 0x3d28cc8e, 0x3e4c695c, 0x3ccc522d, 0x3a4a4d63, 0xbdc5e211, 
    0xbd82e44a, 0xbde443bf, 0x3c80fe90, 0xbe924ed2, 0x3ca5d55b, 0x3d7c2984, 0xbe1c0745, 0xbdce3bef, 
    0x3ddc2bd9, 0x3e0b05a9, 0xbd929713, 0xbda14991, 0x3dcab7b4, 0xbc735acf, 0x3df19c73, 0x3cb05eb8, 
    0xbe548449, 0xbd7a951a, 0xbdb3b360, 0xbcb77f00, 0xbd95adef, 0x3d4591be, 0xbe35ee27, 0x3de82241, 
    0xbd0dcdf5, 0x3cfe609a, 0x3d1e1643, 0xbd7926c6, 0x3cddd81d, 0xbd176856, 0x3b0977a9, 0xbd0168c6, 
    0x3d6951aa, 0xbe2bba9f, 0xbd069c55, 0xbe07c28e, 0xbe4250bb, 0xbe186feb, 0x3e266e84, 0x3c6b22bd, 
    0xbe591f05, 0x3e82caa3, 0x3d2f2556, 0xbe2fa3dd, 0x3d8c72a8, 0x3b94fccf, 0xbdd4323c, 0x3e104240, 
    0xbd975f54, 0x3dfb8388, 0xbd684cf2, 0x3e03703a, 0xb941834d, 0x3e0bf60b, 0xbe602698, 0x3c790610, 
    0x3e20469c, 0xbed923cf, 0x3de4ed57, 0xbda9cc68, 0xbe3affe6, 0x3d1f99c5, 0x3cbb3079, 0x3ca24fad, 
    0xbdc1ab36, 0xbd6358e1, 0x3d80502d, 0xbdca5745, 0xbe5814b5, 0x3d004d11, 0xbd0d047b, 0x3e4668dc, 
    0x3d0e764f, 0x3dfc08fe, 0xbe4f325d, 0xbda3b842, 0xbdfd452b, 0xbe072904, 0xbd8f0199, 0xbddf03b2, 
    0x3e1756a4, 0xbddf32cc, 0xbdcc4999, 0xbc2eef85, 0xbe66f2c0, 0xbce64cd0, 0x3eddfd5d, 0x3cb93839, 
    0xbdb9bb8a, 0xbd513943, 0x3dc5ab28, 0xbdf45635, 0xbce27f23, 0xbd35a142, 0xbd5c4b6d, 0xbd952088, 
    0x3c3df628, 0xbceda172, 0xbdf9284d, 0xbdf7bde2, 0xbccf2008, 0x3e414886, 0xbd1e0a00, 0x3a991ed8, 
    0xbe3d4be4, 0xbdc54be8, 0xbcddace4, 0xbe3bad18, 0xbe588b3d, 0xbdd4acc8, 0x3de5e5ef, 0xbddf9119, 
    0xbc024bca, 0x3e676a9c, 0xbd7f5c90, 0xbd6ebd83, 0xbd0afcb0, 0xbc9ac66c, 0xbb6e9f1c, 0x3eb160a8, 
    0x3dafb63e, 0x3dfb8e04, 0x3da3cbba, 0xbb8075dc, 0x3d29393c, 0x3e45b6b2, 0xbd745865, 0xbda0620d, 
    0x3cde6622, 0xbe4af297, 0x3dd40a71, 0x3d2f6b0d, 0xbca02510, 0xbe2d0109, 0xbdb631c3, 0x3c81c27b, 
    0xbd437a4b, 0x3d5ed19d, 0xbcfdad4e, 0xbda41315, 0xbd1eb8c2, 0xbbd9dbee, 0xbd85a5e5, 0xbe0d150a, 
    0x3d813dc9, 0x3de4cae3, 0xbdce59d1, 0xbe29e641, 0xbc82ff47, 0xbb5cafdb, 0x3e106364, 0x3dcc9077, 
    0xbda4d943, 0xbcb6b741, 0xbafbd286, 0x3d45e521, 0xbe6c66f2, 0x3dff8ca2, 0x3d1cc333, 0x3d1c0b0a, 
    0xbca94158, 0xbd7eceb1, 0x3b978e34, 0xbd508eab, 0xbde72bd1, 0x3caf8765, 0xbe12b1c9, 0xbd007417, 
    0x3d21fa6e, 0x3dd2b103, 0xbcb454b6, 0xbe103908, 0x3da99e9f, 0xbe0c01bf, 0xbdaead34, 0xbe2c34a1, 
    0xbbbd580d, 0xbdc26ee9, 0xbdd3fcad, 0xbdac3b96, 0xbe3f6768, 0xbe13e2b5, 0x3e97ac24, 0x3cb15df5, 
    0xbe473ed0, 0x3dfe486f, 0xbb737bda, 0xbdae4f74, 0xbde95cab, 0xbd7dc63f, 0xbde2133d, 0x3e619aea, 
    0x3d653b33, 0xbdcdc3fd, 0xbc0c8802, 0x3e003091, 0x3d172a94, 0xbdfd67d3, 0xbd8b618c, 0xbdedf5df, 
    0x3d342f62, 0xbe9615f7, 0xbd04a2a0, 0x3e357d87, 0xbd1e9370, 0xbdc47c0e, 0xbdc4be52, 0xbd828687, 
    0xbe74ee0c, 0xbdcb3680, 0x3c8fd987, 0x3c519000, 0xbe48ffc0, 0xbdcf6c30, 0xbdca983e, 0x3d4d0738, 
    0x3d2d9670, 0xbc4ef9c3, 0xbe5807f2, 0xbe608913, 0x3dae38bf, 0xbe20ab42, 0xbde69e95, 0xbdc2d842, 
    0x3ddcb242, 0xbddc87e1, 0xbdd9929b, 0xbcf7cb01, 0xbe2cf74d, 0xbe1a1104, 0x3d39858d, 0x3d41262f, 
    0xbdabc665, 0xbcd927b2, 0x3e1e82fa, 0x3e1dd0e1, 0xbd9051e4, 0x3dadeafc, 0xbe4b1b2c, 0x3e63756d, 
    0x3dbb9e9e, 0x3e0e800c, 0x3dc5199f, 0xbdf599b3, 0x3c09c78b, 0x3e127510, 0x3c3ca804, 0xbccb493c, 
    0x3df734ec, 0xbe1edaec, 0x3d5d2106, 0xbbb9d25b, 0xbe0fff6a, 0xbc98bba6, 0x3c71ce5a, 0xbdc90dca, 
    0xbd8c3e2d, 0x3dd0209e, 0x3dff7991, 0x3e4a984c, 0xbaa08f62, 0xbccd9b19, 0x3c9a302e, 0x3e0adee4, 
    0xbda83ad4, 0x3d6f08d1, 0xbd719e66, 0x3d1f9854, 0xbe0328e4, 0xbcf945bd, 0xbb1adcd1, 0xbd3a5c2e, 
    0xbc518f0e, 0xbe6b4263, 0x3c93f5ea, 0x3e00c32e, 0xbd7c1730, 0xbd84f774, 0x3d3b3ed5, 0x3d8af5fc, 
    0xbdd17704, 0x3e4a488c, 0x3cb8a3d7, 0x3e6c270c, 0xbe0cb41e, 0x3cac9236, 0xbdabc69d, 0x3f0be014, 
    0xbd71e68e, 0x3e035821, 0x3d923788, 0x3e1e11eb, 0xbe14f0f6, 0x3de127cf, 0x3dc42222, 0x3e06e834, 
    0xbde97704, 0xbdb42c11, 0x3c2e2d51, 0x3c9c12cd, 0xbdb5aec9, 0xbad3aeae, 0x3ea5d2c5, 0xbd82ec7e, 
    0xbe645abe, 0xbd854e97, 0x3e0d338a, 0x3e0db0cf, 0xbe44a880, 0xbdc4ceab, 0xbbd7223b, 0x3e0d37f8, 
    0xbc22fb9e, 0x3e170846, 0x3defc1d1, 0xbda564d2, 0xbdcbeb5b, 0x3dc54135, 0x3db6aeac, 0xbd9e236e, 
    0xbc74f52e, 0xbea95ac9, 0x3d92c040, 0x3d0b0b0c, 0xbdca93d1, 0x3c89dd05, 0x3d13d95b, 0x3c3120e6, 
    0xbd6b9835, 0x3dcf3e39, 0x3ddb6eed, 0x3e5a54cb, 0xbaf2e464, 0xbde5e83a, 0xbd8e9992, 0xbd3a35de, 
    0x3cfb5cd0, 0x3ce5973f, 0xbbd74106, 0x3d5584c2, 0xbdf764ec, 0xbb9ecdb4, 0x3d6c2bff, 0xbb87385d, 
    0x38a8f2f8, 0xbeafce96, 0x3deed5cc, 0x3d0c225c, 0x3d050ec2, 0x3d972775, 0x3deecfeb, 0xbd95fca7, 
    0xbe4d37c8, 0x3e705343, 0x3dfffe25, 0x3ec4e941, 0xbca49ee9, 0x3d878db7, 0x3d02af1e, 0x3ee62a9b, 
    0xbd843a7a, 0x3cfa9d40, 0x3c7f0969, 0x3e221745, 0xbe407b5d, 0x3de60426, 0x3dd4aa56, 0xbc966cf6, 
    0xbdb266d0, 0xbe564ed5, 0x3c970b4d, 0x3b99b7e2, 0xbcc3f210, 0x3dc5f9cd, 0xbb649a82, 0xbccd2255, 
    0xbe2c8c3f, 0x3d1f5b61, 0x3e07ff20, 0x3e4aaebb, 0xbcd0ff94, 0xbd2859b3, 0x3e1b54c1, 0x3e189a12, 
    0xbd2e0e23, 0x3d7dca57, 0x3dccaebb, 0xbe42d669, 0x3c90f5c6, 0x3e2b90b9, 0xbbb8977e, 0x3c9368d5, 
    0x3e113c0f, 0xbebb4d47, 0x3dbeaf64, 0xbd17079f, 0xbbaeaef1, 0x3db80bac, 0x3d5c15ed, 0xbe144241, 
    0xbe507e90, 0x3e0dc7c6, 0x3d662d7c, 0x3e0e9f2a, 0x3bea821c, 0xbd9b9d39, 0x3e02f486, 0x3e1c0ada, 
    0xbde8fb38, 0xbd267c54, 0x3cebe913, 0xbbb62c2f, 0xbe16038e, 0x3cbc79dc, 0xbd3fe2e8, 0xbd972cc6, 
    0x3e14f24f, 0xbe9b492d, 0x3e345ed5, 0x3d89f1d0, 0xbc677078, 0xbb91e30e, 0x3e5e316b, 0x3d22d2a7, 
    0xbe4e77af, 0x3e917083, 0xbd6333df, 0x3e9dc282, 0xbd4c8bed, 0xbdd522f7, 0x3de99199, 0x3ea8b4cd, 
    0x3d097529, 0x3e675412, 0xbd909749, 0x3e59089f, 0xbdbb92f4, 0x3e452051, 0x3cc0f0cf, 0x3c7b2446, 
    0xbd9e3edd, 0xbe9ab932, 0x3db9721f, 0xbce2554a, 0x3df71ae7, 0x3caf3593, 0xbe532827, 0xbd91148f, 
    0xbdb8e0b0, 0xbd0889df, 0x3da70170, 0x3bd117dd, 0x3cc7cb84, 0x3e3c1033, 0xbd1e0da8, 0xbc827562, 
    0xbdd2342d, 0xbd86ab1d, 0x3d5ee82c, 0x3de1a1de, 0xbd863d14, 0xbdc8166e, 0xbcf21a2a, 0x3da03b0c, 
    0xbd8e3a11, 0xbe70c96b, 0x3c412c05, 0xbe0b5a5b, 0xbdd75d43, 0xbe1b7a48, 0x3c405831, 0x3dc0f8f1, 
    0xbe7e9d1c, 0x3d4ea13c, 0xbd82ccb3, 0xbd87eff6, 0x3ca00c85, 0xbdfb5b92, 0x3e4aafd7, 0x3da375f1, 
    0xbd8481cc, 0xbe20d780, 0xbe11af61, 0xbd408f76, 0x3d9d1f34, 0xbd7ef599, 0xbe2a2859, 0xbc8e108c, 
    0x3d9a1471, 0xbe7f1dfa, 0x3db2c71c, 0xbcb31265, 0xbe1a946f, 0x3db32fbe, 0x3f0a6cd4, 0x3e147ce4, 
    0x3dada722, 0xbd8590e6, 0xbd9c9ca8, 0xbdea58b9, 0xbda77801, 0xbd9effc5, 0x3da400df, 0xbd1f0ca7, 
    0xbd0e3367, 0x3dadfb32, 0xbdf3a64d, 0xbdf49422, 0x3cabf307, 0xbe233b21, 0xbe140b88, 0xbddef95f, 
    0x3e0d5456, 0xbe374c19, 0xbcd39c37, 0x3d949058, 0xbe837e74, 0xbce40b82, 0xbdc2b687, 0x3b896a5a, 
    0xbde9f06d, 0x3de354df, 0x3c9946db, 0xbdc24ad1, 0x3d843fd7, 0x3da94f3f, 0xbddbba48, 0x3d50e7f6, 
    0x3d472dc4, 0xbdbbed57, 0x3d49e675, 0x3da81833, 0x3c8a02cb, 0x3d6c9a82, 0x3d84a9dc, 0xbd8bd182, 
    0xbe844581, 0xbdc96534, 0xbde18748, 0xbde93ed3, 0xbd5b628f, 0xbe4278e3, 0xbb9aa8e0, 0xbc6c5c05, 
    0xbd9b761d, 0x3e259cbe, 0xbdb9de65, 0xbd6a0dca, 0xbd018e2e, 0x3d94690a, 0xbd258aca, 0x3d045234, 
    0x3d11fb5c, 0xbd50cd81, 0x3da4e944, 0xbd017918, 0x3dd15908, 0xbc19b732, 0xbc9e3927, 0xbd01a73d, 
    0xbcb54836, 0xbe0ca376, 0x3defd4f2, 0x3ddb92c3, 0xbd803636, 0xb9bc4500, 0x3d9d530d, 0x3dd342b1, 
    0x3d6c542f, 0x3d8e31e5, 0xbe139ad6, 0xbdf9fbf6, 0xbdd539ad, 0xbd751910, 0x3cb018cd, 0xbb943816, 
    0xbdf8d309, 0x3e03bcd1, 0xbe10ac9e, 0xbcad1a9d, 0xbdd47c90, 0xbe409e10, 0xbd39e531, 0x3d3a151b, 
    0x3dadf823, 0xbdece05b, 0xbdefa98c, 0x3de926aa, 0xbe6c2020, 0x3cb3dbbb, 0x3bb60300, 0x3d75ceef, 
    0xbd2ccc50, 0x3c1454da, 0xbd3e4302, 0xbd9c7e48, 0xbc1eec53, 0x3d80cc64, 0xbd877706, 0xbd0d299a, 
    0xbd1c23cd, 0xbd8d6387, 0xbd5fadfc, 0x3e34291c, 0xbd765365, 0x3d627e63, 0xbcb27479, 0x3c17318e, 
    0xbec098c4, 0xbdc25a53, 0x3cd418e6, 0xbd9bb35c, 0xbcfe73a5, 0xbe681cf6, 0xbcfdeddb, 0xbd84c3cf, 
    0xbdc6c57d, 0x3e629f14, 0xbd32e126, 0xbd3ca838, 0xbd8feaef, 0x3c9174cc, 0xbe089448, 0x3da4b558, 
    0xbdf75460, 0xbe5ef89b, 0xbe290a74, 0x3d43dd2b, 0x3b5d57a4, 0x3d853272, 0xbdeec021, 0x3d86a976, 
    0xbe8255a3, 0xbe4b0317, 0x3e1b0ae4, 0x3d8f4b38, 0xbd03697d, 0xbdf7e6be, 0x3ef3e98b, 0x3e023e30, 
    0xbdb15898, 0xbd42c94d, 0xbe2291b1, 0xbd57a870, 0xbdd070f3, 0x3d138427, 0xbd00a57b, 0xbc5b1010, 
    0x3d3fc177, 0x3dc1300e, 0xbd0b50be, 0xbdfccaf5, 0xbdb12d64, 0xbe885694, 0xbe297503, 0xbd8b0585, 
    0xbc28bb04, 0xbd498898, 0x3c303509, 0xbdae63e5, 0xbe222713, 0xbd48484a, 0x3bd0ae0b, 0x3db32865, 
    0xbe484e37, 0xbcb49ecf, 0x3d6cb9c3, 0x3d77f2e7, 0xbcfde1b4, 0x3e0d6755, 0xbdf1d09f, 0xbd074541, 
    0xbc425ba3, 0x3d7bf1a2, 0xbd719559, 0xbd7e9a76, 0x3d814452, 0xbdad8d2c, 0x3d393b03, 0x3c8c1d7f, 
    0xbe0fd308, 0xbeb47f05, 0x3c50989f, 0xbe6122cd, 0xbe7ae8bf, 0xbdd6d5d7, 0xbcc0ac76, 0x3d543062, 
    0xbe5a5e93, 0x3e91729d, 0xbc4cf774, 0x3dacca60, 0xbd293635, 0xbdc5bbbe, 0x3e313028, 0x3d4261c8, 
    0x3d80b83a, 0x3d23c033, 0xbcea874f, 0x3b98e0f4, 0x3d816150, 0x3d72152c, 0xbe0752e1, 0x3ddeb69f, 
    0xbccb155e, 0xbee7803e, 0x3e35dcbf, 0xbd02b20f, 0xbe54fc21, 0x3d28a9ec, 0x3e8ceefb, 0xbb06f8a5, 
    0xbe278aa1, 0xbe3f0097, 0xbe4c138e, 0xbe3e6210, 0xbc9a3b4f, 0xbded9f0a, 0x3d0597c4, 0xbde9f9e7, 
    0xbd5583eb, 0xbd51207d, 0xbdadd594, 0xbe5b4561, 0x3d82a0b8, 0xbe6e4572, 0xbe4c3618, 0x3c6220b3, 
    0x3d8861ab, 0xbc00ba8a, 0x3b129b35, 0xbcb039d5, 0xbe7a74f9, 0x3c9fd275, 0x3b8d5c61, 0xbc64fb19, 
    0xbe374882, 0xbda92ec8, 0x3c77737d, 0xbc997a4a, 0x3d27ecfc, 0x3d3f8f2f, 0xbe03831b, 0x3e527574, 
    0x3bbde5d8, 0xbdb46733, 0x3d93ec81, 0x3da58df7, 0xbd4da4a0, 0xbdb81537, 0x3d5bbaad, 0xbd34fa29, 
    0xbe34389c, 0xbe4dbb9b, 0xbd775b3f, 0xbe0b2d48, 0xbdbf0d33, 0xbe843e11, 0xbdd901c9, 0xbcc52277, 
    0xbdcdbdcb, 0x3ea5e8c5, 0x3d4c8fba, 0xbcf2f993, 0xbcfe8d64, 0xbd66a4f0, 0xbe442734, 0x3e0f6676, 
    0xbd89eb8b, 0xbbe1a0ee, 0x3d65f112, 0xbb249382, 0x3cf5b04a, 0x3d0ab9c5, 0xbcb4988a, 0x3c9be0e4, 
    0xbdb777f6, 0xbe84abf0, 0x3e58c844, 0x3d635658, 0xbddc9a73, 0xbde95e56, 0x3ee6703a, 0xbd0f8d15, 
    0xbe3153bc, 0xbe134091, 0xbb5145f0, 0xbe7eeeff, 0xbe3cfe17, 0xbcb5b904, 0xbe15c003, 0x3e7e90e0, 
    0x3d893f5a, 0x3e1c6aac, 0x3b065a02, 0xbc57293a, 0x3b32929c, 0xbe7cc0e1, 0xbd6aa549, 0xbccbe3a7, 
    0x3e2b8046, 0xbdba3f42, 0xbdbe7d85, 0xbde627b8, 0xbe67c490, 0xbd0eefed, 0xbe0da67d, 0x3dd80955, 
    0xbe012067, 0x3cb9e4c0, 0xbbdc2cf6, 0xbd82a549, 0xbdb37731, 0x3c80a3dc, 0xbde8142e, 0x3d577cc4, 
    0x3d6e1223, 0x3d1d3b0f, 0xbc13751c, 0x3d2fc8cc, 0xbdadd3bb, 0x3d95957d, 0xbd73e5b2, 0xbdc4b982, 
    0xbe48069a, 0xbe2ef1c8, 0xbe1b29bb, 0xbe7fa275, 0xbe26361a, 0xbe25a181, 0xbdc80fc5, 0x3d23ef3e, 
    0xbd189b45, 0x3f02080f, 0x3db9ecd7, 0xbe3314e4, 0x3bf35790, 0xbd810a89, 0xbc67db98, 0x3d6d113e, 
    0x3c807da1, 0x3d249fb2, 0xbe060132, 0x3e1bbce0, 0xbd935500, 0x3e92bf05, 0xbddddb58, 0x3e1eeb96, 
    0xbe21aaeb, 0xbe9d570e, 0x3da3c52d, 0xbc160645, 0xbe00acaf, 0xbdbb6f60, 0x3e7c6715, 0x3db265fa, 
    0xbe017e1b, 0x3e0d7926, 0xbe0f1f76, 0xbe2488b8, 0xbdfbe91d, 0x3d7d62bc, 0xbb80e48a, 0xbbbd5962, 
    0xbdb6a848, 0x3dcbb9c8, 0xbd9cef2a, 0xbe0dc48c, 0x3da69af4, 0x3d7a132b, 0xbb1b1f19, 0xbd863cb1, 
    0x3ba97a25, 0xbd2ebade, 0xbc56482b, 0x3d5c0bb1, 0xbe743e47, 0xbda5d194, 0xbe45f363, 0xbd266e64, 
    0xbe794994, 0x3d36d753, 0x3d258cd6, 0xbd10b3ab, 0x3cb228cb, 0x3dc25aae, 0x3c9c2b56, 0x3c1566d7, 
    0x3e0c9e1a, 0xbe0f572a, 0xbe75167d, 0xbcbd1d31, 0xbc5d8ad8, 0xbdc525b8, 0xbd68d3f2, 0x3da9d10c, 
    0xbd9e398e, 0xbd93d0f7, 0xbce3de03, 0xbe49f14b, 0xbdfeafde, 0x3d3be599, 0x3d81de6e, 0x3c813090, 
    0xbdfa0e68, 0x3d644472, 0x3d1b0a91, 0x3d563e14, 0xbc37d696, 0x3dbd2553, 0xbdbacfeb, 0x3d1dcc6e, 
    0x3d87d3eb, 0xbda3254b, 0x3d30c3cd, 0xbd7b33e0, 0xbd814a96, 0x3c5c6229, 0x3c3374dc, 0x3e16d3a2, 
    0xbd213482, 0xbea3aa15, 0xbc674c42, 0xbd1e0562, 0xbe809b8c, 0xbe300bf5, 0x3e420701, 0xbcc5c296, 
    0xbdda2148, 0xbd9f5f46, 0xbe61613a, 0xbdad49a4, 0xbd7da87c, 0x3dbc5437, 0x3dc335c7, 0x3e8c84e2, 
    0x3da3127d, 0xbe122c25, 0x3d54a18d, 0xbd06f4a7, 0x3d85c9d6, 0xbd472a86, 0xbe3c227f, 0x3d3f2718, 
    0x3e605e95, 0xbe895139, 0x3e8ac2aa, 0xbcf60b71, 0xbed56294, 0xbc91d253, 0x3edbd82d, 0x3d943acf, 
    0xbdca60b2, 0x3e242d98, 0xbc7733c8, 0xbdefaa04, 0xbdd4ef52, 0x3d70405f, 0xbd9aa06a, 0x3cd0ff51, 
    0x3d2e97c7, 0x3d464076, 0xbd9955e7, 0x3d1d2301, 0xbd1d0c31, 0x3d715338, 0x3da2bca5, 0xbccd1421, 
    0xbd94921b, 0xbe19781b, 0xbdd4a77e, 0xbd716c24, 0xbc8c2f40, 0xbdede65a, 0x3c85aa54, 0x3d879f91, 
    0xbda6ef19, 0x3e566a56, 0x3d5ca682, 0xbdfec26a, 0xbd6fe4df, 0x3d83be4d, 0xbddd9922, 0x3df82c37, 
    0x3e01d6da, 0xbd71f1b8, 0xbcd99ae2, 0xbd904a48, 0x3d43ceed, 0xbd160ae5, 0x3bf7075b, 0x3d3b10c6, 
    0xbe7ac7d1, 0xbe8adc42, 0x3e299599, 0xbdaa8b33, 0xbe345076, 0xbdf4ef8a, 0x3e192f90, 0x3da9d39f, 
    0x3d4ac570, 0x3ea572b3, 0xbd6ec24d, 0xbe829dfd, 0xbdbdb0dd, 0xbd2596a5, 0x3ca46708, 0x3e5b42e8, 
    0x3b5a0712, 0xbd02b588, 0x3dc5c91b, 0x3d838f8e, 0x3d51666d, 0xbe27cdca, 0xbc8c12da, 0xbc9b0f15, 
    0x3da0fe43, 0xbdb310c7, 0x3e77dd08, 0x3d6caf62, 0xbe5ec030, 0xbbe76d55, 0x3e8a9f1b, 0xbd99477d, 
    0xbdaec6aa, 0xbd735461, 0xbdfa7d0f, 0x3ae4ed1e, 0xbd79a3dd, 0xbd1cff32, 0xbdef06a9, 0xbd5069a8, 
    0x3d8d66f9, 0x3dacd647, 0xbe5b960b, 0x3c963c73, 0xbd6cd897, 0xbd8dee3b, 0xb92fe430, 0xbd87e054, 
    0xbe61c414, 0xbe1e7fed, 0xbc238add, 0xbe02f486, 0xbd383c3d, 0xbd83607a, 0x3ddb82c7, 0x3cff6442, 
    0x3c579702, 0x3e98fc80, 0xbe0a5960, 0xbdce2519, 0xbe21a6eb, 0x3e1d0bbc, 0xbe04d884, 0x3d89b77d, 
    0x3c22f8c2, 0xbc9cf439, 0x3d593873, 0xbd75cd1c, 0x3d462ebc, 0x3db668c7, 0xbda0d710, 0x3df53b3f, 
    0xbe96e6de, 0xbeabe048, 0x3da1aee4, 0xbe68ce41, 0xbdf1e064, 0xbd914c46, 0x3e3c2e75, 0xbd85b72a, 
    0xbe78ab8d, 0x3d0511cd, 0xbe3afb50, 0xbd988102, 0xbd84d088, 0x3e230f84, 0xbd4fbc4b, 0xbd2af790, 
    0xbd30e562, 0xbe1b60ad, 0x3d31d14f, 0xbdb2aae2, 0x3e177d9f, 0xbd233036, 0xbd9de586, 0xbc1b9c19, 
    0xbca4ea26, 0xbdbb600b, 0x3e2a9996, 0x3d6cb6b6, 0xbe806b9f, 0xbe2e836d, 0x3dbfdd87, 0x3c27b3a3, 
    0x3eed8128, 0x3de58902, 0x3e16700e, 0xbdc1c217, 0x3e4b7fc9, 0x3deecec7, 0x3d92b4bb, 0xbe10c37f, 
    0xbde605dd, 0x3e747d77, 0xbcc9defe, 0x3d8c6f72, 0xbd93f405, 0xbe89dbf2, 0x3e26b4b3, 0x3da532f0, 
    0x3d6ba0eb, 0x3f6b71a7, 0x3c1d8dff, 0x3d8ef891, 0x3e43c2f4, 0xbc861144, 0xbe142d13, 0x3db89bd6, 
    0x3ec39f9a, 0x3c8f0a17, 0xba0f18ce, 0xbe0e2bc5, 0x3dd2c67b, 0x3d9867d0, 0xbdd53993, 0xbeb34d02, 
    0xbd91fd18, 0x3de626fb, 0xbc7671f8, 0xbcd475c9, 0xbe0c0542, 0xbdbd6833, 0x3c5ed6d9, 0x3dc417be, 
    0x3cb5a3af, 0xb99636ae, 0x3daad8c4, 0x3d4f3034, 0x3e81cc13, 0xbd8e269f, 0xbe79dbdd, 0x3e2c7404, 
    0x3ddfe545, 0xbe3231c7, 0x3d7aaaec, 0xbe589263, 0x3e46f960, 0xbdec4878, 0xbd0a53f0, 0xbdb08459, 
    0x3da1fca4, 0x3eaf1760, 0x3daeebe3, 0xbcdb048a, 0xbd8761b3, 0xbde812dd, 0x3e1aac0d, 0xbb488125, 
    0x3d8673b7, 0x3e327922, 0xbcb295da, 0x3c71a053, 0xbd95e8d9, 0x3d21515c, 0xbd8b0c9a, 0xbd44f52b, 
    0x3e8ef11a, 0xbd2273a1, 0x3b783ab3, 0x3d1ffeb1, 0x3df0a4db, 0xbd903390, 0x3d6ca6e7, 0xbe271af7, 
    0x3c0f26dc, 0x3b91d51e, 0xbd88826a, 0xbdaada53, 0x3db58bdb, 0xbe22319e, 0x3e6f1eb1, 0x3e334ab1, 
    0xbc7d2337, 0x3f10738c, 0xbd2bfc48, 0x3dcaf496, 0x3e979df8, 0xbd220e5b, 0xbe11ed8b, 0x3d434730, 
    0x3eee6767, 0xbe981eec, 0x3d8014c8, 0x3d9efa97, 0xbd307089, 0x3c1c99bb, 0x3db074d8, 0xbeb855f0, 
    0x3dde46b9, 0x3d64d841, 0x3d81f389, 0xbe375bce, 0xbc22f260, 0x3db07260, 0x3d59307f, 0xbd9ef3a1, 
    0x3bdfb4bb, 0x3d864fac, 0x3c62a193, 0xbd7fba88, 0x3eb4c3a6, 0xbde6b390, 0xbe3ae195, 0x3dd2d9f9, 
    0x3eb7c3fe, 0xbe264f84, 0x3c4f94de, 0xbd650344, 0x3ae843fd, 0x3d2b5a44, 0x3e345b6b, 0xbe36b020, 
    0x3d2e9fe1, 0x3e6095ca, 0x3d883ba7, 0xbec457be, 0x3d585059, 0xbd6aabfe, 0x3d3c24ca, 0xbd3389c4, 
    0x3da4f9c6, 0x3e6e5cb1, 0xbd2e2a0c, 0xbd98a9f1, 0xbd0ef6d4, 0xbd0d8f70, 0xbc075865, 0xbd4117f6, 
    0xbce325b6, 0xbe7a060b, 0xbdc9c734, 0x3e4266cd, 0x3e6a6dc3, 0xbdae7648, 0x3e80da94, 0xbd3e6656, 
    0xbdea4aa3, 0xbe036261, 0xbda2059b, 0x3e0a79d8, 0xbd97f556, 0x3e1e9bd8, 0xbb41c58b, 0xbe3b0d6d, 
    0x3d83e3f4, 0x3f1b055c, 0x3c5c767d, 0xbe4453fd, 0x3f2aaac5, 0x3d2a8e8e, 0xbe167de4, 0x3de9eb02, 
    0xbd806eba, 0xbe160b83, 0xb99050f4, 0x3e227ae7, 0x3bab6ee0, 0x3cba408c, 0x3c969f72, 0xbe112cc6, 
    0xbd198911, 0xbdd61046, 0x3c78c7e3, 0xbd0ba613, 0xbd4a908f, 0x3e3229de, 0x3d3e0bfd, 0xbd968668, 
    0xbd400046, 0xbd7ad466, 0xbe13c980, 0xbdaa53c4, 0x3f16bb07, 0xbd57eeba, 0xbe10d76e, 0xbd64829d, 
    0xbd3f8748, 0xbe888682, 0xbd113364, 0x3d691dab, 0x3e7d26a7, 0x3d9909fa, 0x3e6ce237, 0xbde88178, 
    0xbdbc53a4, 0xbdd3fc61, 0xbe0f3dea, 0xbe25b357, 0xbd708221, 0x3dcd1705, 0x3e132f66, 0xbe37fe1d, 
    0xbd93d691, 0xbd3785ed, 0xbe11ef5c, 0xbd99b6f5, 0x3da4b3fc, 0xbcb6de85, 0xbcd75c48, 0xbd96adff, 
    0x3e57e9ce, 0x3d9dd63a, 0x3e03637e, 0x3d8c82b7, 0x3e20b8a3, 0xbd01626c, 0xbe09aebc, 0xbd28592f, 
    0xbd8bf4ad, 0x3ea01e9a, 0xbdad73eb, 0x3e362f5c, 0x3ac89f42, 0xbdfe9dfe, 0x3e0a4d55, 0x3c12928b, 
    0x3d932de2, 0x3f3640d4, 0x3db6bd68, 0x3d4bb3d4, 0x3d42da23, 0xbcd6ee1b, 0xbdc33f8c, 0x3c5bf37d, 
    0x3e694ed4, 0x3bb93fd8, 0x3ca948d5, 0xbc8f9875, 0x3e2afbf1, 0xbd75831e, 0xbd5f5566, 0xbe5c22d3, 
    0x3c980d0f, 0x3d98ce6a, 0xbd552ea0, 0xbe5a9c88, 0xbd5a0544, 0xbc87abdb, 0xbab71d50, 0x3dc65abd, 
    0xbd9da903, 0xbd94d691, 0x3d8a611d, 0xbcd846f6, 0x3dbfae6b, 0xbd88853b, 0xbd34a498, 0x3de49eff, 
    0x3db8780b, 0xbdf890e6, 0x3daefd73, 0xbe2dc384, 0x3d0b242a, 0x3e03dee6, 0x3d4b4cc9, 0xbdc42023, 
    0xbd9e3c5d, 0x3e213995, 0x3d27740b, 0xbde5e441, 0x3db0c463, 0xbe39a0b7, 0x3ca88908, 0x3d837664, 
    0x3d3c0415, 0x3e530fbe, 0xbceab589, 0xbdcd0da6, 0xbcd2871a, 0x3d88623a, 0xbd9cd635, 0xbbc599a0, 
    0x3e9dc017, 0xbe0895bd, 0xbe04cf40, 0xbe10b698, 0x3e757c2a, 0x3dd97549, 0x3d98d4b5, 0xbe5ce354, 
    0x3ddc3b7c, 0x3d9065dc, 0xbdd9a55e, 0xbdb5a7c5, 0x3b58817d, 0xbcdec8bd, 0x3dc650ba, 0x3d02ef77, 
    0xbd7ec88d, 0x3f01335d, 0x3c8c7aa5, 0x3e4a1790, 0x3e8b280a, 0xbd7bc0ba, 0xbe4bfe5c, 0x3bf76c29, 
    0x3e8683c1, 0xbe255a89, 0x3dcfea40, 0xbc23aad4, 0x3bb148d2, 0x3cbd5751, 0x3d9e753e, 0xbe3f331d, 
    0xbc6d5921, 0x3dae27be, 0x3db984a6, 0xbd681ec4, 0xbcf246bf, 0x3db1edcb, 0xbd883a60, 0xbd1817ae, 
    0x3d35431b, 0x3ddcaaa4, 0xbd8f3244, 0xbd39301d, 0x3e75d4d2, 0xbde34fa4, 0xbe0b00a8, 0xbc279ed5, 
    0x3e8a88c3, 0xbd464ef2, 0x3c485c90, 0xbd146ffb, 0x3ca620f6, 0x3c1b6653, 0x3af415ba, 0xbe36bae6, 
    0x3cd51345, 0x3e4c7637, 0x3d3e4f07, 0xbe76fd52, 0x3e0b1262, 0xbdc3c96c, 0x3d6b8a94, 0xbda410b4, 
    0x3d46b1c5, 0x3e72bde0, 0xbc95e6b0, 0xbd3ec8c2, 0xbd95e8ae, 0xbc9f973c, 0xbdd76578, 0x3da22645, 
    0xbce59079, 0x3d7f558c, 0xbcfe3ed3, 0x3de5309e, 0x3dccff66, 0x3c171381, 0x3e9d65e1, 0xbb5ae7e8, 
    0x3e0b9f82, 0xbe518a9c, 0x3b9cf0cd, 0x3d3f3e81, 0xbda9e45f, 0x3de1a4ae, 0x3d96f69e, 0xbc218aa2, 
    0x3e249f71, 0x3e72a0e7, 0x3cf51002, 0xbdb90b75, 0x3f165457, 0x3d8ad8fa, 0xbd219b34, 0xbc434890, 
    0xbd52a461, 0xbe0e6d8d, 0x3da4c2e4, 0x3dbc4626, 0x3e0cefcc, 0x3d5e0846, 0x3e0daff9, 0xbdfedf6a, 
    0x3db20ffc, 0xbdf34f24, 0xbd4810fe, 0xbd9e076c, 0x3bf74f3e, 0x3e2ba0c6, 0xbc4bc054, 0xbd770bb0, 
    0x3caf0ef2, 0x3e171841, 0xbd35b84d, 0xbcc10dbc, 0x3f067e91, 0x3bf368d5, 0xbd95543c, 0xbd8cd215, 
    0x3d09d759, 0xbe80c162, 0xbcf5d95a, 0x3d7d299c, 0x3e448e4d, 0xbc8a7b09, 0x3dc4d5f3, 0xbe3ac8d0, 
    0xbda03d03, 0xbd933505, 0xbdd4c367, 0xbe6b7a75, 0x3d9fc939, 0x3cfa7380, 0x3cf476ff, 0x3d8c7489, 
    0x3d98ee7b, 0xbd53447f, 0xbcef2d44, 0xbd6ebda3, 0x3ddaedb6, 0xbd8fe8b5, 0x3d9f8d8a, 0x3d83d747, 
    0xbd19c4ce, 0x3e623f51, 0xbd0de500, 0xbdcd2797, 0x3e32d558, 0x3cca7be4, 0x3d87c083, 0xbdd56869, 
    0x3d0c3556, 0x3e4dda09, 0x3abd64c9, 0x3da1b78b, 0xbd0449f0, 0xbda4a47c, 0x3d9a8ff9, 0xbd89a4a8, 
    0x3c83f3ca, 0x3f1e910a, 0x3c207d0c, 0x3dca280a, 0x3e464da2, 0xbd3d820d, 0xbe8a5ed4, 0x3d9f0a03, 
    0xbdc2eed1, 0x3dc82e4d, 0x3d4801c5, 0xbe43ce4a, 0xbd8cba28, 0xbd47dc48, 0xbe7e4bee, 0xbea4f646, 
    0xbb3de074, 0x3c891e74, 0x3d6a8bb2, 0xbe3ae174, 0x3d16df3b, 0x3c45e725, 0x3e7e21cd, 0x3e060260, 
    0xbcdeef49, 0xbb0787d2, 0x3db9b2cb, 0x3bdf8249, 0x3e57594c, 0x3d35df22, 0xbdac4395, 0x3cc4589c, 
    0xbc076bbb, 0xbc1fdafd, 0xbb3b5bd0, 0xbd456bc1, 0x3e14c4f5, 0x3d324373, 0xbb23e304, 0xbe341bdd, 
    0xbddc28a1, 0x3e4d7d3d, 0xbcdcdd4b, 0xbdfcf3db, 0xbd60587d, 0xbe2e397d, 0x3e374ae9, 0x3e095275, 
    0xbd03c130, 0x3d79a6d2, 0xbd9b121f, 0x3a84541f, 0x3d800a23, 0x3d9e878b, 0xbd098354, 0x3d0c5bce, 
    0x3ed5ca9e, 0xbdcccd3b, 0xbd4cf682, 0xbd788b32, 0x3e8f78b9, 0x3a10d708, 0xbdae64fd, 0xbe7b6ae1, 
    0x3d5333a7, 0x3e6df26e, 0xbcf75010, 0xbd9d74dd, 0xbdc32a42, 0xbd36c62c, 0x3d917508, 0xbd9dae84, 
    0xbdbe0206, 0x3ef7f69c, 0x3ddda923, 0x3dc92dd3, 0x3d783334, 0x3d8dc2e3, 0xbd85d63a, 0x3dc4da71, 
    0x3ec2756f, 0xbdefec3f, 0xbe0003d4, 0xbe66bf40, 0xbd210b4e, 0x3c06437f, 0xbce9c7a3, 0xbe4b4330, 
    0x3d986f80, 0x3e1dd184, 0x3cd933a0, 0xbde4bcbf, 0xbc1aaa1f, 0x3d221dee, 0xbbe2b246, 0x3d1bc0c8, 
    0xbce44223, 0x3e453f1b, 0xbe0113d9, 0x3d9feda7, 0x3cdab7ef, 0xbd1b1cdc, 0xbe53f073, 0xbce07e9c, 
    0x3e9f05a6, 0xbe4f49ff, 0x3d871168, 0x3a26a32e, 0x3e4362fb, 0x3d001304, 0xbd4429c8, 0xbd8f28ad, 
    0x3d765972, 0x3e3f755a, 0x3bf9d8c9, 0xbe41400e, 0xbd2f858d, 0x3bd1c3c1, 0xbd7293af, 0xbd92237c, 
    0xbcb7c1cc, 0x3e92ca2c, 0xbd19c860, 0x3cc7fd9b, 0xbe3c7c39, 0x3d5458d8, 0x3c5ba591, 0x3e1c6466, 
    0x3d6288fd, 0xbe85245e, 0x3db240bb, 0x3ddc88c2, 0x3e7fb223, 0xbdcbef8e, 0x3ea48f39, 0xbd804a73, 
    0x3cfbeb6c, 0xbe7687b0, 0xbdaf3275, 0x3d9d2866, 0x3d71f6a2, 0x3d020374, 0x3e587b08, 0x3daf2394, 
    0x3dab3a9d, 0x3e420dbb, 0x3dce210f, 0xbdcc41b3, 0x3f197346, 0xbc4f8a45, 0xbdd27ae0, 0xbd264a35, 
    0x3d79e142, 0xbe90d07c, 0x3d6829d3, 0xbc108923, 0x3e3ef841, 0x3d9f73ae, 0x3e21ba4c, 0xbe3eb13d, 
    0xbd96fc50, 0xbe11cd77, 0x3dbe56aa, 0xbce841fb, 0x3cabd88e, 0x3e0b1e70, 0xbd71ed93, 0xbcd184dc, 
    0xbc841f24, 0x3d1ac166, 0xbd7a950b, 0xbe47d335, 0x3f144f74, 0xbddb3056, 0xbe520849, 0x3c119e95, 
    0x3c564c86, 0xbe9f3257, 0xbd544d93, 0xbcc9c8df, 0x3e1b25b6, 0x3c2a42db, 0x3e1c5710, 0xbe19154c, 
    0x3d77e28d, 0xbd010b57, 0xbc445805, 0xbe431b62, 0x3ca58b8e, 0xbdbc7866, 0x3dbea16f, 0x3bb453b2, 
    0x3dfa7634, 0x3e83080d, 0xbe0926ad, 0xbe1f1c1b, 0x3e06c3fa, 0xbdebfc9f, 0xbba0dfdd, 0xbccfdd75, 
    0xbdf20528, 0x3ce913aa, 0xbcc683c5, 0xbd3eba95, 0xbd1c1ab0, 0x3dbf7f09, 0xbd9243e4, 0xbd86faff, 
    0x3e290be9, 0x3d495dbf, 0xbcd99e93, 0x3cfc951d, 0xbd87ddc2, 0xbe10ea2a, 0x3ce5f8ee, 0x3dc21351, 
    0xbd843803, 0xbddabf60, 0xbc817d41, 0xbe28ea4b, 0xbdb03198, 0xbd0c6985, 0x3b753201, 0xbaff877e, 
    0xbe5855b7, 0x3e135c09, 0x3cd1e566, 0xbd1f45d2, 0x3ce18ac1, 0x3b3c1baa, 0x3dacf88b, 0x3b1a20c6, 
    0xbd58bb7e, 0xbe2214e6, 0xbdd5718d, 0xbdc128be, 0x3e0d8ec9, 0x3d5cd5d3, 0xbdc79ce2, 0x3da08fd5, 
    0xbdab15a1, 0xbe82f3a8, 0x3e9b81b5, 0x3e215f33, 0xbe19ac73, 0xbdafcb52, 0x3ea8e922, 0x3db47868, 
    0xbe5cdb10, 0xbe0b4637, 0xbe09f6ff, 0xbda050a7, 0x3cfa66ca, 0x3d212821, 0xbd91bc10, 0x3eb65acf, 
    0x3cf9eed2, 0x3df744f6, 0x3d93bbbf, 0xbbec1e52, 0xbcc25ac6, 0x3cb7a28d, 0xbe007b7b, 0x3d3049da, 
    0x3e1ddf12, 0xbdca523b, 0x3d732b62, 0xbe2d321f, 0xbea8f10e, 0x3c913875, 0x3c669e92, 0xbdf7a265, 
    0xbe6097c0, 0x3e49f94f, 0x3d53ad64, 0xbd0890f4, 0x3d09cfef, 0x3d4ae02c, 0xbdbc0e45, 0x3dfa3b26, 
    0x3d7222d4, 0x3d155438, 0x3d9bdf53, 0x3da00354, 0xbcdefde9, 0xbe0184d7, 0x3dabce10, 0x3bb420b1, 
    0xbe46bd24, 0xbd8f8129, 0x3c8911d0, 0xbdbd818c, 0xbdad63e8, 0xbd9ecde2, 0xbdb6f25a, 0xbe0c10e7, 
    0x3b714850, 0x3e4e9341, 0xbdd9d461, 0xbd884dbd, 0xbb8f127d, 0x3d458ac7, 0xbbb7c578, 0x3d932f65, 
    0x3d865d4b, 0xbd624134, 0xbe087a3e, 0xbc265cfd, 0xbd8f41c8, 0x3e4fd109, 0xbc926d65, 0xbd1529d5, 
    0xbe0ad225, 0xbe796f18, 0x3e42a44e, 0x3d800bdd, 0xbe08d8a4, 0xbdb5543f, 0x3e68b592, 0xbd8f2c13, 
    0x3c568220, 0x3cde92f8, 0xbe180eb6, 0xbdf93275, 0xbe633576, 0x3e2dffa3, 0xbdb8ac60, 0x3ee2363a, 
    0xbda01d59, 0x3c7aca0e, 0x3e0f3931, 0xbdc1ae21, 0x3d6b8094, 0x3dc568fc, 0xbddc816b, 0x3c96ac52, 
    0x3d98a70a, 0x3db51fe1, 0x3dce4948, 0xbe1d0843, 0xbe7858e5, 0xbd113a0f, 0xbc91324c, 0xbdae55d3, 
    0xbe0ffee4, 0x3a8da5c4, 0xbdc884bd, 0x3d28b5b7, 0xbd7f4a35, 0x3d82e0d0, 0xbe12bf84, 0x3e300f57, 
    0x3e2c9184, 0xbd20dfa3, 0xbd336535, 0xbb4e760b, 0xbe233dc7, 0xbce8e0a6, 0x3c5e9400, 0xbc6875a7, 
    0xbe34b873, 0xbdce76da, 0xbdbbd388, 0xbd2b41e2, 0xbdae641f, 0xbdd36fa8, 0x3db6d5ff, 0xbdc2c595, 
    0xbbbad0a9, 0x3e863770, 0x3d7b7e5d, 0xbdc07df9, 0xbd091e78, 0x3da8a4df, 0x3b63fb56, 0x3d91ebaa, 
    0x3d429f91, 0xbe5af23b, 0xbe1f26bf, 0x3d8ddfdd, 0x3dbef92d, 0x3db171f6, 0xbd35342c, 0xbc11e67e, 
    0xbe96e9a9, 0xbeb5aaa7, 0x3dca8504, 0x3d01bd88, 0xbdd3ce2c, 0xbe057189, 0x3ea87375, 0x3d287630, 
    0xbdf938ab, 0x3c618e40, 0xbdc6bba2, 0x3c0a9b23, 0xbddb6fa7, 0x3e27f9de, 0x3c8a3e44, 0x3ebb45f4, 
    0x3be12fdd, 0x3cfac03e, 0x3db4f2b0, 0xbd8810ad, 0x3c178eb1, 0x3dda36b4, 0xbd440d57, 0x3daf86d6, 
    0x3cdd7216, 0x3d1ca930, 0x3dda757b, 0xbe3ced60, 0xbe7b8321, 0xbc5010fd, 0x3a556572, 0xbe25a87e, 
    0xbe2e9e4d, 0x3d218df6, 0x3ca2ff77, 0x3d8659cf, 0x3d4d6793, 0x3db2584c, 0xbcf1d694, 0x3cfcd1c4, 
    0x3b9f2f62, 0xbe03a954, 0xbbde879c, 0x3d59be4a, 0xbca7cc22, 0xbd8a6333, 0xbc1dd170, 0x3bc5c953, 
    0xbdfd74c6, 0xbe3c25ae, 0xbd7484a0, 0xbe152f45, 0xbe134d40, 0xbddf17e7, 0x3e12731d, 0xbda7bd76, 
    0xbe6b31c6, 0x3e84f929, 0xbd816bb4, 0xbddba7d7, 0x3cd97eb5, 0x3c1d9cb3, 0x3dc16b7e, 0x3d40a62c, 
    0x3e0ad093, 0xbdbf21cf, 0xbdc30f22, 0x3d8b50af, 0xbd4e8fbf, 0xbdda1b54, 0xbe05c491, 0xbd7a167f, 
    0x3d8f4b22, 0xbeb82d33, 0x3e86ce86, 0x3d05991e, 0xbdf93b5c, 0xbc16d949, 0x3ee9d5f3, 0xbc85c3d2, 
    0xbc27ef40, 0xbd94b612, 0xbd70d963, 0xbd457a32, 0xbdc1eb7d, 0x3d82d88e, 0x3db86085, 0xbd4b2c7f, 
    0x3da2e517, 0x3cc8c99f, 0xbdc07ced, 0xbe404898, 0x3dfd088b, 0xbe18d07d, 0xbe3de6cd, 0x3d31c3df, 
    0x3e17c661, 0xbe351717, 0xbd44af27, 0x3d3cc8b9, 0xbea223f1, 0x3ce6d8ea, 0x3d9ac8bd, 0xbd840e46, 
    0xbe182fc8, 0x3db076c8, 0x3b881d4a, 0xbdb53fef, 0xbd9eb5f4, 0xb8e66a8a, 0xbd690100, 0x3d4f0df8, 
    0xbd9a84d4, 0xbda99c96, 0x3d87a6a1, 0x3dbb640f, 0xbb287621, 0x3ce2ed3f, 0x3d4bf218, 0x3da10e16, 
    0xbe07cd56, 0xbda02701, 0x3d18facc, 0xbd892c83, 0xbd4c8664, 0xbe397c34, 0xbd681d9f, 0x3d6510db, 
    0x3dc4aac8, 0x3e89e913, 0xbdb62222, 0xbddef723, 0x3c592fa6, 0x3d95fe12, 0x3d91f7b5, 0x3d722c00, 
    0x3d15b34d, 0xbd9b5f65, 0xbc02c281, 0xbb866b2a, 0xbd31c987, 0x3e0d19ca, 0xbd90558e, 0x3ded194b, 
    0xbe191243, 0xbe02ef10, 0x3e8285c9, 0x3d09e568, 0xbdd732ff, 0x3cbe71ac, 0x3ebf6293, 0x3dcad90c, 
    0x3dc9d371, 0x3e2ae0d2, 0xbd46bb8b, 0xbdda3734, 0xbd4178ab, 0xbcaa9913, 0xbd8d32eb, 0x3bd3477e, 
    0xbdaca6e5, 0xb99fca46, 0xbd60ad88, 0xbd97ebac, 0x3ced6a7c, 0xbe8101f1, 0xbe19ffd1, 0x3e03c1ee, 
    0x3d8dfa94, 0xbc3797da, 0x3d2bf851, 0x3d876a9c, 0xbe800a07, 0xbc4ca51f, 0xbe48ac16, 0xbddf4230, 
    0x3cb71127, 0x3dbf6d46, 0xbca42616, 0x3d1c1b5e, 0x3db56d59, 0xbd3fc99d, 0xbe3a83cf, 0x3ca8a4e5, 
    0xbbd28661, 0xbd7a2cc2, 0xbde46d52, 0x3dfe0122, 0x3d2dc0c2, 0x3bcaa3fc, 0xbd370c34, 0x3c1451d4, 
    0xbec79354, 0xbe17901f, 0xba9f7eb7, 0xbe1d6167, 0xbc8aa228, 0xbe87381d, 0xbc47a444, 0xbd56e566, 
    0xbdb9862d, 0x3ea882c6, 0xbd78f930, 0xbe6dc938, 0xbe067e2f, 0xbdd9fa92, 0xbd113bca, 0x3dbddf58, 
    0x3dabbc00, 0xbe25296f, 0xbd8d47fe, 0x3dc43afd, 0x3d514a26, 0x3cc403d7, 0xbe0554a3, 0x3b374bda, 
    0xbe7148b7, 0xbe5728dd, 0x3e497cf8, 0xbc99af7b, 0xbe08ce9d, 0xbda9c66a, 0x3f04ac7a, 0xbd5f573e, 
    0xbd83828c, 0xbdcb3cbb, 0xbd559bb1, 0xbaa306a0, 0xbe478ce7, 0xbd0597ca, 0xbe0b268f, 0xbd25392f, 
    0x3de62779, 0x3d6635d3, 0xbdc0c636, 0xbdd230bf, 0xbbcae12d, 0xbdf514d5, 0xbd96e638, 0xbd2a9e6f, 
    0xbc578380, 0xbe576a54, 0x3c31c840, 0xbd9412e0, 0xbe4f076c, 0xbc88abc2, 0xbe348454, 0xbb1bcb67, 
    0xbdfb2262, 0xbdce16bf, 0xbd8dc144, 0xbd0d597d, 0x3dd8b98d, 0x3d273f70, 0xbe1aea76, 0x3da18eb9, 
    0x3d814968, 0xbd0f774c, 0xbc9975e7, 0x3dc5b8b7, 0xbe122523, 0xbd47b8b9, 0x3bdd20b6, 0x3de6df19, 
    0xbde54722, 0xbe75399e, 0x3c7e5fdf, 0xbe6c41b5, 0xbe164c7b, 0xbdeb146e, 0x3e19d38c, 0xbdaa19f4, 
    0xbde46aae, 0x3e82da8f, 0xbd164fee, 0xbd35183e, 0x3c1c903c, 0x3d551407, 0x3e073954, 0x3d46e585, 
    0x3d949d7b, 0xbe4def27, 0xbe1f8887, 0x3da053b0, 0x3df22bc8, 0xbdd5d1a1, 0xbdb5d140, 0x3e4007e9, 
    0xbd984ae0, 0xbe8fcd39, 0x3e8ec3a5, 0x3d76a1bb, 0xbe12628c, 0x3d179929, 0x3ec5659e, 0x3dcd28e5, 
    0xbcf635aa, 0xbde4ea45, 0xbd7ae37a, 0xbd866cb8, 0xbd8aadfa, 0xbc29470a, 0x3c8b0895, 0x3d65b69c, 
    0x3df99e6c, 0x3dc470dc, 0xbdeb46b2, 0xbe791ca5, 0x3d8562b1, 0xbe2f9df4, 0xbe8a8ba6, 0x3da1975a, 
    0x3e6dda9f, 0xbe28c34d, 0xbad31624, 0xbe03140d, 0xbe292df6, 0x3d040f9d, 0x3c56f876, 0x3dcdf5c7, 
    0xbe804b5e, 0x3d86ed80, 0xbdfba9ca, 0xbdd79584, 0xbbe1f2e3, 0x3db36daf, 0xbda49ffb, 0x3db22fbc, 
    0xbd47847d, 0xbd9fd192, 0x3dba2f9a, 0x3df3c38d, 0xbdc648ee, 0x3cc8fe25, 0x3d7c9d75, 0xbda49b4c, 
    0xbe9333c7, 0xbe2dc6b8, 0xbd19e6ba, 0xbdf83679, 0xbcf3bc23, 0xbe600fa4, 0xbd2aed66, 0xbdaac00c, 
    0xbd0eb0aa, 0x3e7c6ded, 0x3d82bb0d, 0xbe91620c, 0xbc872e7d, 0xbdef473c, 0x3dec1abe, 0x3db8fb2f, 
    0x3e239e47, 0xbda734d4, 0x3d952291, 0xbd266da4, 0x3d92610a, 0x3e3149c9, 0xbd7f3296, 0x3dc734b7, 
    0xbe2b3129, 0xbe41b63d, 0x3e937ac0, 0xbd0f8722, 0xbe4c02e6, 0xbd82d5bd, 0x3ecf249f, 0x3e30ae3d, 
    0x3e0b3e6d, 0x3d4e2bb1, 0xbe2a23bb, 0xbe02119c, 0xbda03f72, 0x3d97e86c, 0x395f89cc, 0xbd99e31d, 
    0x3e031c56, 0x3db2420f, 0xbd2fd8d6, 0xbcf5d09a, 0xbcddad7d, 0xbe8602dd, 0xbdf28251, 0x3db1929f, 
    0x3d7eac47, 0xbd8ae975, 0xbd7233c3, 0x3de20c22, 0xbe366c55, 0x3d1af20b, 0xbe69c75d, 0xbd38f040, 
    0x3cc4606d, 0x3cf224d8, 0x3d317df1, 0xbd7fd0d4, 0x3d696fbd, 0xbb4a9192, 0xbe3dfa47, 0x3d372b36, 
    0xbd1de1d9, 0xbd642daa, 0xbd8ae7a6, 0x3c5ae7c4, 0xbdb1d35e, 0xbca1a0dd, 0xbd81dfbd, 0xbc7a1c8c, 
    0xbea3fd88, 0xbde6dc5c, 0xbced8dc2, 0xbe264e2b, 0xbcbf1588, 0xbe5da709, 0x3e2bdd19, 0x3d50a4e5, 
    0xbd3435be, 0x3ec760ea, 0xbe3012ab, 0xbdc81c43, 0xbdc72941, 0xbcc588b8, 0xbde0abc7, 0x3d96803b, 
    0x3d9fd519, 0xbe370d2e, 0xbbc56ce9, 0x3e03f7fa, 0x3cc2a8e9, 0x3d25f789, 0xbdcc4556, 0x3de9c775, 
    0xbea0342a, 0xbe59aa8b, 0x3e0e779c, 0x3cf61dc3, 0xbd86263c, 0xbda0ecb0, 0x3ea8717d, 0x3de3b36a, 
    0xbe108313, 0xbc4005fa, 0xbd2ba700, 0xbd6add34, 0xbe5debec, 0x3d097172, 0xbc95c4a9, 0xbdb61865, 
    0x3db2ea6b, 0xbc5fc19e, 0xbdde6629, 0xbe18cf9a, 0x3cb9c960, 0xbe424faf, 0xbdc1b96b, 0xbd8ef1de, 
    0x3d9e2e32, 0xbd24e9f1, 0x3d3254ca, 0xbe0d3674, 0xbe242686, 0xbd9c5185, 0x3e4a144d, 0x3dbab7e9, 
    0x3e5989ec, 0x3e3cd9c8, 0x3d47571e, 0x3d021eb7, 0x3e3417d8, 0xbd0eca31, 0xbd2f8089, 0xbdeaaa5d, 
    0xbe0aaed0, 0x3e30884d, 0x3ab96d7d, 0xbcbbf595, 0xbd2938ac, 0xbe95fa89, 0x3dc983fa, 0x3c1fc37d, 
    0x3b47282d, 0x3f3e40db, 0xbd37f128, 0xbd7f47a1, 0x3d7d157b, 0x3d1e31c7, 0xbe1ebc4b, 0xbdc87c7d, 
    0x3e254350, 0x3e146b5b, 0x3e0d93c3, 0xbe14cad5, 0xbcd38e1a, 0xbc1efcaa, 0xbd064d03, 0xbed86b73, 
    0xbd6bba47, 0x3db9d2ea, 0x3df6b6c2, 0xbd565ade, 0xbe2621b5, 0xbcff3a41, 0xbbe462d3, 0x373f3a3c, 
    0xbdf11bb6, 0xbc9f48d4, 0xbdc2bdf4, 0x3d9b6009, 0xbde8f4c8, 0xbe08c914, 0xbe795def, 0x3e2420f8, 
    0xbda0fbd0, 0xbd180ce3, 0xbc17b2ed, 0xbe248846, 0x3e7e06e9, 0xbcf74873, 0x3db56109, 0xbde94bf4, 
    0xbbd6aa44, 0x3e292cca, 0x3e2081c3, 0xbd9e0037, 0x3daaf6b7, 0xbe2c91dd, 0x3d721249, 0xbd615f8e, 
    0xbd760aee, 0x3dbdfd11, 0xbd5788e7, 0x3dac9ae7, 0xbe08ba56, 0x3da37cd2, 0x3d5def58, 0xbe18db98, 
    0x3e5ad380, 0xbe42c389, 0xbdbed838, 0xbe23521e, 0x3e45b7f3, 0xbcf56d26, 0x3da66f59, 0xbe411e54, 
    0x3d8e37f2, 0x3e04c0e5, 0xbb932e94, 0x3d2a6e6d, 0xbddb9930, 0xbe7ad184, 0x3e7c01a0, 0x3de68100, 
    0xbdb5fec3, 0x3f2ece8e, 0xbd5a60fa, 0x3c2ae599, 0x3ea37175, 0x3dd4c9fc, 0xbdfab1ab, 0xbd38028f, 
    0x3eba2270, 0xbe5f9098, 0x3cb4a11f, 0xbe116dea, 0xbe00dcae, 0x3e26ffeb, 0xbde5b0d4, 0xbea3eb38, 
    0xbdc8b2c5, 0x3d8b0874, 0x3c28ecf1, 0xbd2491a3, 0xbccd8e52, 0x3dd22160, 0xbd6e8582, 0x3de4ae5a, 
    0xbd9b55dd, 0x3ce5b9bc, 0xbdc40bd8, 0x3bb4541b, 0x3ea0449a, 0xbe1881b2, 0xbe6ecf14, 0x3d786c44, 
    0x3ea24809, 0xbdbd60bc, 0x3d0e20dc, 0x3d09fb31, 0x3dd093bf, 0x3d977899, 0xbcfc72c5, 0xbe5cd288, 
    0xbda47495, 0x3e395901, 0xbd365307, 0xbe8a8b23, 0xbdafcf9f, 0xbdd99160, 0x3e93bba9, 0x3d64aa3f, 
    0xbbb8d300, 0x3e3ff823, 0xbdf015e6, 0xbdaeac08, 0xbb2e0cf4, 0x3c67ffbc, 0x3cffb88a, 0xbcf43f80, 
    0xbd28d064, 0xbe61469d, 0xbd7a9e15, 0x3e52448e, 0x3ea90b67, 0xbdb30393, 0x3eb38458, 0xbde27586, 
    0xbd7d9bec, 0xbde69685, 0xbcb5048b, 0x3d606355, 0xbe13b862, 0x3e45a8df, 0x3ec43ad0, 0xbd481ff6, 
    0x3d7d8b74, 0x3ea89e7e, 0x3cd4f9a7, 0x3cee64c0, 0x3f154803, 0xbe0d01f0, 0xbba547ae, 0xbdcd87c3, 
    0x3cbf028c, 0xbed88512, 0xbb565f86, 0x3d2fd6c1, 0xbd1edc04, 0x3df53a9d, 0x3de56f7e, 0xbe35fb2e, 
    0x3d52466f, 0xbd7c726c, 0x3c8d9f29, 0xbdb292b4, 0xbd454da4, 0x3e46760f, 0x3e080855, 0xbce40f3a, 
    0x3de79a9b, 0x3e12047c, 0xbdabc885, 0xbdf2452d, 0x3f262511, 0x3d53df23, 0xbe93ac8f, 0xbda69ff8, 
    0x3e128d83, 0xbec612ec, 0x3d5b2028, 0x3e1700e1, 0x3ec191e2, 0x3cd4997f, 0x3e0c2aec, 0xbe08b6bc, 
    0x3daeb03b, 0xbd18bdc3, 0xbe047b9d, 0xbe51f86a, 0xbda7f8f0, 0x3c81d789, 0x3dae5e0b, 0xbd8ffb78, 
    0x3d9e6951, 0x3e93c30e, 0xbe154984, 0xbe21562f, 0x3ddba7b8, 0xbd933372, 0x3e3d49c8, 0x3d08f78b, 
    0xbe20868d, 0x3d0a56f1, 0x3d28a494, 0x3da5585c, 0xbdf7a888, 0x3d8e98e6, 0xbd896a4f, 0x3ebc4f89, 
    0xbd95912f, 0x3e6d5bc0, 0x3dadd354, 0xbe30d6da, 0xbddae3dd, 0x3e0bb4f3, 0x3cfe2888, 0x3e231f44, 
    0x3e48d989, 0xbe61335c, 0xbd0e2c4a, 0xbdc342af, 0xbd9559b9, 0xbdac5d11, 0xbdc95980, 0xbe861a96, 
    0xbb9fea92, 0xbd3a944b, 0x3e1f45fb, 0x3e179df1, 0xbc5a87c4, 0xbd72c3a3, 0x3dc93da5, 0xb9eb80f4, 
    0xbdc62a7e, 0x3e6e47f6, 0x3e36c780, 0xbe39f6da, 0xbd452807, 0x3d138aa0, 0x3c1fad8a, 0xbc869e00, 
    0x3ddf421a, 0xbe43fdfe, 0xbdf59a82, 0x3d78d265, 0x3cacd7a0, 0xbccd891a, 0x3d016210, 0xbe231f52, 
    0xbe291402, 0x3ee557e4, 0xbc25469b, 0x3eb7b4f4, 0xbcd133a7, 0xbd5d339d, 0x3da81982, 0x3f0f6f10, 
    0xbd057a4b, 0x3cd8ecf5, 0x3d49f097, 0x3de839d2, 0x3db75e2b, 0x3e0c1bd8, 0x3e52a273, 0x3e77d9e1, 
    0x3cdf0b04, 0xbd2dfc33, 0x3e697318, 0x3e2079eb, 0xbdd2464d, 0x3e143e65, 0x3efa31e7, 0x3c2a62c5, 
    0xbe991da1, 0x3d5c9b19, 0x3cde78cb, 0x3e75f0b5, 0xbd533d26, 0x3d09da16, 0x3c1cba27, 0x3e6de025, 
    0xbcb61981, 0x3e0a1e91, 0x3d7200d7, 0xbe8431b4, 0x3d86b600, 0x3d0ded1e, 0xbbc6d2a9, 0xbd19a569, 
    0x3c8703d6, 0xbec6cecc, 0xbdd94e20, 0x3d9cfb71, 0xbbca7644, 0x3dcc6b95, 0x39a2a114, 0xbdab8d1c, 
    0xbc9a531e, 0xbbd5df3a, 0x3e920415, 0x3e06189f, 0xbccb7b42, 0xbcf9341c, 0x3b5c92de, 0xbd2351aa, 
    0xbd8d2a57, 0x3e0e19fb, 0x3e207fc9, 0xbe195e75, 0x3db01c9b, 0x3d818acc, 0x3d12d52e, 0xbc23ed7a, 
    0x3e04a658, 0xbecde663, 0x3dcb97ca, 0xbe29468c, 0x3c7585a8, 0xbcf230e7, 0xbd3f4175, 0x3d5d5492, 
    0xbe3d9648, 0x3ebeac66, 0x3e6fbad1, 0x3e7178c7, 0xbc73f6d4, 0xbdea8301, 0x3cdd7828, 0x3ea39c27, 
    0xbd83ea5d, 0x3e32d6e6, 0x3e19ec9b, 0x3e4da89b, 0x3a93bdf8, 0x3de34a99, 0x3e0c7b6b, 0x3dc111db, 
    0xbdf4d48f, 0xbd2a5473, 0x3e6c8130, 0x3dd08008, 0x3dc763a7, 0x3e806a18, 0x3ebc6baa, 0x3d947734, 
    0xbeb5f3b0, 0x3d3d9c6a, 0xbd92f495, 0x3e452df5, 0xbe02966a, 0x3d2e81c2, 0x3df0903e, 0x3ea6fe34, 
    0xbd3ad99a, 0xbdcde762, 0x3d2cfe06, 0xbe24f234, 0x3d5a554b, 0xbd419342, 0x3c5dcc24, 0x3b7389b4, 
    0x3e62062c, 0xbed3956c, 0x3d9c29bc, 0xbe0679dd, 0x3d3abf89, 0x3e198916, 0x3cf9bdef, 0x3d197cc0, 
    0xbe7374d4, 0xbd4b1479, 0x3d4a807b, 0x3e469ae6, 0xbcd50b2f, 0x3e005865, 0x3e72f85d, 0xbe100fe7, 
    0x3cdae1be, 0x3df7531a, 0x3e411360, 0xbe541df3, 0x3d8909b5, 0xbd4d57d5, 0xbd793a2f, 0x3d022cc8, 
    0x3d8f5170, 0xbecd18b9, 0x3d51ec7f, 0xbe1d47c3, 0x3d60089b, 0x3dc47ff4, 0x3e3cc5c8, 0xbd434268, 
    0xbe1f90b1, 0x3ef7f6a8, 0x3cbf8a76, 0x3ebf39cb, 0xbd66de30, 0x3d9a3617, 0x3e52d411, 0x3f10af42, 
    0xbd690408, 0x3dd4d3f3, 0xbd26ca61, 0x3daacbb0, 0x3d8bb4d4, 0x3e33b521, 0x3d650186, 0x3d18441a, 
    0x3d3fd4ab, 0x3c5ca000, 0x3db54df2, 0x3e936a09, 0x3d5fe5f6, 0x3dc03470, 0x3ebacdc2, 0xbd07d518, 
    0xbd8f5358, 0x3e68d115, 0x3e04036c, 0x3de08b71, 0x3e6ce40b, 0x3da0cd02, 0xbd9ba996, 0xbc2aabce, 
    0xbd0fd7c2, 0x3d336021, 0x3dba1a3d, 0xbe2e7006, 0x3ca9c865, 0x3c4c7d16, 0x3e838a32, 0x3e85c0a1, 
    0x3c8e40be, 0x3ed5f2d5, 0xbd27c086, 0xbca81e63, 0x3e8686a4, 0xbd303244, 0xbe9ce9db, 0x3d29e498, 
    0x3e57741b, 0xbd815b27, 0xbc9c8405, 0xbd873b61, 0xbd91c3b4, 0x3d0d4e37, 0xbe47a4c1, 0xbdbd9707, 
    0xbcb85a33, 0xbcc33813, 0x3dd9b0fd, 0xbdb22aa1, 0x3d6269eb, 0x3dc0945d, 0x3ddd3d86, 0x3e15b618, 
    0xbe2b42b9, 0x3be3be91, 0xbd81b77c, 0x3af7b590, 0x3eaee021, 0xbd1ff857, 0xbdeefbd7, 0xbe1820d0, 
    0xbd3176b3, 0xbcd42c4f, 0xbcbfc013, 0xbe5a40ac, 0x3db0e624, 0xbe0a97a2, 0xbd0d5e0c, 0xbdae82a8, 
    0xbd40ea9a, 0x3d84719e, 0x3e087906, 0xbe8046c1, 0xbc68d5c4, 0xbda0db64, 0x3def0a8d, 0x3d07eacf, 
    0x3c428de8, 0x3debd9b1, 0xbdeb86a4, 0xbda81cb0, 0x3db3c599, 0xbd4bdb9d, 0xbb9f34a6, 0xbe34472b, 
    0x3ea18d71, 0xbe6dba58, 0xbe09e617, 0xbdbbfce2, 0x3e37e6b1, 0xbd33b74e, 0xbe16a83a, 0xbe4dfd54, 
    0x3da00f6e, 0x3dc6f0c9, 0xbdd7a753, 0x3e45bb4d, 0x3d41e304, 0xbd58c5a9, 0x3dc02160, 0xbc34b8bc, 
    0x3c6f4233, 0x3f720d10, 0x3d14aee7, 0x3e799cf2, 0xbc879272, 0x3daae722, 0x3b3cb0ea, 0x3e40f4e1, 
    0x3ea008f4, 0xbe7c4693, 0x3da0d941, 0xbc5c0234, 0xbd8f6715, 0x3e0fc934, 0xbc6364ae, 0xbe8d4f9e, 
    0x3d44738a, 0xbd50a960, 0x3cccd362, 0xbddc1d0d, 0xbcadca3d, 0x3d6a37ed, 0xbdb6a9e8, 0xbdc41534, 
    0xbd13c7cd, 0x3dd5f072, 0xbdd1e9ed, 0xbdbfd5c3, 0x3b984306, 0x3db786fb, 0xbd7d111e, 0x3dc15813, 
    0x3e96d7fd, 0xbe5ecace, 0xbdb3ff49, 0xbde577e8, 0x3dad88ca, 0x3cc70924, 0xbd8f5d91, 0xbc7a546e, 
    0x3d8fcbc1, 0x3e83cc2e, 0xbda13a9a, 0xbe41774a, 0x3d0f4c43, 0xbd67b9b8, 0xbc72d24e, 0xbd563eb8, 
    0xbdc18f40, 0x3e5221ef, 0x3dccaa88, 0xbb527371, 0xbd871d17, 0x3e109727, 0x3e08f804, 0x3dbd1768, 
    0x3daa859d, 0xbe5b42bf, 0xbc98f1c9, 0x3d735929, 0x3ea8d624, 0x3dc2c098, 0x3ed94058, 0xbda1f314, 
    0x3db84e86, 0xbcc4bcb3, 0xbb5900ce, 0x3d9e2e03, 0x3cb33966, 0x3d7bb8e1, 0x3e67bd1a, 0xbd08114d, 
    0x3d08b4b6, 0x3ec6363c, 0xbdafdd35, 0xbdc349e9, 0x3f1d05ab, 0xbe29d886, 0xbdca743f, 0x3cd407cc, 
    0x3e39759b, 0xbe8db19c, 0x3d141b1b, 0xbd0eb849, 0xbd4f583f, 0xbd487d03, 0x3dc67da6, 0xbe322511, 
    0x3cbfbf11, 0xbc8720fb, 0xbd5dcdd4, 0xbe1ec819, 0xbd9bba4c, 0x3d9be05e, 0x3e649d4c, 0xbca7c032, 
    0x3da8ccbe, 0x3d8c2223, 0xbde9e03e, 0xbdc67660, 0x3f17dea9, 0xbde9235a, 0xbe91a54d, 0x3d146a2b, 
    0x3df13afb, 0xbebc72e8, 0x3cc32a98, 0xbe1cc15e, 0x3d785dbb, 0xbd2e4edc, 0x3e0de634, 0xbe06377f, 
    0xbcdfb83c, 0x3ddf1419, 0xbd13b8f0, 0xbe4a44ca, 0xbcc004a7, 0xbd986284, 0x3e5d9dbc, 0x3e01d498, 
    0x3dfa71ea, 0x3e276c92, 0xbde75ab6, 0xbe19ec92, 0x3ccc8a56, 0xbe444acb, 0xbdf3a33f, 0xbd76327a, 
    0x3e152944, 0x3e7bbce2, 0x3e45865e, 0xbdd1b271, 0x3e2871f8, 0x3ca3900a, 0xbd372743, 0xbddf570d, 
    0x3ca3fc02, 0x3e105ac9, 0x3d948d29, 0xbd6cca28, 0x3deca7d5, 0xbe93fea7, 0x3e9b24b6, 0xbac4946b, 
    0xbc9926b0, 0x3ea24037, 0xbbc4c690, 0xbd7b3b4e, 0xbe320586, 0x3d8e16c5, 0xbe69006e, 0xbcb11444, 
    0x3e888035, 0x3cbafb56, 0x3ba86b04, 0xbe1adee7, 0xbd3a1f1c, 0xbc78163e, 0xbca3364c, 0xbe3328cf, 
    0x3d4300e6, 0x3e38b947, 0xbbed7356, 0xbd21ace3, 0xbdfaefe9, 0xbdfff70d, 0x3dac2fe6, 0x3d9a8988, 
    0x3dc0c238, 0x3edf9b28, 0xbab8c7d6, 0x3d45eee1, 0xbdeef32b, 0xbd492140, 0xbe95704c, 0x3ca18795, 
    0xbcf0f53e, 0x3be793aa, 0x3c5336cf, 0xbe3faa65, 0x3e1f3133, 0x3d1d2c07, 0x3ba20cb5, 0xbd327829, 
    0x3d8c90f0, 0x3d890745, 0x3dba3bc1, 0xbdba16b7, 0xbbabf4ae, 0xbe17a660, 0x3e6ebe1f, 0xbc71a204, 
    0xbc6f4b97, 0x3e124081, 0x3d3b8b03, 0xbcead50b, 0xbd8dff4c, 0x3d084ae9, 0x3daf0204, 0x3dbc6c32, 
    0x3ec3a007, 0xbe00d490, 0x3db5ebed, 0xbd83cd2d, 0x3e212d62, 0xbcece74e, 0x3ba4794c, 0xbe6bdbb7, 
    0xbdb386c3, 0x3e36ab1d, 0xbd16e7b1, 0x3d7f0209, 0x3dda1a5f, 0xbe6c0f0c, 0x3c3f2474, 0x3d6f745f, 
    0xbe0d0a4f, 0x3f38bc0a, 0x3dff8567, 0x3d7bdd4f, 0x3db892c0, 0xbe0e67ad, 0xbdc685ec, 0x3dce7cd2, 
    0x3e877c8f, 0xbd9a588b, 0xbd742a78, 0xbd67c976, 0xbbe16787, 0x3dc7ca73, 0xbe057aeb, 0xbe844ef7, 
    0xbd6f87cb, 0x3d92c248, 0x3dfd6490, 0xbd87460e, 0x3c45bcfc, 0x3ca230ee, 0xbcf8b59a, 0x3dac9b45, 
    0x3bc265f2, 0x3dc095ed, 0xbe093ff0, 0x3aa3d1e3, 0x3e190d01, 0xbc99b30b, 0xbe763180, 0x3da57bdd, 
    0x3e828eca, 0xbe7bc591, 0xbc51d651, 0xbc39e0a2, 0x3e8732f4, 0xbd440e51, 0x3dcb9f7b, 0xbe0b059a, 
    0x3de6165f, 0x3e340d6f, 0xbc531d92, 0xbe4cdb17, 0xbda9a87f, 0x3d1d7a8c, 0x3e0dd785, 0xbdc1eb24, 
    0xbd7e98a1, 0x3e4b5c5a, 0xbcb289ef, 0x3d4b4947, 0x3e565a64, 0x3cb65f21, 0x3dc0c7d2, 0x3d82f1d9, 
    0x3ba58bd8, 0xbe19ae87, 0xbc4ff127, 0x3e844e8c, 0x3ed0127c, 0xbdb5823d, 0x3e2324ff, 0xbd7fb12e, 
    0x3d80034f, 0xbcb0c1dd, 0xbd20bc75, 0x3a2db66b, 0xbd40458f, 0x3d2685cf, 0x3e8758e3, 0xbd7b1bd0, 
    0x3e001196, 0x3e9be0e0, 0xbd9ad5f5, 0xbde41a14, 0x3efa1250, 0xbd820ad3, 0xbd9ace92, 0xbce23831, 
    0x3dbf5cc9, 0xbe9b7a16, 0xbd089277, 0xbca5fa76, 0x3b19446a, 0xbd333829, 0x3df32bd5, 0xbe2a9ed7, 
    0x3d05c8d8, 0xbe3435c9, 0xbd044b29, 0xbe81d7b6, 0x3dbd574a, 0x3dc8ea2e, 0x3d134990, 0xbdeebf31, 
    0xbce24a50, 0xbc237f71, 0xbdfd9e30, 0xbcdc5ebe, 0x3ecdd913, 0x3d91c1d9, 0xbd54a0fb, 0x3dcf696d, 
    0xbcbe2a08, 0xbe5cb6d2, 0x3c5758e9, 0xbc5e2635, 0x3e99e243, 0x3dbb5598, 0x3da3d9a4, 0xbdebfc82, 
    0xbda78b66, 0xbc035a5d, 0xbdc26c24, 0xbe28fad7, 0x3d8f1d8d, 0x3e0fe104, 0x3e0e05f6, 0xbd9632d6, 
    0x3dca1195, 0x3e37ecc2, 0xbd999f43, 0xbe082cf8, 0x3ec7c454, 0xbd5e8718, 0x3e2b4201, 0x3e160f33, 
    0x3eb31b88, 0x3dcd7237, 0x3bdc4694, 0xbd461259, 0x3e312d83, 0xbdb897ca, 0xbd38161d, 0xbdefcad0, 
    0x3d26f4d0, 0x3e91cee6, 0xbc85368b, 0x3c12e639, 0xbca3e848, 0xbe5b10c0, 0x3e68960d, 0x3e4fa3fe, 
    0x3d31f131, 0x3f306850, 0xbd82728f, 0x3dc682d6, 0x3e906eb9, 0xbdc37bf8, 0xbe54fc95, 0x3e1cba82, 
    0x3ef166f3, 0x3d415791, 0x3d23f735, 0xbdc7b7fb, 0x3e5bad8e, 0x3d846df4, 0x3dc5bf5d, 0xbe7e3f4d, 
    0x3cf914b1, 0x3e16849a, 0x3d501224, 0xbdb56dd8, 0xbdfff681, 0x3d0830e6, 0xbd6ce131, 0x3e2e0fc9, 
    0xbd40d78e, 0x3dc3f328, 0x3e0b0cce, 0x3d02a4d9, 0x3eb2066f, 0xbdaca58e, 0xbd41cf23, 0xbc17c6da, 
    0x3eb2e72f, 0xbdfd8dc6, 0x3cb845fd, 0xbe2c2a30, 0x3cb2df04, 0xbd70a7f9, 0x3c132001, 0xbe381b3d, 
    0xbcff606f, 0x3e6e5f48, 0x3d4c3e90, 0xbe3480d9, 0x3d5f06c5, 0xbddd4a4e, 0x3e1fed4e, 0x3d891dc8, 
    0x3e2cc1e1, 0x3e7a8295, 0x3dcd5dbe, 0x3e300cc9, 0x3e0e5df7, 0xbccf2112, 0xbc0c5543, 0xbd7bb5b0, 
    0x3e77f34f, 0xb9865606, 0xbd2666c7, 0xbc16761e, 0x3e633b17, 0x3d0a9381, 0x3d8c06ea, 0xbe2db2e8, 
    0xbdc3bd57, 0x3e0a01fc, 0xbd869e3e, 0xbc6bedf0, 0xbda881a0, 0xbd587cce, 0x3df27d00, 0x3d2e8933, 
    0x3d87dcba, 0x3f141799, 0x3d5293ae, 0x3d5aa561, 0x3e8c0154, 0xbd756cdb, 0xbdd343cc, 0x3e1df69b, 
    0x3e0471bf, 0xbe5f9ff8, 0x3c81f381, 0x3dc69bf0, 0xbc80325f, 0xbd38ba34, 0x3e0fe3d1, 0xbe1fe3d8, 
    0xbd8102fe, 0x3d317fe3, 0x3c746b2f, 0xbe3c282d, 0xbdc253fb, 0x3d1bc389, 0x3e1d8ad9, 0xbd945884, 
    0x3c61585c, 0xbd0d0fb0, 0xbdcaccbf, 0xbde3ac98, 0x3edf8762, 0x3cb299ad, 0xbdabcd40, 0x3de65fcb, 
    0x3dcc1ce1, 0xbe880f04, 0xbce62210, 0xbd18db94, 0xbd2b89d2, 0xbd76d34c, 0xbc852aab, 0xbe1ef6cb, 
    0x3d333a07, 0x3e265ba3, 0x3adaea90, 0xbe619570, 0x3deab494, 0xbc8056aa, 0xbcebe795, 0x3d3a3d4a, 
    0xbb821f88, 0x3ddabdf8, 0x3cb19659, 0xbda05640, 0x3c854bd1, 0x3c344b96, 0xbe03a254, 0x3d8efb16, 
    0x3dbfa367, 0xbe4723c0, 0xbddb3e3b, 0x3e888e62, 0x3e8debf3, 0x3dd47059, 0x3e058a2d, 0x3badf024, 
    0x3e043903, 0xbda7d1cd, 0xbd525aaf, 0x3dec9648, 0xbd94f240, 0x3ccca70e, 0x3dab4c93, 0xbe057751, 
    0x3dbee67a, 0x3efae8ca, 0x3e06b262, 0xbdf8e32e, 0x3f04ba14, 0x3dd1380a, 0xbd84a4eb, 0x3d9f3830, 
    0x3e57e649, 0xbe4e13d9, 0xbd6b4272, 0xbdeb8b82, 0xbd2b989b, 0xbd916980, 0xbdd812e4, 0xbdb85e38, 
    0x3daf8b1d, 0xbe6b5870, 0xbddab8bb, 0xbd53f05f, 0x3c85f62d, 0x3e3ef9fe, 0x3bfcbadc, 0xbdbcf96e, 
    0x3d819527, 0xbc5dbfce, 0xbde2674d, 0xbdc140a3, 0x3eec961e, 0xbd9b8db8, 0xbde79dce, 0xbc024e3b, 
    0xbd395d44, 0xbe66795d, 0xbd766374, 0xbd3863c7, 0x3e3ab7d6, 0xbd631065, 0x3d76b38b, 0xbd957e92, 
    0x3de7a7e3, 0xbdca763a, 0xbc58f767, 0xbe144445, 0xbd82b7c2, 0x3dcfa8b6, 0x3d8b16c1, 0x3ddbf07e, 
    0xba2c2259, 0x3d4e8714, 0xbde06a68, 0x3c135ded, 0xbccf1d8f, 0xbde93b17, 0x3d6e25fc, 0x3d9c0b84, 
    0xbdfd3905, 0x3d32f8b1, 0xbd6360be, 0x3d778b65, 0x3ca66ebb, 0x3e08ccd8, 0xbd6d2731, 0xbd05fbf8, 
    0xbe1a69fc, 0xbde3072b, 0xbd1e8a13, 0x3daadf48, 0xbd25f14d, 0xbba44b1f, 0xbda15257, 0x3d41c2df, 
    0xbd6bcd5b, 0xbe526b61, 0xbcd2568e, 0xbe26640d, 0xbe1758f2, 0xbe338644, 0x3e464f6e, 0xbd9adee6, 
    0xbe2c97b9, 0x3e16740b, 0xbd848832, 0x3be5b248, 0xbcb6d140, 0xbdc8c850, 0x3e0d0be6, 0xbcca7b1e, 
    0x3c0d0379, 0xbe8a1236, 0xbad55553, 0x3d640b87, 0xbd7cdf23, 0xbdab4219, 0xbe1f53c6, 0x3c84321d, 
    0xbd29dc03, 0xbe5e811f, 0x3e45608c, 0xbdf1a022, 0xbdc7ffff, 0x3da5b782, 0x3f01d337, 0xbde03646, 
    0x3c117c05, 0xbd9e7e32, 0xbe3db89c, 0xbe4171ef, 0x3b3d8167, 0x3d27336d, 0x3dbf87a7, 0x3e800207, 
    0xb9eb6ad9, 0x3da43a6d, 0xbe133eba, 0xbe2d7105, 0x3d231ecf, 0xbe53f81a, 0xbdbcbbbd, 0x3d50c961, 
    0x3e4b5c5e, 0xbe1b20c1, 0x3d607684, 0x3e112027, 0xbde3d233, 0xbd05bf77, 0xbd515f4c, 0xbe1075d8, 
    0xbe2beba4, 0x3d9db30d, 0xbd4c76fd, 0xbd11f875, 0x3e13da8b, 0xbc2af0b9, 0xbe338b6e, 0x3d8c9b06, 
    0xbca01d63, 0xbd9a42ec, 0x3df379f3, 0x3df2ec24, 0xbe09cc41, 0x3dfbfb42, 0x3c8c9dc2, 0xbd5addd7, 
    0xbddbc411, 0xbdc7259e, 0xbd9b252c, 0xbe3cbe15, 0xbddf736c, 0xbe49c277, 0x3dfb03a1, 0xbda09765, 
    0xbc032881, 0x3ddd26ad, 0xbdc8e899, 0xbe4d52e3, 0x3b9c60b0, 0xbd769aa3, 0x3d906b3c, 0x3db4ddb0, 
    0x3ced2707, 0xbd297221, 0xbdf7546b, 0x3d734e2c, 0xbdcb2dd1, 0x3de558c5, 0xbe06a24d, 0xbc2ea1ad, 
    0x3bb0f0e5, 0xbe4812f6, 0x3d65c3f4, 0x3dd4e4cf, 0xbdc37f7c, 0xbc92d1f6, 0x3e372c24, 0xbd13d27b, 
    0x3e0a5f04, 0x3e017bf6, 0xbd4ef669, 0xbe3217e9, 0xbe224e6d, 0xbdd5a85f, 0xbd358afc, 0x3dfc4c84, 
    0xbd95c43d, 0x3bc6b73d, 0xbb814a6e, 0xbdaefc9c, 0x3db52aa1, 0xbed5dac4, 0xbcc6ba39, 0x3aaa1ea5, 
    0x3d1fab6d, 0xbd08c441, 0xbd4efad8, 0x3dc74ed3, 0xbe005c49, 0x3c85b2b1, 0xbe39ad81, 0x3db611f2, 
    0xbd81dc9e, 0x3a8d9046, 0xbdbb647d, 0xbd55a081, 0xbd470096, 0x3d4a0f21, 0xbe0276a1, 0xbc76ca62, 
    0xbd8bffa1, 0xbbfc7c37, 0xbd114f16, 0x3e3a50b8, 0xbd7feb1c, 0x3cdec625, 0xbe3490cc, 0xbd6c93a8, 
    0xbec6f26c, 0xbd52c209, 0xbd7371e6, 0xbe15b2da, 0x3cb41a05, 0xbe829dcb, 0x3da6600f, 0xbda3feb6, 
    0x3d33e820, 0x3e01c56d, 0x3cf38500, 0xbe1db76e, 0xbd3d9e58, 0xbde7d7f9, 0xbe65ac1e, 0x3e04b3a2, 
    0xbddbb95a, 0xbda94dad, 0x3bd49180, 0xbd479c64, 0xbcdfe52a, 0x3e25596f, 0xbdf6e07a, 0xbc9c6a06, 
    0xbea14b99, 0xbdf7f0e8, 0x3dd3d403, 0x3d9a904c, 0xbdd3247a, 0xbdb099d9, 0x3ebe2996, 0x3d8252b0, 
    0xbde9bf80, 0xbc1cd2c2, 0xbd46f5a2, 0xbd087d05, 0xbd2911d6, 0xbcd684a6, 0xbdcbf6da, 0xbe0133bd, 
    0xbd630d72, 0xbd3ab2a2, 0xbe04bbb2, 0xbde951c2, 0xbd522312, 0xbe70017d, 0xbd178b92, 0x3cd888e1, 
    0x3db24c66, 0xbd1cc33d, 0x3d1b0304, 0xbc92c030, 0xbd89a2ca, 0x3cbe6944, 0xbe404bcb, 0xbc2f2511
};

static const uint32_t _k32[] = {
    0x3fcfde0e, 0x3fa0fcb1, 0x3fea6ae6, 0x3fc3a4b7, 0x3fbfbb4b, 0x3f80380f, 0x3f9f84be, 0x3f8b66cd, 
    0x3ffac49b, 0x3fa50dca, 0x3fb473aa, 0x3f9bd40f, 0x3fb3ba16, 0x3fd5340d, 0x3fc12578, 0x3fb17981, 
    0x3f9b19cb, 0x3fa17697, 0x3fb44303, 0x3fd0c003, 0x3fd1b406, 0x3fad0486, 0x3fbe15f3, 0x3fbdff85
};

static const uint32_t _k33[] = {
    0x3fea9fdd, 0x4027eab7, 0xbffae604, 0xbebaf8ee, 0x3fe4f0d0, 0xbf01521d, 0xbec34ece, 0x3fe5ad0e, 
    0xbf881d74, 0x3fefcf7e, 0x402589bc, 0x3fea4634, 0xbe39c278, 0xbe3ac2d3, 0xbedeba68, 0x3fae3eba, 
    0x3fd59ffb, 0x3ffad2a8, 0xbe72c1e7, 0xc0111dec, 0xbdfc0ae4, 0xbeb779a7, 0xbe86448a, 0x3ff668ce
};

static const uint32_t _k38[] = {
    0xbf3eb485, 0xbf475f4b, 0xbf5c2bd5, 0xbe504358, 0xbe9736fc, 0x3e461ff4, 0x3e487b22, 0xbf53dede, 
    0xbf080de6, 0xbd9009fa, 0xbf2d70c4, 0xbeb9ac29, 0x3cb46883, 0xbedf390c, 0xbe7dddac, 0x3e823e3e, 
    0x3e1b48e3, 0xbe97f8e9, 0xbe3e029f, 0xbe432290, 0xbf0e36e8, 0xbe6594b6, 0xbe21928d, 0xbe271487, 
    0x3eed30b3, 0x3c144d7b, 0x3f0ecd03, 0xbf0bb87e, 0x3ea2808a, 0xbf1086c9, 0xbea19552, 0x3eab9982, 
    0x3e4b7416, 0x3e815c6d, 0x3ee0012d, 0xb9b3e6f4, 0xbed1b564, 0xbefe3558, 0xbe67aa8f, 0x3ed4be2f, 
    0x3ea9de80, 0x3eafa7ac, 0xbb49941e, 0x3f155e3f, 0xbeb862e0, 0xbea1d4b6, 0xbeee7590, 0x3f0b5176, 
    0xbdfe3a39, 0xbed636ee, 0x3e701287, 0x3f14d6ee, 0xbf122bef, 0x3e99c19a, 0x3ef18605, 0xbe9b617f, 
    0xbe71aed9, 0xbeb60b46, 0x3bb3be4c, 0xbeeb9e01, 0x3ee2b77c, 0x3f481f20, 0x3f0bfbad, 0xbec6fe41, 
    0xbe8a5762, 0xbeba1d37, 0x3f1f4bfe, 0x3e7a298e, 0x3e907ef0, 0x3ebd85ef, 0x3ea18d28, 0xbea1307c
};

static const uint32_t _k40[] = {
    0xbe936ed5, 0x3d9afd13, 0x3c8c39d3
};

// Memory mapped buffers
#define _K12             ((int *)_k12)                       // s32[34] (136 bytes) 
#define _K20             ((float *)_k20)                     // f32[12,3,3,1] (432 bytes) 
#define _K22             ((float *)_k22)                     // f32[12] (48 bytes) 
#define _K23             ((float *)_k23)                     // f32[12] (48 bytes) 
#define _K27             ((float *)_k27)                     // f32[24,3,3,12] (10368 bytes) 
#define _K30             ((float *)_k30)                     // f32[24,3,3,24] (20736 bytes) 
#define _K32             ((float *)_k32)                     // f32[24] (96 bytes) 
#define _K33             ((float *)_k33)                     // f32[24] (96 bytes) 
#define _K38             ((float *)_k38)                     // f32[3,24] (288 bytes) 
#define _K40             ((float *)_k40)                     // f32[3] (12 bytes) 
#define _K5              ((float *)_k5)                      // f32[512] (2048 bytes) 
#define _K18             ((int8_t *)(_state + 0x000008d0))   // s8[6608] (6608 bytes) 
#define _K4              ((int8_t *)(_state + 0x00000000))   // s8[2256] (2256 bytes) 
#define _K8              ((int *)(_state + 0x000022a0))      // s32[24] (96 bytes) 
#define _K9              ((float *)(_state + 0x00002300))    // f32[258] (1032 bytes) 
#define _K10             ((float *)(_buffer + 0x00000000))   // f32[512] (2048 bytes) 
#define _K11             ((float *)(_buffer + 0x00000000))   // f32[257] (1028 bytes) 
#define _K13             ((float *)(_buffer + 0x00000404))   // f32[32] (128 bytes) 
#define _K14             ((float *)(_buffer + 0x00000000))   // f32[32] (128 bytes) 
#define _K15             ((float *)(_buffer + 0x00000080))   // f32[32] (128 bytes) 
#define _K16             ((float *)(_buffer + 0x00000000))   // f32[32] (128 bytes) 
#define _K17             ((float *)(_buffer + 0x00000000))   // f32[50,32] (6400 bytes) 
#define _K19             ((float *)(_buffer + 0x00001900))   // f32[25,16,12] (19200 bytes) 
#define _K21             ((float *)(_buffer + 0x00000000))   // f32[25,16,12] (19200 bytes) 
#define _K24             ((float *)(_buffer + 0x00006400))   // f32[25,16,12] (19200 bytes) 
#define _K25             ((float *)(_buffer + 0x00004b00))   // f32[25,16,12] (19200 bytes) 
#define _K26             ((float *)(_buffer + 0x00009600))   // f32[25,16,24] (38400 bytes) 
#define _K28             ((float *)(_buffer + 0x00000000))   // f32[25,16,24] (38400 bytes) 
#define _K29             ((float *)(_buffer + 0x00009600))   // f32[25,16,24] (38400 bytes) 
#define _K3              ((float *)(_buffer + 0x00000000))   // f32[512] (2048 bytes) 
#define _K31             ((float *)(_buffer + 0x00009600))   // f32[25,16,24] (38400 bytes) 
#define _K34             ((float *)(_buffer + 0x00000000))   // f32[25,16,24] (38400 bytes) 
#define _K35             ((float *)(_buffer + 0x00000000))   // f32[25,16,24] (38400 bytes) 
#define _K36             ((float *)(_buffer + 0x00009600))   // f32[12,8,24] (9216 bytes) 
#define _K37             ((float *)(_buffer + 0x00000000))   // f32[24] (96 bytes) 
#define _K39             ((float *)(_buffer + 0x00000060))   // f32[3] (12 bytes) 
#define _K41             ((float *)(_buffer + 0x00000000))   // f32[3] (12 bytes) 
#define _K6              ((float *)(_buffer + 0x00000800))   // f32[512] (2048 bytes) 
#define _K7              ((float *)(_buffer + 0x00001000))   // f32[257,2] (2056 bytes) 

// Represents a Circular Buffer
// https://en.wikipedia.org/wiki/Circular_buffer
typedef struct
{
	char *buf;
	int size;		// total bytes allocated in *buf
	int used;		// current bytes used in buffer.
	int read;
	int write;
} cbuffer_t;

#define CBUFFER_SUCCESS 0
#define CBUFFER_NOMEM -1

// Reset instance (clear buffer)
static inline void cbuffer_reset(cbuffer_t* buf) {
	buf->read = 0;
	buf->write = 0;
	buf->used = 0;
}

// Initializes a cbuffer handle with given memory and size.
static inline void cbuffer_init(cbuffer_t *dest, void *mem, int size) {
	dest->buf = mem;
	dest->size = size;
	cbuffer_reset(dest);
}

// Returns the number of free bytes in buffer.
static inline int cbuffer_get_free(cbuffer_t *buf) {
	return buf->size - buf->used;
}

// Returns the number of used bytes in buffer.
static inline int cbuffer_get_used(cbuffer_t *buf) {
	return buf->used;
}

// Writes given data to buffer.
// Returns CBUFFER_SUCCESS or CBUFFER_NOMEM if out of memory.
static inline int cbuffer_enqueue(cbuffer_t *buf, const void *data, int data_size) {
	int free = cbuffer_get_free(buf);

	// Out of memory?
	if (free < data_size)
		return CBUFFER_NOMEM;

	// Is the data split in the end?
	if (buf->write + data_size > buf->size) {
		int first_size = buf->size - buf->write;
		memcpy(buf->buf + buf->write, data, first_size);
		memcpy(buf->buf, ((char *)data) + first_size, data_size - first_size);
	}
	else {
		memcpy(buf->buf + buf->write, data, data_size);
	}
	buf->write += data_size;
	if (buf->write >= buf->size)
		buf->write -= buf->size;

	buf->used += data_size;
	return CBUFFER_SUCCESS;
}

// Advances the read pointer by given count.
// Returns CBUFFER_SUCCESS on success or CBUFFER_NOMEM if count is more than available data
static inline int cbuffer_advance(cbuffer_t *buf, int count) {
	int used = cbuffer_get_used(buf);

	if (count > used)
		return CBUFFER_NOMEM;

	buf->read += count;
	if (buf->read >= buf->size)
		buf->read -= buf->size;

	// Reset pointers to 0 if buffer is empty in order to avoid unwanted wrapps.
	if (buf->read == buf->write) {
		buf->read = 0;
		buf->write = 0;
	}

	buf->used -= count;
	return CBUFFER_SUCCESS;
}

// Returns a read pointer at given offset and  
// updates *can_read_bytes (if not NULL) with the number of bytes that can be read.
// 
// Note! Byte count written to can_read_bytes can be less than what cbuffer_get_used() returns.
// This happens when the read has to be split in two since it's a circular buffer.
static inline void* cbuffer_readptr(cbuffer_t* buf, int offset, int* can_read_bytes)
{
	int a0 = buf->read + offset;
	if (a0 >= buf->size)
		a0 -= buf->size;
	if (can_read_bytes != NULL)
	{
		int c0 = buf->used;
		if (a0 + c0 > buf->size)
			c0 = buf->size - a0;

		*can_read_bytes = c0;
	}
	return buf->buf + a0;
}

// Copies given "count" bytes to the "dst" buffer without advancing the buffer read offset.
// Returns CBUFFER_SUCCESS on success or CBUFFER_NOMEM if count is more than available data.
static inline int cbuffer_copyto(cbuffer_t *buf, void *dst, int count, int offset) {
	
	if (count > cbuffer_get_used(buf))
		return CBUFFER_NOMEM;

	int can_read_bytes;
	void* src_ptr = cbuffer_readptr(buf, offset, &can_read_bytes);

	int c0 = (count < can_read_bytes) ? count : can_read_bytes;
	memcpy(dst, src_ptr, c0);
	
	int c1 = count - c0;

	if (c1 > 0)
		memcpy(((char *)dst) + c0, buf->buf, c1);

	return CBUFFER_SUCCESS;
}

typedef struct {
	cbuffer_t data_buffer;			// Circular Buffer for features
	int input_size;					// Number of bytes in each input chunk
	int window_count;				// Number of input chunks in output window.
} fixwin_t;

#ifdef _MSC_VER
static_assert(sizeof(fixwin_t) <= 64, "Data structure 'fixwin_t' is too big");
#endif

#define IPWIN_RET_SUCCESS 0
#define IPWIN_RET_NODATA -1
#define IPWIN_RET_NOMEM -2

/*
* Try to dequeue a window.
*
* @param handle Pointer to an initialized handle.
* @param dst Pointer where to write window.
* @param stride_count Number of items (of size handle->input_size) to stride window.
* @return IPWIN_RET_SUCCESS (0) or IPWIN_RET_NODATA (-1) is no data is available.
*/
static inline int fixwin_dequeuef32(void* restrict handle, void* restrict dst, int stride_count)
{
	fixwin_t* fep = (fixwin_t*)handle;

	const int stride_bytes = stride_count * fep->input_size;
	const int size = fep->window_count * fep->input_size;
	if (cbuffer_get_used(&fep->data_buffer) >= size) {
		if (cbuffer_copyto(&fep->data_buffer, dst, size, 0) != 0)
			return IPWIN_RET_NOMEM;

		if (cbuffer_advance(&fep->data_buffer, stride_bytes) != 0)
			return IPWIN_RET_NOMEM;

		return IPWIN_RET_SUCCESS;
	}
	return IPWIN_RET_NODATA;
}

static inline void hammingmul_f32(const float* restrict a, const float* restrict b, float* restrict result, int d0, int d1)
{
	for (int j = 0; j < d1; j++) {
		for (int i = 0; i < d0; i++) {
			*result++ = *a++ * b[i];
		}
	}
}

static void makeipt(int nw, int *ip)
{
    int j, l, m, m2, p, q;
    
    ip[2] = 0;
    ip[3] = 16;
    m = 2;
    for (l = nw; l > 32; l >>= 2) {
        m2 = m << 1;
        q = m2 << 3;
        for (j = m; j < m2; j++) {
            p = ip[j] << 2;
            ip[m + j] = p;
            ip[m2 + j] = p + q;
        }
        m = m2;
    }
}

static void makewt(int nw, int *ip, float *w)
{
    void makeipt(int nw, int *ip);
    int j, nwh, nw0, nw1;
    float delta, wn4r, wk1r, wk1i, wk3r, wk3i;
    
    ip[0] = nw;
    ip[1] = 1;
    if (nw > 2) {
        nwh = nw >> 1;
        delta = atan(1.0) / nwh;
        wn4r = cos(delta * nwh);
        w[0] = 1;
        w[1] = wn4r;
        if (nwh == 4) {
            w[2] = cos(delta * 2);
            w[3] = sin(delta * 2);
        } else if (nwh > 4) {
            makeipt(nw, ip);
            w[2] = 0.5 / cos(delta * 2);
            w[3] = 0.5 / cos(delta * 6);
            for (j = 4; j < nwh; j += 4) {
                w[j] = cos(delta * j);
                w[j + 1] = sin(delta * j);
                w[j + 2] = cos(3 * delta * j);
                w[j + 3] = -sin(3 * delta * j);
            }
        }
        nw0 = 0;
        while (nwh > 2) {
            nw1 = nw0 + nwh;
            nwh >>= 1;
            w[nw1] = 1;
            w[nw1 + 1] = wn4r;
            if (nwh == 4) {
                wk1r = w[nw0 + 4];
                wk1i = w[nw0 + 5];
                w[nw1 + 2] = wk1r;
                w[nw1 + 3] = wk1i;
            } else if (nwh > 4) {
                wk1r = w[nw0 + 4];
                wk3r = w[nw0 + 6];
                w[nw1 + 2] = 0.5 / wk1r;
                w[nw1 + 3] = 0.5 / wk3r;
                for (j = 4; j < nwh; j += 4) {
                    wk1r = w[nw0 + 2 * j];
                    wk1i = w[nw0 + 2 * j + 1];
                    wk3r = w[nw0 + 2 * j + 2];
                    wk3i = w[nw0 + 2 * j + 3];
                    w[nw1 + j] = wk1r;
                    w[nw1 + j + 1] = wk1i;
                    w[nw1 + j + 2] = wk3r;
                    w[nw1 + j + 3] = wk3i;
                }
            }
            nw0 = nw1;
        }
    }
}

static void makect(int nc, int *ip, float *c)
{
    int j, nch;
    float delta;
    
    ip[1] = nc;
    if (nc > 1) {
        nch = nc >> 1;
        delta = atan(1.0) / nch;
        c[0] = cos(delta * nch);
        c[nch] = 0.5 * c[0];
        for (j = 1; j < nch; j++) {
            c[j] = 0.5 * cos(delta * j);
            c[nc - j] = 0.5 * sin(delta * j);
        }
    }
}

static void bitrv2(int n, int *ip, float *a)
{
    int j, j1, k, k1, l, m, nh, nm;
    float xr, xi, yr, yi;
    
    m = 1;
    for (l = n >> 2; l > 8; l >>= 2) {
        m <<= 1;
    }
    nh = n >> 1;
    nm = 4 * m;
    if (l == 8) {
        for (k = 0; k < m; k++) {
            for (j = 0; j < k; j++) {
                j1 = 4 * j + 2 * ip[m + k];
                k1 = 4 * k + 2 * ip[m + j];
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nm;
                k1 += 2 * nm;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nm;
                k1 -= nm;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nm;
                k1 += 2 * nm;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nh;
                k1 += 2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nm;
                k1 -= 2 * nm;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nm;
                k1 += nm;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nm;
                k1 -= 2 * nm;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += 2;
                k1 += nh;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nm;
                k1 += 2 * nm;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nm;
                k1 -= nm;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nm;
                k1 += 2 * nm;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nh;
                k1 -= 2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nm;
                k1 -= 2 * nm;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nm;
                k1 += nm;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nm;
                k1 -= 2 * nm;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
            }
            k1 = 4 * k + 2 * ip[m + k];
            j1 = k1 + 2;
            k1 += nh;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            j1 += nm;
            k1 += 2 * nm;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            j1 += nm;
            k1 -= nm;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            j1 -= 2;
            k1 -= nh;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            j1 += nh + 2;
            k1 += nh + 2;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            j1 -= nh - nm;
            k1 += 2 * nm - 2;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
        }
    } else {
        for (k = 0; k < m; k++) {
            for (j = 0; j < k; j++) {
                j1 = 4 * j + ip[m + k];
                k1 = 4 * k + ip[m + j];
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nm;
                k1 += nm;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nh;
                k1 += 2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nm;
                k1 -= nm;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += 2;
                k1 += nh;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nm;
                k1 += nm;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nh;
                k1 -= 2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nm;
                k1 -= nm;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
            }
            k1 = 4 * k + ip[m + k];
            j1 = k1 + 2;
            k1 += nh;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            j1 += nm;
            k1 += nm;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
        }
    }
}

static void bitrv216(float *a)
{
    float x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i, 
        x5r, x5i, x7r, x7i, x8r, x8i, x10r, x10i, 
        x11r, x11i, x12r, x12i, x13r, x13i, x14r, x14i;
    
    x1r = a[2];
    x1i = a[3];
    x2r = a[4];
    x2i = a[5];
    x3r = a[6];
    x3i = a[7];
    x4r = a[8];
    x4i = a[9];
    x5r = a[10];
    x5i = a[11];
    x7r = a[14];
    x7i = a[15];
    x8r = a[16];
    x8i = a[17];
    x10r = a[20];
    x10i = a[21];
    x11r = a[22];
    x11i = a[23];
    x12r = a[24];
    x12i = a[25];
    x13r = a[26];
    x13i = a[27];
    x14r = a[28];
    x14i = a[29];
    a[2] = x8r;
    a[3] = x8i;
    a[4] = x4r;
    a[5] = x4i;
    a[6] = x12r;
    a[7] = x12i;
    a[8] = x2r;
    a[9] = x2i;
    a[10] = x10r;
    a[11] = x10i;
    a[14] = x14r;
    a[15] = x14i;
    a[16] = x1r;
    a[17] = x1i;
    a[20] = x5r;
    a[21] = x5i;
    a[22] = x13r;
    a[23] = x13i;
    a[24] = x3r;
    a[25] = x3i;
    a[26] = x11r;
    a[27] = x11i;
    a[28] = x7r;
    a[29] = x7i;
}

static void bitrv208(float *a)
{
    float x1r, x1i, x3r, x3i, x4r, x4i, x6r, x6i;
    
    x1r = a[2];
    x1i = a[3];
    x3r = a[6];
    x3i = a[7];
    x4r = a[8];
    x4i = a[9];
    x6r = a[12];
    x6i = a[13];
    a[2] = x4r;
    a[3] = x4i;
    a[6] = x6r;
    a[7] = x6i;
    a[8] = x1r;
    a[9] = x1i;
    a[12] = x3r;
    a[13] = x3i;
}

static void cftf1st(int n, float *a, float *w)
{
    int j, j0, j1, j2, j3, k, m, mh;
    float wn4r, csc1, csc3, wk1r, wk1i, wk3r, wk3i, 
        wd1r, wd1i, wd3r, wd3i;
    float x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i;
    
    mh = n >> 3;
    m = 2 * mh;
    j1 = m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[0] + a[j2];
    x0i = a[1] + a[j2 + 1];
    x1r = a[0] - a[j2];
    x1i = a[1] - a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[0] = x0r + x2r;
    a[1] = x0i + x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i - x2i;
    a[j2] = x1r - x3i;
    a[j2 + 1] = x1i + x3r;
    a[j3] = x1r + x3i;
    a[j3 + 1] = x1i - x3r;
    wn4r = w[1];
    csc1 = w[2];
    csc3 = w[3];
    wd1r = 1;
    wd1i = 0;
    wd3r = 1;
    wd3i = 0;
    k = 0;
    for (j = 2; j < mh - 2; j += 4) {
        k += 4;
        wk1r = csc1 * (wd1r + w[k]);
        wk1i = csc1 * (wd1i + w[k + 1]);
        wk3r = csc3 * (wd3r + w[k + 2]);
        wk3i = csc3 * (wd3i + w[k + 3]);
        wd1r = w[k];
        wd1i = w[k + 1];
        wd3r = w[k + 2];
        wd3i = w[k + 3];
        j1 = j + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j] + a[j2];
        x0i = a[j + 1] + a[j2 + 1];
        x1r = a[j] - a[j2];
        x1i = a[j + 1] - a[j2 + 1];
        y0r = a[j + 2] + a[j2 + 2];
        y0i = a[j + 3] + a[j2 + 3];
        y1r = a[j + 2] - a[j2 + 2];
        y1i = a[j + 3] - a[j2 + 3];
        x2r = a[j1] + a[j3];
        x2i = a[j1 + 1] + a[j3 + 1];
        x3r = a[j1] - a[j3];
        x3i = a[j1 + 1] - a[j3 + 1];
        y2r = a[j1 + 2] + a[j3 + 2];
        y2i = a[j1 + 3] + a[j3 + 3];
        y3r = a[j1 + 2] - a[j3 + 2];
        y3i = a[j1 + 3] - a[j3 + 3];
        a[j] = x0r + x2r;
        a[j + 1] = x0i + x2i;
        a[j + 2] = y0r + y2r;
        a[j + 3] = y0i + y2i;
        a[j1] = x0r - x2r;
        a[j1 + 1] = x0i - x2i;
        a[j1 + 2] = y0r - y2r;
        a[j1 + 3] = y0i - y2i;
        x0r = x1r - x3i;
        x0i = x1i + x3r;
        a[j2] = wk1r * x0r - wk1i * x0i;
        a[j2 + 1] = wk1r * x0i + wk1i * x0r;
        x0r = y1r - y3i;
        x0i = y1i + y3r;
        a[j2 + 2] = wd1r * x0r - wd1i * x0i;
        a[j2 + 3] = wd1r * x0i + wd1i * x0r;
        x0r = x1r + x3i;
        x0i = x1i - x3r;
        a[j3] = wk3r * x0r + wk3i * x0i;
        a[j3 + 1] = wk3r * x0i - wk3i * x0r;
        x0r = y1r + y3i;
        x0i = y1i - y3r;
        a[j3 + 2] = wd3r * x0r + wd3i * x0i;
        a[j3 + 3] = wd3r * x0i - wd3i * x0r;
        j0 = m - j;
        j1 = j0 + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j0] + a[j2];
        x0i = a[j0 + 1] + a[j2 + 1];
        x1r = a[j0] - a[j2];
        x1i = a[j0 + 1] - a[j2 + 1];
        y0r = a[j0 - 2] + a[j2 - 2];
        y0i = a[j0 - 1] + a[j2 - 1];
        y1r = a[j0 - 2] - a[j2 - 2];
        y1i = a[j0 - 1] - a[j2 - 1];
        x2r = a[j1] + a[j3];
        x2i = a[j1 + 1] + a[j3 + 1];
        x3r = a[j1] - a[j3];
        x3i = a[j1 + 1] - a[j3 + 1];
        y2r = a[j1 - 2] + a[j3 - 2];
        y2i = a[j1 - 1] + a[j3 - 1];
        y3r = a[j1 - 2] - a[j3 - 2];
        y3i = a[j1 - 1] - a[j3 - 1];
        a[j0] = x0r + x2r;
        a[j0 + 1] = x0i + x2i;
        a[j0 - 2] = y0r + y2r;
        a[j0 - 1] = y0i + y2i;
        a[j1] = x0r - x2r;
        a[j1 + 1] = x0i - x2i;
        a[j1 - 2] = y0r - y2r;
        a[j1 - 1] = y0i - y2i;
        x0r = x1r - x3i;
        x0i = x1i + x3r;
        a[j2] = wk1i * x0r - wk1r * x0i;
        a[j2 + 1] = wk1i * x0i + wk1r * x0r;
        x0r = y1r - y3i;
        x0i = y1i + y3r;
        a[j2 - 2] = wd1i * x0r - wd1r * x0i;
        a[j2 - 1] = wd1i * x0i + wd1r * x0r;
        x0r = x1r + x3i;
        x0i = x1i - x3r;
        a[j3] = wk3i * x0r + wk3r * x0i;
        a[j3 + 1] = wk3i * x0i - wk3r * x0r;
        x0r = y1r + y3i;
        x0i = y1i - y3r;
        a[j3 - 2] = wd3i * x0r + wd3r * x0i;
        a[j3 - 1] = wd3i * x0i - wd3r * x0r;
    }
    wk1r = csc1 * (wd1r + wn4r);
    wk1i = csc1 * (wd1i + wn4r);
    wk3r = csc3 * (wd3r - wn4r);
    wk3i = csc3 * (wd3i - wn4r);
    j0 = mh;
    j1 = j0 + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j0 - 2] + a[j2 - 2];
    x0i = a[j0 - 1] + a[j2 - 1];
    x1r = a[j0 - 2] - a[j2 - 2];
    x1i = a[j0 - 1] - a[j2 - 1];
    x2r = a[j1 - 2] + a[j3 - 2];
    x2i = a[j1 - 1] + a[j3 - 1];
    x3r = a[j1 - 2] - a[j3 - 2];
    x3i = a[j1 - 1] - a[j3 - 1];
    a[j0 - 2] = x0r + x2r;
    a[j0 - 1] = x0i + x2i;
    a[j1 - 2] = x0r - x2r;
    a[j1 - 1] = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[j2 - 2] = wk1r * x0r - wk1i * x0i;
    a[j2 - 1] = wk1r * x0i + wk1i * x0r;
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    a[j3 - 2] = wk3r * x0r + wk3i * x0i;
    a[j3 - 1] = wk3r * x0i - wk3i * x0r;
    x0r = a[j0] + a[j2];
    x0i = a[j0 + 1] + a[j2 + 1];
    x1r = a[j0] - a[j2];
    x1i = a[j0 + 1] - a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[j0] = x0r + x2r;
    a[j0 + 1] = x0i + x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[j2] = wn4r * (x0r - x0i);
    a[j2 + 1] = wn4r * (x0i + x0r);
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    a[j3] = -wn4r * (x0r + x0i);
    a[j3 + 1] = -wn4r * (x0i - x0r);
    x0r = a[j0 + 2] + a[j2 + 2];
    x0i = a[j0 + 3] + a[j2 + 3];
    x1r = a[j0 + 2] - a[j2 + 2];
    x1i = a[j0 + 3] - a[j2 + 3];
    x2r = a[j1 + 2] + a[j3 + 2];
    x2i = a[j1 + 3] + a[j3 + 3];
    x3r = a[j1 + 2] - a[j3 + 2];
    x3i = a[j1 + 3] - a[j3 + 3];
    a[j0 + 2] = x0r + x2r;
    a[j0 + 3] = x0i + x2i;
    a[j1 + 2] = x0r - x2r;
    a[j1 + 3] = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[j2 + 2] = wk1i * x0r - wk1r * x0i;
    a[j2 + 3] = wk1i * x0i + wk1r * x0r;
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    a[j3 + 2] = wk3i * x0r + wk3r * x0i;
    a[j3 + 3] = wk3i * x0i - wk3r * x0r;
}

static void cftmdl1(int n, float *a, float *w)
{
    int j, j0, j1, j2, j3, k, m, mh;
    float wn4r, wk1r, wk1i, wk3r, wk3i;
    float x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    
    mh = n >> 3;
    m = 2 * mh;
    j1 = m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[0] + a[j2];
    x0i = a[1] + a[j2 + 1];
    x1r = a[0] - a[j2];
    x1i = a[1] - a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[0] = x0r + x2r;
    a[1] = x0i + x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i - x2i;
    a[j2] = x1r - x3i;
    a[j2 + 1] = x1i + x3r;
    a[j3] = x1r + x3i;
    a[j3 + 1] = x1i - x3r;
    wn4r = w[1];
    k = 0;
    for (j = 2; j < mh; j += 2) {
        k += 4;
        wk1r = w[k];
        wk1i = w[k + 1];
        wk3r = w[k + 2];
        wk3i = w[k + 3];
        j1 = j + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j] + a[j2];
        x0i = a[j + 1] + a[j2 + 1];
        x1r = a[j] - a[j2];
        x1i = a[j + 1] - a[j2 + 1];
        x2r = a[j1] + a[j3];
        x2i = a[j1 + 1] + a[j3 + 1];
        x3r = a[j1] - a[j3];
        x3i = a[j1 + 1] - a[j3 + 1];
        a[j] = x0r + x2r;
        a[j + 1] = x0i + x2i;
        a[j1] = x0r - x2r;
        a[j1 + 1] = x0i - x2i;
        x0r = x1r - x3i;
        x0i = x1i + x3r;
        a[j2] = wk1r * x0r - wk1i * x0i;
        a[j2 + 1] = wk1r * x0i + wk1i * x0r;
        x0r = x1r + x3i;
        x0i = x1i - x3r;
        a[j3] = wk3r * x0r + wk3i * x0i;
        a[j3 + 1] = wk3r * x0i - wk3i * x0r;
        j0 = m - j;
        j1 = j0 + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j0] + a[j2];
        x0i = a[j0 + 1] + a[j2 + 1];
        x1r = a[j0] - a[j2];
        x1i = a[j0 + 1] - a[j2 + 1];
        x2r = a[j1] + a[j3];
        x2i = a[j1 + 1] + a[j3 + 1];
        x3r = a[j1] - a[j3];
        x3i = a[j1 + 1] - a[j3 + 1];
        a[j0] = x0r + x2r;
        a[j0 + 1] = x0i + x2i;
        a[j1] = x0r - x2r;
        a[j1 + 1] = x0i - x2i;
        x0r = x1r - x3i;
        x0i = x1i + x3r;
        a[j2] = wk1i * x0r - wk1r * x0i;
        a[j2 + 1] = wk1i * x0i + wk1r * x0r;
        x0r = x1r + x3i;
        x0i = x1i - x3r;
        a[j3] = wk3i * x0r + wk3r * x0i;
        a[j3 + 1] = wk3i * x0i - wk3r * x0r;
    }
    j0 = mh;
    j1 = j0 + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j0] + a[j2];
    x0i = a[j0 + 1] + a[j2 + 1];
    x1r = a[j0] - a[j2];
    x1i = a[j0 + 1] - a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[j0] = x0r + x2r;
    a[j0 + 1] = x0i + x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[j2] = wn4r * (x0r - x0i);
    a[j2 + 1] = wn4r * (x0i + x0r);
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    a[j3] = -wn4r * (x0r + x0i);
    a[j3 + 1] = -wn4r * (x0i - x0r);
}

static void cftmdl2(int n, float *a, float *w)
{
    int j, j0, j1, j2, j3, k, kr, m, mh;
    float wn4r, wk1r, wk1i, wk3r, wk3i, wd1r, wd1i, wd3r, wd3i;
    float x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, y0r, y0i, y2r, y2i;
    
    mh = n >> 3;
    m = 2 * mh;
    wn4r = w[1];
    j1 = m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[0] - a[j2 + 1];
    x0i = a[1] + a[j2];
    x1r = a[0] + a[j2 + 1];
    x1i = a[1] - a[j2];
    x2r = a[j1] - a[j3 + 1];
    x2i = a[j1 + 1] + a[j3];
    x3r = a[j1] + a[j3 + 1];
    x3i = a[j1 + 1] - a[j3];
    y0r = wn4r * (x2r - x2i);
    y0i = wn4r * (x2i + x2r);
    a[0] = x0r + y0r;
    a[1] = x0i + y0i;
    a[j1] = x0r - y0r;
    a[j1 + 1] = x0i - y0i;
    y0r = wn4r * (x3r - x3i);
    y0i = wn4r * (x3i + x3r);
    a[j2] = x1r - y0i;
    a[j2 + 1] = x1i + y0r;
    a[j3] = x1r + y0i;
    a[j3 + 1] = x1i - y0r;
    k = 0;
    kr = 2 * m;
    for (j = 2; j < mh; j += 2) {
        k += 4;
        wk1r = w[k];
        wk1i = w[k + 1];
        wk3r = w[k + 2];
        wk3i = w[k + 3];
        kr -= 4;
        wd1i = w[kr];
        wd1r = w[kr + 1];
        wd3i = w[kr + 2];
        wd3r = w[kr + 3];
        j1 = j + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j] - a[j2 + 1];
        x0i = a[j + 1] + a[j2];
        x1r = a[j] + a[j2 + 1];
        x1i = a[j + 1] - a[j2];
        x2r = a[j1] - a[j3 + 1];
        x2i = a[j1 + 1] + a[j3];
        x3r = a[j1] + a[j3 + 1];
        x3i = a[j1 + 1] - a[j3];
        y0r = wk1r * x0r - wk1i * x0i;
        y0i = wk1r * x0i + wk1i * x0r;
        y2r = wd1r * x2r - wd1i * x2i;
        y2i = wd1r * x2i + wd1i * x2r;
        a[j] = y0r + y2r;
        a[j + 1] = y0i + y2i;
        a[j1] = y0r - y2r;
        a[j1 + 1] = y0i - y2i;
        y0r = wk3r * x1r + wk3i * x1i;
        y0i = wk3r * x1i - wk3i * x1r;
        y2r = wd3r * x3r + wd3i * x3i;
        y2i = wd3r * x3i - wd3i * x3r;
        a[j2] = y0r + y2r;
        a[j2 + 1] = y0i + y2i;
        a[j3] = y0r - y2r;
        a[j3 + 1] = y0i - y2i;
        j0 = m - j;
        j1 = j0 + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j0] - a[j2 + 1];
        x0i = a[j0 + 1] + a[j2];
        x1r = a[j0] + a[j2 + 1];
        x1i = a[j0 + 1] - a[j2];
        x2r = a[j1] - a[j3 + 1];
        x2i = a[j1 + 1] + a[j3];
        x3r = a[j1] + a[j3 + 1];
        x3i = a[j1 + 1] - a[j3];
        y0r = wd1i * x0r - wd1r * x0i;
        y0i = wd1i * x0i + wd1r * x0r;
        y2r = wk1i * x2r - wk1r * x2i;
        y2i = wk1i * x2i + wk1r * x2r;
        a[j0] = y0r + y2r;
        a[j0 + 1] = y0i + y2i;
        a[j1] = y0r - y2r;
        a[j1 + 1] = y0i - y2i;
        y0r = wd3i * x1r + wd3r * x1i;
        y0i = wd3i * x1i - wd3r * x1r;
        y2r = wk3i * x3r + wk3r * x3i;
        y2i = wk3i * x3i - wk3r * x3r;
        a[j2] = y0r + y2r;
        a[j2 + 1] = y0i + y2i;
        a[j3] = y0r - y2r;
        a[j3 + 1] = y0i - y2i;
    }
    wk1r = w[m];
    wk1i = w[m + 1];
    j0 = mh;
    j1 = j0 + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j0] - a[j2 + 1];
    x0i = a[j0 + 1] + a[j2];
    x1r = a[j0] + a[j2 + 1];
    x1i = a[j0 + 1] - a[j2];
    x2r = a[j1] - a[j3 + 1];
    x2i = a[j1 + 1] + a[j3];
    x3r = a[j1] + a[j3 + 1];
    x3i = a[j1 + 1] - a[j3];
    y0r = wk1r * x0r - wk1i * x0i;
    y0i = wk1r * x0i + wk1i * x0r;
    y2r = wk1i * x2r - wk1r * x2i;
    y2i = wk1i * x2i + wk1r * x2r;
    a[j0] = y0r + y2r;
    a[j0 + 1] = y0i + y2i;
    a[j1] = y0r - y2r;
    a[j1 + 1] = y0i - y2i;
    y0r = wk1i * x1r - wk1r * x1i;
    y0i = wk1i * x1i + wk1r * x1r;
    y2r = wk1r * x3r - wk1i * x3i;
    y2i = wk1r * x3i + wk1i * x3r;
    a[j2] = y0r - y2r;
    a[j2 + 1] = y0i - y2i;
    a[j3] = y0r + y2r;
    a[j3 + 1] = y0i + y2i;
}

static int cfttree(int n, int j, int k, float *a, int nw, float *w)
{
    void cftmdl1(int n, float *a, float *w);
    void cftmdl2(int n, float *a, float *w);
    int i, isplt, m;
    
    if ((k & 3) != 0) {
        isplt = k & 1;
        if (isplt != 0) {
            cftmdl1(n, &a[j - n], &w[nw - (n >> 1)]);
        } else {
            cftmdl2(n, &a[j - n], &w[nw - n]);
        }
    } else {
        m = n;
        for (i = k; (i & 3) == 0; i >>= 2) {
            m <<= 2;
        }
        isplt = i & 1;
        if (isplt != 0) {
            while (m > 128) {
                cftmdl1(m, &a[j - m], &w[nw - (m >> 1)]);
                m >>= 2;
            }
        } else {
            while (m > 128) {
                cftmdl2(m, &a[j - m], &w[nw - m]);
                m >>= 2;
            }
        }
    }
    return isplt;
}

static void cftf161(float *a, float *w)
{
    float wn4r, wk1r, wk1i, 
        x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, 
        y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i, 
        y8r, y8i, y9r, y9i, y10r, y10i, y11r, y11i, 
        y12r, y12i, y13r, y13i, y14r, y14i, y15r, y15i;
    
    wn4r = w[1];
    wk1r = w[2];
    wk1i = w[3];
    x0r = a[0] + a[16];
    x0i = a[1] + a[17];
    x1r = a[0] - a[16];
    x1i = a[1] - a[17];
    x2r = a[8] + a[24];
    x2i = a[9] + a[25];
    x3r = a[8] - a[24];
    x3i = a[9] - a[25];
    y0r = x0r + x2r;
    y0i = x0i + x2i;
    y4r = x0r - x2r;
    y4i = x0i - x2i;
    y8r = x1r - x3i;
    y8i = x1i + x3r;
    y12r = x1r + x3i;
    y12i = x1i - x3r;
    x0r = a[2] + a[18];
    x0i = a[3] + a[19];
    x1r = a[2] - a[18];
    x1i = a[3] - a[19];
    x2r = a[10] + a[26];
    x2i = a[11] + a[27];
    x3r = a[10] - a[26];
    x3i = a[11] - a[27];
    y1r = x0r + x2r;
    y1i = x0i + x2i;
    y5r = x0r - x2r;
    y5i = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    y9r = wk1r * x0r - wk1i * x0i;
    y9i = wk1r * x0i + wk1i * x0r;
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    y13r = wk1i * x0r - wk1r * x0i;
    y13i = wk1i * x0i + wk1r * x0r;
    x0r = a[4] + a[20];
    x0i = a[5] + a[21];
    x1r = a[4] - a[20];
    x1i = a[5] - a[21];
    x2r = a[12] + a[28];
    x2i = a[13] + a[29];
    x3r = a[12] - a[28];
    x3i = a[13] - a[29];
    y2r = x0r + x2r;
    y2i = x0i + x2i;
    y6r = x0r - x2r;
    y6i = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    y10r = wn4r * (x0r - x0i);
    y10i = wn4r * (x0i + x0r);
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    y14r = wn4r * (x0r + x0i);
    y14i = wn4r * (x0i - x0r);
    x0r = a[6] + a[22];
    x0i = a[7] + a[23];
    x1r = a[6] - a[22];
    x1i = a[7] - a[23];
    x2r = a[14] + a[30];
    x2i = a[15] + a[31];
    x3r = a[14] - a[30];
    x3i = a[15] - a[31];
    y3r = x0r + x2r;
    y3i = x0i + x2i;
    y7r = x0r - x2r;
    y7i = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    y11r = wk1i * x0r - wk1r * x0i;
    y11i = wk1i * x0i + wk1r * x0r;
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    y15r = wk1r * x0r - wk1i * x0i;
    y15i = wk1r * x0i + wk1i * x0r;
    x0r = y12r - y14r;
    x0i = y12i - y14i;
    x1r = y12r + y14r;
    x1i = y12i + y14i;
    x2r = y13r - y15r;
    x2i = y13i - y15i;
    x3r = y13r + y15r;
    x3i = y13i + y15i;
    a[24] = x0r + x2r;
    a[25] = x0i + x2i;
    a[26] = x0r - x2r;
    a[27] = x0i - x2i;
    a[28] = x1r - x3i;
    a[29] = x1i + x3r;
    a[30] = x1r + x3i;
    a[31] = x1i - x3r;
    x0r = y8r + y10r;
    x0i = y8i + y10i;
    x1r = y8r - y10r;
    x1i = y8i - y10i;
    x2r = y9r + y11r;
    x2i = y9i + y11i;
    x3r = y9r - y11r;
    x3i = y9i - y11i;
    a[16] = x0r + x2r;
    a[17] = x0i + x2i;
    a[18] = x0r - x2r;
    a[19] = x0i - x2i;
    a[20] = x1r - x3i;
    a[21] = x1i + x3r;
    a[22] = x1r + x3i;
    a[23] = x1i - x3r;
    x0r = y5r - y7i;
    x0i = y5i + y7r;
    x2r = wn4r * (x0r - x0i);
    x2i = wn4r * (x0i + x0r);
    x0r = y5r + y7i;
    x0i = y5i - y7r;
    x3r = wn4r * (x0r - x0i);
    x3i = wn4r * (x0i + x0r);
    x0r = y4r - y6i;
    x0i = y4i + y6r;
    x1r = y4r + y6i;
    x1i = y4i - y6r;
    a[8] = x0r + x2r;
    a[9] = x0i + x2i;
    a[10] = x0r - x2r;
    a[11] = x0i - x2i;
    a[12] = x1r - x3i;
    a[13] = x1i + x3r;
    a[14] = x1r + x3i;
    a[15] = x1i - x3r;
    x0r = y0r + y2r;
    x0i = y0i + y2i;
    x1r = y0r - y2r;
    x1i = y0i - y2i;
    x2r = y1r + y3r;
    x2i = y1i + y3i;
    x3r = y1r - y3r;
    x3i = y1i - y3i;
    a[0] = x0r + x2r;
    a[1] = x0i + x2i;
    a[2] = x0r - x2r;
    a[3] = x0i - x2i;
    a[4] = x1r - x3i;
    a[5] = x1i + x3r;
    a[6] = x1r + x3i;
    a[7] = x1i - x3r;
}

static void cftf162(float *a, float *w)
{
    float wn4r, wk1r, wk1i, wk2r, wk2i, wk3r, wk3i, 
        x0r, x0i, x1r, x1i, x2r, x2i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, 
        y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i, 
        y8r, y8i, y9r, y9i, y10r, y10i, y11r, y11i, 
        y12r, y12i, y13r, y13i, y14r, y14i, y15r, y15i;
    
    wn4r = w[1];
    wk1r = w[4];
    wk1i = w[5];
    wk3r = w[6];
    wk3i = -w[7];
    wk2r = w[8];
    wk2i = w[9];
    x1r = a[0] - a[17];
    x1i = a[1] + a[16];
    x0r = a[8] - a[25];
    x0i = a[9] + a[24];
    x2r = wn4r * (x0r - x0i);
    x2i = wn4r * (x0i + x0r);
    y0r = x1r + x2r;
    y0i = x1i + x2i;
    y4r = x1r - x2r;
    y4i = x1i - x2i;
    x1r = a[0] + a[17];
    x1i = a[1] - a[16];
    x0r = a[8] + a[25];
    x0i = a[9] - a[24];
    x2r = wn4r * (x0r - x0i);
    x2i = wn4r * (x0i + x0r);
    y8r = x1r - x2i;
    y8i = x1i + x2r;
    y12r = x1r + x2i;
    y12i = x1i - x2r;
    x0r = a[2] - a[19];
    x0i = a[3] + a[18];
    x1r = wk1r * x0r - wk1i * x0i;
    x1i = wk1r * x0i + wk1i * x0r;
    x0r = a[10] - a[27];
    x0i = a[11] + a[26];
    x2r = wk3i * x0r - wk3r * x0i;
    x2i = wk3i * x0i + wk3r * x0r;
    y1r = x1r + x2r;
    y1i = x1i + x2i;
    y5r = x1r - x2r;
    y5i = x1i - x2i;
    x0r = a[2] + a[19];
    x0i = a[3] - a[18];
    x1r = wk3r * x0r - wk3i * x0i;
    x1i = wk3r * x0i + wk3i * x0r;
    x0r = a[10] + a[27];
    x0i = a[11] - a[26];
    x2r = wk1r * x0r + wk1i * x0i;
    x2i = wk1r * x0i - wk1i * x0r;
    y9r = x1r - x2r;
    y9i = x1i - x2i;
    y13r = x1r + x2r;
    y13i = x1i + x2i;
    x0r = a[4] - a[21];
    x0i = a[5] + a[20];
    x1r = wk2r * x0r - wk2i * x0i;
    x1i = wk2r * x0i + wk2i * x0r;
    x0r = a[12] - a[29];
    x0i = a[13] + a[28];
    x2r = wk2i * x0r - wk2r * x0i;
    x2i = wk2i * x0i + wk2r * x0r;
    y2r = x1r + x2r;
    y2i = x1i + x2i;
    y6r = x1r - x2r;
    y6i = x1i - x2i;
    x0r = a[4] + a[21];
    x0i = a[5] - a[20];
    x1r = wk2i * x0r - wk2r * x0i;
    x1i = wk2i * x0i + wk2r * x0r;
    x0r = a[12] + a[29];
    x0i = a[13] - a[28];
    x2r = wk2r * x0r - wk2i * x0i;
    x2i = wk2r * x0i + wk2i * x0r;
    y10r = x1r - x2r;
    y10i = x1i - x2i;
    y14r = x1r + x2r;
    y14i = x1i + x2i;
    x0r = a[6] - a[23];
    x0i = a[7] + a[22];
    x1r = wk3r * x0r - wk3i * x0i;
    x1i = wk3r * x0i + wk3i * x0r;
    x0r = a[14] - a[31];
    x0i = a[15] + a[30];
    x2r = wk1i * x0r - wk1r * x0i;
    x2i = wk1i * x0i + wk1r * x0r;
    y3r = x1r + x2r;
    y3i = x1i + x2i;
    y7r = x1r - x2r;
    y7i = x1i - x2i;
    x0r = a[6] + a[23];
    x0i = a[7] - a[22];
    x1r = wk1i * x0r + wk1r * x0i;
    x1i = wk1i * x0i - wk1r * x0r;
    x0r = a[14] + a[31];
    x0i = a[15] - a[30];
    x2r = wk3i * x0r - wk3r * x0i;
    x2i = wk3i * x0i + wk3r * x0r;
    y11r = x1r + x2r;
    y11i = x1i + x2i;
    y15r = x1r - x2r;
    y15i = x1i - x2i;
    x1r = y0r + y2r;
    x1i = y0i + y2i;
    x2r = y1r + y3r;
    x2i = y1i + y3i;
    a[0] = x1r + x2r;
    a[1] = x1i + x2i;
    a[2] = x1r - x2r;
    a[3] = x1i - x2i;
    x1r = y0r - y2r;
    x1i = y0i - y2i;
    x2r = y1r - y3r;
    x2i = y1i - y3i;
    a[4] = x1r - x2i;
    a[5] = x1i + x2r;
    a[6] = x1r + x2i;
    a[7] = x1i - x2r;
    x1r = y4r - y6i;
    x1i = y4i + y6r;
    x0r = y5r - y7i;
    x0i = y5i + y7r;
    x2r = wn4r * (x0r - x0i);
    x2i = wn4r * (x0i + x0r);
    a[8] = x1r + x2r;
    a[9] = x1i + x2i;
    a[10] = x1r - x2r;
    a[11] = x1i - x2i;
    x1r = y4r + y6i;
    x1i = y4i - y6r;
    x0r = y5r + y7i;
    x0i = y5i - y7r;
    x2r = wn4r * (x0r - x0i);
    x2i = wn4r * (x0i + x0r);
    a[12] = x1r - x2i;
    a[13] = x1i + x2r;
    a[14] = x1r + x2i;
    a[15] = x1i - x2r;
    x1r = y8r + y10r;
    x1i = y8i + y10i;
    x2r = y9r - y11r;
    x2i = y9i - y11i;
    a[16] = x1r + x2r;
    a[17] = x1i + x2i;
    a[18] = x1r - x2r;
    a[19] = x1i - x2i;
    x1r = y8r - y10r;
    x1i = y8i - y10i;
    x2r = y9r + y11r;
    x2i = y9i + y11i;
    a[20] = x1r - x2i;
    a[21] = x1i + x2r;
    a[22] = x1r + x2i;
    a[23] = x1i - x2r;
    x1r = y12r - y14i;
    x1i = y12i + y14r;
    x0r = y13r + y15i;
    x0i = y13i - y15r;
    x2r = wn4r * (x0r - x0i);
    x2i = wn4r * (x0i + x0r);
    a[24] = x1r + x2r;
    a[25] = x1i + x2i;
    a[26] = x1r - x2r;
    a[27] = x1i - x2i;
    x1r = y12r + y14i;
    x1i = y12i - y14r;
    x0r = y13r - y15i;
    x0i = y13i + y15r;
    x2r = wn4r * (x0r - x0i);
    x2i = wn4r * (x0i + x0r);
    a[28] = x1r - x2i;
    a[29] = x1i + x2r;
    a[30] = x1r + x2i;
    a[31] = x1i - x2r;
}

static void cftf081(float *a, float *w)
{
    float wn4r, x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, 
        y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i;
    
    wn4r = w[1];
    x0r = a[0] + a[8];
    x0i = a[1] + a[9];
    x1r = a[0] - a[8];
    x1i = a[1] - a[9];
    x2r = a[4] + a[12];
    x2i = a[5] + a[13];
    x3r = a[4] - a[12];
    x3i = a[5] - a[13];
    y0r = x0r + x2r;
    y0i = x0i + x2i;
    y2r = x0r - x2r;
    y2i = x0i - x2i;
    y1r = x1r - x3i;
    y1i = x1i + x3r;
    y3r = x1r + x3i;
    y3i = x1i - x3r;
    x0r = a[2] + a[10];
    x0i = a[3] + a[11];
    x1r = a[2] - a[10];
    x1i = a[3] - a[11];
    x2r = a[6] + a[14];
    x2i = a[7] + a[15];
    x3r = a[6] - a[14];
    x3i = a[7] - a[15];
    y4r = x0r + x2r;
    y4i = x0i + x2i;
    y6r = x0r - x2r;
    y6i = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    x2r = x1r + x3i;
    x2i = x1i - x3r;
    y5r = wn4r * (x0r - x0i);
    y5i = wn4r * (x0r + x0i);
    y7r = wn4r * (x2r - x2i);
    y7i = wn4r * (x2r + x2i);
    a[8] = y1r + y5r;
    a[9] = y1i + y5i;
    a[10] = y1r - y5r;
    a[11] = y1i - y5i;
    a[12] = y3r - y7i;
    a[13] = y3i + y7r;
    a[14] = y3r + y7i;
    a[15] = y3i - y7r;
    a[0] = y0r + y4r;
    a[1] = y0i + y4i;
    a[2] = y0r - y4r;
    a[3] = y0i - y4i;
    a[4] = y2r - y6i;
    a[5] = y2i + y6r;
    a[6] = y2r + y6i;
    a[7] = y2i - y6r;
}

static void cftf082(float *a, float *w)
{
    float wn4r, wk1r, wk1i, x0r, x0i, x1r, x1i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, 
        y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i;
    
    wn4r = w[1];
    wk1r = w[2];
    wk1i = w[3];
    y0r = a[0] - a[9];
    y0i = a[1] + a[8];
    y1r = a[0] + a[9];
    y1i = a[1] - a[8];
    x0r = a[4] - a[13];
    x0i = a[5] + a[12];
    y2r = wn4r * (x0r - x0i);
    y2i = wn4r * (x0i + x0r);
    x0r = a[4] + a[13];
    x0i = a[5] - a[12];
    y3r = wn4r * (x0r - x0i);
    y3i = wn4r * (x0i + x0r);
    x0r = a[2] - a[11];
    x0i = a[3] + a[10];
    y4r = wk1r * x0r - wk1i * x0i;
    y4i = wk1r * x0i + wk1i * x0r;
    x0r = a[2] + a[11];
    x0i = a[3] - a[10];
    y5r = wk1i * x0r - wk1r * x0i;
    y5i = wk1i * x0i + wk1r * x0r;
    x0r = a[6] - a[15];
    x0i = a[7] + a[14];
    y6r = wk1i * x0r - wk1r * x0i;
    y6i = wk1i * x0i + wk1r * x0r;
    x0r = a[6] + a[15];
    x0i = a[7] - a[14];
    y7r = wk1r * x0r - wk1i * x0i;
    y7i = wk1r * x0i + wk1i * x0r;
    x0r = y0r + y2r;
    x0i = y0i + y2i;
    x1r = y4r + y6r;
    x1i = y4i + y6i;
    a[0] = x0r + x1r;
    a[1] = x0i + x1i;
    a[2] = x0r - x1r;
    a[3] = x0i - x1i;
    x0r = y0r - y2r;
    x0i = y0i - y2i;
    x1r = y4r - y6r;
    x1i = y4i - y6i;
    a[4] = x0r - x1i;
    a[5] = x0i + x1r;
    a[6] = x0r + x1i;
    a[7] = x0i - x1r;
    x0r = y1r - y3i;
    x0i = y1i + y3r;
    x1r = y5r - y7r;
    x1i = y5i - y7i;
    a[8] = x0r + x1r;
    a[9] = x0i + x1i;
    a[10] = x0r - x1r;
    a[11] = x0i - x1i;
    x0r = y1r + y3i;
    x0i = y1i - y3r;
    x1r = y5r + y7r;
    x1i = y5i + y7i;
    a[12] = x0r - x1i;
    a[13] = x0i + x1r;
    a[14] = x0r + x1i;
    a[15] = x0i - x1r;
}

static void cftleaf(int n, int isplt, float *a, int nw, float *w)
{
    void cftmdl1(int n, float *a, float *w);
    void cftmdl2(int n, float *a, float *w);
    void cftf161(float *a, float *w);
    void cftf162(float *a, float *w);
    void cftf081(float *a, float *w);
    void cftf082(float *a, float *w);
    
    if (n == 512) {
        cftmdl1(128, a, &w[nw - 64]);
        cftf161(a, &w[nw - 8]);
        cftf162(&a[32], &w[nw - 32]);
        cftf161(&a[64], &w[nw - 8]);
        cftf161(&a[96], &w[nw - 8]);
        cftmdl2(128, &a[128], &w[nw - 128]);
        cftf161(&a[128], &w[nw - 8]);
        cftf162(&a[160], &w[nw - 32]);
        cftf161(&a[192], &w[nw - 8]);
        cftf162(&a[224], &w[nw - 32]);
        cftmdl1(128, &a[256], &w[nw - 64]);
        cftf161(&a[256], &w[nw - 8]);
        cftf162(&a[288], &w[nw - 32]);
        cftf161(&a[320], &w[nw - 8]);
        cftf161(&a[352], &w[nw - 8]);
        if (isplt != 0) {
            cftmdl1(128, &a[384], &w[nw - 64]);
            cftf161(&a[480], &w[nw - 8]);
        } else {
            cftmdl2(128, &a[384], &w[nw - 128]);
            cftf162(&a[480], &w[nw - 32]);
        }
        cftf161(&a[384], &w[nw - 8]);
        cftf162(&a[416], &w[nw - 32]);
        cftf161(&a[448], &w[nw - 8]);
    } else {
        cftmdl1(64, a, &w[nw - 32]);
        cftf081(a, &w[nw - 8]);
        cftf082(&a[16], &w[nw - 8]);
        cftf081(&a[32], &w[nw - 8]);
        cftf081(&a[48], &w[nw - 8]);
        cftmdl2(64, &a[64], &w[nw - 64]);
        cftf081(&a[64], &w[nw - 8]);
        cftf082(&a[80], &w[nw - 8]);
        cftf081(&a[96], &w[nw - 8]);
        cftf082(&a[112], &w[nw - 8]);
        cftmdl1(64, &a[128], &w[nw - 32]);
        cftf081(&a[128], &w[nw - 8]);
        cftf082(&a[144], &w[nw - 8]);
        cftf081(&a[160], &w[nw - 8]);
        cftf081(&a[176], &w[nw - 8]);
        if (isplt != 0) {
            cftmdl1(64, &a[192], &w[nw - 32]);
            cftf081(&a[240], &w[nw - 8]);
        } else {
            cftmdl2(64, &a[192], &w[nw - 64]);
            cftf082(&a[240], &w[nw - 8]);
        }
        cftf081(&a[192], &w[nw - 8]);
        cftf082(&a[208], &w[nw - 8]);
        cftf081(&a[224], &w[nw - 8]);
    }
}

static void cftrec4(int n, float *a, int nw, float *w)
{
    int cfttree(int n, int j, int k, float *a, int nw, float *w);
    void cftleaf(int n, int isplt, float *a, int nw, float *w);
    void cftmdl1(int n, float *a, float *w);
    int isplt, j, k, m;
    
    m = n;
    while (m > 512) {
        m >>= 2;
        cftmdl1(m, &a[n - m], &w[nw - (m >> 1)]);
    }
    cftleaf(m, 1, &a[n - m], nw, w);
    k = 0;
    for (j = n - m; j > 0; j -= m) {
        k++;
        isplt = cfttree(m, j, k, a, nw, w);
        cftleaf(m, isplt, &a[j - m], nw, w);
    }
}

static void cftfx41(int n, float *a, int nw, float *w)
{
    void cftf161(float *a, float *w);
    void cftf162(float *a, float *w);
    void cftf081(float *a, float *w);
    void cftf082(float *a, float *w);
    
    if (n == 128) {
        cftf161(a, &w[nw - 8]);
        cftf162(&a[32], &w[nw - 32]);
        cftf161(&a[64], &w[nw - 8]);
        cftf161(&a[96], &w[nw - 8]);
    } else {
        cftf081(a, &w[nw - 8]);
        cftf082(&a[16], &w[nw - 8]);
        cftf081(&a[32], &w[nw - 8]);
        cftf081(&a[48], &w[nw - 8]);
    }
}

static void cftf040(float *a)
{
    float x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    
    x0r = a[0] + a[4];
    x0i = a[1] + a[5];
    x1r = a[0] - a[4];
    x1i = a[1] - a[5];
    x2r = a[2] + a[6];
    x2i = a[3] + a[7];
    x3r = a[2] - a[6];
    x3i = a[3] - a[7];
    a[0] = x0r + x2r;
    a[1] = x0i + x2i;
    a[2] = x1r - x3i;
    a[3] = x1i + x3r;
    a[4] = x0r - x2r;
    a[5] = x0i - x2i;
    a[6] = x1r + x3i;
    a[7] = x1i - x3r;
}

static void cftx020(float *a)
{
    float x0r, x0i;
    
    x0r = a[0] - a[2];
    x0i = a[1] - a[3];
    a[0] += a[2];
    a[1] += a[3];
    a[2] = x0r;
    a[3] = x0i;
}

#ifdef USE_CDFT_THREADS
struct cdft_arg_st {
    int n0;
    int n;
    float *a;
    int nw;
    float *w;
};
typedef struct cdft_arg_st cdft_arg_t;


static void cftrec4_th(int n, float *a, int nw, float *w)
{
    void *cftrec1_th(void *p);
    void *cftrec2_th(void *p);
    int i, idiv4, m, nthread;
    cdft_thread_t th[4];
    cdft_arg_t ag[4];
    
    nthread = 2;
    idiv4 = 0;
    m = n >> 1;
    if (n > CDFT_4THREADS_BEGIN_N) {
        nthread = 4;
        idiv4 = 1;
        m >>= 1;
    }
    for (i = 0; i < nthread; i++) {
        ag[i].n0 = n;
        ag[i].n = m;
        ag[i].a = &a[i * m];
        ag[i].nw = nw;
        ag[i].w = w;
        if (i != idiv4) {
            cdft_thread_create(&th[i], cftrec1_th, &ag[i]);
        } else {
            cdft_thread_create(&th[i], cftrec2_th, &ag[i]);
        }
    }
    for (i = 0; i < nthread; i++) {
        cdft_thread_wait(th[i]);
    }
}


static void *cftrec1_th(void *p)
{
    int cfttree(int n, int j, int k, float *a, int nw, float *w);
    void cftleaf(int n, int isplt, float *a, int nw, float *w);
    void cftmdl1(int n, float *a, float *w);
    int isplt, j, k, m, n, n0, nw;
    float *a, *w;
    
    n0 = ((cdft_arg_t *) p)->n0;
    n = ((cdft_arg_t *) p)->n;
    a = ((cdft_arg_t *) p)->a;
    nw = ((cdft_arg_t *) p)->nw;
    w = ((cdft_arg_t *) p)->w;
    m = n0;
    while (m > 512) {
        m >>= 2;
        cftmdl1(m, &a[n - m], &w[nw - (m >> 1)]);
    }
    cftleaf(m, 1, &a[n - m], nw, w);
    k = 0;
    for (j = n - m; j > 0; j -= m) {
        k++;
        isplt = cfttree(m, j, k, a, nw, w);
        cftleaf(m, isplt, &a[j - m], nw, w);
    }
    return (void *) 0;
}


static void *cftrec2_th(void *p)
{
    int cfttree(int n, int j, int k, float *a, int nw, float *w);
    void cftleaf(int n, int isplt, float *a, int nw, float *w);
    void cftmdl2(int n, float *a, float *w);
    int isplt, j, k, m, n, n0, nw;
    float *a, *w;
    
    n0 = ((cdft_arg_t *) p)->n0;
    n = ((cdft_arg_t *) p)->n;
    a = ((cdft_arg_t *) p)->a;
    nw = ((cdft_arg_t *) p)->nw;
    w = ((cdft_arg_t *) p)->w;
    k = 1;
    m = n0;
    while (m > 512) {
        m >>= 2;
        k <<= 2;
        cftmdl2(m, &a[n - m], &w[nw - m]);
    }
    cftleaf(m, 0, &a[n - m], nw, w);
    k >>= 1;
    for (j = n - m; j > 0; j -= m) {
        k++;
        isplt = cfttree(m, j, k, a, nw, w);
        cftleaf(m, isplt, &a[j - m], nw, w);
    }
    return (void *) 0;
}
#endif /* USE_CDFT_THREADS */

static void cftfsub(int n, float *a, int *ip, int nw, float *w)
{
    void bitrv2(int n, int *ip, float *a);
    void bitrv216(float *a);
    void bitrv208(float *a);
    void cftf1st(int n, float *a, float *w);
    void cftrec4(int n, float *a, int nw, float *w);
    void cftleaf(int n, int isplt, float *a, int nw, float *w);
    void cftfx41(int n, float *a, int nw, float *w);
    void cftf161(float *a, float *w);
    void cftf081(float *a, float *w);
    void cftf040(float *a);
    void cftx020(float *a);
#ifdef USE_CDFT_THREADS
    void cftrec4_th(int n, float *a, int nw, float *w);
#endif /* USE_CDFT_THREADS */
    
    if (n > 8) {
        if (n > 32) {
            cftf1st(n, a, &w[nw - (n >> 2)]);
#ifdef USE_CDFT_THREADS
            if (n > CDFT_THREADS_BEGIN_N) {
                cftrec4_th(n, a, nw, w);
            } else 
#endif /* USE_CDFT_THREADS */
            if (n > 512) {
                cftrec4(n, a, nw, w);
            } else if (n > 128) {
                cftleaf(n, 1, a, nw, w);
            } else {
                cftfx41(n, a, nw, w);
            }
            bitrv2(n, ip, a);
        } else if (n == 32) {
            cftf161(a, &w[nw - 8]);
            bitrv216(a);
        } else {
            cftf081(a, w);
            bitrv208(a);
        }
    } else if (n == 8) {
        cftf040(a);
    } else if (n == 4) {
        cftx020(a);
    }
}

static void bitrv2conj(int n, int *ip, float *a)
{
    int j, j1, k, k1, l, m, nh, nm;
    float xr, xi, yr, yi;
    
    m = 1;
    for (l = n >> 2; l > 8; l >>= 2) {
        m <<= 1;
    }
    nh = n >> 1;
    nm = 4 * m;
    if (l == 8) {
        for (k = 0; k < m; k++) {
            for (j = 0; j < k; j++) {
                j1 = 4 * j + 2 * ip[m + k];
                k1 = 4 * k + 2 * ip[m + j];
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nm;
                k1 += 2 * nm;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nm;
                k1 -= nm;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nm;
                k1 += 2 * nm;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nh;
                k1 += 2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nm;
                k1 -= 2 * nm;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nm;
                k1 += nm;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nm;
                k1 -= 2 * nm;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += 2;
                k1 += nh;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nm;
                k1 += 2 * nm;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nm;
                k1 -= nm;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nm;
                k1 += 2 * nm;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nh;
                k1 -= 2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nm;
                k1 -= 2 * nm;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nm;
                k1 += nm;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nm;
                k1 -= 2 * nm;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
            }
            k1 = 4 * k + 2 * ip[m + k];
            j1 = k1 + 2;
            k1 += nh;
            a[j1 - 1] = -a[j1 - 1];
            xr = a[j1];
            xi = -a[j1 + 1];
            yr = a[k1];
            yi = -a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            a[k1 + 3] = -a[k1 + 3];
            j1 += nm;
            k1 += 2 * nm;
            xr = a[j1];
            xi = -a[j1 + 1];
            yr = a[k1];
            yi = -a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            j1 += nm;
            k1 -= nm;
            xr = a[j1];
            xi = -a[j1 + 1];
            yr = a[k1];
            yi = -a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            j1 -= 2;
            k1 -= nh;
            xr = a[j1];
            xi = -a[j1 + 1];
            yr = a[k1];
            yi = -a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            j1 += nh + 2;
            k1 += nh + 2;
            xr = a[j1];
            xi = -a[j1 + 1];
            yr = a[k1];
            yi = -a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            j1 -= nh - nm;
            k1 += 2 * nm - 2;
            a[j1 - 1] = -a[j1 - 1];
            xr = a[j1];
            xi = -a[j1 + 1];
            yr = a[k1];
            yi = -a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            a[k1 + 3] = -a[k1 + 3];
        }
    } else {
        for (k = 0; k < m; k++) {
            for (j = 0; j < k; j++) {
                j1 = 4 * j + ip[m + k];
                k1 = 4 * k + ip[m + j];
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nm;
                k1 += nm;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nh;
                k1 += 2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nm;
                k1 -= nm;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += 2;
                k1 += nh;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nm;
                k1 += nm;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nh;
                k1 -= 2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nm;
                k1 -= nm;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
            }
            k1 = 4 * k + ip[m + k];
            j1 = k1 + 2;
            k1 += nh;
            a[j1 - 1] = -a[j1 - 1];
            xr = a[j1];
            xi = -a[j1 + 1];
            yr = a[k1];
            yi = -a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            a[k1 + 3] = -a[k1 + 3];
            j1 += nm;
            k1 += nm;
            a[j1 - 1] = -a[j1 - 1];
            xr = a[j1];
            xi = -a[j1 + 1];
            yr = a[k1];
            yi = -a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            a[k1 + 3] = -a[k1 + 3];
        }
    }
}

static void bitrv216neg(float *a)
{
    float x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i, 
        x5r, x5i, x6r, x6i, x7r, x7i, x8r, x8i, 
        x9r, x9i, x10r, x10i, x11r, x11i, x12r, x12i, 
        x13r, x13i, x14r, x14i, x15r, x15i;
    
    x1r = a[2];
    x1i = a[3];
    x2r = a[4];
    x2i = a[5];
    x3r = a[6];
    x3i = a[7];
    x4r = a[8];
    x4i = a[9];
    x5r = a[10];
    x5i = a[11];
    x6r = a[12];
    x6i = a[13];
    x7r = a[14];
    x7i = a[15];
    x8r = a[16];
    x8i = a[17];
    x9r = a[18];
    x9i = a[19];
    x10r = a[20];
    x10i = a[21];
    x11r = a[22];
    x11i = a[23];
    x12r = a[24];
    x12i = a[25];
    x13r = a[26];
    x13i = a[27];
    x14r = a[28];
    x14i = a[29];
    x15r = a[30];
    x15i = a[31];
    a[2] = x15r;
    a[3] = x15i;
    a[4] = x7r;
    a[5] = x7i;
    a[6] = x11r;
    a[7] = x11i;
    a[8] = x3r;
    a[9] = x3i;
    a[10] = x13r;
    a[11] = x13i;
    a[12] = x5r;
    a[13] = x5i;
    a[14] = x9r;
    a[15] = x9i;
    a[16] = x1r;
    a[17] = x1i;
    a[18] = x14r;
    a[19] = x14i;
    a[20] = x6r;
    a[21] = x6i;
    a[22] = x10r;
    a[23] = x10i;
    a[24] = x2r;
    a[25] = x2i;
    a[26] = x12r;
    a[27] = x12i;
    a[28] = x4r;
    a[29] = x4i;
    a[30] = x8r;
    a[31] = x8i;
}

static void bitrv208neg(float *a)
{
    float x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i, 
        x5r, x5i, x6r, x6i, x7r, x7i;
    
    x1r = a[2];
    x1i = a[3];
    x2r = a[4];
    x2i = a[5];
    x3r = a[6];
    x3i = a[7];
    x4r = a[8];
    x4i = a[9];
    x5r = a[10];
    x5i = a[11];
    x6r = a[12];
    x6i = a[13];
    x7r = a[14];
    x7i = a[15];
    a[2] = x7r;
    a[3] = x7i;
    a[4] = x3r;
    a[5] = x3i;
    a[6] = x5r;
    a[7] = x5i;
    a[8] = x1r;
    a[9] = x1i;
    a[10] = x6r;
    a[11] = x6i;
    a[12] = x2r;
    a[13] = x2i;
    a[14] = x4r;
    a[15] = x4i;
}

static void cftb1st(int n, float *a, float *w)
{
    int j, j0, j1, j2, j3, k, m, mh;
    float wn4r, csc1, csc3, wk1r, wk1i, wk3r, wk3i, 
        wd1r, wd1i, wd3r, wd3i;
    float x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i;
    
    mh = n >> 3;
    m = 2 * mh;
    j1 = m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[0] + a[j2];
    x0i = -a[1] - a[j2 + 1];
    x1r = a[0] - a[j2];
    x1i = -a[1] + a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[0] = x0r + x2r;
    a[1] = x0i - x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i + x2i;
    a[j2] = x1r + x3i;
    a[j2 + 1] = x1i + x3r;
    a[j3] = x1r - x3i;
    a[j3 + 1] = x1i - x3r;
    wn4r = w[1];
    csc1 = w[2];
    csc3 = w[3];
    wd1r = 1;
    wd1i = 0;
    wd3r = 1;
    wd3i = 0;
    k = 0;
    for (j = 2; j < mh - 2; j += 4) {
        k += 4;
        wk1r = csc1 * (wd1r + w[k]);
        wk1i = csc1 * (wd1i + w[k + 1]);
        wk3r = csc3 * (wd3r + w[k + 2]);
        wk3i = csc3 * (wd3i + w[k + 3]);
        wd1r = w[k];
        wd1i = w[k + 1];
        wd3r = w[k + 2];
        wd3i = w[k + 3];
        j1 = j + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j] + a[j2];
        x0i = -a[j + 1] - a[j2 + 1];
        x1r = a[j] - a[j2];
        x1i = -a[j + 1] + a[j2 + 1];
        y0r = a[j + 2] + a[j2 + 2];
        y0i = -a[j + 3] - a[j2 + 3];
        y1r = a[j + 2] - a[j2 + 2];
        y1i = -a[j + 3] + a[j2 + 3];
        x2r = a[j1] + a[j3];
        x2i = a[j1 + 1] + a[j3 + 1];
        x3r = a[j1] - a[j3];
        x3i = a[j1 + 1] - a[j3 + 1];
        y2r = a[j1 + 2] + a[j3 + 2];
        y2i = a[j1 + 3] + a[j3 + 3];
        y3r = a[j1 + 2] - a[j3 + 2];
        y3i = a[j1 + 3] - a[j3 + 3];
        a[j] = x0r + x2r;
        a[j + 1] = x0i - x2i;
        a[j + 2] = y0r + y2r;
        a[j + 3] = y0i - y2i;
        a[j1] = x0r - x2r;
        a[j1 + 1] = x0i + x2i;
        a[j1 + 2] = y0r - y2r;
        a[j1 + 3] = y0i + y2i;
        x0r = x1r + x3i;
        x0i = x1i + x3r;
        a[j2] = wk1r * x0r - wk1i * x0i;
        a[j2 + 1] = wk1r * x0i + wk1i * x0r;
        x0r = y1r + y3i;
        x0i = y1i + y3r;
        a[j2 + 2] = wd1r * x0r - wd1i * x0i;
        a[j2 + 3] = wd1r * x0i + wd1i * x0r;
        x0r = x1r - x3i;
        x0i = x1i - x3r;
        a[j3] = wk3r * x0r + wk3i * x0i;
        a[j3 + 1] = wk3r * x0i - wk3i * x0r;
        x0r = y1r - y3i;
        x0i = y1i - y3r;
        a[j3 + 2] = wd3r * x0r + wd3i * x0i;
        a[j3 + 3] = wd3r * x0i - wd3i * x0r;
        j0 = m - j;
        j1 = j0 + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j0] + a[j2];
        x0i = -a[j0 + 1] - a[j2 + 1];
        x1r = a[j0] - a[j2];
        x1i = -a[j0 + 1] + a[j2 + 1];
        y0r = a[j0 - 2] + a[j2 - 2];
        y0i = -a[j0 - 1] - a[j2 - 1];
        y1r = a[j0 - 2] - a[j2 - 2];
        y1i = -a[j0 - 1] + a[j2 - 1];
        x2r = a[j1] + a[j3];
        x2i = a[j1 + 1] + a[j3 + 1];
        x3r = a[j1] - a[j3];
        x3i = a[j1 + 1] - a[j3 + 1];
        y2r = a[j1 - 2] + a[j3 - 2];
        y2i = a[j1 - 1] + a[j3 - 1];
        y3r = a[j1 - 2] - a[j3 - 2];
        y3i = a[j1 - 1] - a[j3 - 1];
        a[j0] = x0r + x2r;
        a[j0 + 1] = x0i - x2i;
        a[j0 - 2] = y0r + y2r;
        a[j0 - 1] = y0i - y2i;
        a[j1] = x0r - x2r;
        a[j1 + 1] = x0i + x2i;
        a[j1 - 2] = y0r - y2r;
        a[j1 - 1] = y0i + y2i;
        x0r = x1r + x3i;
        x0i = x1i + x3r;
        a[j2] = wk1i * x0r - wk1r * x0i;
        a[j2 + 1] = wk1i * x0i + wk1r * x0r;
        x0r = y1r + y3i;
        x0i = y1i + y3r;
        a[j2 - 2] = wd1i * x0r - wd1r * x0i;
        a[j2 - 1] = wd1i * x0i + wd1r * x0r;
        x0r = x1r - x3i;
        x0i = x1i - x3r;
        a[j3] = wk3i * x0r + wk3r * x0i;
        a[j3 + 1] = wk3i * x0i - wk3r * x0r;
        x0r = y1r - y3i;
        x0i = y1i - y3r;
        a[j3 - 2] = wd3i * x0r + wd3r * x0i;
        a[j3 - 1] = wd3i * x0i - wd3r * x0r;
    }
    wk1r = csc1 * (wd1r + wn4r);
    wk1i = csc1 * (wd1i + wn4r);
    wk3r = csc3 * (wd3r - wn4r);
    wk3i = csc3 * (wd3i - wn4r);
    j0 = mh;
    j1 = j0 + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j0 - 2] + a[j2 - 2];
    x0i = -a[j0 - 1] - a[j2 - 1];
    x1r = a[j0 - 2] - a[j2 - 2];
    x1i = -a[j0 - 1] + a[j2 - 1];
    x2r = a[j1 - 2] + a[j3 - 2];
    x2i = a[j1 - 1] + a[j3 - 1];
    x3r = a[j1 - 2] - a[j3 - 2];
    x3i = a[j1 - 1] - a[j3 - 1];
    a[j0 - 2] = x0r + x2r;
    a[j0 - 1] = x0i - x2i;
    a[j1 - 2] = x0r - x2r;
    a[j1 - 1] = x0i + x2i;
    x0r = x1r + x3i;
    x0i = x1i + x3r;
    a[j2 - 2] = wk1r * x0r - wk1i * x0i;
    a[j2 - 1] = wk1r * x0i + wk1i * x0r;
    x0r = x1r - x3i;
    x0i = x1i - x3r;
    a[j3 - 2] = wk3r * x0r + wk3i * x0i;
    a[j3 - 1] = wk3r * x0i - wk3i * x0r;
    x0r = a[j0] + a[j2];
    x0i = -a[j0 + 1] - a[j2 + 1];
    x1r = a[j0] - a[j2];
    x1i = -a[j0 + 1] + a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[j0] = x0r + x2r;
    a[j0 + 1] = x0i - x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i + x2i;
    x0r = x1r + x3i;
    x0i = x1i + x3r;
    a[j2] = wn4r * (x0r - x0i);
    a[j2 + 1] = wn4r * (x0i + x0r);
    x0r = x1r - x3i;
    x0i = x1i - x3r;
    a[j3] = -wn4r * (x0r + x0i);
    a[j3 + 1] = -wn4r * (x0i - x0r);
    x0r = a[j0 + 2] + a[j2 + 2];
    x0i = -a[j0 + 3] - a[j2 + 3];
    x1r = a[j0 + 2] - a[j2 + 2];
    x1i = -a[j0 + 3] + a[j2 + 3];
    x2r = a[j1 + 2] + a[j3 + 2];
    x2i = a[j1 + 3] + a[j3 + 3];
    x3r = a[j1 + 2] - a[j3 + 2];
    x3i = a[j1 + 3] - a[j3 + 3];
    a[j0 + 2] = x0r + x2r;
    a[j0 + 3] = x0i - x2i;
    a[j1 + 2] = x0r - x2r;
    a[j1 + 3] = x0i + x2i;
    x0r = x1r + x3i;
    x0i = x1i + x3r;
    a[j2 + 2] = wk1i * x0r - wk1r * x0i;
    a[j2 + 3] = wk1i * x0i + wk1r * x0r;
    x0r = x1r - x3i;
    x0i = x1i - x3r;
    a[j3 + 2] = wk3i * x0r + wk3r * x0i;
    a[j3 + 3] = wk3i * x0i - wk3r * x0r;
}

static void cftb040(float *a)
{
    float x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    
    x0r = a[0] + a[4];
    x0i = a[1] + a[5];
    x1r = a[0] - a[4];
    x1i = a[1] - a[5];
    x2r = a[2] + a[6];
    x2i = a[3] + a[7];
    x3r = a[2] - a[6];
    x3i = a[3] - a[7];
    a[0] = x0r + x2r;
    a[1] = x0i + x2i;
    a[2] = x1r + x3i;
    a[3] = x1i - x3r;
    a[4] = x0r - x2r;
    a[5] = x0i - x2i;
    a[6] = x1r - x3i;
    a[7] = x1i + x3r;
}

static void cftbsub(int n, float *a, int *ip, int nw, float *w)
{
    void bitrv2conj(int n, int *ip, float *a);
    void bitrv216neg(float *a);
    void bitrv208neg(float *a);
    void cftb1st(int n, float *a, float *w);
    void cftrec4(int n, float *a, int nw, float *w);
    void cftleaf(int n, int isplt, float *a, int nw, float *w);
    void cftfx41(int n, float *a, int nw, float *w);
    void cftf161(float *a, float *w);
    void cftf081(float *a, float *w);
    void cftb040(float *a);
    void cftx020(float *a);
#ifdef USE_CDFT_THREADS
    void cftrec4_th(int n, float *a, int nw, float *w);
#endif /* USE_CDFT_THREADS */
    
    if (n > 8) {
        if (n > 32) {
            cftb1st(n, a, &w[nw - (n >> 2)]);
#ifdef USE_CDFT_THREADS
            if (n > CDFT_THREADS_BEGIN_N) {
                cftrec4_th(n, a, nw, w);
            } else 
#endif /* USE_CDFT_THREADS */
            if (n > 512) {
                cftrec4(n, a, nw, w);
            } else if (n > 128) {
                cftleaf(n, 1, a, nw, w);
            } else {
                cftfx41(n, a, nw, w);
            }
            bitrv2conj(n, ip, a);
        } else if (n == 32) {
            cftf161(a, &w[nw - 8]);
            bitrv216neg(a);
        } else {
            cftf081(a, w);
            bitrv208neg(a);
        }
    } else if (n == 8) {
        cftb040(a);
    } else if (n == 4) {
        cftx020(a);
    }
}

static void rftfsub(int n, float *a, int nc, float *c)
{
    int j, k, kk, ks, m;
    float wkr, wki, xr, xi, yr, yi;
    
    m = n >> 1;
    ks = 2 * nc / m;
    kk = 0;
    for (j = 2; j < m; j += 2) {
        k = n - j;
        kk += ks;
        wkr = 0.5 - c[nc - kk];
        wki = c[kk];
        xr = a[j] - a[k];
        xi = a[j + 1] + a[k + 1];
        yr = wkr * xr - wki * xi;
        yi = wkr * xi + wki * xr;
        a[j] -= yr;
        a[j + 1] -= yi;
        a[k] += yr;
        a[k + 1] -= yi;
    }
}

static void rftbsub(int n, float *a, int nc, float *c)
{
    int j, k, kk, ks, m;
    float wkr, wki, xr, xi, yr, yi;
    
    m = n >> 1;
    ks = 2 * nc / m;
    kk = 0;
    for (j = 2; j < m; j += 2) {
        k = n - j;
        kk += ks;
        wkr = 0.5 - c[nc - kk];
        wki = c[kk];
        xr = a[j] - a[k];
        xi = a[j + 1] + a[k + 1];
        yr = wkr * xr + wki * xi;
        yi = wkr * xi - wki * xr;
        a[j] -= yr;
        a[j + 1] -= yi;
        a[k] += yr;
        a[k + 1] -= yi;
    }
}

static void rdft(int n, int isgn, float *a, int *ip, float *w)
{
    void makewt(int nw, int *ip, float *w);
    void makect(int nc, int *ip, float *c);
    void cftfsub(int n, float *a, int *ip, int nw, float *w);
    void cftbsub(int n, float *a, int *ip, int nw, float *w);
    void rftfsub(int n, float *a, int nc, float *c);
    void rftbsub(int n, float *a, int nc, float *c);
    int nw, nc;
    float xi;
    
    nw = ip[0];
    if (n > (nw << 2)) {
        nw = n >> 2;
        makewt(nw, ip, w);
    }
    nc = ip[1];
    if (n > (nc << 2)) {
        nc = n >> 2;
        makect(nc, ip, w + nw);
    }
    if (isgn >= 0) {
        if (n > 4) {
            cftfsub(n, a, ip, nw, w);
            rftfsub(n, a, nc, w + nw);
        } else if (n == 4) {
            cftfsub(n, a, ip, nw, w);
        }
        xi = a[0] - a[1];
        a[0] += a[1];
        a[1] = xi;
    } else {
        a[1] = 0.5 * (a[0] - a[1]);
        a[0] -= a[1];
        if (n > 4) {
            rftbsub(n, a, nc, w + nw);
            cftbsub(n, a, ip, nw, w);
        } else if (n == 4) {
            cftbsub(n, a, ip, nw, w);
        }
    }
}

// input array (any shape >= 1D)
// output array (shape = input.shape.replace(axis, n).insert(0,2))
// d0 = input.shape.step(axis)
// d1 = input.shape.size(axis)
// d2 = input.shape.slot(axis)
static inline void rdft_ndim_f32(
    const float* restrict input, 
    float* restrict output, 
    int d0, int d1, int d2,
    int* restrict temp_ip, float* restrict temp_w, float* restrict temp_a)
{
    void rdft(int n, int isgn, float* a, int* ip, float* w);

    int d3 = d0 * d1;
    int d_out = (d1 >> 1) + 1;

    for (int k = 0; k < d2; k++)
    {
        int dk = k * d3;
        int dm = k * 2 * d_out * d0;
        for (int i = 0; i < d0; i++)
        {                	           
            for (int j = 0; j < d1; j++)
            {
                temp_a[j] = input[dk + j * d0 + i];
            }
            rdft(d1, 1, temp_a, temp_ip, temp_w);

            for (int m = 2; m < d1; m+=2)
            {
                int index = (m * d0) + 2 * i + dm;
                output[index] = temp_a[m];
                output[index + 1] = -temp_a[m + 1];
            }
            int beta = dm + 2 * i;
            output[beta] = temp_a[0];
            output[beta + 1] = 0;
            output[beta + d3] = temp_a[1];
            output[beta + d3 + 1] = 0;
        }
    }
}

static inline float __norm_f32(const float* input, int step, int count)
{
	float sum = 0;
	for (int j = 0; j < count; j++) {
		float item = input[j * step];
		sum += item * item;
	}
	return sqrtf(sum);
}

// input array (any shape >= 2D)
// output array (same shape as input array except with axis removed)
// d0 = input.shape.step(axis)
// d1 = input.shape.size(axis)
// d2 = input.shape.slot(axis)
static inline void norm_f32(const float* restrict input, int d0, int d1, int d2, float* restrict output)
{
	const int d3 = d0 * d1;

	for (int k = 0; k < d2; k++) {
		int k3 = k * d3;
		int k0 = k * d0;
		for (int i = 0; i < d0; i++) {			
			*(output + i + k0) = __norm_f32(input + i + k3, d0, d1);
		}
	}
}

static inline float __mel_f32(const float* restrict input, const int* restrict filter_points, int filter)
{
	int n0 = filter_points[filter];
	int n1 = filter_points[filter+1];
	int n2 = filter_points[filter+2];
	int c0 = n1 - n0;
	int c1 = n2 - n1;
	float sum = 0;
	
	for (int i = 0; i <= c0; i++) {
		float rate = i / (float)c0;
		float value = input[i + n0];
		sum += value * rate;
	}

	for (int i = 1; i <= c1; i++) {
		float rate = i / (float)c1;
		float value = input[i + n1];
		sum += value * (1.0 - rate);
	}

	return sum;
}

// input array (any shape >= 1D)
// output array (same shape as input array except with 0 replaced with num_filter)
// size = input.shape.size(0)
// slot = input.shape.slot(0)
static inline void mel_f32(const float* restrict input, const int* restrict filter_points, int size, int slot, int num_filter, float* restrict output)
{	
	for (int k = 0; k < slot; k++) {
		const float *ip = input + k * size;
		for (int i = 0; i < num_filter; i++) {
			*output++ = __mel_f32(ip, filter_points, i);
		}
	}
}

static inline void addi_f32(
	const float* restrict x,
	int count,
	float immediate,
	float* restrict output)
{
	for (int i = 0; i < count; i++) {
		output[i] = x[i] + immediate;
	}
}

static inline void loge_f32(const float* restrict x, int count, float* restrict result)
{
	for (int i = 0; i < count; i++) {
		*result++ = logf(*x++);
	}
}

static inline void clip_f32(const float* restrict input, int count, float min, float max, float* restrict output)
{	
	for (int i = 0; i < count; i++) {
		float value = input[i];
		if (value > max)
			value = max;
		if (value < min)
			value = min;

		output[i] = value;
	}
}

/**
 * Enqueue handle->input_size values from given *data pointer to internal window buffer.
 *
 * @param handle Pointer to an initialized handle.
 * @param data Data to enqueue.
 * @return IPWIN_RET_SUCCESS (0) or IPWIN_RET_NOMEM (-2) if internal buffer is out of memory.
 */
static inline int fixwin_enqueuef32(void* restrict handle, const void* restrict data)
{
	fixwin_t* fep = (fixwin_t*)handle;

	if (cbuffer_enqueue(&fep->data_buffer, data, fep->input_size) != 0)
		return IPWIN_RET_NOMEM;

	return IPWIN_RET_SUCCESS;
}

// Single dot product
static inline float _conv2d_dotsum_f32(const float* restrict a, const float* restrict b, int count)
{
	float sum = 0;
	for (int i = 0; i < count; i++) {
		sum += *a++ * *b++;
	}
	return sum;
}

// Matrix vector dot product
static inline void _conv2d_dot_f32(const float* restrict a, int ksize_cols, int ksize_rows, int filters, const float* restrict b, float* restrict out, int input_shape_cols, int input_shape_rows, int channels, int kernel_row_start, int kernel_col_start, int kernel_row_size, int kernel_col_size)
{
	for (int i = 0; i < filters; i++) {
		float in_res = 0;
		for (int j = 0; j < kernel_col_size; j++) {
			in_res += _conv2d_dotsum_f32(a + i * ksize_cols * ksize_rows * channels + (j + kernel_col_start) * ksize_rows * channels + kernel_row_start * channels, b + j * input_shape_cols * channels, kernel_row_size * channels);
		}
		*out++ = in_res;
	}
}

static inline void conv2d_f32(
	const float* restrict input,
	const float* restrict weight,
	float* restrict output,
	int filters,
	int kernel_size_cols,
	int kernel_size_rows,
	int strides_rows,
	int strides_cols,
	int input_shape_rows,
	int input_shape_cols,
	int channels,
	int padding_rows_start,
	int padding_rows_end,
	int padding_cols_start,
	int padding_cols_end
	)
{
	const float* wp = weight;		// Weight matrix
	const float* bp = input;		// Input matrix
	int k = 0;
	padding_rows_start = padding_rows_start < 0 ? 0 : padding_rows_start;
	padding_rows_end = padding_rows_end < 0 ? 0 : padding_rows_end;
	padding_cols_start = padding_cols_start < 0 ? 0 : padding_cols_start;
	padding_cols_end = padding_cols_end < 0 ? 0 : padding_cols_end;

	for (int i = 0; i * strides_cols + kernel_size_cols <= input_shape_rows + padding_cols_start + padding_cols_end; i++) {
		int k_col_s = i * strides_cols - padding_cols_start;
		int kernel_col_start = (k_col_s < 0) ? (-k_col_s) : 0;
		int kernel_col_end = (k_col_s + kernel_size_cols - input_shape_rows > 0) ? (input_shape_rows - k_col_s) : kernel_size_cols;
		int ip = (k_col_s >= 0) ? (k_col_s) : 0;
		for (int j = 0; j * strides_rows + kernel_size_rows <= input_shape_cols + padding_rows_start + padding_rows_end; j++) {
			int k_row_s = j * strides_rows - padding_rows_start;
			int kernel_row_start = (k_row_s < 0) ? (-k_row_s) : 0;
			int kernel_row_end = (k_row_s + kernel_size_rows - input_shape_cols > 0) ? (input_shape_cols - k_row_s) : kernel_size_rows;
			int jp = (k_row_s >= 0) ? (k_row_s) : 0;
			int step = (ip * input_shape_cols + jp) * channels;

			_conv2d_dot_f32(wp, kernel_size_cols, kernel_size_rows, filters, bp + step, output + k * filters, input_shape_cols, input_shape_rows, channels, kernel_row_start, kernel_col_start, kernel_row_end- kernel_row_start, kernel_col_end- kernel_col_start);
			k++;
		}
	}
}

static inline void mul_f32(
	const float* restrict a,
	const float* restrict b,
	int l, int g1, int m, int g2, int r,
	float* restrict output)
{
	int index = 0;
	for (int x = 0; x < l; x++) {
		for (int i = 0; i < g1; i++) {
			for (int y = 0; y < m; y++) {
				for (int j = 0; j < g2; j++) {
					for (int z = 0; z < r; z++) {
						output[index] = a[index] * b[x * m * r + y * r + z];
						index++;
					}
				}
			}
		}
	}
}

static inline void add_f32(
	const float* restrict a,
	const float* restrict b,
	int l, int g1, int m, int g2, int r,
	float* restrict output)
{
	int index = 0;
	for (int x = 0; x < l; x++) {
		for (int i = 0; i < g1; i++) {
			for (int y = 0; y < m; y++) {
				for (int j = 0; j < g2; j++) {
					for (int z = 0; z < r; z++) {
						output[index] = a[index] + b[x * m * r + y * r + z];
						index++;
					}
				}
			}
		}
	}
}

static inline void relu_f32(const float* restrict x, int count, float* restrict result)
{
	for (int i = 0; i < count; i++) {
		const float value = *x++;
		*result++ = value > 0 ? value : 0;
	}
}

// Find the max
static inline float _maxpool2d_row_f32(const float* restrict b, int count, int channels)
{
	float max = *b;
	for (int i = 1; i < count; i++) {
		float temp = *(b + i * channels);
		max = temp > max ? temp : max;
	}
	return max;
}

// Matrix vector
static inline void _maxpool2d_col_f32(const float* restrict b, float* restrict out, int input_shape_cols, int input_shape_rows, int channels, int row_count, int col_count)
{
	for (int i = 0; i < channels; i++) {
		float in_res = _maxpool2d_row_f32(b + i, row_count, channels);
		for (int j = 1; j < col_count; j++) {
			float tmp = _maxpool2d_row_f32(b + i + j * input_shape_cols * channels, row_count, channels);
			in_res = tmp > in_res ? tmp : in_res;
		}
		*out++ = in_res;
	}
}

static inline void maxpool2d_f32(
	const float* restrict input,
	float* restrict output,
	int pool_size_rows,
	int pool_size_cols,
	int strides_rows,
	int strides_cols,
	int input_shape_rows,
	int input_shape_cols,
	int channels,
	int padding_rows_start,
	int padding_rows_end,
	int padding_cols_start,
	int padding_cols_end
	)
{
	const float* bp = input;		// Input matrix
	int k = 0;
	padding_rows_start = padding_rows_start < 0 ? 0 : padding_rows_start;
	padding_rows_end = padding_rows_end < 0 ? 0 : padding_rows_end;
	padding_cols_start = padding_cols_start < 0 ? 0 : padding_cols_start;
	padding_cols_end = padding_cols_end < 0 ? 0 : padding_cols_end;
	int condition_row = input_shape_rows + padding_cols_start + padding_cols_end;
	int condition_col = input_shape_cols + padding_rows_start + padding_rows_end;
	for (int i = 0; i * strides_cols + pool_size_cols <= condition_row; i++) {
		for (int j = 0; j * strides_rows + pool_size_rows <= condition_col; j++) {
			int ps_col = i * strides_cols - padding_cols_start;
			int ps_row = j * strides_rows - padding_rows_start;
			int pool_col_start = (ps_col < 0) ? -ps_col : 0;
			int pool_row_start = (ps_row < 0) ? -ps_row : 0;
			int pool_col_end = (ps_col + pool_size_cols - input_shape_rows > 0) ? (input_shape_rows - ps_col) : pool_size_cols;
			int pool_row_end = (ps_row + pool_size_rows - input_shape_cols > 0) ? (input_shape_cols - ps_row) : pool_size_rows;
			int ip = (ps_col >= 0) ? ps_col : 0;
			int jp = (ps_row >= 0) ? ps_row : 0;
			int step = (ip * input_shape_cols + jp) * channels;

			_maxpool2d_col_f32(bp + step, output + k * channels, input_shape_cols, input_shape_rows, channels, pool_row_end - pool_row_start, pool_col_end - pool_col_start);
			k++;
		}
	}
}

static inline float _globav2d_f32_mean(const float* restrict x, int nchannel, int nsteps)
{
	float mean = 0.0;
	for (int i = 0; i < nsteps; i++) {
		const float value = *(x + i * nchannel);
		mean = mean + value;
	}
	mean = mean / (float)nsteps;
	return mean;
}

static inline void globav2d_f32(const float* restrict x, int width, int height, int nchannel, float* restrict result)
{
	// Loop over all channels
	int nsteps = height * width;
	for (int i = 0; i < nchannel; i++) {
		const float* xp = x + i; 
		*result++ = _globav2d_f32_mean(xp, nchannel, nsteps); // one result per channel
	}
}

static inline float mac_f32(const float* restrict a, const float* restrict b, int count)
{
	float sum = 0;
	for (int i = 0; i < count; i++) {
		sum += *a++ * *b++;
	}
	return sum;
}

static inline void dott_f32(const float *restrict a, const float *restrict b, float *restrict out, int d0, int d1, int d2)
{
	for (int i = 0; i < d2; i++) {
		float* op = out;
		for (int j = 0; j < d1; j++) {
			*op++ = mac_f32(a + j * d0, b, d0);
		}
		out += d1;
		b += d0;
	}
}

static inline void softmax_f32(const float* restrict x, int count, float* restrict result)
{
	float sum = 0;
	for (int i = 0; i < count; i++) {
		float value = expf(x[i]);
		sum += value;
		result[i] = value;
	}
	for (int i = 0; i < count; i++) {
		result[i] /= sum;
	}
}

/**
* Initializes a fixwin sampler handle.
*
* @param handle Pointer to a preallocated memory area of fixwin_handle_size() bytes to initialize.
*
* @param input_size Number of bytes to enqueue.
* @param window_count Number of items (of size input_size) in each window
*/
static inline void fixwin_initf32(void* restrict handle, int input_size, int window_count)
{
	fixwin_t* fep = (fixwin_t*)handle;
	fep->input_size = input_size;
	fep->window_count = window_count;

	char* mem = ((char*)handle) + sizeof(fixwin_t);

	int data_buffer = input_size * window_count;
	
	cbuffer_init(&fep->data_buffer, mem, data_buffer);
}

#define __RETURN_ERROR(_exp) do { int __ret = (_exp); if(__ret < 0) return __ret; } while(0)
#define __RETURN_ERROR_BREAK_EMPTY(_exp) {  int __ret = (_exp); if(__ret == -1) break; if(__ret < 0) return __ret;  } 

int IMAI_dequeue(float *restrict data_out) {    
    while(1) {
        __RETURN_ERROR_BREAK_EMPTY(fixwin_dequeuef32(_K4, _K3, 320));
        hammingmul_f32(_K3, _K5, _K6, 512, 1);
        rdft_ndim_f32(_K6, _K7, 1, 512, 1, _K8, _K9, _K10);
        norm_f32(_K7, 1, 2, 257, _K11);
        mel_f32(_K11, _K12, 257, 1, 32, _K13);
        addi_f32(_K13, 32, 1, _K14);
        loge_f32(_K14, 32, _K15);
        clip_f32(_K15, 32, 0, 4, _K16);
        __RETURN_ERROR_BREAK_EMPTY(fixwin_enqueuef32(_K18, _K16));
    }
    __RETURN_ERROR(fixwin_dequeuef32(_K18, _K17, 25));
    conv2d_f32(_K17, _K20, _K19, 12, 3, 3, 2, 2, 50, 32, 1, 0, 1, 0, 1);
    mul_f32(_K19, _K22, 1, 1, 1, 400, 12, _K24);
    add_f32(_K24, _K23, 1, 1, 1, 400, 12, _K21);
    relu_f32(_K21, 4800, _K25);
    conv2d_f32(_K25, _K27, _K26, 24, 3, 3, 1, 1, 25, 16, 12, 1, 1, 1, 1);
    relu_f32(_K26, 9600, _K28);
    conv2d_f32(_K28, _K30, _K29, 24, 3, 3, 1, 1, 25, 16, 24, 1, 1, 1, 1);
    mul_f32(_K29, _K32, 1, 1, 1, 400, 24, _K34);
    add_f32(_K34, _K33, 1, 1, 1, 400, 24, _K31);
    relu_f32(_K31, 9600, _K35);
    maxpool2d_f32(_K35, _K36, 2, 2, 2, 2, 25, 16, 24, 0, 0, 0, 0);
    globav2d_f32(_K36, 8, 12, 24, _K37);
    dott_f32(_K38, _K37, _K39, 24, 3, 1);
    add_f32(_K39, _K40, 1, 1, 1, 1, 3, _K41);
    softmax_f32(_K41, 3, data_out);
    return 0;
}

int IMAI_enqueue(const float *restrict data_in) {    
    __RETURN_ERROR(fixwin_enqueuef32(_K4, data_in));
    return 0;
}

void IMAI_init(void) {    
    fixwin_initf32(_K4, 4, 512);
    fixwin_initf32(_K18, 128, 50);
}

