/*
* DEEPCRAFT Studio 5.16.7527.0+cda5e80bc6d2ff8f9dd9f30a82f9ad44f49da0e3
* Copyright c 2023- Imagimob AB, All Rights Reserved.
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

#ifndef _IMAI_MODEL_H_
#define _IMAI_MODEL_H_
#ifdef _MSC_VER
#pragma once
#endif

#include <stdint.h>

typedef struct {    
    char *name;
    double TP; // True Positive or Correct Positive Prediction
    double FN; // False Negative or Incorrect Negative Prediction
    double FP; // False Positive or Incorrect Positive Prediction
    double TN; // True Negative or Correct Negative Prediction
    double TPR; // True Positive Rate or Sensitivity, Recall
    double TNR; // True Negative Rate or Specificity, Selectivity
    double PPV; // Positive Predictive Value or Precision
    double NPV; // Negative Predictive Value
    double FNR; // False Negative Rate or Miss Rate
    double FPR; // False Positive Rate or Fall-Out
    double FDR; // False Discovery Rate
    double FOR; // False Omission Rate
    double F1S; // F1 Score
} IMAI_stats;

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


#define IMAI_TEST_AVG_ACC 0.9431818181818182 // Accuracy
#define IMAI_TEST_AVG_F1S 0.9431321763316118 // F1 Score

#define IMAI_TEST_STATS { \
 {name: "unlabeled", TP: 0, FN: 0, FP: 0, TN: 440, TPR: 1, TNR: 1, PPV: 1, NPV: 1, FNR: 1, FPR: 0, FDR: 1, FOR: 0, F1S: 1, }, \
 {name: "background", TP: 214, FN: 6, FP: 19, TN: 201, TPR: 0.9727272727272, TNR: 0.9136363636363, PPV: 0.9184549356223, NPV: 0.9710144927536, FNR: 0.0272727272727, FPR: 0.0863636363636, FDR: 0.0815450643776, FOR: 0.0289855072463, F1S: 0.9448123620309, }, \
 {name: "human_acti...", TP: 201, FN: 19, FP: 6, TN: 214, TPR: 0.9136363636363, TNR: 0.9727272727272, PPV: 0.9710144927536, NPV: 0.9184549356223, FNR: 0.0863636363636, FPR: 0.0272727272727, FDR: 0.0289855072463, FOR: 0.0815450643776, F1S: 0.9414519906323, }, \
}

#ifdef IMAI_STATS_ENABLED
static const IMAI_stats IMAI_test_stats[] = IMAI_TEST_STATS;
#endif

/*
* Tensorflow Train Set
* 
* (ACC) Accuracy 96.094 %
* (F1S) F1 Score 96.088 %
* 
* Name of class                                               unlabeled       background   human_activity
* (TP) True Positive or Correct Positive Prediction                   0              640              590
* (FN) False Negative or Incorrect Negative Prediction                0                0               50
* (FP) False Positive or Incorrect Positive Prediction                0               50                0
* (TN) True Negative or Correct Negative Prediction                1280              590              640
* (TPR) True Positive Rate or Sensitivity, Recall              100.00 %         100.00 %          92.19 %
* (TNR) True Negative Rate or Specificity, Selectivity         100.00 %          92.19 %         100.00 %
* (PPV) Positive Predictive Value or Precision                 100.00 %          92.75 %         100.00 %
* (NPV) Negative Predictive Value                              100.00 %         100.00 %          92.75 %
* (FNR) False Negative Rate or Miss Rate                       100.00 %           0.00 %           7.81 %
* (FPR) False Positive Rate or Fall-Out                          0.00 %           7.81 %           0.00 %
* (FDR) False Discovery Rate                                   100.00 %           7.25 %           0.00 %
* (FOR) False Omission Rate                                      0.00 %           0.00 %           7.25 %
* (F1S) F1 Score                                               100.00 %          96.24 %          95.93 %
*/


#define IMAI_TRAIN_AVG_ACC 0.9609375 // Accuracy
#define IMAI_TRAIN_AVG_F1S 0.9608778042667645 // F1 Score

#define IMAI_TRAIN_STATS { \
 {name: "unlabeled", TP: 0, FN: 0, FP: 0, TN: 1280, TPR: 1, TNR: 1, PPV: 1, NPV: 1, FNR: 1, FPR: 0, FDR: 1, FOR: 0, F1S: 1, }, \
 {name: "background", TP: 640, FN: 0, FP: 50, TN: 590, TPR: 1, TNR: 0.921875, PPV: 0.9275362318840, NPV: 1, FNR: 0, FPR: 0.078125, FDR: 0.0724637681159, FOR: 0, F1S: 0.9624060150375, }, \
 {name: "human_acti...", TP: 590, FN: 50, FP: 0, TN: 640, TPR: 0.921875, TNR: 1, PPV: 1, NPV: 0.9275362318840, FNR: 0.078125, FPR: 0, FDR: 0, FOR: 0.0724637681159, F1S: 0.9593495934959, }, \
}

#ifdef IMAI_STATS_ENABLED
static const IMAI_stats IMAI_train_stats[] = IMAI_TRAIN_STATS;
#endif

/*
* Tensorflow Validation Set
* 
* (ACC) Accuracy 93.119 %
* (F1S) F1 Score 93.114 %
* 
* Name of class                                               unlabeled       background   human_activity
* (TP) True Positive or Correct Positive Prediction                   0              209              197
* (FN) False Negative or Incorrect Negative Prediction                0                9               21
* (FP) False Positive or Incorrect Positive Prediction                0               21                9
* (TN) True Negative or Correct Negative Prediction                 436              197              209
* (TPR) True Positive Rate or Sensitivity, Recall              100.00 %          95.87 %          90.37 %
* (TNR) True Negative Rate or Specificity, Selectivity         100.00 %          90.37 %          95.87 %
* (PPV) Positive Predictive Value or Precision                 100.00 %          90.87 %          95.63 %
* (NPV) Negative Predictive Value                              100.00 %          95.63 %          90.87 %
* (FNR) False Negative Rate or Miss Rate                       100.00 %           4.13 %           9.63 %
* (FPR) False Positive Rate or Fall-Out                          0.00 %           9.63 %           4.13 %
* (FDR) False Discovery Rate                                   100.00 %           9.13 %           4.37 %
* (FOR) False Omission Rate                                      0.00 %           4.37 %           9.13 %
* (F1S) F1 Score                                               100.00 %          93.30 %          92.92 %
*/


#define IMAI_VALIDATION_AVG_ACC 0.9311926605504587 // Accuracy
#define IMAI_VALIDATION_AVG_F1S 0.9311404986522911 // F1 Score

#define IMAI_VALIDATION_STATS { \
 {name: "unlabeled", TP: 0, FN: 0, FP: 0, TN: 436, TPR: 1, TNR: 1, PPV: 1, NPV: 1, FNR: 1, FPR: 0, FDR: 1, FOR: 0, F1S: 1, }, \
 {name: "background", TP: 209, FN: 9, FP: 21, TN: 197, TPR: 0.9587155963302, TNR: 0.9036697247706, PPV: 0.9086956521739, NPV: 0.9563106796116, FNR: 0.0412844036697, FPR: 0.0963302752293, FDR: 0.0913043478260, FOR: 0.0436893203883, F1S: 0.9330357142857, }, \
 {name: "human_acti...", TP: 197, FN: 21, FP: 9, TN: 209, TPR: 0.9036697247706, TNR: 0.9587155963302, PPV: 0.9563106796116, NPV: 0.9086956521739, FNR: 0.0963302752293, FPR: 0.0412844036697, FDR: 0.0436893203883, FOR: 0.0913043478260, F1S: 0.9292452830188, }, \
}

#ifdef IMAI_STATS_ENABLED
static const IMAI_stats IMAI_validation_stats[] = IMAI_VALIDATION_STATS;
#endif

#define IMAI_API_QUEUE

// All symbols in order
#define IMAI_SYMBOL_MAP {"unlabeled", "background", "human_activity"}

// Model GUID (16 bytes)
#define IMAI_MODEL_ID {0x9d, 0x1b, 0x8a, 0xe4, 0x8a, 0x54, 0xc8, 0x41, 0x86, 0x0a, 0x86, 0xac, 0x08, 0xe9, 0x5c, 0x34}

// First nibble is bit encoding, second nibble is number of bytes
#define IMAGINET_TYPES_NONE	(0x0)
#define IMAGINET_TYPES_FLOAT32	(0x14)
#define IMAGINET_TYPES_FLOAT64	(0x18)
#define IMAGINET_TYPES_INT8	(0x21)
#define IMAGINET_TYPES_INT16	(0x22)
#define IMAGINET_TYPES_INT32	(0x24)
#define IMAGINET_TYPES_INT64	(0x28)
#define IMAGINET_TYPES_QDYN8	(0x31)
#define IMAGINET_TYPES_QDYN16	(0x32)
#define IMAGINET_TYPES_QDYN32	(0x34)

// data_in [1] (4 bytes)
#define IMAI_DATA_IN_COUNT (1)
#define IMAI_DATA_IN_TYPE float
#define IMAI_DATA_IN_TYPE_ID IMAGINET_TYPES_FLOAT32
#define IMAI_DATA_IN_SCALE (1)
#define IMAI_DATA_IN_OFFSET (0)
#define IMAI_DATA_IN_IS_QUANTIZED (0)

// data_out [3] (12 bytes)
#define IMAI_DATA_OUT_COUNT (3)
#define IMAI_DATA_OUT_TYPE float
#define IMAI_DATA_OUT_TYPE_ID IMAGINET_TYPES_FLOAT32
#define IMAI_DATA_OUT_SCALE (1)
#define IMAI_DATA_OUT_OFFSET (0)
#define IMAI_DATA_OUT_IS_QUANTIZED (0)

#define IMAI_KEY_MAX (43)



// Return codes
#define IMAI_RET_SUCCESS 0
#define IMAI_RET_NODATA -1
#define IMAI_RET_NOMEM -2

// Exported methods
int IMAI_dequeue(float *restrict data_out);
int IMAI_enqueue(const float *restrict data_in);
void IMAI_init(void);

#endif /* _IMAI_MODEL_H_ */
